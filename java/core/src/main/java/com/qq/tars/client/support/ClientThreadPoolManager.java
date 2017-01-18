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

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.qq.tars.client.CommunicatorConfig;
import com.qq.tars.client.rpc.ServantClient;
import com.qq.tars.common.util.concurrent.TaskQueue;
import com.qq.tars.common.util.concurrent.TaskThreadFactory;
import com.qq.tars.common.util.concurrent.TaskThreadPoolExecutor;

public class ClientThreadPoolManager {

    private static ConcurrentHashMap<String, ThreadPoolExecutor> clientThreadPoolMap = new ConcurrentHashMap<String, ThreadPoolExecutor>();

    public static ThreadPoolExecutor getClientThreadPoolExecutor(CommunicatorConfig communicatorConfig) {
        String contextIdentity = resolveCurrentContextIdentity();
        ThreadPoolExecutor clientPoolExecutor = clientThreadPoolMap.get(contextIdentity);
        if (clientPoolExecutor == null) {
            synchronized (ServantClient.class) {
                clientPoolExecutor = clientThreadPoolMap.get(contextIdentity);
                if (clientPoolExecutor == null) {
                    clientThreadPoolMap.put(contextIdentity, createThreadPool(communicatorConfig));
                    clientPoolExecutor = clientThreadPoolMap.get(contextIdentity);
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

    private static String resolveCurrentContextIdentity() {
        Object slefClassLoader = ClientThreadPoolManager.class.getClassLoader();
        Object contextClassLoader = Thread.currentThread().getContextClassLoader();
        String contextIdentity = ClientThreadPoolManager.class.getName();
        if (slefClassLoader != null && contextClassLoader != null && slefClassLoader != contextClassLoader) {
            contextIdentity = contextClassLoader.toString();
        }
        return contextIdentity;
    }
}
