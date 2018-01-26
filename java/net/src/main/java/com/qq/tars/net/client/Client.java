/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

package com.qq.tars.net.client;

import java.io.IOException;
import java.lang.reflect.Method;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.qq.tars.net.client.ticket.Ticket;
import com.qq.tars.net.client.ticket.TicketManager;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.net.core.Request.InvokeStatus;
import com.qq.tars.net.core.Session.SessionStatus;
import com.qq.tars.net.core.nio.SelectorManager;
import com.qq.tars.net.core.nio.TCPSession;
import com.qq.tars.net.core.nio.UDPSession;
import com.qq.tars.net.protocol.ProtocolFactory;

public final class Client<REQ extends Request, RES extends Response> {

    private Session session = null;

    private String host = null;

    private int port = -1;

    private SelectorManager selectorManager = null;

    private int minPoolSize = 5;

    private int maxPoolSize = 10;

    private int queueSize = 20000;

    private long readTimeout = 1000;

    private long connectTimeout = 200;

    private boolean keepAlive = true;

    private int selectorPoolSize = 2;

    private boolean udpMode = false;

    private int tc = INVALID_TRAFFIC_CLASS_VALUE;

    private int bufferSize = 1024 * 4;

    private boolean tcpNoDelay = false;

    private static final int INVALID_TRAFFIC_CLASS_VALUE = -1;

    protected static final ConcurrentHashMap<String /* Protocol Factory Name */, SelectorManager> selectors = new ConcurrentHashMap<String, SelectorManager>();

    public Client(String ip, int port, ProtocolFactory protocolFactory) {
        this(ip, port, protocolFactory, false);
    }

    public Client(String ip, int port, ProtocolFactory protocolFactory, boolean udpMode) {
        this(ip, port, protocolFactory, null, udpMode);
    }

    public Client(String ip, int port, ProtocolFactory protocolFactory, ThreadPoolExecutor threadPoolExecutor,
                  boolean udpMode) {
        try {
            this.host = ip;
            this.port = port;
            this.udpMode = udpMode;
            this.selectorManager = initSelectorManager(protocolFactory, threadPoolExecutor);
            reConnect();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    protected void ensureConnected() throws IOException {
        if (isNotConnected()) reConnect();
    }

    protected synchronized void reConnect() throws IOException {
        //0. Don't send connect request if it is connecting.
        if (isNotConnected()) {
            SocketAddress server = new InetSocketAddress(this.host, this.port);
            SelectableChannel channel = null;
            Session session2 = null;
            int event;

            if (this.udpMode) {
                //1. Create socket channel
                channel = DatagramChannel.open();
                channel.configureBlocking(false);

                //2. Create NioSession for each client connection
                session2 = new UDPSession(this.selectorManager);
                ((UDPSession) session2).setBufferSize(bufferSize);
                ((UDPSession) session2).setTarget(server);
                event = SelectionKey.OP_READ;
                session2.setStatus(SessionStatus.CLIENT_CONNECTED);
            } else {
                //1. Create socket channel
                channel = SocketChannel.open();
                channel.configureBlocking(false);
                try {
                    if (this.tc != INVALID_TRAFFIC_CLASS_VALUE) ((SocketChannel) channel).socket().setTrafficClass(this.tc);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                ((SocketChannel) channel).connect(server);

                //2. Create NioSession for each client connection
                session2 = new TCPSession(this.selectorManager);
                ((TCPSession) session2).setTcpNoDelay(this.tcpNoDelay);
                event = SelectionKey.OP_CONNECT;
            }

            session2.setChannel(channel);
            session2.setKeepAlive(selectorManager.isKeepAlive());

            //3. Register event
            this.selectorManager.nextReactor().registerChannel(channel, event, session2);

            if (!this.udpMode) {
                //4. Wait to connect
                if (!session2.waitToConnect(this.connectTimeout)) {
                    session2.asyncClose();
                    throw new IOException("connect timed out to " + this.host + ":" + this.port);
                }

                //5. Handle exception
                if (session2.getStatus() == SessionStatus.NOT_CONNECTED) {
                    session2.asyncClose();
                    throw new IOException("connect failed to " + this.host + ":" + this.port);
                } else if (session2.getStatus() == SessionStatus.CLOSED) { //Already closed
                    throw new IOException("connect failed to " + this.host + ":" + this.port); //Please see stderr.log for more details.
                }
            }

            this.session = session2;
        }
    }

    public void setBufferSize(int bufferSize) {
        this.bufferSize = bufferSize;
    }

    public RES invokeWithSync(REQ request) throws IOException {
        Ticket<RES> ticket = null;
        RES response = null;

        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.SYNC_CALL);
            ticket = TicketManager.createTicket(request, session, this.getReadTimeout());
            Session session2 = session;
            session2.write(request);
            if (!ticket.await(this.readTimeout, TimeUnit.MILLISECONDS)) {
                if (session2 != null && session2.getStatus() != SessionStatus.CLIENT_CONNECTED) {
                    throw new IOException("Connection reset by peer");
                } else {
                    throw new IOException("The operation has timed out.");
                }
            }
            response = ticket.response();
            if (response == null) throw new IOException("The operation is failed.");
            return response;
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            if (ticket != null) TicketManager.removeTicket(ticket.getTicketNumber());
        }

        return response;
    }

    public void invokeWithAsync(REQ request, Callback<RES> callback) throws IOException {
        Ticket<RES> ticket = null;

        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.ASYNC_CALL);
            ticket = TicketManager.createTicket(request, session, this.getReadTimeout(), callback);
            session.write(request);
        } catch (Exception ex) {
            if (ticket != null) TicketManager.removeTicket(ticket.getTicketNumber());
            throw new IOException("invokeWithAsync error:", ex);
        }
    }

    @SuppressWarnings("unchecked")
    public Future<RES> invokeWithFuture(REQ request) throws IOException {
        Ticket<Response> ticket = null;

        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.FUTURE_CALL);
            ticket = TicketManager.createTicket(request, session, this.getReadTimeout());
            session.write(request);
            return new FutureImpl(ticket);
        } catch (IOException ex) {
            if (ticket != null) TicketManager.removeTicket(ticket.getTicketNumber());
            throw ex;
        }
    }

    public synchronized void shutdown() throws IOException {
        this.session.asyncClose();
    }

    public Session getIoSession() {
        return this.session;
    }

    public long getReadTimeout() {
        return readTimeout;
    }

    public void setReadTimeout(long readTimeout) {
        this.readTimeout = readTimeout;
    }

    public int getTrafficClass() {
        return tc;
    }

    public void setTrafficClass(int tc) {
        if (this.session != null && this.session instanceof TCPSession) {
            try {
                ((SocketChannel) ((TCPSession) this.session).getChannel()).socket().setTrafficClass(tc);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }

        this.tc = tc;

    }

    public void setTcpNoDelay(boolean on) {
        this.tcpNoDelay = on;

        if (this.session != null && this.session instanceof TCPSession) {
            try {
                ((SocketChannel) ((TCPSession) this.session).getChannel()).socket().setTcpNoDelay(on);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    public long getConnectTimeout() {
        return connectTimeout;
    }

    public void setConnectTimeout(long connectTimeout) {
        this.connectTimeout = connectTimeout;
    }

    public String getAddress() {
        return host + ":" + port;
    }

    private boolean isNotConnected() {
        if (session == null || session.getStatus() == SessionStatus.CLOSED || session.getStatus() == SessionStatus.NOT_CONNECTED) {
            return true;
        }

        return false;
    }

    private SelectorManager initSelectorManager(ProtocolFactory protocolFactory, ThreadPoolExecutor threadPoolExecutor) throws IOException {
        String protocolFactoryName = resolveProtocolFactoryName(protocolFactory);
        String currentContextIdentity = resolveCurrentContextIdentity();
        String key = protocolFactoryName + "-" + currentContextIdentity;
        ThreadPoolExecutor threadPool = null;

        SelectorManager selector = selectors.get(key);

        if (selector == null) {
            synchronized (selectors) {
                selector = selectors.get(key);
                if (selector == null) { //Double-Checked Locking
                    selectorPoolSize = convertInt(System.getProperty("com.qq.nami.client.selectorPoolSize"), selectorPoolSize);
                    minPoolSize = convertInt(System.getProperty("com.qq.nami.client.minPoolSize"), minPoolSize);
                    maxPoolSize = convertInt(System.getProperty("com.qq.nami.client.maxPoolSize"), maxPoolSize);
                    queueSize = convertInt(System.getProperty("com.qq.nami.client.queueSize"), queueSize);

                    if (threadPoolExecutor == null) {
                        threadPool = new ThreadPoolExecutor(minPoolSize, maxPoolSize, 120, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(queueSize));
                    } else {
                        threadPool = threadPoolExecutor;
                    }
                    selector = new SelectorManager(selectorPoolSize, protocolFactory, threadPool, null, keepAlive, "taserver-proxy", this.udpMode);
                    selector.start();
                    selectors.put(key, selector);
                }
            }
        }

        return selector;
    }

    private String resolveProtocolFactoryName(ProtocolFactory protocolFactory) {
        String protocolFactoryName = null;

        try {
            protocolFactoryName = protocolFactory.getClass().getName();
            Method method = protocolFactory.getClass().getMethod("getProtocolName", new Class[] {});
            protocolFactoryName = (String) method.invoke(protocolFactory, new Object[] {});
        } catch (Exception ex) {
        }

        return protocolFactoryName;
    }

    private String resolveCurrentContextIdentity() {
        String contextIdentity = "";

        Object slefClassLoader = this.getClass().getClassLoader();
        Object contextClassLoader = Thread.currentThread().getContextClassLoader();

        if (slefClassLoader != null && contextClassLoader != null && slefClassLoader != contextClassLoader) {
            contextIdentity = contextClassLoader.toString();
        }

        return contextIdentity;
    }

    private int convertInt(String str, int defaults) {
        if (str == null) {
            return defaults;
        }
        try {
            return Integer.parseInt(str);
        } catch (Exception e) {
            return defaults;
        }
    }
}
