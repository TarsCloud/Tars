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

import com.qq.tars.common.util.StringUtils;
import com.qq.tars.net.core.Processor;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.server.config.*;
import com.qq.tars.server.core.AppContextListener;
import com.qq.tars.server.core.ServantAdapter;
import com.qq.tars.server.core.ServantHomeSkeleton;
import com.qq.tars.spring.config.ListenerConfig;
import com.qq.tars.spring.config.ServantConfig;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import java.util.Map;

public class SpringAppContext extends BaseAppContext {

    private ApplicationContext applicationContext = null;

    public SpringAppContext() {
    }

    @Override
    protected void loadServants() {
        this.applicationContext = new ClassPathXmlApplicationContext("servants-spring.xml");

        loadAppContextListeners(this.applicationContext);
        loadAppServants(this.applicationContext);
        loadDefaultFilter();
        loadAppFilters(this.applicationContext);
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
            try {
                ServantHomeSkeleton skeleton = loadServant(entry.getValue());
                skeletonMap.put(skeleton.name(), skeleton);
                appServantStarted(skeleton);
            } catch (Exception e) {
                System.err.println("init a service failed");
                e.printStackTrace();
            }
        }
    }

    private void loadAppFilters(ApplicationContext applicationContext) {
    	
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
}
