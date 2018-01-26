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
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

import com.qq.tars.net.core.Session;

public final class Reactor extends Thread {

    protected volatile Selector selector = null;

    private volatile boolean crashed = false;

    private final Queue<Object[]> register = new LinkedBlockingQueue<Object[]>();

    private final Queue<Session> unregister = new LinkedBlockingQueue<Session>();

    private Acceptor acceptor = null;

    public Reactor(SelectorManager selectorManager, String name) throws IOException {
        this(selectorManager, name, false);
    }

    public Reactor(SelectorManager selectorManager, String name, boolean udpMode) throws IOException {
        super(name);

        if (udpMode) {
            this.acceptor = new UDPAcceptor(selectorManager);
        } else {
            this.acceptor = new TCPAcceptor(selectorManager);
        }

        this.selector = Selector.open();
    }

    public void registerChannel(SelectableChannel channel, int ops) throws IOException {
        registerChannel(channel, ops, null);
    }

    public void unRegisterChannel(Session session) {
        if (this.unregister.contains(session)) return;
        this.unregister.add(session);
        this.selector.wakeup();
    }

    public void registerChannel(SelectableChannel channel, int ops, Object attachment) throws IOException {
        if (crashed) {
            throw new IOException("The reactor thread carsh.... ");
        }

        if (Thread.currentThread() == this) {
            SelectionKey key = channel.register(this.selector, ops, attachment);

            if (attachment instanceof TCPSession) {
                ((TCPSession) attachment).setKey(key);
            }
        } else {
            this.register.offer(new Object[] { channel, ops, attachment });
            this.selector.wakeup();
        }
    }

    public void run() {
        try {
            while (!Thread.interrupted()) {
                selector.select();
                processRegister();
                Iterator<SelectionKey> iter = selector.selectedKeys().iterator();
                while (iter.hasNext()) {
                    SelectionKey key = iter.next();
                    iter.remove();

                    if (!key.isValid()) continue;

                    try {
                        //1. Update the last operation time
                        if (key.attachment() != null && key.attachment() instanceof Session) {
                            ((Session) key.attachment()).updateLastOperationTime();
                        }

                        //2. Dispatch I/O event
                        dispatchEvent(key);

                    } catch (Throwable ex) {
                        disConnectWithException(key, ex);
                    }
                }
                processUnRegister();
            }
        } catch (Throwable e) {
            crashed = true;
            e.printStackTrace();
        } finally {
            try {
                selector.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void disConnectWithException(final SelectionKey key, final Throwable ex) {
        try {
            Session session = (Session) key.attachment();

            if (session == null) {
                if (key.channel() instanceof SocketChannel) key.channel().close();
            } else {
                session.close();
            }

            ex.printStackTrace();
        } catch (Throwable e2) {
            ex.printStackTrace();
        }
    }

    private void processUnRegister() {
        Session session = null;

        while ((session = this.unregister.poll()) != null) {
            try {
                ((TCPSession) session).close();
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void processRegister() {
        Object[] object = null;
        SelectableChannel channel = null;
        int ops = -1;
        Object attachment = null;

        while ((object = this.register.poll()) != null) {
            try {
                channel = (SelectableChannel) object[0];
                ops = (Integer) object[1];
                attachment = object[2];

                if (!channel.isOpen()) continue;

                SelectionKey key = channel.register(this.selector, ops, attachment);

                if (attachment instanceof TCPSession) {
                    ((TCPSession) attachment).setKey(key);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void dispatchEvent(final SelectionKey key) throws IOException {
        if (key.isConnectable()) {
            acceptor.handleConnectEvent(key);
        } else if (key.isAcceptable()) {
            acceptor.handleAcceptEvent(key);
        } else if (key.isReadable()) {
            acceptor.handleReadEvent(key);
        } else if (key.isValid() && key.isWritable()) {
            acceptor.handleWriteEvent(key);
        }
    }
}
