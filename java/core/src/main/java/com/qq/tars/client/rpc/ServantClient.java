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

package com.qq.tars.client.rpc;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.net.client.Callback;
import com.qq.tars.net.client.FutureImpl;
import com.qq.tars.net.client.ticket.Ticket;
import com.qq.tars.net.client.ticket.TicketManager;
import com.qq.tars.net.core.Session;
import com.qq.tars.net.core.Request.InvokeStatus;
import com.qq.tars.net.core.Session.SessionStatus;
import com.qq.tars.net.core.nio.SelectorManager;
import com.qq.tars.net.core.nio.TCPSession;
import com.qq.tars.net.core.nio.UDPSession;
import com.qq.tars.rpc.exc.NotConnectedException;
import com.qq.tars.rpc.exc.TimeoutException;
import com.qq.tars.rpc.protocol.ServantRequest;
import com.qq.tars.rpc.protocol.ServantResponse;

public class ServantClient {

    private Session session = null;
    private String host = null;
    private int port = -1;
    private SelectorManager selectorManager = null;
    private long asyncTimeout = 1000;
    private long syncTimeout = 1000;
    private long connectTimeout = 200;
    private boolean udpMode = false;
    private int tc = INVALID_TRAFFIC_CLASS_VALUE;
    private int bufferSize = 1024 * 4;
    private boolean tcpNoDelay = false;
    private static final int INVALID_TRAFFIC_CLASS_VALUE = -1;

    public ServantClient(String ip, int port, SelectorManager selectorManager, boolean udpMode) {
        this.host = ip;
        this.port = port;
        this.udpMode = udpMode;
        this.selectorManager = selectorManager;
    }

    protected synchronized void reConnect() throws IOException {
        if (isNotConnected()) {
            SocketAddress server = new InetSocketAddress(this.host, this.port);
            SelectableChannel channel = null;
            Session temp = null;
            int event;

            if (this.udpMode) {
                channel = DatagramChannel.open();
                channel.configureBlocking(false);

                temp = new UDPSession(this.selectorManager);
                ((UDPSession) temp).setBufferSize(bufferSize);
                ((UDPSession) temp).setTarget(server);
                event = SelectionKey.OP_READ;
                temp.setStatus(SessionStatus.CLIENT_CONNECTED);
            } else {
                channel = SocketChannel.open();
                channel.configureBlocking(false);
                try {
                    if (this.tc != INVALID_TRAFFIC_CLASS_VALUE) {
                        ((SocketChannel) channel).socket().setTrafficClass(this.tc);
                    }
                } catch (Exception ex) {
                    ClientLogger.getLogger().error(ex.getLocalizedMessage());
                }
                ((SocketChannel) channel).connect(server);

                temp = new TCPSession(this.selectorManager);
                ((TCPSession) temp).setTcpNoDelay(this.tcpNoDelay);
                event = SelectionKey.OP_CONNECT;
            }

            temp.setChannel(channel);
            temp.setKeepAlive(selectorManager.isKeepAlive());

            this.selectorManager.nextReactor().registerChannel(channel, event, temp);

            if (!this.udpMode) {
                if (!temp.waitToConnect(this.connectTimeout)) {
                    temp.asyncClose();
                    throw new TimeoutException("connect " + this.connectTimeout + "ms timed out to " + this.getAddress());
                }

                if (temp.getStatus() == SessionStatus.NOT_CONNECTED) {
                    temp.asyncClose();
                    throw new NotConnectedException("connect failed to " + this.getAddress());
                } else if (temp.getStatus() == SessionStatus.CLOSED) {
                    throw new NotConnectedException("connect failed to " + this.getAddress());
                }
            }
            this.session = temp;
        }
    }

    public void ensureConnected() throws IOException {
        if (isNotConnected()) {
            reConnect();
        }
    }

    public <T extends ServantResponse> T invokeWithSync(ServantRequest request) throws IOException {
        Ticket<T> ticket = null;
        T response = null;
        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.SYNC_CALL);
            ticket = TicketManager.createTicket(request, session, this.syncTimeout);

            Session current = session;
            current.write(request);
            if (!ticket.await(this.syncTimeout, TimeUnit.MILLISECONDS)) {
                if (current != null && current.getStatus() != SessionStatus.CLIENT_CONNECTED) {
                    throw new IOException("Connection reset by peer|" + this.getAddress());
                } else {
                    throw new TimeoutException("the operation has timeout, " + this.syncTimeout + "ms|" + this.getAddress());
                }
            }
            response = ticket.response();
            if (response == null) {
                throw new IOException("the operation is failed.");
            }
            return response;
        } catch (InterruptedException e) {
            ClientLogger.getLogger().error(e.getLocalizedMessage());
        } finally {
            if (ticket != null) {
                TicketManager.removeTicket(ticket.getTicketNumber());
            }
        }
        return response;
    }

    public <T extends ServantResponse> void invokeWithAsync(ServantRequest request, Callback<T> callback) throws IOException {
        Ticket<T> ticket = null;

        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.ASYNC_CALL);
            ticket = TicketManager.createTicket(request, session, this.asyncTimeout, callback);

            Session current = session;
            current.write(request);
        } catch (Exception ex) {
            if (ticket != null) {
                TicketManager.removeTicket(ticket.getTicketNumber());
            }
            throw new IOException("error occurred on invoker with async", ex);
        }
    }

    public <T extends ServantResponse> Future<T> invokeWithFuture(ServantRequest request) throws IOException {
        Ticket<T> ticket = null;
        try {
            ensureConnected();
            request.setInvokeStatus(InvokeStatus.FUTURE_CALL);
            ticket = TicketManager.createTicket(request, session, this.syncTimeout);

            Session current = session;
            current.write(request);
            return new FutureImpl<T>(ticket);
        } catch (Exception ex) {
            if (ticket != null) {
                TicketManager.removeTicket(ticket.getTicketNumber());
            }
            throw new IOException("error occurred on invoker with future", ex);
        }
    }

    private synchronized void shutdown() throws IOException {
        if (this.session != null) {
            this.session.asyncClose();
        }
    }

    public void setBufferSize(int bufferSize) {
        this.bufferSize = bufferSize;
    }

    public Session getIoSession() {
        return this.session;
    }

    public int getTrafficClass() {
        return tc;
    }

    public void setTrafficClass(int tc) {
        if (this.session != null && this.session instanceof TCPSession) {
            try {
                ((SocketChannel) ((TCPSession) this.session).getChannel()).socket().setTrafficClass(tc);
            } catch (Exception ex) {
                ClientLogger.getLogger().error(ex.getLocalizedMessage());
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
                ClientLogger.getLogger().error(ex.getLocalizedMessage());
            }
        }
    }

    public long getConnectTimeout() {
        return connectTimeout;
    }

    public void setConnectTimeout(long connectTimeout) {
        this.connectTimeout = connectTimeout;
    }

    public long getSyncTimeout() {
        return syncTimeout;
    }

    public void setSyncTimeout(long syncTimeout) {
        this.syncTimeout = syncTimeout;
    }

    public void setAsyncTimeout(long asyncTimeout) {
        this.asyncTimeout = asyncTimeout;
    }

    public void close() throws IOException {
        this.shutdown();
    }

    private boolean isNotConnected() {
        return session == null || session.getStatus() == SessionStatus.CLOSED || session.getStatus() == SessionStatus.NOT_CONNECTED;
    }

    public String getAddress() {
        return host + ":" + port;
    }

    public String toString() {
        return "ServantClient [client=" + getAddress() + "]";
    }
}
