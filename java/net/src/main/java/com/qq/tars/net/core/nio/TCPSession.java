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

package com.qq.tars.net.core.nio;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.tars.net.client.ticket.Ticket;
import com.qq.tars.net.core.IoBuffer;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.net.core.SessionManager;
import com.qq.tars.net.protocol.ProtocolException;
import com.qq.tars.net.protocol.ProtocolFactory;

public class TCPSession extends Session {

    private SelectionKey key = null;

    private SelectableChannel channel = null;

    private SelectorManager selectorManager = null;

    private int bufferSize = 1024 * 4;

    private IoBuffer readBuffer = null;

    private boolean tcpNoDelay = false;

    private Queue<ByteBuffer> queue = new LinkedBlockingQueue<ByteBuffer>(1024 * 8);

    private static final AtomicInteger hashCodeGenerator = new AtomicInteger();

    private int hashCode = 0;

    public TCPSession(SelectorManager selectorManager) {
        this.selectorManager = selectorManager;
        this.hashCode = hashCodeGenerator.incrementAndGet();
    }

    public void asyncClose() throws IOException {
        if (this.key == null) return; //Already closed if the key is null.

        Reactor reactor = selectorManager.getReactor(this.key);

        if (reactor != null) {
            reactor.unRegisterChannel(this);
        } else {
            throw new IOException("failed to find the selector for this session.");
        }
    }

    public void close() throws IOException {
        if (this.status == SessionStatus.CLOSED) return;

        this.status = SessionStatus.CLOSED;
        try {
            if (channel != null) channel.close();
            if (key != null) key.cancel();
        }catch(IOException e){
            /*ignore IOException ,when client first clost connection,it cannot be fast unregisterSession*/
        }

        this.key = null;
        this.channel = null;

        SessionManager.getSessionManager().unregisterSession(this);
    }

    protected void read() throws IOException {
        int ret = readChannel();

        if (this.status == SessionStatus.CLIENT_CONNECTED) {
            readResponse();
        } else if (this.status == SessionStatus.SERVER_CONNECTED) {
            readRequest();
        } else {
            throw new IllegalStateException("The current session status is invalid. [status:" + this.status + "]");
        }

        if (ret < 0) {
            close();
            return;
        }
    }

    public void readRequest() throws IOException {
        Request request = null;
        IoBuffer tempBuffer = null;

        try {
            tempBuffer = readBuffer.duplicate().flip();

            while (true) {
                tempBuffer.mark();

                if (tempBuffer.remaining() > 0) {
                    request = selectorManager.getProtocolFactory().getDecoder().decodeRequest(tempBuffer, this);
                } else {
                    request = null;
                }

                if (request != null) {
                    try {
                        request.resetBornTime();
                        selectorManager.getThreadPool().execute(new WorkThread(request, selectorManager));
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                } else {
                    tempBuffer.reset();
                    readBuffer = resetIoBuffer(tempBuffer);
                    break;
                }
            }
        } catch (ProtocolException ex) {
            close();
            ex.printStackTrace();
        }
    }

    public void readResponse() throws IOException {
        Response response = null;
        IoBuffer tempBuffer = null;

        try {
            tempBuffer = readBuffer.duplicate().flip();

            while (true) {
                tempBuffer.mark();

                if (tempBuffer.remaining() > 0) {
                    response = selectorManager.getProtocolFactory().getDecoder().decodeResponse(tempBuffer, this);
                } else {
                    response = null;
                }

                if (response != null) {
                    try {
                        if (response.getTicketNumber() == Ticket.DEFAULT_TICKET_NUMBER) response.setTicketNumber(response.getSession().hashCode());
                        selectorManager.getThreadPool().execute(new WorkThread(response, selectorManager));
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                } else {
                    tempBuffer.reset();
                    readBuffer = resetIoBuffer(tempBuffer);
                    break;
                }
            }

        } catch (ProtocolException ex) {
            close();
            ex.printStackTrace();
        }
    }

    public void write(Request request) throws IOException {
        try {
            IoBuffer buffer = selectorManager.getProtocolFactory().getEncoder().encodeRequest(request, this);
            write(buffer);
        } catch (ProtocolException ex) {
            throw new IOException("protocol error:", ex);
        }
    }

    public void write(Response response) throws IOException {
        try {
            IoBuffer buffer = selectorManager.getProtocolFactory().getEncoder().encodeResponse(response, this);
            write(buffer);
        } catch (ProtocolException ex) {
            throw new IOException("protocol error:", ex);
        }
    }

    public String getRemoteIp() {
        if (this.status != SessionStatus.CLOSED) {
            return ((SocketChannel) this.channel).socket().getInetAddress().getHostAddress();
        }

        return null;
    }

    public int getRemotePort() {
        if (this.status != SessionStatus.CLOSED) {
            return ((SocketChannel) this.channel).socket().getPort();
        }

        return -1;
    }

    public void accept() throws IOException {
    }

    protected void write(IoBuffer buffer) throws IOException {
        if (buffer == null) return;

        if (channel == null || key == null) throw new IOException("Connection is closed");

        if (!this.queue.offer(buffer.buf())) {
            throw new IOException("The session queue is full. [ queue size:" + queue.size() + " ]");
        }

        if (key != null) {
            key.interestOps(key.interestOps() | SelectionKey.OP_WRITE);
            key.selector().wakeup();
        }
    }

    protected synchronized int doWrite() throws IOException {
        int writeBytes = 0;

        while (true) {
            ByteBuffer wBuf = queue.peek();

            if (wBuf == null) {
                key.interestOps(SelectionKey.OP_READ);

                if (queue.peek() != null) {
                    key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
                }

                key.selector().wakeup();
                break;
            }

            int bytesWritten = ((SocketChannel) channel).write(wBuf);

            if (bytesWritten == 0 && wBuf.remaining() > 0) // Socket buffer is full.
            {
                key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
                key.selector().wakeup();
                break;
            }

            if (wBuf.remaining() == 0) {
                writeBytes++;
                queue.remove();
                continue;
            } else {
                return -1;
            }
        }

        if (!isKeepAlive()) close();

        return writeBytes;
    }

    protected IoBuffer resetIoBuffer(IoBuffer buffer) {
        IoBuffer newBuffer = null;

        if (buffer != null && buffer.remaining() > 0) {
            int len = buffer.remaining();
            byte[] bb = new byte[len];
            buffer.get(bb);
            newBuffer = IoBuffer.wrap(bb);
            newBuffer.position(len);
        }

        return newBuffer;
    }

    public SelectionKey getKey() {
        return key;
    }

    public void setKey(SelectionKey key) {
        this.key = key;
    }

    public boolean isTcpNoDelay() {
        return tcpNoDelay;
    }

    public void setTcpNoDelay(boolean on) {
        this.tcpNoDelay = on;
    }

    public SelectableChannel getChannel() {
        return channel;
    }

    public void setChannel(SelectableChannel channel) {
        this.channel = channel;
    }

    public ProtocolFactory getProtocolFactory() {
        return this.selectorManager.getProtocolFactory();
    }

    private int readChannel() throws IOException {
        int readBytes = 0, ret = 0;

        ByteBuffer data = ByteBuffer.allocate(1024 * 2);

        if (readBuffer == null) {
            readBuffer = IoBuffer.allocate(bufferSize);
        }

        while ((ret = ((SocketChannel) channel).read(data)) > 0) {
            data.flip();
            readBytes += data.remaining();
            readBuffer.put(data.array(), data.position(), data.remaining());
            data.clear();
        }

        return ret < 0 ? ret : readBytes;
    }

    @Override
    public int hashCode() {
        return this.hashCode;
    }
}
