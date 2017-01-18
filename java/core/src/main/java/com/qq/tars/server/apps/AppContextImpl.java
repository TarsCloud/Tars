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

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.lang.reflect.Constructor;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.common.util.StringUtils;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.annotation.ServantCodec;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.ext.ExtendedProtocolFactory;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.rpc.protocol.tars.support.AnalystManager;
import com.qq.tars.server.common.XMLConfigElement;
import com.qq.tars.server.common.XMLConfigFile;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.server.core.AppContext;
import com.qq.tars.server.core.AppContextListener;
import com.qq.tars.server.core.ServantHomeSkeleton;
import com.qq.tars.support.admin.AdminFServant;
import com.qq.tars.support.admin.impl.AdminFServantImpl;
import com.qq.tars.support.om.OmConstants;

public class AppContextImpl implements AppContext {

    private String name = null;

    private File path = null;

    private AppClassLoader classLoader = null;

    private boolean ready = true;

    private ConcurrentHashMap<String, ServantHomeSkeleton> skeletonMap = new ConcurrentHashMap<String, ServantHomeSkeleton>();

    private HashMap<String, String> contextParams = new HashMap<String, String>();

    private Set<AppContextListener> listeners = new HashSet<AppContextListener>(4);

    public AppContextImpl(String name, File path) {
        ClassLoader oldClassLoader = null;

        try {
            this.name = name;
            this.path = path;
            oldClassLoader = Thread.currentThread().getContextClassLoader();
            this.classLoader = new AppClassLoader(name, findURLClassPath());
            Thread.currentThread().setContextClassLoader(this.classLoader);
            initFromConfigFile();
            injectOmServants();
            initServants();
            appContextStarted();
            System.out.println("[SERVER] The application started successfully.  {appname=" + name + "}");
        } catch (Exception ex) {
            ready = false;
            System.out.println("[SERVER] failed to start the applicaton. {appname=" + this.name + "}");
        } finally {
            if (oldClassLoader != null) Thread.currentThread().setContextClassLoader(oldClassLoader);
        }
    }

    private void injectOmServants() {
        try {
            String skeletonName = OmConstants.AdminServant;
            ServantHomeSkeleton skeleton = new ServantHomeSkeleton(skeletonName, new AdminFServantImpl(), AdminFServant.class, -1);
            skeleton.setAppContext(this);
            skeletonMap.put(skeletonName, skeleton);
        } catch (Exception e) {
            System.err.println("init om service failed:context=[" + name + "]");
        }
    }

    public ServantHomeSkeleton getCapHomeSkeleton(String homeName) {
        if (!ready) {
            throw new RuntimeException("The application isn't started.");
        }
        return skeletonMap.get(homeName);
    }

    public Set<String> getAllServiceName() {
        return this.skeletonMap.keySet();
    }

    public ClassLoader getAppContextClassLoader() {
        return this.classLoader;
    }

    protected void initFromConfigFile() throws Exception {
        URL url = this.classLoader.getResource("WEB-INF/servants.xml");
        if (url == null) {
            System.out.println("WARN\tfailed to find WEB-INF/servants.xml, " + "tas service will be disabled:contextName=[" + name + "]");
            return;
        }

        XMLConfigFile cfg = new XMLConfigFile();
        cfg.parse(new FileInputStream(new File(url.toURI())));
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

    private void loadAppServants(ArrayList<XMLConfigElement> elements) throws ClassNotFoundException, InstantiationException, IllegalAccessException {
        for (XMLConfigElement element : elements) {
            if ("servant".equals(element.getName())) {
                try {
                    ServantHomeSkeleton skeleton = loadServant(element);
                    skeletonMap.put(skeleton.name(), skeleton);
                    appServantstarted(skeleton);
                } catch (Exception e) {
                    System.err.println("init a service failed:context=[" + name + "]");
                }
            }
        }
    }

    private void appServantstarted(ServantHomeSkeleton skeleton) {
        for (AppContextListener listener : listeners) {
            listener.appServantStarted(new DefaultAppServantEvent(skeleton));
        }
    }

    private void appContextStarted() {
        for (AppContextListener listener : listeners) {
            listener.appContextStarted(new DefaultAppContextEvent(this));
        }
    }

    private ServantHomeSkeleton loadServant(XMLConfigElement element) throws ClassNotFoundException, InstantiationException, IllegalAccessException {
        String homeName = null, homeApi = null, homeClass = null;
        Class<?> homeApiClazz = null;
        Object homeClassImpl = null;
        ServantHomeSkeleton skeleton = null;
        int maxLoadLimit = -1;

        ServerConfig serverCfg = ConfigurationManager.getInstance().getserverConfig();

        homeName = element.getStringAttribute("name");
        if (StringUtils.isEmpty(homeName)) {
            throw new RuntimeException("servant name is null.");
        }
        homeName = String.format("%s.%s.%s", serverCfg.getApplication(), serverCfg.getServerName(), homeName);
        homeApi = getChildNodeValue(element, "home-api");
        homeClass = getChildNodeValue(element, "home-class");
        maxLoadLimit = StringUtils.convertInt(getChildNodeValue(element, "load-limit"), maxLoadLimit);

        homeApiClazz = this.classLoader.loadClass(homeApi);
        homeClassImpl = this.classLoader.loadClass(homeClass).newInstance();

        if (TarsHelper.isServant(homeApiClazz)) {
            String servantName = homeApiClazz.getAnnotation(Servant.class).name();
            if (!StringUtils.isEmpty(servantName) && servantName.matches("^[\\w]+\\.[\\w]+\\.[\\w]+$")) {
                homeName = servantName;
            }
        }

        ServantAdapterConfig servantAdapterConfig = serverCfg.getServantAdapterConfMap().get(homeName);
        skeleton = new ServantHomeSkeleton(homeName, homeClassImpl, homeApiClazz, maxLoadLimit);
        skeleton.setAppContext(this);

        skeleton.setMaxThreadPoolSize(servantAdapterConfig.getThreads());
        skeleton.setMinThreadPoolSize(servantAdapterConfig.getThreads());
        skeleton.setQueueSize(servantAdapterConfig.getQueueCap());

        if (homeApiClazz.isAnnotationPresent(ServantCodec.class)) {
            Codec codec = createCodec(homeApiClazz, serverCfg);
            if (codec == null) {
                throw new RuntimeException("Can't found codec for servant [" + homeApi + "]");
            }
            ExtendedProtocolFactory.registerExtendedCodecImpl(codec, name());
        }
        return skeleton;
    }

    private Codec createCodec(Class<?> api, ServerConfig serverCfg) throws InstantiationException {
        Codec codec = null;
        ServantCodec servantCodec = api.getAnnotation(ServantCodec.class);
        if (servantCodec != null) {
            Class<? extends Codec> codecClass = servantCodec.codec();
            if (codecClass != null) {
                Constructor<? extends Codec> constructor;
                try {
                    constructor = codecClass.getConstructor(new Class[] { String.class });
                    codec = constructor.newInstance(serverCfg.getCharsetName());
                } catch (Exception e) {
                    throw new InstantiationException("error occurred on create codec, codec=" + codecClass.getName());
                }
            }
        }
        return codec;
    }

    private void loadAppContextListeners(ArrayList<XMLConfigElement> elements) {
        for (XMLConfigElement element : elements) {
            if ("listener".equals(element.getName())) {
                String listenerClass = getChildNodeValue(element, "listener-class");
                AppContextListener listener;

                try {
                    listener = (AppContextListener) this.classLoader.loadClass(listenerClass).newInstance();
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

    protected URL[] findURLClassPath() {
        final List<URL> urls = new ArrayList<URL>();

        try {
            urls.add(path.toURI().toURL());

            urls.add(new File(this.path + "/WEB-INF/classes").toURI().toURL());

            new File(this.path + "/WEB-INF/lib/").listFiles(new FileFilter() {

                @Override
                public boolean accept(File pathname) {
                    try {
                        if (pathname.getName().endsWith(".jar")) {
                            urls.add(pathname.toURI().toURL());
                        }
                    } catch (MalformedURLException ex) {
                    }

                    return false;
                }
            });
        } catch (Exception ex) {
            System.err.println(ex.getLocalizedMessage());
        }
        return urls.toArray(new URL[urls.size()]);
    }

    private String getChildNodeValue(XMLConfigElement element, String nodeName) {
        if (element == null) return null;

        XMLConfigElement childElement = element.getChildByName(nodeName);

        if (childElement == null) return null;

        return StringUtils.trim(childElement.getContent());
    }

    @Override
    public String getInitParameter(String name) {
        return contextParams.get(name);
    }

    @Override
    public String name() {
        return this.name;
    }

    private void initServants() {
        for (String skeletonName : skeletonMap.keySet()) {
            ServantHomeSkeleton skeleton = skeletonMap.get(skeletonName);
            Class<?> api = skeleton.getApiClass();
            try {
                AnalystManager.getInstance().registry(name(), api, skeleton.name());
            } catch (Exception e) {
                System.err.println("app[" + name + "] init servant[" + api.getName() + "] failed");
            }
        }
    }
}
