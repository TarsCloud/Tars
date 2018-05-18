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
import com.qq.tars.server.common.XMLConfigElement;
import com.qq.tars.server.common.XMLConfigFile;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.core.AppContextListener;
import com.qq.tars.server.core.ServantAdapter;
import com.qq.tars.server.core.ServantHomeSkeleton;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;

public class XmlAppContext extends BaseAppContext {
    public XmlAppContext() {
        try {
            initFromConfigFile();
            injectAdminServant();
            initServants();
            appContextStarted();
            setAppContext();
            System.out.println("[SERVER] The application started successfully.  {appname=}");
        } catch (Exception ex) {
            ready = false;
            System.out.println("[SERVER] failed to start the applicaton. {appname=}");
        }
    }

    private void initFromConfigFile() throws Exception {
        XMLConfigFile cfg = new XMLConfigFile();
        cfg.parse(getClass().getClassLoader().getResource("servants.xml").openStream());
        XMLConfigElement root = cfg.getRootElement();
        ArrayList<XMLConfigElement> elements = root.getChildList();

        loadInitParams(root.getChildListByName("context-param"));

        loadAppContextListeners(elements);

        loadAppServants(elements);
    }

    private void loadInitParams(ArrayList<XMLConfigElement> list) {
        if (list == null || list.isEmpty()) return;
        for (XMLConfigElement e : list) {
            String name = getChildNodeValue(e, "param-name");
            String value = getChildNodeValue(e, "param-value");
            if (!StringUtils.isEmpty(name)) contextParams.put(name, value);
        }
    }

    private void loadAppContextListeners(ArrayList<XMLConfigElement> elements) {
        for (XMLConfigElement element : elements) {
            if ("listener".equals(element.getName())) {
                String listenerClass = getChildNodeValue(element, "listener-class");
                AppContextListener listener;

                try {
                    listener = (AppContextListener) Class.forName(listenerClass).newInstance();
                    listeners.add(listener);
                } catch (ClassNotFoundException e) {
                    System.err.println("invalid listener config|ClassNotFoundException:" + listenerClass);
                } catch (ClassCastException e) {
                    System.err.println("invalid listener config|It is NOT a ContextListener:" + listenerClass);
                } catch (Exception e) {
                    System.err.println("create listener instance failed.");
                }
            }
        }
    }

    private void loadAppServants(ArrayList<XMLConfigElement> elements) throws ClassNotFoundException, InstantiationException, IllegalAccessException {
        for (XMLConfigElement element : elements) {
            if ("servant".equals(element.getName())) {
                try {
                    ServantHomeSkeleton skeleton = loadServant(element);
                    skeletonMap.put(skeleton.name(), skeleton);
                    appServantStarted(skeleton);
                } catch (Exception e) {
                    System.err.println("init a service failed:context=[]");
                }
            }
        }
    }

    @SuppressWarnings("unchecked")
    private ServantHomeSkeleton loadServant(XMLConfigElement element) throws ClassNotFoundException, InstantiationException, IllegalAccessException, IOException {
        String homeName = null, homeApiName = null, homeClassName = null, processorClazzName = null,
                codecClazzName = null;
        Class<?> homeApiClazz = null;
        Class<? extends Codec> codecClazz = null;
        Class<? extends Processor> processorClazz = null;
        Object homeClassImpl = null;
        ServantHomeSkeleton skeleton = null;
        int maxLoadLimit = -1;

        ServerConfig serverCfg = ConfigurationManager.getInstance().getServerConfig();

        homeName = element.getStringAttribute("name");
        if (StringUtils.isEmpty(homeName)) {
            throw new RuntimeException("servant name is null.");
        }
        homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), homeName);
        homeApiName = getChildNodeValue(element, "home-api");
        homeClassName = getChildNodeValue(element, "home-class");
        processorClazzName = getChildNodeValue(element, "home-processor-class");
        codecClazzName = getChildNodeValue(element, "home-codec-class");

        homeApiClazz = Class.forName(homeApiName);
        homeClassImpl = Class.forName(homeClassName).newInstance();
        codecClazz = (Class<? extends Codec>) (StringUtils.isEmpty(codecClazzName) ? null : Class.forName(codecClazzName));
        processorClazz = (Class<? extends Processor>) (StringUtils.isEmpty(processorClazzName) ? null : Class.forName(processorClazzName));

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

    private String getChildNodeValue(XMLConfigElement element, String nodeName) {
        if (element == null) return null;

        XMLConfigElement childElement = element.getChildByName(nodeName);

        if (childElement == null) return null;

        return StringUtils.trim(childElement.getContent());
    }

}
