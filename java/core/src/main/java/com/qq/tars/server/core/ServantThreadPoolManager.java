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

import java.util.HashMap;
import java.util.concurrent.Executor;
import java.util.concurrent.TimeUnit;

import com.qq.tars.common.util.concurrent.TaskQueue;
import com.qq.tars.common.util.concurrent.TaskThreadFactory;
import com.qq.tars.common.util.concurrent.TaskThreadPoolExecutor;
import com.qq.tars.server.config.ServantAdapterConfig;

public final class ServantThreadPoolManager {

    private final static HashMap<String, Executor> threadExecutors = new HashMap<String, Executor>();

    public static Executor get(ServantAdapterConfig config) {
        return getAndcreateExecutor(config);
    }

    private static synchronized Executor createDefaultExecutor(String key) {
        Executor executor = threadExecutors.get(null);

        if (executor != null) {
            threadExecutors.put(key, executor);
            return executor;
        }

        TaskQueue taskqueue = new TaskQueue(20000);
        TaskThreadPoolExecutor pool = new TaskThreadPoolExecutor(5, 512, 120, TimeUnit.SECONDS, taskqueue, new TaskThreadFactory("ServantThreadPool-exec-"));
        taskqueue.setParent(pool);
        threadExecutors.put(null, pool);
        threadExecutors.put(key, pool);
        return pool;
    }

    private static synchronized Executor getAndcreateExecutor(ServantAdapterConfig config) {
        String key = config.getHandleGroup();
        Executor executor = null;

        executor = threadExecutors.get(key);
        if (executor != null) return executor;

        int minPoolSize = -1, maxPoolSize = -1, queueSize = -1;

        minPoolSize = config.getThreads();
        maxPoolSize = config.getThreads();
        queueSize = config.getQueueCap();

        if (minPoolSize < 0 || maxPoolSize < 0 || queueSize < 0) {
            return createDefaultExecutor(key);
        }

        TaskQueue taskqueue = new TaskQueue(queueSize);
        TaskThreadPoolExecutor pool = new TaskThreadPoolExecutor(minPoolSize, maxPoolSize, 120, TimeUnit.SECONDS, taskqueue, new TaskThreadFactory("ServantThreadPool-exec-" + key + '-'));
        taskqueue.setParent(pool);
        threadExecutors.put(key, pool);
        return pool;
    }
}
