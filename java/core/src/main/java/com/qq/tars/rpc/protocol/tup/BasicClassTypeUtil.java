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

package com.qq.tars.rpc.protocol.tup;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class BasicClassTypeUtil {

    private static void addType(ArrayList<String> list, String type) {
        int point = type.length();
        while (type.charAt(point - 1) == '>') {
            point--;
            if (point == 0) {
                break;
            }
        }
        list.add(0, uni2JavaType(type.substring(0, point)));
    }

    public static ArrayList<String> getTypeList(String fullType) {
        ArrayList<String> type = new ArrayList<String>();
        int point = 0;
        int splitPoint = fullType.indexOf("<");
        int mapPoint = -1;
        while (point < splitPoint) {
            addType(type, fullType.substring(point, splitPoint));
            point = splitPoint + 1;
            splitPoint = fullType.indexOf("<", point);
            mapPoint = fullType.indexOf(",", point);
            if (splitPoint == -1) {
                splitPoint = mapPoint;
            }
            if (mapPoint != -1 && mapPoint < splitPoint) {
                splitPoint = mapPoint;
            }
        }
        addType(type, fullType.substring(point, fullType.length()));
        return type;
    }

    public static void main(String[] args) {
        ArrayList<String> src = new ArrayList<String>();
        src.add("char");
        src.add("list<char>");
        src.add("list<list<char>>");
        src.add("map<short,string>");
        src.add("map<double,map<float,list<bool>>>");
        src.add("map<int64,list<Test.UserInfo>>");
        src.add("map<short,Test.FriendInfo>");

        for (String ss : src) {
            ArrayList<String> list = getTypeList(ss);
            for (String s : list) {
                System.out.println(s);
            }
            Collections.reverse(list);
            System.out.println("-------------finished " + transTypeList(list));
        }
    }

    public static String transTypeList(ArrayList<String> listTpye) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < listTpye.size(); i++) {
            listTpye.set(i, java2UniType(listTpye.get(i)));
        }
        Collections.reverse(listTpye);
        for (int i = 0; i < listTpye.size(); i++) {
            String type = listTpye.get(i);
            if (type.equals("list")) {
                listTpye.set(i - 1, "<" + listTpye.get(i - 1));
                listTpye.set(0, listTpye.get(0) + ">");
            } else if (type.equals("map")) {
                listTpye.set(i - 1, "<" + listTpye.get(i - 1) + ",");
                listTpye.set(0, listTpye.get(0) + ">");
            } else if (type.equals("Array")) {
                listTpye.set(i - 1, "<" + listTpye.get(i - 1));
                listTpye.set(0, listTpye.get(0) + ">");
            }
        }
        Collections.reverse(listTpye);
        for (String s : listTpye) {
            sb.append(s);
        }
        return sb.toString();
    }

    public static Object createClassByUni(String className) throws ObjectCreateException {
        ArrayList<String> list = getTypeList(className);
        Object last = null;
        Object last2 = null;
        Object returnObject = null;
        for (String name : list) {
            returnObject = createClassByName(name);

            if (returnObject instanceof String) {
                if ("Array".equals((String) (returnObject))) {
                    if (last == null) {
                        returnObject = Array.newInstance(Byte.class, 0);
                    }
                } else if ("?".equals((String) (returnObject))) {

                } else {
                    if (last == null) {
                        last = returnObject;
                    } else {
                        last2 = last;
                        last = returnObject;
                    }
                }
            } else if (returnObject instanceof List) {
                if (null != last && last instanceof Byte) {
                    returnObject = Array.newInstance(Byte.class, 1);
                    Array.set(returnObject, 0, last);
                } else {
                    if (last != null) {
                        ((List) returnObject).add(last);
                    } else {

                    }
                    last = null;
                }
            } else if (returnObject instanceof Map) {
                if (last != null & last2 != null) {
                    ((Map) returnObject).put(last, last2);
                } else {

                }
                last = null;
                last2 = null;
            } else {
                if (last == null) {
                    last = returnObject;
                } else {
                    last2 = last;
                    last = returnObject;
                }
            }
        }
        return returnObject;
    }

    public static Object createClassByName(String name) throws ObjectCreateException {
        if (name.equals("java.lang.Integer")) {
            return 0;
        } else if (name.equals("java.lang.Boolean")) {
            return false;
        } else if (name.equals("java.lang.Byte")) {
            return (byte) 0;
        } else if (name.equals("java.lang.Double")) {
            return (double) 0;
        } else if (name.equals("java.lang.Float")) {
            return (float) 0;
        } else if (name.equals("java.lang.Long")) {
            return (long) 0;
        } else if (name.equals("java.lang.Short")) {
            return (short) 0;
        } else if (name.equals("java.lang.Character")) {
            throw new java.lang.IllegalArgumentException("can not support java.lang.Character");
        } else if (name.equals("java.lang.String")) {
            return "";
        } else if (name.equals("java.util.List")) {
            return new ArrayList();
        } else if (name.equals("java.util.Map")) {
            return new HashMap();
        } else if (name.equals("Array")) {
            return "Array";
        } else if (name.equals("?")) {
            return name;
        } else {
            try {
                Class newoneClass = Class.forName(name);
                Constructor cons = newoneClass.getConstructor();
                return cons.newInstance();
            } catch (Exception e) {
                throw new ObjectCreateException(e);
            }
        }
    }

    public static String java2UniType(String srcType) {
        if (srcType.equals("java.lang.Integer") || srcType.equals("int")) {
            return "int32";
        } else if (srcType.equals("java.lang.Boolean") || srcType.equals("boolean")) {
            return "bool";
        } else if (srcType.equals("java.lang.Byte") || srcType.equals("byte")) {
            return "char";
        } else if (srcType.equals("java.lang.Double") || srcType.equals("double")) {
            return "double";
        } else if (srcType.equals("java.lang.Float") || srcType.equals("float")) {
            return "float";
        } else if (srcType.equals("java.lang.Long") || srcType.equals("long")) {
            return "int64";
        } else if (srcType.equals("java.lang.Short") || srcType.equals("short")) {
            return "short";
        } else if (srcType.equals("java.lang.Character")) {
            throw new java.lang.IllegalArgumentException("can not support java.lang.Character");
        } else if (srcType.equals("java.lang.String")) {
            return "string";
        } else if (srcType.equals("java.util.List")) {
            return "list";
        } else if (srcType.equals("java.util.Map")) {
            return "map";
        } else {
            return srcType;
        }
    }

    public static String uni2JavaType(String srcType) {
        if (srcType.equals("int32")) {
            return "java.lang.Integer";
        } else if (srcType.equals("bool")) {
            return "java.lang.Boolean";
        } else if (srcType.equals("char")) {
            return "java.lang.Byte";
        } else if (srcType.equals("double")) {
            return "java.lang.Double";
        } else if (srcType.equals("float")) {
            return "java.lang.Float";
        } else if (srcType.equals("int64")) {
            return "java.lang.Long";
        } else if (srcType.equals("short")) {
            return "java.lang.Short";
        } else if (srcType.equals("string")) {
            return "java.lang.String";
        } else if (srcType.equals("list")) {
            return "java.util.List";
        } else if (srcType.equals("map")) {
            return "java.util.Map";
        } else {
            return srcType;
        }
    }

    public static boolean isBasicType(String name) {
        if (name.equals("int")) {
            return true;
        } else if (name.equals("boolean")) {
            return true;
        } else if (name.equals("byte")) {
            return true;
        } else if (name.equals("double")) {
            return true;
        } else if (name.equals("float")) {
            return true;
        } else if (name.equals("long")) {
            return true;
        } else if (name.equals("short")) {
            return true;
        } else if (name.equals("char")) {
            return true;
        } else if (name.equals("Integer")) {
            return true;
        } else if (name.equals("Boolean")) {
            return true;
        } else if (name.equals("Byte")) {
            return true;
        } else if (name.equals("Double")) {
            return true;
        } else if (name.equals("Float")) {
            return true;
        } else if (name.equals("Long")) {
            return true;
        } else if (name.equals("Short")) {
            return true;
        } else if (name.equals("Char")) {
            return true;
        }
        return false;
    }

    public static String getClassTransName(String name) {
        if (name.equals("int")) {
            name = "Integer";
        } else if (name.equals("boolean")) {
            name = "Boolean";
        } else if (name.equals("byte")) {
            name = "Byte";
        } else if (name.equals("double")) {
            name = "Double";
        } else if (name.equals("float")) {
            name = "Float";
        } else if (name.equals("long")) {
            name = "Long";
        } else if (name.equals("short")) {
            name = "Short";
        } else if (name.equals("char")) {
            name = "Character";
        }
        return name;
    }

    public static String getVariableInit(String name, String type) {
        if (type.equals("int")) {
            return type + " " + name + "=0 ;\n";
        } else if (type.equals("boolean")) {
            return type + " " + name + "=false ;\n";
        } else if (type.equals("byte")) {
            return type + " " + name + " ;\n";
        } else if (type.equals("double")) {
            return type + " " + name + "=0 ;\n";
        } else if (type.equals("float")) {
            return type + " " + name + "=0 ;\n";
        } else if (type.equals("long")) {
            return type + " " + name + "=0 ;\n";
        } else if (type.equals("short")) {
            return type + " " + name + "=0 ;\n";
        } else if (type.equals("char")) {
            return type + " " + name + " ;\n";
        } else {
            return type + " " + name + " = null ;\n";
        }
    }
}
