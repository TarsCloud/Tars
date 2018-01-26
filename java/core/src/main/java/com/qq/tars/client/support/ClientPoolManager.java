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

package com.qq.tars.client.support;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.qq.tars.client.CommunicatorConfig;
import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.rpc.ServantClient;
import com.qq.tars.common.util.concurrent.TaskQueue;
import com.qq.tars.common.util.concurrent.TaskThreadFactory;
import com.qq.tars.common.util.concurrent.TaskThreadPoolExecutor;
import com.qq.tars.net.core.nio.SelectorManager;
import com.qq.tars.net.protocol.ProtocolFactory;

public class ClientPoolManager {

    private final static ConcurrentHashMap<CommunicatorConfig, ThreadPoolExecutor> clientThreadPoolMap = new ConcurrentHashMap<CommunicatorConfig, ThreadPoolExecutor>();
    private final static ConcurrentHashMap<ServantProxyConfig, SelectorManager> selectorsMap = new ConcurrentHashMap<ServantProxyConfig, SelectorManager>();

    public static ThreadPoolExecutor getClientThreadPoolExecutor(CommunicatorConfig communicatorConfig) {
        ThreadPoolExecutor clientPoolExecutor = clientThreadPoolMap.get(communicatorConfig);
        if (clientPoolExecutor == null) {
            synchronized (ServantClient.class) {
                clientPoolExecutor = clientThreadPoolMap.get(communicatorConfig);
                if (clientPoolExecutor == null) {
                    clientThreadPoolMap.put(communicatorConfig, createThreadPool(communicatorConfig));
                    clientPoolExecutor = clientThreadPoolMap.get(communicatorConfig);
                }
            }
        }
        return clientPoolExecutor;
    }

    private static ThreadPoolExecutor createThreadPool(CommunicatorConfig communicatorConfig) {
        int corePoolSize = communicatorConfig.getCorePoolSize();
        int maxPoolSize = communicatorConfig.getMaxPoolSize();
        int keepAliveTime = communicatorConfig.getKeepAliveTime();
        int queueSize = communicatorConfig.getQueueSize();
        TaskQueue taskqueue = new TaskQueue(queueSize);

        String namePrefix = "tars-client-executor-";
        TaskThreadPoolExecutor executor = new TaskThreadPoolExecutor(corePoolSize, maxPoolSize, keepAliveTime, TimeUnit.SECONDS, taskqueue, new TaskThreadFactory(namePrefix));
        taskqueue.setParent(executor);
        return executor;
    }

    public static SelectorManager getSelectorManager(ProtocolFactory protocolFactory,
                                                     ThreadPoolExecutor threadPoolExecutor, boolean keepAlive,
                                                     boolean udpMode, ServantProxyConfig servantProxyConfig) throws IOException {
        SelectorManager selector = selectorsMap.get(servantProxyConfig);
        if (selector == null) {
            synchronized (selectorsMap) {
                selector = selectorsMap.get(servantProxyConfig);
                if (selector == null) {
                    int selectorPoolSize = convertInt(System.getProperty("com.qq.tars.net.client.selectorPoolSize"), 2);
                    selector = new SelectorManager(selectorPoolSize, protocolFactory, threadPoolExecutor, null, keepAlive, "servant-proxy-" + servantProxyConfig.getCommunicatorId(), udpMode);
                    selector.start();
                    selectorsMap.put(servantProxyConfig, selector);
                }
            }
        }
        return selector;
    }

    private static int convertInt(String value, int defaults) {
        if (value == null) {
            return defaults;
        }
        try {
            return Integer.parseInt(value);
        } catch (Exception e) {
            return defaults;
        }
    }
}
