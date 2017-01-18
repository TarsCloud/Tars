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

public class ProxyStatHead implements Comparable<ProxyStatHead> {

    public String masterName = "";
    public String slaveName = "";
    public String interfaceName = "";
    public String masterIp = "";
    public String slaveIp = "";
    public int slavePort = 0;
    public int returnValue = 0;
    public String slaveSetName = "";
    public String slaveSetArea = "";
    public String slaveSetID = "";
    public String tarsVersion = "";

    public ProxyStatHead(String masterName, String slaveName, String interfaceName, String masterIp, String slaveIp,
                         int slavePort, int returnValue, String slaveSetName, String slaveSetArea, String slaveSetID,
                         String tarsVersion) {
        this.masterName = masterName;
        this.slaveName = slaveName;
        this.interfaceName = interfaceName;
        this.masterIp = masterIp;
        this.slaveIp = slaveIp;
        this.slavePort = slavePort;
        this.returnValue = returnValue;
        this.slaveSetName = slaveSetName;
        this.slaveSetArea = slaveSetArea;
        this.slaveSetID = slaveSetID;
        this.tarsVersion = tarsVersion;
    }

    public int compareTo(ProxyStatHead o) {
        int[] r = { com.qq.tars.protocol.util.TarsUtil.compareTo(masterName, o.masterName), com.qq.tars.protocol.util.TarsUtil.compareTo(slaveName, o.slaveName), com.qq.tars.protocol.util.TarsUtil.compareTo(interfaceName, o.interfaceName), com.qq.tars.protocol.util.TarsUtil.compareTo(masterIp, o.masterIp), com.qq.tars.protocol.util.TarsUtil.compareTo(slaveIp, o.slaveIp), com.qq.tars.protocol.util.TarsUtil.compareTo(slavePort, o.slavePort), com.qq.tars.protocol.util.TarsUtil.compareTo(returnValue, o.returnValue), com.qq.tars.protocol.util.TarsUtil.compareTo(slaveSetName, o.slaveSetName), com.qq.tars.protocol.util.TarsUtil.compareTo(slaveSetArea, o.slaveSetArea), com.qq.tars.protocol.util.TarsUtil.compareTo(slaveSetID, o.slaveSetID), com.qq.tars.protocol.util.TarsUtil.compareTo(tarsVersion, o.tarsVersion) };
        for (int i = 0; i < r.length; ++i) {
            if (r[i] != 0) return r[i];
        }
        return 0;
    }

    public boolean equals(Object o) {
        if (o == null) {
            return false;
        }

        ProxyStatHead t = (ProxyStatHead) o;
        return (com.qq.tars.protocol.util.TarsUtil.equals(masterName, t.masterName) && com.qq.tars.protocol.util.TarsUtil.equals(slaveName, t.slaveName) && com.qq.tars.protocol.util.TarsUtil.equals(interfaceName, t.interfaceName) && com.qq.tars.protocol.util.TarsUtil.equals(masterIp, t.masterIp) && com.qq.tars.protocol.util.TarsUtil.equals(slaveIp, t.slaveIp) && com.qq.tars.protocol.util.TarsUtil.equals(slavePort, t.slavePort) && com.qq.tars.protocol.util.TarsUtil.equals(returnValue, t.returnValue) && com.qq.tars.protocol.util.TarsUtil.equals(slaveSetName, t.slaveSetName) && com.qq.tars.protocol.util.TarsUtil.equals(slaveSetArea, t.slaveSetArea) && com.qq.tars.protocol.util.TarsUtil.equals(slaveSetID, t.slaveSetID) && com.qq.tars.protocol.util.TarsUtil.equals(tarsVersion, t.tarsVersion));
    }

    public int hashCode() {
        int[] hc = { com.qq.tars.protocol.util.TarsUtil.hashCode(masterName), com.qq.tars.protocol.util.TarsUtil.hashCode(slaveName), com.qq.tars.protocol.util.TarsUtil.hashCode(interfaceName), com.qq.tars.protocol.util.TarsUtil.hashCode(masterIp), com.qq.tars.protocol.util.TarsUtil.hashCode(slaveIp), com.qq.tars.protocol.util.TarsUtil.hashCode(slavePort), com.qq.tars.protocol.util.TarsUtil.hashCode(returnValue), com.qq.tars.protocol.util.TarsUtil.hashCode(slaveSetName), com.qq.tars.protocol.util.TarsUtil.hashCode(slaveSetArea), com.qq.tars.protocol.util.TarsUtil.hashCode(slaveSetID), com.qq.tars.protocol.util.TarsUtil.hashCode(tarsVersion) };
        return java.util.Arrays.hashCode(hc);
    }

    public String getMasterName() {
        return masterName;
    }

    public void setMasterName(String masterName) {
        this.masterName = masterName;
    }

    public String getSlaveName() {
        return slaveName;
    }

    public void setSlaveName(String slaveName) {
        this.slaveName = slaveName;
    }

    public String getInterfaceName() {
        return interfaceName;
    }

    public void setInterfaceName(String interfaceName) {
        this.interfaceName = interfaceName;
    }

    public String getMasterIp() {
        return masterIp;
    }

    public void setMasterIp(String masterIp) {
        this.masterIp = masterIp;
    }

    public String getSlaveIp() {
        return slaveIp;
    }

    public void setSlaveIp(String slaveIp) {
        this.slaveIp = slaveIp;
    }

    public int getSlavePort() {
        return slavePort;
    }

    public void setSlavePort(int slavePort) {
        this.slavePort = slavePort;
    }

    public int getReturnValue() {
        return returnValue;
    }

    public void setReturnValue(int returnValue) {
        this.returnValue = returnValue;
    }

    public String getSlaveSetName() {
        return slaveSetName;
    }

    public void setSlaveSetName(String slaveSetName) {
        this.slaveSetName = slaveSetName;
    }

    public String getSlaveSetArea() {
        return slaveSetArea;
    }

    public void setSlaveSetArea(String slaveSetArea) {
        this.slaveSetArea = slaveSetArea;
    }

    public String getSlaveSetID() {
        return slaveSetID;
    }

    public void setSlaveSetID(String slaveSetID) {
        this.slaveSetID = slaveSetID;
    }

    public String getTafVersion() {
        return tarsVersion;
    }

    public void setTafVersion(String tarsVersion) {
        this.tarsVersion = tarsVersion;
    }
}
