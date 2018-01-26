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

package com.qq.tars.common.util;

import java.util.Collection;
import java.util.Map;

@SuppressWarnings("unchecked")
public class CommonUtils {

    public static <T> T newInstance(Class<?> clazz) {
        try {
            return (T) clazz.newInstance();
        } catch (Exception e) {
            throw new RuntimeException("instance Class: " + clazz.getName() + " with ex: " + e.getMessage(), e);
        }
    }

    public static <T> T newInstance(String className) {
        try {
            return (T) Class.forName(className).newInstance();
        } catch (Exception e) {
            throw new RuntimeException("instance Class: " + className + " with ex: " + e.getMessage(), e);
        }
    }

    public static Class<?> forName(String className) {
        try {
            return Class.forName(className);
        } catch (Exception e) {
            throw new RuntimeException("forName Class: " + className + " with ex: " + e.getMessage(), e);
        }
    }

    public static boolean isEmpty(String input) {
        return null == input || input.trim().isEmpty();
    }

    public static boolean isEmptyArray(Object[] array) {
        return null == array || array.length == 0;
    }

    public static boolean isEmptyCollection(Collection<?> collection) {
        return collection == null || collection.isEmpty();
    }

    public static boolean isEmptyMap(Map<?, ?> map) {
        return map == null || map.isEmpty();
    }

    public static boolean isJavaBase(Class<?> clazz) {
        return clazz == boolean.class || clazz == Boolean.class || clazz == byte.class || clazz == Byte.class || clazz == short.class || clazz == Short.class || clazz == int.class || clazz == Integer.class || clazz == long.class || clazz == Long.class || clazz == float.class || clazz == Float.class || clazz == double.class || clazz == Double.class || clazz == String.class;
    }

    public static int getPoolSize() {
        int processors = Runtime.getRuntime().availableProcessors();
        return processors > 8 ? 4 + (processors * 5 / 8) : processors + 1;
    }
}
