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
import java.nio.channels.Selector;
import java.util.concurrent.Executor;
import java.util.concurrent.atomic.AtomicLong;

import com.qq.tars.net.core.Processor;
import com.qq.tars.net.protocol.ProtocolFactory;

public final class SelectorManager {

    private final AtomicLong sets = new AtomicLong(0);

    private final Reactor[] reactorSet;

    private ProtocolFactory protocolFactory = null;

    private Executor threadPool = null;

    private Processor processor = null;

    private final int selectorPoolSize;

    private volatile boolean started;

    private boolean keepAlive;

    private boolean isTcpNoDelay = false;

    public SelectorManager(int selectorPoolSize, ProtocolFactory protocolFactory, Executor threadPool,
                           Processor processor, boolean keepAlive, String reactorNamePrefix) throws IOException {
        this(selectorPoolSize, protocolFactory, threadPool, processor, keepAlive, reactorNamePrefix, false);
    }

    public SelectorManager(int selectorPoolSize, ProtocolFactory protocolFactory, Executor threadPool,
                           Processor processor, boolean keepAlive, String reactorNamePrefix, boolean udpMode) throws IOException {
        if (udpMode) selectorPoolSize = 1;

        this.selectorPoolSize = selectorPoolSize;
        this.protocolFactory = protocolFactory;
        this.threadPool = threadPool;
        this.processor = processor;
        this.keepAlive = keepAlive;

        reactorSet = new Reactor[selectorPoolSize];

        for (int i = 0; i < reactorSet.length; i++) {
            reactorSet[i] = new Reactor(this, reactorNamePrefix + "-" + protocolFactory.getClass().getSimpleName().toLowerCase() + "-" + String.valueOf(i), udpMode);
        }
    }

    public synchronized void start() {
        if (this.started) {
            return;
        }

        this.started = true;
        for (Reactor reactor : this.reactorSet) {
            reactor.start();
        }
    }

    public synchronized void stop() {
        if (!this.started) return;

        this.started = false;
        for (Reactor reactor : this.reactorSet) {
            reactor.interrupt();
        }
    }

    public Reactor getReactor(int index) {
        if (index < 0 || index > this.reactorSet.length - 1) {
            throw new IllegalArgumentException("failed to get one reactor thread...");
        }

        return this.reactorSet[index];
    }

    public final Reactor nextReactor() {
        return this.reactorSet[(int) (this.sets.incrementAndGet() % this.selectorPoolSize)];
    }

    public final Reactor getReactor(SelectionKey key) {
        Reactor reactor = null;
        Selector selector = key.selector();

        for (int i = 0; i < this.reactorSet.length; i++) {
            reactor = this.reactorSet[i];
            if (reactor.selector == selector) {
                return reactor;
            }
        }

        return null;
    }

    public ProtocolFactory getProtocolFactory() {
        return protocolFactory;
    }

    public void setProtocolFactory(ProtocolFactory protocolFactory) {
        this.protocolFactory = protocolFactory;
    }

    public Processor getProcessor() {
        return processor;
    }

    public void setProcessor(Processor processor) {
        this.processor = processor;
    }

    public Executor getThreadPool() {
        return threadPool;
    }

    public void setThreadPool(Executor threadPool) {
        this.threadPool = threadPool;
    }

    public boolean isKeepAlive() {
        return keepAlive;
    }

    public boolean isTcpNoDelay() {
        return isTcpNoDelay;
    }

    public void setTcpNoDelay(boolean on) {
        this.isTcpNoDelay = on;
    }
}
