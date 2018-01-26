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

package com.qq.tars.net.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.URL;
import java.util.Enumeration;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Utils {

    public static int getSelectorPoolSize() {
        int processors = Runtime.getRuntime().availableProcessors();
        return processors > 8 ? 4 + (processors * 5 / 8) : processors + 1;
    }

    public static String getLocalIp() {
        try {
            Pattern pattern = Pattern.compile("(172|10)\\.[0-9]+\\.[0-9]+\\.[0-9]+");
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();

            while (interfaces.hasMoreElements()) {
                NetworkInterface ni = interfaces.nextElement();
                Enumeration<InetAddress> en = ni.getInetAddresses();
                while (en.hasMoreElements()) {
                    InetAddress addr = en.nextElement();
                    String ip = addr.getHostAddress();
                    Matcher matcher = pattern.matcher(ip);
                    if (matcher.matches()) {
                        return ip;
                    }
                }
            }
            return "0.0.0.0";
        } catch (Throwable e) {
            e.printStackTrace();
            return "0.0.0.0";
        }
    }

    public static void setQosFlag(Socket socket) {
        if (NETWORK_QOS_FLAG == null || socket == null) return;

        try {
            if ("B1_REAL_TIME_QOS".equals(NETWORK_QOS_FLAG)) {
                socket.setTrafficClass(144);
            } else if ("B1_NON_REAL_TIME_QOS".equals(NETWORK_QOS_FLAG)) {
                socket.setTrafficClass(24);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public static InputStream loadConfigFile(String resource, Class<?> clazz) {
        ClassLoader classLoader = null;
        try {
            Method method = Thread.class.getMethod("getContextClassLoader");
            classLoader = (ClassLoader) method.invoke(Thread.currentThread());
        } catch (Exception e) {
            System.out.println("loadConfigFile error: ");
            e.printStackTrace();
        }
        if (classLoader == null) {
            classLoader = clazz.getClassLoader();
        }
        try {
            if (classLoader != null) {
                URL url = classLoader.getResource(resource);
                if (url == null) {
                    System.out.println("Can not find resource:" + resource);
                    return null;
                }
                if (url.toString().startsWith("jar:file:")) {
                    System.out.println("Get resource \"" + resource + "\" from jar:\t" + url.toString());
                    return clazz.getResourceAsStream(resource.startsWith("/") ? resource : "/" + resource);
                } else {
                    System.out.println("Get resource \"" + resource + "\" from:\t" + url.toString());
                    return new FileInputStream(new File(url.toURI()));
                }
            }
        } catch (Exception e) {
            System.out.println("loadConfigFile error: ");
            e.printStackTrace();
        }
        return null;
    }

    private static String NETWORK_QOS_FLAG = null;

    static {
        NETWORK_QOS_FLAG = System.getProperty("network.QOS");
    }
}
