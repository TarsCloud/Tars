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

import com.qq.tars.net.core.Session.SessionStatus;

public class UDPAcceptor extends Acceptor {

    private int bufferSize;

    public UDPAcceptor(SelectorManager selectorManager) {
        super(selectorManager);
        String s = System.getProperty("com.qq.nami.server.udp.bufferSize", "4096");
        try {
            bufferSize = Integer.parseInt(s);
        } catch (NumberFormatException e) {
            bufferSize = 4096;
        }
    }

    @Override
    public void handleConnectEvent(SelectionKey key) throws IOException {
        throw new IllegalStateException("UDP can't handle OP_CONNECT event");
    }

    @Override
    public void handleAcceptEvent(SelectionKey key) throws IOException {
        throw new IllegalStateException("UDP can't handle OP_ACCEPT event");
    }

    @Override
    public void handleReadEvent(SelectionKey key) throws IOException {
        UDPSession session = null;

        if (key.attachment() == null) // server side
        {
            session = new UDPSession(this.selectorManager);
            session.setBufferSize(bufferSize);
            session.setChannel(key.channel());
            session.setStatus(SessionStatus.SERVER_CONNECTED);
        } else // client side
        {
            session = (UDPSession) key.attachment();
        }

        session.read();

    }

    @Override
    public void handleWriteEvent(SelectionKey key) throws IOException {
    }
}
