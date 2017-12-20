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

import java.lang.reflect.Constructor;


import com.qq.tars.client.rpc.loadbalance.DefaultLoadBalance;
import com.qq.tars.client.rpc.tars.TarsProtocolInvoker;
import com.qq.tars.client.support.ServantCacheManager;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.annotation.ServantCodec;
import com.qq.tars.rpc.common.LoadBalance;
import com.qq.tars.rpc.common.ProtocolInvoker;
import com.qq.tars.rpc.exc.ClientException;
import com.qq.tars.rpc.exc.CommunicatorConfigException;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.rpc.protocol.ServantProtocolFactory;
import com.qq.tars.rpc.protocol.tars.TarsCodec;

class ObjectProxyFactory {

    private final Communicator communicator;

    public ObjectProxyFactory(Communicator communicator) {
        this.communicator = communicator;
    }

    public <T> ObjectProxy<T> getObjectProxy(Class<T> api, String objName, ServantProxyConfig servantProxyConfig,
                                             LoadBalance<T> loadBalance, ProtocolInvoker<T> protocolInvoker) throws ClientException {
        if (servantProxyConfig == null) {
            servantProxyConfig = createServantProxyConfig(objName);
        } else {
            servantProxyConfig.setCommunicatorId(communicator.getId());
            servantProxyConfig.setLocator(communicator.getCommunicatorConfig().getLocator());
        }
        updateServantEndpoints(servantProxyConfig);

        if (loadBalance == null) {
            loadBalance = createLoadBalance(servantProxyConfig);
        }

        if (protocolInvoker == null) {
            protocolInvoker = createProtocolInvoker(api, objName, servantProxyConfig);
        }
        return new ObjectProxy<T>(api, objName, servantProxyConfig, loadBalance, protocolInvoker, communicator);
    }

    private <T> ProtocolInvoker<T> createProtocolInvoker(Class<T> api, String objName,
                                                         ServantProxyConfig servantProxyConfig) throws ClientException {
        ProtocolInvoker<T> protocolInvoker = null;
        Codec codec = createCodec(api, servantProxyConfig);
        if (api.isAnnotationPresent(Servant.class)) {
            if (codec == null) {
                codec = new TarsCodec(servantProxyConfig.getCharsetName());
            }
            servantProxyConfig.setProtocol(codec.getProtocol());
            protocolInvoker = new TarsProtocolInvoker<T>(api, servantProxyConfig, new ServantProtocolFactory(codec), communicator.getThreadPoolExecutor());
        } else {
            throw new ClientException(servantProxyConfig.getSimpleObjectName(), "unkonw protocol servant invoker", null);
        }
        return protocolInvoker;
    }

    private <T> LoadBalance<T> createLoadBalance(ServantProxyConfig servantProxyConfig) {
        return new DefaultLoadBalance<T>(servantProxyConfig);
    }

    private <T> Codec createCodec(Class<T> api, ServantProxyConfig servantProxyConfig) throws ClientException {
        Codec codec = null;
        ServantCodec servantCodec = api.getAnnotation(ServantCodec.class);
        if (servantCodec != null) {
            Class<? extends Codec> codecClass = servantCodec.codec();
            if (codecClass != null) {
                Constructor<? extends Codec> constructor;
                try {
                    constructor = codecClass.getConstructor(new Class[] { String.class });
                    codec = constructor.newInstance(servantProxyConfig.getCharsetName());
                } catch (Exception e) {
                    throw new ClientException(servantProxyConfig.getSimpleObjectName(), "error occurred on create codec, codec=" + codecClass.getName(), e);
                }
            }
        }
        return codec;
    }

    private ServantProxyConfig createServantProxyConfig(String objName) throws CommunicatorConfigException {
        CommunicatorConfig communicatorConfig = communicator.getCommunicatorConfig();
        ServantProxyConfig cfg = new ServantProxyConfig(communicator.getId(), communicatorConfig.getLocator(), objName);
        cfg.setAsyncTimeout(communicatorConfig.getAsyncInvokeTimeout());
        cfg.setSyncTimeout(communicatorConfig.getSyncInvokeTimeout());
        cfg.setEnableSet(communicatorConfig.isEnableSet());
        cfg.setSetDivision(communicatorConfig.getSetDivision());
        cfg.setModuleName(communicatorConfig.getModuleName());
        cfg.setStat(communicatorConfig.getStat());
        cfg.setCharsetName(communicatorConfig.getCharsetName());
        cfg.setConnections(communicatorConfig.getConnections());
        return cfg;
    }

    private void updateServantEndpoints(ServantProxyConfig cfg) {
        CommunicatorConfig communicatorConfig = communicator.getCommunicatorConfig();

        String endpoints = null;
        if (StringUtils.isNotEmpty(communicatorConfig.getLocator()) && !cfg.isDirectConnection() && !communicatorConfig.getLocator().startsWith(cfg.getSimpleObjectName())) {
            try {
                /** 从主控拉取server node */
                endpoints = communicator.getQueryHelper().getServerNodes(cfg);
                if (StringUtils.isEmpty(endpoints)) {
                    throw new CommunicatorConfigException(cfg.getSimpleObjectName(), "servant node is empty on get by registry! communicator id=" + communicator.getId());
                }
                ServantCacheManager.getInstance().save(communicator.getId(), cfg.getSimpleObjectName(), endpoints, communicatorConfig.getDataPath());
            } catch (CommunicatorConfigException e) {
                /** 如果失败，从本地绶存文件中拉取 */
                endpoints = ServantCacheManager.getInstance().get(communicator.getId(), cfg.getSimpleObjectName(), communicatorConfig.getDataPath());
                ClientLogger.getLogger().error(cfg.getSimpleObjectName() + " error occurred on get by registry, use by local cache=" + endpoints + "|" + e.getLocalizedMessage(), e);
            }

            if (StringUtils.isEmpty(endpoints)) {
                throw new CommunicatorConfigException(cfg.getSimpleObjectName(), "error occurred on create proxy, servant endpoint is empty! locator =" + communicatorConfig.getLocator() + "|communicator id=" + communicator.getId());
            }
            cfg.setObjectName(endpoints);
        }

        if (StringUtils.isEmpty(cfg.getObjectName())) {
            throw new CommunicatorConfigException(cfg.getSimpleObjectName(), "error occurred on create proxy, servant endpoint is empty!");
        }
    }
}
