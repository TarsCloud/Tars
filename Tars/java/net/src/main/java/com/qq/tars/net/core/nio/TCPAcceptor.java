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
import java.nio.channels.SelectionKey;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

import com.qq.tars.net.core.SessionManager;
import com.qq.tars.net.core.Session.SessionStatus;
import com.qq.tars.net.util.Utils;

public class TCPAcceptor extends Acceptor {

    public TCPAcceptor(SelectorManager selectorManager) {
        super(selectorManager);
    }

    public void handleConnectEvent(SelectionKey key) throws IOException {
        //1. Get the client channel
        SocketChannel client = (SocketChannel) key.channel();

        //2. Set the session status
        TCPSession session = (TCPSession) key.attachment();
        if (session == null) throw new RuntimeException("The session is null when connecting to ...");

        //3. Connect to server
        try {
            client.finishConnect();
            key.interestOps(SelectionKey.OP_READ);
            session.setStatus(SessionStatus.CLIENT_CONNECTED);
        } finally {
            session.finishConnect();
        }
    }

    public void handleAcceptEvent(SelectionKey key) throws IOException {
        //1. Accept TCP request
        ServerSocketChannel server = (ServerSocketChannel) key.channel();
        SocketChannel channel = server.accept();
        channel.socket().setTcpNoDelay(selectorManager.isTcpNoDelay());
        channel.configureBlocking(false);
        Utils.setQosFlag(channel.socket());

        //2. Create NioSession for each TCP connection
        TCPSession session = new TCPSession(selectorManager);
        session.setChannel(channel);
        session.setStatus(SessionStatus.SERVER_CONNECTED);
        session.setKeepAlive(selectorManager.isKeepAlive());
        session.setTcpNoDelay(selectorManager.isTcpNoDelay());

        //3. Register session
        SessionManager.getSessionManager().registerSession(session);

        //4. Register channel with the specified session
        selectorManager.nextReactor().registerChannel(channel, SelectionKey.OP_READ, session);
    }

    public void handleReadEvent(SelectionKey key) throws IOException {
        TCPSession session = (TCPSession) key.attachment();
        if (session == null) throw new RuntimeException("The session is null when reading data...");
        session.read();
    }

    public void handleWriteEvent(SelectionKey key) throws IOException {
        TCPSession session = (TCPSession) key.attachment();
        if (session == null) throw new RuntimeException("The session is null when writing data...");
        session.doWrite();
    }
}
