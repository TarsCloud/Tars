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

package com.qq.tars.net.core;

import java.io.IOException;
import java.nio.channels.SelectableChannel;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import com.qq.tars.net.protocol.ProtocolFactory;

public abstract class Session {

    protected SessionStatus status = SessionStatus.NOT_CONNECTED;

    protected long lastOperationTime = System.currentTimeMillis();

    protected boolean keepAlive = true;

    protected CountDownLatch connectLatch = new CountDownLatch(1);

    public static enum SessionStatus {
        NOT_CONNECTED, CLIENT_CONNECTED, SERVER_CONNECTED, CLOSED
    }

    public abstract void close() throws IOException;

    public abstract void asyncClose() throws IOException;

    protected abstract void read() throws IOException;

    protected abstract void accept() throws IOException;

    public abstract void write(Request request) throws IOException;

    public abstract void write(Response response) throws IOException;

    public abstract void setChannel(SelectableChannel channel) throws IOException;

    public abstract String getRemoteIp();

    public abstract int getRemotePort();

    public abstract ProtocolFactory getProtocolFactory();

    public SessionStatus getStatus() {
        return status;
    }

    public void setStatus(SessionStatus status) {
        this.status = status;
    }

    public void finishConnect() {
        this.connectLatch.countDown();
    }

    public boolean waitToConnect(long connectTimeout) {
        try {
            return this.connectLatch.await(connectTimeout, TimeUnit.MILLISECONDS);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return false;
    }

    public void updateLastOperationTime() {
        this.lastOperationTime = System.currentTimeMillis();
    }

    public long getLastOperationTime() {
        return this.lastOperationTime;
    }

    public void setKeepAlive(boolean keepAlive) {
        this.keepAlive = keepAlive;
    }

    public boolean isKeepAlive() {
        return this.keepAlive;
    }
}
