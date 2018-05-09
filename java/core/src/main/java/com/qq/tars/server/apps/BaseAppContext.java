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

import com.qq.tars.rpc.protocol.ext.ExtendedServant;
import com.qq.tars.rpc.protocol.tars.support.AnalystManager;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.core.*;
import com.qq.tars.server.core.AppContext;
import com.qq.tars.support.admin.AdminFServant;
import com.qq.tars.support.admin.impl.AdminFServantImpl;
import com.qq.tars.support.om.OmConstants;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public abstract class BaseAppContext implements AppContext {
    boolean ready = true;

    ConcurrentHashMap<String, ServantHomeSkeleton> skeletonMap = new ConcurrentHashMap<String, ServantHomeSkeleton>();
    ConcurrentHashMap<String, Adapter> servantAdapterMap = new ConcurrentHashMap<String, Adapter>();

    HashMap<String, String> contextParams = new HashMap<String, String>();

    Set<AppContextListener> listeners = new HashSet<AppContextListener>(4);

    BaseAppContext() {
    }

    void injectAdminServant() {
        try {
            String skeletonName = OmConstants.AdminServant;
            ServantHomeSkeleton skeleton = new ServantHomeSkeleton(skeletonName, new AdminFServantImpl(), AdminFServant.class, null, null, -1);
            skeleton.setAppContext(this);

            ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();
            ServantAdapterConfig config = serverCfg.getServantAdapterConfMap().get(OmConstants.AdminServant);
            ServantAdapter servantAdapter = new ServantAdapter(config);
            servantAdapter.bind(skeleton);
            servantAdapterMap.put(skeletonName, servantAdapter);

            skeletonMap.put(skeletonName, skeleton);
        } catch (Exception e) {
            System.err.println("init om service failed:context=[]");
            e.printStackTrace();
        }
    }

    void appServantStarted(AppService appService) {
        for (AppContextListener listener : listeners) {
            listener.appServantStarted(new DefaultAppServantEvent(appService));
        }
    }

    void initServants() {
        for (String skeletonName : skeletonMap.keySet()) {
            ServantHomeSkeleton skeleton = skeletonMap.get(skeletonName);
            Class<?> api = skeleton.getApiClass();
            try {
                if (api.isAssignableFrom(ExtendedServant.class)) {
                    continue;
                }
                AnalystManager.getInstance().registry(name(), api, skeleton.name());
            } catch (Exception e) {
                System.err.println("app[] init servant[" + api.getName() + "] failed");
                e.printStackTrace();
            }
        }
    }

    void appContextStarted() {
        for (AppContextListener listener : listeners) {
            listener.appContextStarted(new DefaultAppContextEvent(this));
        }
    }

    void setAppContext() {
        AppContextManager.getInstance().setAppContext(this);
    }

    @Override
    public String getInitParameter(String name) {
        return contextParams.get(name);
    }

    @Override
    public String name() {
        return "";
    }

    @Override
    public void stop() {
        for (Adapter servantAdapter : servantAdapterMap.values()) {
            servantAdapter.stop();
        }
    }

    @Override
    public ServantHomeSkeleton getCapHomeSkeleton(String homeName) {
        if (!ready) {
            throw new RuntimeException("The application isn't started.");
        }
        return skeletonMap.get(homeName);
    }
}
