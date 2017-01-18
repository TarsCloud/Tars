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

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import sun.misc.Unsafe;

public class BeanAccessor {

    private static Unsafe unsafe;
    private static Map<Class<?>, Map<String, Field>> beaninfoMap;
    private static Map<Class<?>, Map<String, Long>> beanFieldOffsetMap;
    public static String SERIALVERSIONUID = "serialVersionUID";
    public static String FINAL = "final";
    public static String STATIC = "static";

    public static Unsafe getUnsafeInstance() {
        if (unsafe != null) {
            return unsafe;
        }

        synchronized (BeanAccessor.class) {
            if (unsafe == null) {
                unsafe = getUnsafe();
            }
        }
        return unsafe;
    }

    public static Unsafe getUnsafe() {
        Unsafe value;
        try {
            Class<?> clazz = Class.forName("sun.misc.Unsafe");
            Field field = clazz.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            value = (Unsafe) field.get(null);
        } catch (Exception e) {
            throw new RuntimeException("error to get theUnsafe", e);
        }
        return value;
    }

    public static Map<String, Field> getBeanFieldMap(Class<?> clazz) {
        if (clazz == null) {
            return null;
        }
        if (beaninfoMap == null) {
            synchronized (BeanAccessor.class) {
                if (beaninfoMap == null) {
                    beaninfoMap = new HashMap<Class<?>, Map<String, Field>>();
                }
            }
        }

        if (beaninfoMap == null) {
            return null;
        }

        Map<String, Field> fieldMap = beaninfoMap.get(clazz);
        if (fieldMap == null) {
            synchronized (clazz) {
                if ((fieldMap = beaninfoMap.get(clazz)) == null) {
                    Field[] fields = clazz.getDeclaredFields();
                    fieldMap = new LinkedHashMap<String, Field>(fields.length);
                    for (Field field : fields) {
                        fieldMap.put(field.getName(), field);
                    }
                    beaninfoMap.put(clazz, fieldMap);
                }
            }
        }
        return fieldMap;
    }

    public static long getBeanFieldOffset(Class<?> clazz, String fieldName) {
        if (clazz == null || fieldName == null) {
            return -1;
        }
        if (beanFieldOffsetMap == null) {
            synchronized (BeanAccessor.class) {
                if (beanFieldOffsetMap == null) {
                    beanFieldOffsetMap = new HashMap<Class<?>, Map<String, Long>>();
                }
            }
        }

        Map<String, Long> offsetMap = beanFieldOffsetMap.get(clazz);
        if (offsetMap == null) {
            synchronized (clazz) {
                if ((offsetMap = beanFieldOffsetMap.get(clazz)) == null) {
                    Map<String, Field> fieldMap = getBeanFieldMap(clazz);
                    offsetMap = new HashMap<String, Long>(fieldMap.size());
                    for (String key : fieldMap.keySet()) {
                        Field field = fieldMap.get(key);
                        String modifiers = Modifier.toString(field.getModifiers());
                        if (field.getName().equals(SERIALVERSIONUID) || modifiers.contains(FINAL) || modifiers.contains(STATIC)) {
                            continue;
                        }
                        long offset = getUnsafeInstance().objectFieldOffset(field);
                        offsetMap.put(key, offset);
                    }
                    beanFieldOffsetMap.put(clazz, offsetMap);
                }
            }
        }

        Long result = offsetMap == null ? null : offsetMap.get(fieldName);
        return result == null ? -1 : result.longValue();
    }

    public static Object getBeanValue(Object bean, String fieldName) {
        if (bean == null || fieldName == null) {
            return null;
        }
        Class<?> clazz = bean.getClass();

        Map<String, Field> fieldMap = getBeanFieldMap(clazz);
        Field field = fieldMap.get(fieldName);
        Unsafe unsafe = getUnsafeInstance();
        if (unsafe == null) {
            try {
                if (!field.isAccessible()) {
                    field.setAccessible(true);
                }
                return field.get(bean);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        long offset = getBeanFieldOffset(clazz, fieldName);
        if (offset == -1) {
            return null;
        }

        Class<?> type = field.getType();
        if (type == boolean.class) {
            return unsafe.getBoolean(bean, offset);
        } else if (type == byte.class) {
            return unsafe.getByte(bean, offset);
        } else if (type == short.class) {
            return unsafe.getShort(bean, offset);
        } else if (type == char.class) {
            return unsafe.getChar(bean, offset);
        } else if (type == int.class) {
            return unsafe.getInt(bean, offset);
        } else if (type == long.class) {
            return unsafe.getLong(bean, offset);
        } else if (type == float.class) {
            return unsafe.getFloat(bean, offset);
        } else if (type == double.class) {
            return unsafe.getDouble(bean, offset);
        }
        return unsafe.getObject(bean, offset);

    }

    public static void setBeanValue(Object bean, String fieldName, Object data) {
        if (bean == null || fieldName == null) {
            return;
        }

        Class<?> clazz = bean.getClass();

        Map<String, Field> fieldMap = getBeanFieldMap(clazz);
        Field field = fieldMap.get(fieldName);
        Unsafe unsafe = getUnsafeInstance();
        if (unsafe == null) {
            try {
                field.set(bean, data);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        long offset = getBeanFieldOffset(clazz, fieldName);
        if (offset == -1) {
            return;
        }

        Class<?> type = field.getType();
        if (type == boolean.class) {
            unsafe.putBoolean(bean, offset, ((Boolean) data).booleanValue());
        } else if (type == byte.class) {
            unsafe.putByte(bean, offset, ((Byte) data).byteValue());
        } else if (type == short.class) {
            unsafe.putShort(bean, offset, ((Short) data).shortValue());
        } else if (type == char.class) {
            unsafe.putChar(bean, offset, ((Character) data).charValue());
        } else if (type == int.class) {
            unsafe.putInt(bean, offset, ((Integer) data).intValue());
        } else if (type == long.class) {
            unsafe.putLong(bean, offset, ((Long) data).longValue());
        } else if (type == float.class) {
            unsafe.putFloat(bean, offset, ((Float) data).floatValue());
        } else if (type == double.class) {
            unsafe.putDouble(bean, offset, ((Double) data).doubleValue());
        } else {
            unsafe.putObject(bean, offset, data);
        }
    }
}
