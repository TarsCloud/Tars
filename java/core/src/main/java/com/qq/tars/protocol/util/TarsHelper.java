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

package com.qq.tars.protocol.util;

import java.lang.annotation.Annotation;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.GenericArrayType;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qq.tars.common.support.Holder;
import com.qq.tars.common.util.BeanAccessor;
import com.qq.tars.common.util.CommonUtils;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.protocol.tars.annotation.TarsCallback;
import com.qq.tars.protocol.tars.annotation.TarsContext;
import com.qq.tars.protocol.tars.annotation.TarsHolder;
import com.qq.tars.protocol.tars.annotation.TarsMethod;
import com.qq.tars.protocol.tars.annotation.TarsMethodParameter;
import com.qq.tars.protocol.tars.annotation.TarsRouteKey;
import com.qq.tars.protocol.tars.annotation.TarsStruct;
import com.qq.tars.protocol.tars.annotation.TarsStructProperty;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.tars.support.TarsMethodParameterInfo;
import com.qq.tars.protocol.tars.support.TarsStructInfo;
import com.qq.tars.protocol.tars.support.TarsStrutPropertyInfo;

public class TarsHelper {

    public final static int PACKAGE_MAX_LENGTH = 10 * 1024 * 1024;
    public final static int HEAD_SIZE = 4;

    public static final short VERSION = 0x01;

    public static final short VERSION2 = 0x02;

    public static final short VERSION3 = 0x03;

    public static final byte NORMAL = 0x00;

    public static final byte ONEWAY = 0x01;

    public final static int SERVERSUCCESS = 0;

    public final static int SERVERDECODEERR = -1;

    public final static int SERVERENCODEERR = -2;

    public final static int SERVERNOFUNCERR = -3;

    public final static int SERVERNOSERVANTERR = -4;

    public final static int SERVERRESETGRID = -5;

    public final static int SERVERQUEUETIMEOUT = -6;

    public final static int ASYNCCALLTIMEOUT = -7;

    public final static int PROXYCONNECTERR = -8;

    public static final int SERVEROVERLOAD = -9;

    public final static int SERVERUNKNOWNERR = -99;

    public static final int MESSAGETYPENULL = 0x00;
    public static final int MESSAGETYPEHASH = 0x01;
    public static final int MESSAGETYPEGRID = 0x02;
    public static final int MESSAGETYPEDYED = 0x04;
    public static final int MESSAGETYPESAMPLE = 0x08;
    public static final int MESSAGETYPEASYNC = 0x10;
    public static final int MESSAGETYPELOADED = 0x20;

    public static final String STATUS_RESULT_CODE = "STATUS_RESULT_CODE";

    public static final String STATUS_RESULT_DESC = "STATUS_RESULT_DESC";

    public static final Boolean STAMP_BOOLEAN = Boolean.TRUE;
    public static final Byte STAMP_BYTE = Byte.valueOf((byte) 0);
    public static final Short STAMP_SHORT = Short.valueOf((short) 0);
    public static final Float STAMP_FLOAT = Float.valueOf(0);
    public static final Integer STAMP_INT = Integer.valueOf(0);
    public static final Long STAMP_LONG = Long.valueOf(0);
    public static final Double STAMP_DOUBLE = Double.valueOf(0);
    public static final String STAMP_STRING = "";

    public static final boolean[] STAMP_BOOLEAN_ARRAY = new boolean[] { true };
    public static final byte[] STAMP_BYTE_ARRAY = new byte[] { 0 };
    public static final short[] STAMP_SHORT_ARRAY = new short[] { 0 };
    public static final int[] STAMP_INT_ARRAY = new int[] { 0 };
    public static final long[] STAMP_LONG_ARRAY = new long[] { 0 };
    public static final float[] STAMP_FLOAT_ARRAY = new float[] { 0 };
    public static final double[] STAMP_DOUBLE_ARRAY = new double[] { 0 };
    public static final Map<String, String> STAMP_MAP = new HashMap<String, String>();

    static {
        STAMP_MAP.put("", "");
    }

    private static Map<Class<?>, TarsStructInfo> tarsStructCache = new HashMap<Class<?>, TarsStructInfo>();

    private static Comparator<Field> tarsStructFieldsListComparator = new Comparator<Field>() {

        @Override
        public int compare(Field one, Field other) {
            TarsStructProperty oneProperty = one.getAnnotation(TarsStructProperty.class);
            TarsStructProperty otherProperty = other.getAnnotation(TarsStructProperty.class);
            if (oneProperty.order() == otherProperty.order()) {
                throw new RuntimeException("Field[" + one.getName() + "] , Field[" + other.getName() + "] order is:" + oneProperty.order());
            }
            return oneProperty.order() - otherProperty.order();
        }
    };

    private static Map<Class<?>, Object> stampCache = new HashMap<Class<?>, Object>();

    public static Object getJavaBaseOrArrayOrJavaBeanStamp(Class<?> clazz) {
        if (clazz == boolean.class || clazz == Boolean.class) {
            return STAMP_BOOLEAN;
        } else if (clazz == byte.class || clazz == Byte.class) {
            return STAMP_BYTE;
        } else if (clazz == short.class || clazz == Short.class) {
            return STAMP_SHORT;
        } else if (clazz == int.class || clazz == Integer.class) {
            return STAMP_INT;
        } else if (clazz == long.class || clazz == Long.class) {
            return STAMP_LONG;
        } else if (clazz == float.class || clazz == Float.class) {
            return STAMP_FLOAT;
        } else if (clazz == double.class || clazz == Double.class) {
            return STAMP_DOUBLE;
        } else if (clazz == String.class) {
            return STAMP_STRING;
        } else if (clazz == boolean[].class) {
            return STAMP_BOOLEAN_ARRAY;
        } else if (clazz == byte[].class) {
            return STAMP_BYTE_ARRAY;
        } else if (clazz == short[].class) {
            return STAMP_SHORT_ARRAY;
        } else if (clazz == int[].class) {
            return STAMP_INT_ARRAY;
        } else if (clazz == long[].class) {
            return STAMP_LONG_ARRAY;
        } else if (clazz == float[].class) {
            return STAMP_FLOAT_ARRAY;
        } else if (clazz == double[].class) {
            return STAMP_DOUBLE_ARRAY;
        }

        Object stamp = stampCache.get(clazz);
        if (stamp == null) {
            if (clazz.isArray()) {
                Class<?> componentType = clazz.getComponentType();
                Object[] array = (Object[]) Array.newInstance(componentType, 1);
                Object e = getJavaBaseOrArrayOrJavaBeanStamp(componentType);
                array[0] = e;
                stamp = array;
            } else {
                stamp = CommonUtils.newInstance(clazz);
            }
            stampCache.put(clazz, stamp);
        }
        return stamp;
    }

    public static String getParameterName(Annotation[] annotations) {
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsMethodParameter.class) {
                TarsMethodParameter parameter = (TarsMethodParameter) annotation;
                return parameter.name();
            }
        }
        return null;
    }

    public static Object getParameterStamp(Type type) {
        if (type instanceof Class<?>) {
            Class<?> clazz = (Class<?>) type;
            if (CommonUtils.isJavaBase(clazz) || clazz.isArray() || isStruct(clazz)) {
                return getJavaBaseOrArrayOrJavaBeanStamp((Class<?>) type);
            } else {
                return clazz;
//                throw new RuntimeException("the class: " + clazz + " not a exact class, please check it.");
            }
        } else if (type instanceof ParameterizedType) {
            ParameterizedType parameterizedType = (ParameterizedType) type;
            Class<?> clazz = (Class<?>) parameterizedType.getRawType();
            if (isStruct(clazz)) {
                return getJavaBaseOrArrayOrJavaBeanStamp((Class<?>) type);
            } else if (isMap(clazz)) {
                Type[] types = parameterizedType.getActualTypeArguments();
                Type keyType = types[0];
                Type valueType = types[1];

                Object key = getParameterStamp(keyType);
                Object value = getParameterStamp(valueType);
                Map<Object, Object> map = new HashMap<Object, Object>(1);
                map.put(key, value);
                return map;
            } else if (isCollection(clazz)) {
                Type[] types = parameterizedType.getActualTypeArguments();
                Type valueType = types[0];
                Object e = getParameterStamp(valueType);
                List<Object> list = new ArrayList<Object>(1);
                list.add(e);
                return list;
            } else if (isHolder(clazz)) {
                Type[] types = parameterizedType.getActualTypeArguments();
                if (Holder.class == clazz) {
                    return getParameterStamp(types[0]);
                } else {
                    throw new RuntimeException("getStamp for Holder Not Implement Yet, parameterizedType=" + parameterizedType);
                }
            }
        } else if (type instanceof GenericArrayType) {
            GenericArrayType genericArrayType = (GenericArrayType) type;
            Type componentType = genericArrayType.getGenericComponentType();
            Object component = getParameterStamp(componentType);
            Object[] array = (Object[]) Array.newInstance(component.getClass(), 1);
            array[0] = component;
            return array;
        }
        throw new RuntimeException("Generic Type: " + type + " no permission, please check it.");
    }

    public static TarsMethodParameterInfo createParameterInfo(Type genericParameterType) {
        if (genericParameterType == null) {
            throw new NullPointerException("genericParameterType is null.");
        }

        TarsMethodParameterInfo parameterInfo = new TarsMethodParameterInfo();
        Object stamp = getParameterStamp(genericParameterType);
        parameterInfo.setStamp(stamp);
        return parameterInfo;
    }

    public static TarsStructInfo getStructInfo(Class<?> clazz) {
        TarsStructInfo tarsStructInfo = tarsStructCache.get(clazz);
        if (tarsStructInfo == null) {
            synchronized (TarsHelper.class) {
                tarsStructInfo = tarsStructCache.get(clazz);
                if (tarsStructInfo == null) {
                    tarsStructInfo = new TarsStructInfo();
                    Field[] fields = clazz.getDeclaredFields();
                    if (!CommonUtils.isEmptyArray(fields)) {
                        List<Field> fieldList = new ArrayList<Field>(fields.length);

                        for (Field field : fields) {
                            if (field.isAnnotationPresent(TarsStructProperty.class)) fieldList.add(field);
                        }

                        try {
                            Collections.sort(fieldList, tarsStructFieldsListComparator);
                        } catch (Exception e) {
                            throw new RuntimeException("class[" + clazz + "] , Annotation StructProperty order error: " + e.getMessage(), e);
                        }

                        List<TarsStrutPropertyInfo> propertyList = new ArrayList<TarsStrutPropertyInfo>(fieldList.size());
                        int order = 0;
                        Object bean = CommonUtils.newInstance(clazz);
                        for (Field field : fieldList) {
                            TarsStrutPropertyInfo propertyInfo = new TarsStrutPropertyInfo();
                            Type type = field.getGenericType();
                            Object stamp = null;
                            try {
                                stamp = getParameterStamp(type);
                            } catch (Exception e) {
                                throw new RuntimeException("class[" + clazz + "] , Field[" + field.getName() + "] create stamp failed:" + e.getMessage(), e);
                            }
                            propertyInfo.setStamp(stamp);
                            propertyInfo.setName(field.getName());

                            TarsStructProperty propertyAnnotation = field.getAnnotation(TarsStructProperty.class);
                            order = propertyAnnotation.order();
                            propertyInfo.setOrder(order);
                            propertyInfo.setRequire(propertyAnnotation.isRequire());
                            propertyInfo.setComment(propertyAnnotation.comment());

                            propertyInfo.setDefaultValue(getPropertyDefaultValue(BeanAccessor.getBeanValue(bean, field.getName())));

                            propertyList.add(propertyInfo);
                        }
                        tarsStructInfo.setPropertyList(propertyList);
                        TarsStruct struct = clazz.getAnnotation(TarsStruct.class);
                        if (struct != null) {
                            String comment = struct.comment();
                            if (!CommonUtils.isEmpty(comment)) {
                                tarsStructInfo.setComment(comment);
                            }
                        }
                    }
                    tarsStructCache.put(clazz, tarsStructInfo);
                }
            }
        }

        return tarsStructInfo;
    }

    public static Map<Method, TarsMethodInfo> getMethodInfo(Class<?> api, String objectName) {
        Method[] methods = api.getDeclaredMethods();
        Map<Method, TarsMethodInfo> methodMap = new HashMap<Method, TarsMethodInfo>(methods.length);
        for (Method method : methods) {
            if (method == null) {
                continue;
            }

            TarsMethodInfo methodInfo = getMethodInfo(api, method, objectName);
            if (methodInfo != null) {
                methodMap.put(method, methodInfo);
            }
        }
        return methodMap;
    }

    public static TarsMethodInfo getMethodInfo(Class<?> api, Method method, String objectName) {
        if (!isServant(api) && !isCallback(api)) {
            return null;
        }

        TarsMethodInfo methodInfo = new TarsMethodInfo();
        methodInfo.setServiceName(objectName);
        methodInfo.setMethod(method);

        Type[] genericParameterTypes = method.getGenericParameterTypes();
        TarsMethod tarsMethod = method.getAnnotation(TarsMethod.class);
        if (tarsMethod != null && !CommonUtils.isEmpty(tarsMethod.comment())) {
            methodInfo.setComment(tarsMethod.comment());
        }
        List<TarsMethodParameterInfo> parametersList = new ArrayList<TarsMethodParameterInfo>(genericParameterTypes.length);
        methodInfo.setParametersList(parametersList);
        int order = 0;
        Annotation[][] allParameterAnnotations = method.getParameterAnnotations();
        for (Type genericParameterType : genericParameterTypes) {
            String name = getParameterName(allParameterAnnotations[order]);
            if (name == null) {
                name = "args" + order;
            }
            TarsMethodParameterInfo parameterInfo = new TarsMethodParameterInfo();
            parametersList.add(parameterInfo);
            try {
                parameterInfo.setName(name);
                parameterInfo.setOrder(isAsync(method.getName()) ? order : order + 1);
                parameterInfo.setAnnotations(allParameterAnnotations[order]);
                if (!isCallback(allParameterAnnotations[order])) {
                    parameterInfo.setStamp(TarsHelper.getParameterStamp(genericParameterType));
                }
                
                if (isRoutekey(allParameterAnnotations[order])) {
                	methodInfo.setRouteKeyIndex(order);
                }
            } catch (Exception e) {
                throw new RuntimeException("failed to create parameter info:" + method + ", index=[" + order + "]", e);
            }
            order++;
        }

        Type returnType = method.getGenericReturnType();
        if (returnType != void.class) {
            TarsMethodParameterInfo returnInfo = new TarsMethodParameterInfo();
            returnInfo.setStamp(TarsHelper.getParameterStamp(returnType));
            returnInfo.setName("result");
            returnInfo.setOrder(0);
            methodInfo.setReturnInfo(returnInfo);
        }
        return methodInfo;
    }

    public static boolean isAsync(String methodName) {
        return methodName != null && methodName.startsWith("async_");
    }

    public static boolean isPing(String methodName) {
        return methodName != null && methodName.equals("tars_ping");
    }

    public static boolean isHolder(Annotation[] annotations) {
        if (annotations == null || annotations.length < 0) {
            return false;
        }
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsHolder.class) {
                return true;
            }
        }
        return false;
    }

    public static String getHolderName(Annotation[] annotations) {
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsHolder.class) {
                TarsHolder holder = (TarsHolder) annotation;
                return holder.name();
            }
        }
        return null;
    }

    public static boolean isCallback(Annotation[] annotations) {
        if (annotations == null || annotations.length < 0) {
            return false;
        }
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsCallback.class) {
                return true;
            }
        }
        return false;
    }

    public static boolean isParameter(Annotation[] annotations) {
        if (annotations == null || annotations.length < 0) {
            return false;
        }
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsMethodParameter.class) {
                return true;
            }
        }
        return false;
    }

    public static boolean isContext(Annotation[] annotations) {
        if (annotations == null || annotations.length < 0) {
            return false;
        }
        for (Annotation annotation : annotations) {
            if (annotation.annotationType() == TarsContext.class) {
                return true;
            }
        }
        return false;
    }
    
    public static boolean isRoutekey(Annotation[] annotations) {
    	if (annotations == null || annotations.length < 0) {
    		return false;
    	}
    	for (Annotation annotation : annotations) {
    		if (annotation.annotationType() ==TarsRouteKey.class) {
    			return true;
    		}
    	}
    	return false;
    }

    public static boolean isStruct(Class<?> clazz) {
        boolean isStruct = clazz.isAnnotationPresent(TarsStruct.class);
        if (isStruct) {
            getStructInfo(clazz);
        }
        return isStruct;
    }

    public static boolean isServant(Class<?> apiClass) {
        return apiClass.isAnnotationPresent(Servant.class);
    }

    public static boolean isCallback(Class<?> apiClass) {
        return apiClass.isAnnotationPresent(TarsCallback.class);
    }

    public static boolean isHolder(Class<?> clazz) {
        return Holder.class.isAssignableFrom(clazz);
    }

    public static boolean isMap(Class<?> clazz) {
        return Map.class.isAssignableFrom(clazz);
    }

    public static boolean isVector(Class<?> clazz) {
        return clazz.isArray() || isCollection(clazz);
    }

    public static boolean isCollection(Class<?> clazz) {
        return Collection.class.isAssignableFrom(clazz);
    }

    public static String getDataType(Class<?> clazz) {
        if (clazz == boolean.class || clazz == Boolean.class) {
            return "bool";
        } else if (clazz == byte.class || clazz == Byte.class) {
            return "byte";
        } else if (clazz == short.class || clazz == Short.class) {
            return "short";
        } else if (clazz == int.class || clazz == Integer.class) {
            return "int";
        } else if (clazz == long.class || clazz == Long.class) {
            return "long";
        } else if (clazz == float.class || clazz == Float.class) {
            return "float";
        } else if (clazz == double.class || clazz == Double.class) {
            return "double";
        } else if (clazz == String.class) {
            return "string";
        } else if (isStruct(clazz)) {
            return clazz.getSimpleName();
        } else if (isCollection(clazz) || clazz.isArray()) {
            return "vector";
        } else if (isMap(clazz)) {
            return "map";
        }
        return null;
    }

    public static Object getJavaBaseArrayType(Object stamp) {
        if (STAMP_BOOLEAN_ARRAY == stamp) {
            return STAMP_BOOLEAN;
        } else if (STAMP_BYTE_ARRAY == stamp) {
            return STAMP_BYTE;
        } else if (STAMP_SHORT_ARRAY == stamp) {
            return STAMP_SHORT;
        } else if (STAMP_INT_ARRAY == stamp) {
            return STAMP_INT;
        } else if (STAMP_LONG_ARRAY == stamp) {
            return STAMP_LONG;
        } else if (STAMP_FLOAT_ARRAY == stamp) {
            return STAMP_FLOAT;
        } else if (STAMP_DOUBLE_ARRAY == stamp) {
            return STAMP_DOUBLE;
        }
        return null;
    }

    public static Object getPropertyDefaultValue(Object value) {
        if (value == null) {
            return null;
        } else if (value instanceof Integer && ((Integer) value).intValue() != 0) {
            return value;
        } else if (value instanceof Long && ((Long) value).intValue() != 0) {
            return value;
        } else if (value instanceof Double && ((Double) value).intValue() != 0) {
            return value;
        } else if (value instanceof Float && ((Float) value).intValue() != 0) {
            return value;
        } else if (value instanceof Short && ((Short) value).intValue() != 0) {
            return value;
        } else if (value instanceof Byte && ((Byte) value).intValue() != 0) {
            return value;
        } else if (value instanceof Boolean && ((Boolean) value).booleanValue() != false) {
            return value;
        } else if (value instanceof String) {
            return value;
        }
        return null;
    }

    public static void setHolderValue(Object holder, Object value) throws Exception {
        BeanAccessor.setBeanValue(holder, "value", value);
    }

    public static Object getHolderValue(Object holder) throws Exception {
        return BeanAccessor.getBeanValue(holder, "value");
    }
}
