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
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.core.AppContextListener;
import com.qq.tars.server.core.ServantAdapter;
import com.qq.tars.server.core.ServantHomeSkeleton;
import com.qq.tars.spring.annotation.TarsListener;
import com.qq.tars.spring.annotation.TarsServant;
import org.springframework.context.ApplicationContext;

import java.util.Map;

public class SpringBootAppContext extends BaseAppContext {
    private ApplicationContext applicationContext;

    public SpringBootAppContext(ApplicationContext applicationContext) {
        super();
        this.applicationContext = applicationContext;
    }

    @Override
    protected void loadServants() {
        loadAppContextListeners();
        loadAppServants();
    }

    private void loadAppContextListeners() {
        Map<String, Object> listenerMap = applicationContext.getBeansWithAnnotation(TarsListener.class);
        for (Map.Entry<String, Object> entry : listenerMap.entrySet()) {
            AppContextListener listener;

            try {
                listener = (AppContextListener) entry.getValue();
                listeners.add(listener);
            } catch (ClassCastException e) {
                System.err.println("invalid listener config|It is NOT a ContextListener:" + entry.getValue().getClass());
            } catch (Exception e) {
                System.err.println("create listener instance failed.");
                e.printStackTrace();
            }
        }
    }

    private void loadAppServants() {
        Map<String, Object> servantMap = applicationContext.getBeansWithAnnotation(TarsServant.class);
        for (Map.Entry<String, Object> entry : servantMap.entrySet()) {
            try {
                ServantHomeSkeleton skeleton = loadServant(entry.getValue());
                skeletonMap.put(skeleton.name(), skeleton);
                appServantStarted(skeleton);
            } catch (Exception e) {
                System.err.println("init a Servant failed");
                e.printStackTrace();
            }
        }

    }

    private ServantHomeSkeleton loadServant(Object bean) throws Exception {
        String homeName = null;
        Class<?> homeApiClazz = null;
        Class<? extends Codec> codecClazz = null;
        Class<? extends Processor> processorClazz = null;
        Object homeClassImpl = null;
        ServantHomeSkeleton skeleton = null;
        int maxLoadLimit = -1;

        ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();

        homeName = bean.getClass().getAnnotation(TarsServant.class).value();
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

        if (homeApiClazz == null)
            throw new Exception("servant is not TarServant");

        homeClassImpl = bean;

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
