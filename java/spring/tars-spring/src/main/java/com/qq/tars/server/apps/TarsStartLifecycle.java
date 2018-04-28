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

package com.qq.tars.server.apps;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.springframework.beans.BeansException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;
import org.springframework.context.SmartLifecycle;

import com.qq.tars.client.Communicator;
import com.qq.tars.client.CommunicatorFactory;
import com.qq.tars.client.config.TarsClientProperties;
import com.qq.tars.common.support.Endpoint;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.server.annotation.TarsServant;
import com.qq.tars.server.common.ServerLogger;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.config.TarsServerProperties;
import com.qq.tars.server.core.ServantAdapter;
import com.qq.tars.server.core.ServantHomeSkeleton;
import com.qq.tars.support.log.LogConfCacheMngr;
import com.qq.tars.support.log.LoggerFactory;

public class TarsStartLifecycle extends BaseAppContext implements SmartLifecycle, ApplicationContextAware {

    private static final Log log = LogFactory.getLog(TarsStartLifecycle.class);

    @Autowired
    private TarsServerProperties serverProperties;

    @Autowired
    private TarsClientProperties clientProperties;

    private ApplicationContext applicationContext;

    private ServantAdapterConfig servantAdapterConfig;

    private boolean isRunning = false;

    public TarsStartLifecycle() {
        super();
    }

    @Override
    public boolean isAutoStartup() {
        return true;
    }

    @Override
    public void stop(Runnable runnable) {
    }

    @Override
    public void start() {
        try {
            log.info("Start init TarsServer");

            initConfig();
            configLogger();
            loadAppServants();
            initServants();
            appContextStarted();
            startServantAdapter();
            setAppContext();

            this.isRunning = true;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {
        isRunning = false;
    }

    @Override
    public boolean isRunning() {
        return this.isRunning;
    }

    @Override
    public int getPhase() {
        return 0;
    }

    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        this.applicationContext = applicationContext;
    }

    private void initConfig() {
        serverProperties.setCommunicatorConfig(this.clientProperties);
        serverProperties.setServantAdapterConfMap(new LinkedHashMap<String, ServantAdapterConfig>());
        ConfigurationManager.getInstance().setServerConfig(this.serverProperties);
        ConfigurationManager.getInstance().setCommunicatorConfig(this.clientProperties);

        servantAdapterConfig = new ServantAdapterConfig();
        servantAdapterConfig.setEndpoint(new Endpoint(serverProperties.isUdp() ? "udp" : "tcp", StringUtils.isEmpty(serverProperties.getLocalIP()) ? "0.0.0.0" : serverProperties.getLocalIP(), serverProperties.getPort(), 0, 0, 0, null));
        servantAdapterConfig.setHandleGroup("default");
        servantAdapterConfig.setMaxConns(serverProperties.getMaxConns());
        servantAdapterConfig.setThreads(serverProperties.getThreads());
        servantAdapterConfig.setQueueCap(serverProperties.getQueueCap());
        servantAdapterConfig.setQueueTimeout(serverProperties.getQueueTimeout());
    }

    private void loadAppServants() {
        Map<String, Object> servantMap = applicationContext.getBeansWithAnnotation(TarsServant.class);
        for (Map.Entry<String, Object> entry : servantMap.entrySet()) {
            try {
                ServantHomeSkeleton skeleton = loadServant(entry.getValue());
                skeletonMap.put(skeleton.name(), skeleton);
                appServantStarted(skeleton);
            } catch (Exception e) {
                log.error("load servant faild", e);
            }
        }

    }

    private ServantHomeSkeleton loadServant(Object bean) throws Exception {
        String homeName;
        Class<?> homeApiClazz = null;
        Object homeClassImpl;
        ServantHomeSkeleton skeleton;
        int maxLoadLimit = -1;

        ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();

        homeName = bean.getClass().getAnnotation(TarsServant.class).name();
        if (StringUtils.isEmpty(homeName)) {
            throw new RuntimeException("servant name is null.");
        }
        homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), homeName);

        for (Class clazz : bean.getClass().getInterfaces()) {
            if (clazz.isAnnotationPresent(Servant.class)) {
                homeApiClazz = clazz;
                break;
            }
        }

        homeClassImpl = bean;

        if (TarsHelper.isServant(homeApiClazz)) {
            String servantName = homeApiClazz != null ? homeApiClazz.getAnnotation(Servant.class).name() : null;
            if (!StringUtils.isEmpty(servantName) && servantName.matches("^[\\w]+\\.[\\w]+\\.[\\w]+$")) {
                homeName = servantName;
            }
        }

        skeleton = new ServantHomeSkeleton(homeName, homeClassImpl, homeApiClazz, null, null, maxLoadLimit);
        skeleton.setAppContext(this);

        ConfigurationManager.getInstance().getServerConfig().getServantAdapterConfMap().put(homeName, servantAdapterConfig);

        return skeleton;
    }

    private void startServantAdapter() throws IOException {
        ServantAdapter ServerAdapter = new ServantAdapter(servantAdapterConfig);

        ServerAdapter.bind();
        servantAdapterMap.put("", ServerAdapter);
    }

    private void configLogger() {
        Communicator communicator = CommunicatorFactory.getInstance().getCommunicator();

        String objName = ConfigurationManager.getInstance().getServerConfig().getLog();
        String appName = ConfigurationManager.getInstance().getServerConfig().getApplication();
        String serviceName = ConfigurationManager.getInstance().getServerConfig().getServerName();

        String defaultLevel = ConfigurationManager.getInstance().getServerConfig().getLogLevel();
        String defaultRoot = ConfigurationManager.getInstance().getServerConfig().getLogPath();
        String dataPath = ConfigurationManager.getInstance().getServerConfig().getDataPath();

        LoggerFactory.config(communicator, objName, appName, serviceName, defaultLevel, defaultRoot);

        LogConfCacheMngr.getInstance().init(dataPath);
        if (StringUtils.isNotEmpty(LogConfCacheMngr.getInstance().getLevel())) {
            LoggerFactory.setDefaultLoggerLevel(LogConfCacheMngr.getInstance().getLevel());
        }
        ServerLogger.init();
    }
}
