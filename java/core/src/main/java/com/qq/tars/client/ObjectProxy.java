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

package com.qq.tars.client;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.util.Random;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import com.qq.tars.client.support.ServantCacheManager;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.client.util.ParseTools;
import com.qq.tars.common.support.ScheduledExecutorManager;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.register.RegisterManager;
import com.qq.tars.rpc.common.*;
import com.qq.tars.rpc.common.exc.NoInvokerException;
import com.qq.tars.rpc.exc.ClientException;
import com.qq.tars.rpc.exc.NoConnectionException;
import com.qq.tars.support.stat.InvokeStatHelper;

public final class ObjectProxy<T> implements ServantProxy, InvocationHandler {

    private final Class<T> api;
    private final String objName;
    private final Communicator communicator;

    private final ServantCacheManager servantCacheManager = ServantCacheManager.getInstance();

    private volatile ServantProxyConfig servantProxyConfig;

    private LoadBalance loadBalancer;
    private ProtocolInvoker<T> protocolInvoker;
    private ScheduledFuture<?> statReportFuture;
    private ScheduledFuture<?> queryRefreshFuture;

    private final Object refreshLock = new Object();

    private final Random random = new Random(System.currentTimeMillis() / 1000);

    public ObjectProxy(Class<T> api, String objName, ServantProxyConfig servantProxyConfig, LoadBalance loadBalance,
                       ProtocolInvoker<T> protocolInvoker, Communicator communicator) {
        this.api = api;
        this.objName = objName;
        this.communicator = communicator;
        this.servantProxyConfig = servantProxyConfig;
        this.loadBalancer = loadBalance;
        this.protocolInvoker = protocolInvoker;
        this.initialize();
    }

    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        String methodName = method.getName();
        Class<?>[] parameterTypes = method.getParameterTypes();

        InvokeContext context = protocolInvoker.createContext(proxy, method, args);
        try {
            if ("toString".equals(methodName) && parameterTypes.length == 0) {
                return this.toString();
            } else if ("hashCode".equals(methodName) && parameterTypes.length == 0) {
                return this.hashCode();
            } else if ("equals".equals(methodName) && parameterTypes.length == 1) {
                return this.equals(args[0]);
            } else if ("getObjectName".equals(methodName) && parameterTypes.length == 0) {
                return this.getObjectName();
            } else if ("getApi".equals(methodName) && parameterTypes.length == 0) {
                return this.getApi();
            } else if ("getConfig".equals(methodName) && parameterTypes.length == 0) {
                return this.getConfig();
            } else if ("destroy".equals(methodName) && parameterTypes.length == 0) {
                this.destroy();
                return null;
            } else if ("refresh".equals(methodName) && parameterTypes.length == 0) {
                this.refresh();
                return null;
            }

            Invoker invoker = loadBalancer.select(context);
            return invoker.invoke(context);
        } catch (Throwable e) {
            e.printStackTrace();
            if (ClientLogger.getLogger().isDebugEnabled()) {
                ClientLogger.getLogger().debug(servantProxyConfig.getSimpleObjectName() + " error occurred on invoke|" + e.getLocalizedMessage(), e);
            }
            if (e instanceof NoInvokerException) {
                throw new NoConnectionException(servantProxyConfig.getSimpleObjectName(), e.getLocalizedMessage(), e);
            }
            throw new ClientException(servantProxyConfig.getSimpleObjectName(), e.getLocalizedMessage(), e);
        }
    }

    public Url selectUrl() {
        return loadBalancer.select(null).getUrl();
    }

    public Class<T> getApi() {
        return api;
    }

    public String getObjectName() {
        return servantProxyConfig.getSimpleObjectName();
    }

    public void refresh() {
        synchronized (refreshLock) {
            registryStatReproter();
            registryServantNodeRefresher();
            protocolInvoker.refresh();
            loadBalancer.refresh(protocolInvoker.getInvokers());
        }
    }

    public void destroy() {
        if (statReportFuture != null)
            statReportFuture.cancel(false);
        if (queryRefreshFuture != null)
            queryRefreshFuture.cancel(false);
        protocolInvoker.destroy();
    }

    public ServantProxyConfig getConfig() {
        return servantProxyConfig;
    }

    private void initialize() {
        loadBalancer.refresh(protocolInvoker.getInvokers());

        if (StringUtils.isNotEmpty(this.servantProxyConfig.getLocator()) && !StringUtils.isEmpty(this.servantProxyConfig.getStat())) {
            this.registryStatReproter();
        }
        if (!servantProxyConfig.isDirectConnection()) {
            this.registryServantNodeRefresher();
        }
    }

    private void registryStatReproter() {
        if (this.statReportFuture != null && !this.statReportFuture.isCancelled()) {
            this.statReportFuture.cancel(false);
        }
        if (!StringUtils.isEmpty(communicator.getCommunicatorConfig().getStat())) {
            int interval = servantProxyConfig.getReportInterval();
            int initialDelay = interval + (random.nextInt(30) * 1000);
            this.statReportFuture = ScheduledExecutorManager.getInstance().scheduleAtFixedRate(new ServantStatReproter(), initialDelay, interval, TimeUnit.MILLISECONDS);
        }
    }

    private void registryServantNodeRefresher() {
        if (this.queryRefreshFuture != null && !this.queryRefreshFuture.isCancelled()) {
            this.queryRefreshFuture.cancel(false);
        }
        if (!servantProxyConfig.isDirectConnection()) {
            int interval = servantProxyConfig.getRefreshInterval();
            int initialDelay = interval + (random.nextInt(30) * 1000);
            this.queryRefreshFuture = ScheduledExecutorManager.getInstance().scheduleAtFixedRate(new ServantNodeRefresher(), initialDelay, interval, TimeUnit.MILLISECONDS);
        }
    }

    private class ServantNodeRefresher implements Runnable {

        public void run() {
            long begin = System.currentTimeMillis();
            try {
                String nodes;
                if (RegisterManager.getInstance().getHandler() != null) {
                    nodes = ParseTools.parse(RegisterManager.getInstance().getHandler().query(servantProxyConfig.getSimpleObjectName()),
                            servantProxyConfig.getSimpleObjectName());
                } else {
                    nodes = communicator.getQueryHelper().getServerNodes(servantProxyConfig);
                }
                if (nodes != null && !nodes.equals(servantProxyConfig.getObjectName())) {
                    servantCacheManager.save(communicator.getId(), servantProxyConfig.getSimpleObjectName(), nodes, communicator.getCommunicatorConfig().getDataPath());
                    servantProxyConfig.setObjectName(nodes);
                    refresh();
                }
                ClientLogger.getLogger().debug(objName + " sync server|" + nodes);
            } catch (Throwable e) {
                ClientLogger.getLogger().error(objName + " error sync server", e);
            } finally {
                ClientLogger.getLogger().info("ServantNodeRefresher run(" + objName + "), use: " + (System.currentTimeMillis() - begin));
            }
        }
    }

    private class ServantStatReproter implements Runnable {

        public void run() {
            long begin = System.currentTimeMillis();
            try {
                communicator.getStatHelper().report(InvokeStatHelper.getInstance().getProxyStat(servantProxyConfig.getSimpleObjectName()));
            } catch (Exception e) {
                ClientLogger.getLogger().error("report stat worker error|" + servantProxyConfig.getSimpleObjectName(), e);
            } finally {
                ClientLogger.getLogger().info("ServantStatReproter run(), use: " + (System.currentTimeMillis() - begin));
            }
        }
    }
}