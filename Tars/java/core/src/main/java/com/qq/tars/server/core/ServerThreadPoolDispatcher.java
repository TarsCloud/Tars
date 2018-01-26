///**
// * Tencent is pleased to support the open source community by making Tars available.
// *
// * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
// *
// * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// * in compliance with the License. You may obtain a copy of the License at
// *
// * https://opensource.org/licenses/BSD-3-Clause
// *
// * Unless required by applicable law or agreed to in writing, software distributed
// * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// * CONDITIONS OF ANY KIND, either express or implied. See the License for the
// * specific language governing permissions and limitations under the License.
// */
//
//package com.qq.tars.server.core;
//
//import java.util.HashMap;
//import java.util.concurrent.Executor;
//import java.util.concurrent.TimeUnit;
//
//import com.qq.tars.common.util.concurrent.TaskQueue;
//import com.qq.tars.common.util.concurrent.TaskThreadFactory;
//import com.qq.tars.common.util.concurrent.TaskThreadPoolExecutor;
//import com.qq.tars.net.core.Request;
//import com.qq.tars.net.core.nio.WorkThread;
//import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
//import com.qq.tars.server.apps.AppContextImpl;
//
//public class ServerThreadPoolDispatcher implements Executor {
//
//    private final static HashMap<String, Executor> threadExecutors = new HashMap<String, Executor>();
//
//    public void execute(Runnable command) {
//        getExecutor(command).execute(command);
//    }
//
//    private static Executor getExecutor(Runnable command) {
//        TarsServantRequest request = getPortalServiceRequest(command);
//
//        if (request == null) return getDefaultExecutor();
//
//        return getExecutor(request);
//    }
//
//    private static Executor getDefaultExecutor() {
//        Executor executor = threadExecutors.get(null);
//
//        if (executor == null) {
//            executor = createExecutor(null);
//        }
//
//        return executor;
//    }
//
//    private static Executor getExecutor(TarsServantRequest request) {
//        String service = request.getServantName();
//
//        Executor executor = threadExecutors.get(service);
//
//        if (executor == null) {
//            executor = createExecutor(request);
//        }
//
//        return executor;
//    }
//
//    private static synchronized Executor createDefaultExecutor(String key) {
//        Executor executor = threadExecutors.get(null);
//
//        if (executor != null) {
//            threadExecutors.put(key, executor);
//            return executor;
//        }
//
//        TaskQueue taskqueue = new TaskQueue(20000);
//        TaskThreadPoolExecutor pool = new TaskThreadPoolExecutor(5, 512, 120, TimeUnit.SECONDS, taskqueue, new TaskThreadFactory("taserverThreadPool-exec-"));
//        taskqueue.setParent(pool);
//        threadExecutors.put(null, pool);
//        threadExecutors.put(key, pool);
//
//        return pool;
//    }
//
//    private static synchronized Executor createExecutor(TarsServantRequest request) {
//        String key = null, contextName = null, serviceName = null;
//        Executor executor = null;
//
//        if (request == null) return createDefaultExecutor(null);
//
////        contextName = request.getWebappContext();
//        serviceName = request.getServantName();
//        key = contextName + '_' + serviceName;
//
//        executor = threadExecutors.get(key);
//        if (executor != null) return executor;
//
//        int minPoolSize = -1, maxPoolSize = -1, queueSize = -1;
//        AppContainer container = ContainerManager.getContainer(AppContainer.class);
//        AppContextImpl context = container.getDefaultAppContext();
//
//        // 客户端请求错误的app或service时，返回默认的executor.
//        if (context == null) {
//            return getDefaultExecutor();
//        }
//        ServantHomeSkeleton service = context.getCapHomeSkeleton(serviceName);
//        if (service == null) {
//            return getDefaultExecutor();
//        }
//
//        minPoolSize = service.getMinThreadPoolSize();
//        maxPoolSize = service.getMaxThreadPoolSize();
//        queueSize = service.getQueueSize();
//
//        if (minPoolSize < 0 || maxPoolSize < 0 || queueSize < 0) {
//            return createDefaultExecutor(key);
//        }
//
//        // ----------------------------------
//        TaskQueue taskqueue = new TaskQueue(queueSize);
//        TaskThreadPoolExecutor pool = new TaskThreadPoolExecutor(minPoolSize, maxPoolSize, 120, TimeUnit.SECONDS, taskqueue, new TaskThreadFactory("taserverThreadPool-exec-" + contextName + '.' + serviceName + '-'));
//        taskqueue.setParent(pool);
//        threadExecutors.put(key, pool);
//
//        return pool;
//    }
//
//    private static TarsServantRequest getPortalServiceRequest(Runnable command) {
//        if (!(command instanceof WorkThread)) return null;
//
//        WorkThread workThread = (WorkThread) command;
//        Request req = workThread.getRequest();
//
//        if (!(req instanceof TarsServantRequest)) return null;
//
//        return (TarsServantRequest) req;
//    }
//}
