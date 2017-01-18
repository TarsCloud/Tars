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
import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.ServerSocketChannel;
import java.util.Properties;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.qq.tars.net.core.nio.SelectorManager;
import com.qq.tars.net.protocol.ProtocolFactory;
import com.qq.tars.net.util.Utils;

public abstract class Server {

    private String host = null;

    private int port = DEFAULT_MAIN_PORT;

    private static final int DEFAULT_MAIN_PORT = 9000;

    private ProtocolFactory protocolFactory = null;

    private SelectorManager selectorManager = null;

    private Processor processor = null;

    private Executor threadPool = null;

    private Properties props = new Properties();

    private int minPoolSize = 10;

    private int maxPoolSize = 128;

    private boolean keepAlive = true;

    private boolean udpMode = false;

    public enum ServerConf {
        SERVER_LOCAL_IP, SERVER_BIND_PORT, SERVER_THREAD_POOL_MAX, SERVER_THREAD_POOL_MIN, SERVER_KEEP_ALIVE
    }

    public Server(String ip, int port) throws IOException {
        this(ip, port, null, null, false);
    }

    public Server(String ip, int port, boolean udpMode) throws IOException {
        this(ip, port, null, null, udpMode);
    }

    public Server(String ip, int port, ProtocolFactory protocolFactory, Processor processor, boolean udpMode) throws IOException {
        initServerConfig();

        this.host = ip;
        this.port = port;
        this.udpMode = udpMode;
        this.protocolFactory = protocolFactory;
        this.processor = processor;
        this.threadPool = new ThreadPoolExecutor(minPoolSize, maxPoolSize, 120, TimeUnit.SECONDS, new ArrayBlockingQueue<Runnable>(20000));
        this.selectorManager = new SelectorManager(Utils.getSelectorPoolSize(), protocolFactory, threadPool, processor, keepAlive, "server", this.udpMode);
    }

    public void startUp(String args[]) throws Exception {
        startNIOServer();

        if (!this.udpMode) SessionManager.getSessionManager().start();
    }

    protected void startNIOServer() throws Exception {
        SelectableChannel server = null;
        int interestKey;

        //1. Start reactor service
        selectorManager.start();

        //2. Start server on the specified port
        if (this.udpMode) {
            server = DatagramChannel.open();
            ((DatagramChannel) server).socket().bind(new InetSocketAddress(host, port));
            interestKey = SelectionKey.OP_READ;
        } else {
            server = ServerSocketChannel.open();
            ((ServerSocketChannel) server).socket().bind(new InetSocketAddress(host, port), 1024);
            interestKey = SelectionKey.OP_ACCEPT;

        }

        server.configureBlocking(false);

        //3. Choose one reactor to handle NIO event
        selectorManager.getReactor(0).registerChannel(server, interestKey);
        System.out.println("INFO: NAMI Server started on port " + String.valueOf(port) + "...");

    }

    protected void initServerConfig() {
        try {
            props.load(Utils.loadConfigFile("server.properties", Server.class));
            minPoolSize = Integer.valueOf(props.getProperty("nami_min_thread", "10"));
            maxPoolSize = Integer.valueOf(props.getProperty("nami_max_thread", "128"));
            keepAlive = Boolean.valueOf(props.getProperty("nami_keep_alive", "true"));
        } catch (Exception ex) {
            System.out.println("ERROR: Failed to init server config file...");
            ex.printStackTrace();
        }
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
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
}
