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

package com.qq.tars.support.stat;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.UnknownHostException;
import java.util.Enumeration;

import com.qq.tars.client.ClientVersion;
import com.qq.tars.common.util.StringUtils;

public class ProxyStatUtils {

    public final static int MAX_MASTER_NAME_LEN = 127;

    public static ProxyStatHead getHead(String masterName, String slaveName, String interfaceName, String masterIp,
                                        String slaveIp, int slavePort, int returnValue, String masterSetName,
                                        String masterSetArea, String masterSetID, String slaveSetDivision) {
        if (masterSetName == null || masterSetName.length() == 0) {
            masterName = masterName + "@" + ClientVersion.getVersion();
        } else {
            String shortMasterName = getShortModuleName(masterName);
            masterName = masterSetName + masterSetArea + masterSetID + shortMasterName + "@" + ClientVersion.getVersion();
        }

        String slaveSetName = "";
        String slaveSetArea = "";
        String slaveSetID = "";

        boolean setExist = false;
        if (slaveSetDivision != null && slaveSetDivision.length() > 0) {
            String[] tempStrs = StringUtils.split(slaveSetDivision, ".");
            if (tempStrs.length == 3) {
                slaveSetName = tempStrs[0];
                slaveSetArea = tempStrs[1];
                slaveSetID = tempStrs[2];
                setExist = true;

                String shortSlaveName = getShortModuleName(trimAndLimit(slaveName, MAX_MASTER_NAME_LEN));
                slaveName = slaveSetName + slaveSetArea + slaveSetID + "." + shortSlaveName;
            }
        }

        if (!setExist) {
            slaveName = trimAndLimit(slaveName, MAX_MASTER_NAME_LEN);
        }

        ProxyStatHead head = new ProxyStatHead(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort, returnValue, slaveSetName, slaveSetArea, slaveSetID, "");
        return head;
    }

    private static String getShortModuleName(String moduleName) {
        String shortModuleName = "";
        if (moduleName != null) {
            int pos = moduleName.indexOf('.');
            if (pos >= 0) {
                shortModuleName = moduleName.substring(pos + 1);
            }
        }
        return shortModuleName;
    }

    private static String trimAndLimit(String str, int limit) {
        String ret = "";
        if (str != null) {
            str = str.trim();
            if (str.length() > limit) {
                str = str.substring(0, limit);
            }
            ret = str;
        }
        return ret;
    }

    private static String cacheIP = null;
    private static final Object cacheLock = new Object();

    public static String getLocalIP() {
        if (cacheIP == null) {
            synchronized (cacheLock) {
                if (cacheIP == null) {
                    try {
                        cacheIP = getLocalIPByNetworkInterface();
                    } catch (Exception e) {
                    }
                    if (cacheIP == null) {
                        try {
                            cacheIP = InetAddress.getLocalHost().getHostAddress();
                        } catch (UnknownHostException e) {
                            cacheIP = "0.0.0.0";
                        }
                    }
                }
            }
        }
        return cacheIP;
    }

    private static String getLocalIPByNetworkInterface() throws Exception {
        Enumeration<NetworkInterface> enumeration = NetworkInterface.getNetworkInterfaces();
        while (enumeration.hasMoreElements()) {
            NetworkInterface networkInterface = enumeration.nextElement();
            if (networkInterface.isUp() && !networkInterface.isVirtual() && !networkInterface.isLoopback()) {
                Enumeration<InetAddress> addresses = networkInterface.getInetAddresses();
                while (addresses.hasMoreElements()) {
                    InetAddress inetAddress = addresses.nextElement();
                    if (inetAddress instanceof Inet4Address) {
                        return inetAddress.getHostAddress();
                    }
                }
            }
        }
        return null;
    }
}
