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

import java.lang.reflect.Method;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;
import com.qq.tars.server.apps.AppContextImpl;

public class ServantHomeSkeleton extends AppService {

    private AtomicInteger invokeNumbers = new AtomicInteger(0);

    private String name = null;

    private Object service = null;

    private Class<?> apiClass = null;

    private int maxLoadLimit = -1;

    private int minThreadPoolSize = -1;

    private int maxThreadPoolSize = -1;

    private int queueSize = -1;

    public ServantHomeSkeleton(String name, Object service, Class<?> apiClass, int loadLimit) {
        this.name = name;
        this.service = service;
        this.apiClass = apiClass;
        this.maxLoadLimit = loadLimit;
    }

    public Object getService() {
        return service;
    }

    public Object invoke(Method method, Object... args) throws Exception {
        Object value = null;
        Context<TarsServantRequest, TarsServantResponse> context = null;
        try {
            context = ContextManager.getContext();
            preInvokeCapHomeSkeleton(context);

//            if ("isAlive".equals(methodName) && method == null && (args == null || args.length == 0)) {
//                return Boolean.TRUE;
//            }

//            if (method == null) throw new RuntimeException("The serivce has no method named:" + methodName);

//            if (!enableOverload || methodID == 0) { // 服务器或客户端不支持重载
//                if (!badMethodMap.isEmpty() && badMethodMap.containsKey(method.getName())) {
//                    String x = enableOverload ? "client side's taserver-proxy too old, update it to fix this problem."//
//                    : "only TAS protocol support Method Overload, " + "remove @Service annotation to diable NONE TAS protocols and then Method Overload will be enabled automatically.";
//                    throw new UnsupportedOperationException("Method Overload Not Supported( " + x + "), method=" + methodName);
//                }
//            }
            value = method.invoke(this.service, fixParamValueType(method, args));
        } finally {
            if (!ContextManager.getContext().response().isAsyncMode()) {
                postInvokeCapHomeSkeleton(context);
            }
        }
        return value;
    }

    private Object[] fixParamValueType(Method method, Object args[]) {
        if (args == null || args.length == 0) return args;
        Class<?> parameterTypes[] = method.getParameterTypes();
        if (parameterTypes == null || parameterTypes.length == 0) return args;

        if (args.length != parameterTypes.length) return args;

        for (int i = 0; i < parameterTypes.length; i++) {
            args[i] = fixValueDataType(parameterTypes[i], args[i]);
        }

        return args;
    }

    private final Object fixValueDataType(Class<?> dataType, Object value) {
        Object dataValue = value;

        if (dataType != null && dataValue != null) {
            if ("short" == dataType.getName()) {
                dataValue = Short.valueOf(dataValue.toString());
            } else if ("byte" == dataType.getName()) {
                dataValue = Byte.valueOf(dataValue.toString());
            } else if (char.class == dataType) {
                dataValue = ((String) value).charAt(0);
            } else if ("float" == dataType.getName()) {
                dataValue = Float.valueOf(dataValue.toString());
            }
        }

        return dataValue;
    }

    public void preInvokeCapHomeSkeleton(Context<TarsServantRequest, TarsServantResponse> context) {
        if (this.maxLoadLimit == -1) {
            return;
        }
        this.invokeNumbers.incrementAndGet();
        if (this.invokeNumbers.intValue() > this.maxLoadLimit) {
            throw new RuntimeException(this.name + " is overload. limit=" + this.maxLoadLimit);
        }
    }

    public void postInvokeCapHomeSkeleton(Context<TarsServantRequest, ?> context) {
        if (this.maxLoadLimit == -1) {
            return;
        }
        this.invokeNumbers.decrementAndGet();
    }

    public Class<?> getApiClass() {
        return this.apiClass;
    }

    public int getMinThreadPoolSize() {
        return minThreadPoolSize;
    }

    public int getMaxThreadPoolSize() {
        return maxThreadPoolSize;
    }

    public int getQueueSize() {
        return queueSize;
    }

    public void setMinThreadPoolSize(int minThreadPoolSize) {
        this.minThreadPoolSize = minThreadPoolSize;
    }

    public void setMaxThreadPoolSize(int maxThreadPoolSize) {
        this.maxThreadPoolSize = maxThreadPoolSize;
    }

    public void setQueueSize(int queueSize) {
        this.queueSize = queueSize;
    }

    public String name() {
        return this.name;
    }

    private AppContextImpl appContext;

    public void setAppContext(AppContextImpl context) {
        appContext = context;
    }

    public AppContextImpl getAppContext() {
        return appContext;
    }
}
