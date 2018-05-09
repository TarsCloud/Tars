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

import java.lang.reflect.Constructor;
import java.util.Map;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import com.qq.tars.common.util.StringUtils;
import com.qq.tars.net.core.Processor;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ListenerConfig;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServantConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.core.Adapter;
import com.qq.tars.server.core.AppContextListener;
import com.qq.tars.server.core.AppService;
import com.qq.tars.server.core.ServantAdapter;
import com.qq.tars.server.core.ServantHomeSkeleton;

public class SpringAppContext extends BaseAppContext {

    private ApplicationContext applicationContext = null;

    public SpringAppContext() {
        try {
            initFromConfigFile();
            injectAdminServant();
            initServants();
            appContextStarted();
            setAppContext();
            System.out.println("[SERVER] The application started successfully.  {appname=}");
        } catch (Exception ex) {
            ready = false;
            System.err.println("[SERVER] failed to start the applicaton. {appname=}");
            ex.printStackTrace();
        }
    }

    private void initFromConfigFile() {
        this.applicationContext = new ClassPathXmlApplicationContext("servants-spring.xml");

        loadAppContextListeners(this.applicationContext);
        loadAppServants(this.applicationContext);
    }

    private void loadAppContextListeners(ApplicationContext applicationContext) {
        Map<String, ListenerConfig> servantMap = applicationContext.getBeansOfType(ListenerConfig.class);
        for (Map.Entry<String, ListenerConfig> entry : servantMap.entrySet()) {
            AppContextListener listener;

            listener = (AppContextListener) applicationContext.getBean(entry.getValue().getRef());
            listeners.add(listener);
        }
    }

    private void loadAppServants(ApplicationContext applicationContext) {
        Map<String, ServantConfig> servantMap = applicationContext.getBeansOfType(ServantConfig.class);
        for (Map.Entry<String, ServantConfig> entry : servantMap.entrySet()) {
            if (StringUtils.isEmpty(entry.getValue().getProtocol()) || !entry.getValue().getProtocol().equals("rest")) {
                try {
                    ServantHomeSkeleton skeleton = loadServant(entry.getValue());
                    skeletonMap.put(skeleton.name(), skeleton);
                    appServantStarted(skeleton);
                } catch (Exception e) {
                    System.err.println("init a service failed");
                    e.printStackTrace();
                }
            } else {
                try {
                    AppService appService = loadRestServant(entry.getValue());
                    appServantStarted(appService);
                } catch (Exception e) {
                    System.err.println("init a service failed");
                    e.printStackTrace();
                }
            }
        }
    }

    private AppService loadRestServant(ServantConfig servantConfig) throws Exception {
        String homeName = servantConfig.getName();

        ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();

        if (StringUtils.isEmpty(homeName)) {
            throw new RuntimeException("servant name is null.");
        }
        homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), homeName);

        ServantAdapterConfig servantAdapterConfig = serverCfg.getServantAdapterConfMap().get(homeName);

        Class clazz = Class.forName("com.qq.tars.rest.RestServantAdapter");
        Constructor constructor = clazz.getConstructor(ServantAdapterConfig.class);
        Adapter adapter = (Adapter) constructor.newInstance(servantAdapterConfig);
        RestService restService = new RestService(homeName, servantConfig.getSrc(), this.applicationContext);
        restService.setAppContext(this);
        adapter.bind(restService);
        servantAdapterMap.put(homeName, adapter);

        return null;
    }

    private ServantHomeSkeleton loadServant(ServantConfig servantConfig) throws Exception {
        String homeName = null, homeApiName = null;
        Class<?> homeApiClazz = null;
        Class<? extends Codec> codecClazz = null;
        Class<? extends Processor> processorClazz = null;
        Object homeClassImpl = null;
        ServantHomeSkeleton skeleton = null;
        int maxLoadLimit = -1;

        ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();

        homeName = servantConfig.getName();
        if (StringUtils.isEmpty(homeName)) {
            throw new RuntimeException("servant name is null.");
        }
        homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), homeName);
        homeApiName = servantConfig.getInterface();

        homeApiClazz = Class.forName(homeApiName);
        homeClassImpl = this.applicationContext.getBean(servantConfig.getRef());

        if (TarsHelper.isServant(homeApiClazz)) {
            String servantName = homeApiClazz.getAnnotation(Servant.class).name();
            if (!StringUtils.isEmpty(servantName) && servantName.matches("^[\\w]+\\.[\\w]+\\.[\\w]+$")) {
                homeName = servantName;
            }
        }

        ServantAdapterConfig servantAdapterConfig = serverCfg.getServantAdapterConfMap().get(homeName);

        ServantAdapter ServerAdapter = new ServantAdapter(servantAdapterConfig);
        skeleton = new ServantHomeSkeleton(homeName, homeClassImpl, homeApiClazz, codecClazz, processorClazz, maxLoadLimit);
        skeleton.setAppContext(this);
        ServerAdapter.bind(skeleton);
        servantAdapterMap.put(homeName, ServerAdapter);
        return skeleton;
    }

    public ApplicationContext getApplicationContext() {
        return this.applicationContext;
    }
}
