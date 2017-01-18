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

package com.qq.tars.server.core;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.ServerSocketChannel;
import java.util.LinkedHashMap;
import java.util.Map.Entry;
import java.util.concurrent.Executor;

import com.qq.tars.client.Communicator;
import com.qq.tars.client.CommunicatorConfig;
import com.qq.tars.client.CommunicatorFactory;
import com.qq.tars.common.support.Endpoint;
import com.qq.tars.common.util.BeanAccessor;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.net.core.Processor;
import com.qq.tars.net.core.SessionManager;
import com.qq.tars.net.core.nio.SelectorManager;
import com.qq.tars.net.protocol.ProtocolFactory;
import com.qq.tars.net.util.Utils;
import com.qq.tars.rpc.ext.ExtendedProtocolFactory;
import com.qq.tars.rpc.protocol.tars.TarsCodec;
import com.qq.tars.rpc.protocol.tars.TarsServantProtocolFactory;
import com.qq.tars.server.common.ServerLogger;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.ha.ConnectionSessionListener;
import com.qq.tars.support.log.LogConfCacheMngr;
import com.qq.tars.support.log.LoggerFactory;
import com.qq.tars.support.om.OmConstants;
import com.qq.tars.support.om.OmServiceMngr;

public class Server {

    private String host = null;
    private int port;
    private int udpPort;
    private int tcpPort;

    private String adminHost = null;
    private int adminPort;

    private ProtocolFactory mainProtocolFactory = null;

    private SelectorManager mainSelectorManager = null;

    private SelectorManager udpSelectorManager = null;

    private SelectorManager tcpSelectorManager = null;

    private Processor processor = null;

    private Container container = null;

    private Executor threadPool = null;

    private boolean keepAlive = true;

    private int sessionTimeOut;

    private int sessionCheckInterval;

    private ProtocolFactory udpProtocolFactory;

    private ProtocolFactory tcpProtocolFactory;

    public Server() {
        loadServerConfig();
    }

    public void startUp(String args[]) {
        try {
            initCommunicator();

            configLogger();

            ServerLogger.init();

            startOmService();

            startAppContainer();

            startNIOServer();

            startSessionManager();

            registerServerHook();

            System.out.println("[SERVER] server is ready...");
        } catch (Throwable ex) {
            System.out.println("[SERVER] failed to start server...");
            System.out.close();
            System.err.close();
            System.exit(-1);
        }
    }

    protected void startAppContainer() throws Exception {
        this.mainProtocolFactory = new TarsServantProtocolFactory(new TarsCodec(ConfigurationManager.getInstance().getserverConfig().getCharsetName()));
        this.processor = new ServantProcessor();
        this.container = new AppContainer();
        ContainerManager.registerContainer(this.container);

        this.threadPool = new ServerThreadPoolDispatcher();
        this.mainSelectorManager = new SelectorManager(Utils.getSelectorPoolSize(), mainProtocolFactory, threadPool, processor, keepAlive, "server-tcp-reactor", false);
        this.mainSelectorManager.setTcpNoDelay(ConfigurationManager.getInstance().getserverConfig().isTcpNoDelay());

        this.container.start();
    }

    protected void startNIOServer() throws IOException {
        startMainServer(host, port);
        startMainServer(adminHost, adminPort);

        startUDPServer();

        startTCPServer();
    }

    private void startOmService() {
        OmServiceMngr.getInstance().initAndStartOmService();
    }

    private void initCommunicator() {
        CommunicatorConfig config = ConfigurationManager.getInstance().getserverConfig().getCommunicatorConfig();
        Communicator communicator = CommunicatorFactory.getInstance().getCommunicator(config);

        BeanAccessor.setBeanValue(CommunicatorFactory.getInstance(), "communicator", communicator);
    }

    private void configLogger() {
        Communicator communicator = CommunicatorFactory.getInstance().getCommunicator();

        String objName = ConfigurationManager.getInstance().getserverConfig().getLog();
        String appName = ConfigurationManager.getInstance().getserverConfig().getApplication();
        String serviceName = ConfigurationManager.getInstance().getserverConfig().getServerName();

        String defaultLevel = ConfigurationManager.getInstance().getserverConfig().getLogLevel();
        String defaultRoot = ConfigurationManager.getInstance().getserverConfig().getLogPath();
        String dataPath = ConfigurationManager.getInstance().getserverConfig().getDataPath();

        LoggerFactory.config(communicator, objName, appName, serviceName, defaultLevel, defaultRoot);

        LogConfCacheMngr.getInstance().init(dataPath);
        if (StringUtils.isNotEmpty(LogConfCacheMngr.getInstance().getLevel())) {
            LoggerFactory.setDefaultLoggerLevel(LogConfCacheMngr.getInstance().getLevel());
        }
    }

    private void startUDPServer() throws IOException {
        if (udpPort <= 0) {
            System.out.println("server.udpPort Not Exist or Invalid,  server Custom Protocol Service on UDP will be disabled.");
            return;
        }

        if (ExtendedProtocolFactory.getInstance() == null) {
            System.out.println("No Service use Custom Protocol Codec, Custom Protocol Service on UDP will be disabled.");
            return;
        }

        udpProtocolFactory = ExtendedProtocolFactory.getInstance();
        udpSelectorManager = new SelectorManager(1, udpProtocolFactory, threadPool, processor, false, "server-udp-reactor", true);
        udpSelectorManager.start();

        String[] ips = host.split(";");
        if (ips == null || ips.length == 0) throw new IllegalArgumentException("The host is illegal.");

        DatagramChannel serverChannel = null;
        for (String ip : ips) {
            if (ip == null || ip.trim().length() == 0) continue;
            ip = ip.trim();

            serverChannel = DatagramChannel.open();
            DatagramSocket socket = serverChannel.socket();
            socket.bind(new InetSocketAddress(ip, udpPort));
            serverChannel.configureBlocking(false);

            udpSelectorManager.getReactor(0).registerChannel(serverChannel, SelectionKey.OP_READ);

            System.out.println("[SERVER] server started at " + ip + " on ext UDP port " + String.valueOf(udpPort) + "...");
        }
    }

    private void startTCPServer() throws IOException {
        if (tcpPort <= 0) {
            System.out.println("server.tcpPort Not Exist or Invalid,  server Custom Protocol Service on TCP will be disabled.");
            return;
        }

        if (ExtendedProtocolFactory.getInstance() == null) {
            System.out.println("No Service use Custom Protocol Codec, Custom Protocol Service on TCP will be disabled.");
            return;
        }

        this.tcpProtocolFactory = ExtendedProtocolFactory.getInstance();
        tcpSelectorManager = new SelectorManager(Utils.getSelectorPoolSize(), tcpProtocolFactory, threadPool, processor, keepAlive, "server-tcp-custom-reactor", false);
        tcpSelectorManager.setTcpNoDelay(ConfigurationManager.getInstance().getserverConfig().isTcpNoDelay());
        tcpSelectorManager.start();

        String[] ips = host.split(";");
        if (ips == null || ips.length == 0) throw new IllegalArgumentException("The host is illegal.");

        ServerSocketChannel serverChannel = null;
        for (String ip : ips) {
            if (ip == null || ip.trim().length() == 0) continue;
            ip = ip.trim();

            serverChannel = ServerSocketChannel.open();
            serverChannel.socket().bind(new InetSocketAddress(ip, tcpPort), 1024);
            serverChannel.configureBlocking(false);

            tcpSelectorManager.getReactor(0).registerChannel(serverChannel, SelectionKey.OP_ACCEPT);

            System.out.println("[SERVER] server started at " + ip + " on ext TCP port " + String.valueOf(tcpPort) + "...");
        }

    }

    private void startMainServer(String ip, int port) throws IOException {
        if (port <= 0) {
            System.out.println("server.port Not Exist or Invalid,  server Protocol Service on TCP will be disabled.");
            return;
        }

        mainSelectorManager.start();

        String[] ips = host.split(";");
        if (ips == null || ips.length == 0) {
            throw new IllegalArgumentException("The host is illegal.");
        }

        ServerSocketChannel serverChannel = null;
        if (ip == null) return;
        ip = ip.trim();
        System.out.println("[SERVER] server starting at " + ip + " on TCP port " + String.valueOf(port) + "...");
        serverChannel = ServerSocketChannel.open();
        serverChannel.socket().bind(new InetSocketAddress(ip, port), 1024);
        serverChannel.configureBlocking(false);

        mainSelectorManager.getReactor(0).registerChannel(serverChannel, SelectionKey.OP_ACCEPT);

        System.out.println("[SERVER] server started at " + ip + " on TCP port " + String.valueOf(port) + "...");
    }

    protected void startSessionManager() throws IOException {
        SessionManager sessionManager = SessionManager.getSessionManager();
        sessionManager.setTimeout(sessionTimeOut);
        sessionManager.setCheckInterval(sessionCheckInterval);

        int connCount = 0;
        for (Entry<String, ServantAdapterConfig> adapterConfigEntry : ConfigurationManager.getInstance().getserverConfig().getServantAdapterConfMap().entrySet()) {
            if (OmConstants.AdminServant.equals(adapterConfigEntry.getKey())) {
                continue;
            }
            connCount += adapterConfigEntry.getValue().getMaxConns();
        }
        ConnectionSessionListener sessionListener = new ConnectionSessionListener(connCount);
        sessionManager.addSessionListener(sessionListener);

        sessionManager.start();
    }

    protected void loadServerConfig() {
        try {
            ConfigurationManager.getInstance().init();

            ServerConfig cfg = ConfigurationManager.getInstance().getserverConfig();
            ServerLogger.initNamiCoreLog(cfg.getLogPath(), cfg.getLogLevel());
            System.setProperty("com.qq.nami.server.udp.bufferSize", String.valueOf(cfg.getUdpBufferSize()));
            System.setProperty("server.root", cfg.getBasePath());

            Endpoint adminEndpoint = cfg.getLocal();

            adminHost = adminEndpoint.host();
            adminPort = adminEndpoint.port();

            host = cfg.getLocalIP();
            LinkedHashMap<String, ServantAdapterConfig> map = cfg.getServantAdapterConfMap();
            for (ServantAdapterConfig sc : map.values()) {
                if (sc.getProtocol().equals("tars")) {
                    if (port == 0) {
                        port = sc.getEndpoint().port();
                    }
                } else {
                    if (sc.getEndpoint().type().equals("tcp") && tcpPort == 0) {
                        tcpPort = sc.getEndpoint().port();
                    }
                    if (sc.getEndpoint().type().equals("udp") && udpPort == 0) {
                        udpPort = sc.getEndpoint().port();
                    }
                }
            }
            cfg.setLocalPort(port);
            sessionTimeOut = cfg.getSessionTimeOut();
            sessionCheckInterval = cfg.getSessionCheckInterval();
        } catch (Throwable ex) {
            ex.printStackTrace(System.err);
            System.err.println("The exception occured at load server config");
            System.exit(2);
        }
    }

    private void registerServerHook() {
        Runtime.getRuntime().addShutdownHook(new Thread() {

            @Override
            public void run() {
                try {
                    if (mainSelectorManager != null) {
                        mainSelectorManager.stop();
                    }
                    if (udpSelectorManager != null) {
                        udpSelectorManager.stop();
                    }

                    System.out.println("[SERVER] server stopped successfully.");

                    // 2. Stop Container
                    if (container != null) {
                        container.stop();
                    }
                } catch (Exception ex) {
                    System.err.println("The exception occured at stopping server...");
                }
            }
        });
    }
}
