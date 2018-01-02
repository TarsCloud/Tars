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

package com.qq.tars.rpc.protocol.proto;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.concurrent.ConcurrentHashMap;

public class MethodCache {
    private ConcurrentHashMap<Class, Method> cache = new ConcurrentHashMap<Class, Method>();

    private static final MethodCache instance = new MethodCache();

    public static MethodCache getInstance() {
        return instance;
    }

    private MethodCache() {}

    public Object parseFrom(Class clazz, byte[] data) throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Method method = this.cache.get(clazz);
        if (method == null) {
            method = clazz.getMethod("parseFrom", byte[].class);
            method.setAccessible(true);
            this.cache.putIfAbsent(clazz, method);
            method = this.cache.get(clazz);
        }
        return method.invoke(null, data);
    }
}
