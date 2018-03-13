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

import java.io.File;
import java.io.FileFilter;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.common.support.ClassLoaderManager;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.server.apps.AppContextImpl;
import com.qq.tars.server.apps.XmlAppContext;
import com.qq.tars.server.config.ConfigurationManager;

public class AppContainer implements Container {

    AppContext defaultApp = null;

    private final ConcurrentHashMap<String, AppContext> contexts = new ConcurrentHashMap<String, AppContext>();

    public void start() throws Exception {
        loadApp();
        defaultApp = contexts.get("");
        System.out.println("[SERVER] The container started successfully.");
    }

    public void stop() throws Exception {
        stopApps();
        System.out.println("[SERVER] The container stopped successfully.");
    }

    private void loadApp() throws Exception {
        String root = ConfigurationManager.getInstance().getServerConfig().getBasePath();
        File path = new File(root);
        AppContext context = null;

        URL servantXML = getClass().getClassLoader().getResource("servants.xml");
        if (servantXML != null) {
            context = new XmlAppContext();
        } else if (getClass().getClassLoader().getResource("servants-spring.xml") != null){
            System.out.println("[SERVER] find servants-spring.xml, use Spring mode.");
            Class clazz = Class.forName("com.qq.tars.server.apps.SpringAppContext");
            context = (AppContext) clazz.newInstance();
        } else {
            System.out.println("[SERVER] servants profile does not exist, start failed.");
            throw new TarsException("servants profile does not exist");
        }

        contexts.put("", context);
    }

    @Deprecated
    public void loadApps() throws Exception {
        String root = ConfigurationManager.getInstance().getServerConfig().getBasePath();
        File dirs = new File(root + "/apps");
        final ClassLoaderManager manager = ClassLoaderManager.getInstance();

        dirs.listFiles(new FileFilter() {

            public boolean accept(File path) {
                String name = path.getName();
                if (name.equals("ROOT")) {
                    name = "";
                }
                if (path.isDirectory()) {
                    AppContextImpl context = new AppContextImpl(name, path);
                    contexts.put(name, context);
                    manager.registerClassLoader(name, context.getAppContextClassLoader());
                }
                return false;
            }
        });
    }

    public AppContext getDefaultAppContext() {
        return defaultApp;
    }

    public AppContext getAppContext(String name) {
        return contexts.get(name);
    }

    public String getContextPath(String reqeustURI) {
        String contextPath = "/";
        int pos = -1;
        pos = reqeustURI.indexOf("/", 1);

        if (pos > 0) {
            contextPath = reqeustURI.substring(1, pos - 1);
            if (contexts.get(contextPath) == null) {
                contextPath = "/";
            }
        }

        return contextPath;
    }

    public void stopApps() throws Exception {
        for (AppContext appContext : contexts.values()) {
            appContext.stop();
        }
    }
}
