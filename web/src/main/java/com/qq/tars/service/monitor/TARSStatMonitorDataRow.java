/*
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

package com.qq.tars.service.monitor;

public class TARSStatMonitorDataRow {

    private String showDate;

    private String showTime;

    private String masterName;

    private String slaveName;

    private String interfaceName;

    private String masterIp;

    private String slaveIp;

    private String theTotalCount;

    private String preTotalCount;

    private String totalCountWave;

    private String theAvgTime;

    private String preAvgTime;

    private String theFailRate;

    private String preFailRate;

    private String theTimeoutRate;

    private String preTimeoutRate;

    public String getShowDate() {
        return showDate;
    }

    public void setShowDate(String showDate) {
        this.showDate = showDate;
    }

    public String getShowTime() {
        return showTime;
    }

    public void setShowTime(String showTime) {
        this.showTime = showTime;
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

    public String getTheTotalCount() {
        return theTotalCount;
    }

    public void setTheTotalCount(String theTotalCount) {
        this.theTotalCount = theTotalCount;
    }

    public String getPreTotalCount() {
        return preTotalCount;
    }

    public void setPreTotalCount(String preTotalCount) {
        this.preTotalCount = preTotalCount;
    }

    public String getTotalCountWave() {
        return totalCountWave;
    }

    public void setTotalCountWave(String totalCountWave) {
        this.totalCountWave = totalCountWave;
    }

    public String getTheAvgTime() {
        return theAvgTime;
    }

    public void setTheAvgTime(String theAvgTime) {
        this.theAvgTime = theAvgTime;
    }

    public String getPreAvgTime() {
        return preAvgTime;
    }

    public void setPreAvgTime(String preAvgTime) {
        this.preAvgTime = preAvgTime;
    }

    public String getTheFailRate() {
        return theFailRate;
    }

    public void setTheFailRate(String theFailRate) {
        this.theFailRate = theFailRate;
    }

    public String getPreFailRate() {
        return preFailRate;
    }

    public void setPreFailRate(String preFailRate) {
        this.preFailRate = preFailRate;
    }

    public String getTheTimeoutRate() {
        return theTimeoutRate;
    }

    public void setTheTimeoutRate(String theTimeoutRate) {
        this.theTimeoutRate = theTimeoutRate;
    }

    public String getPreTimeoutRate() {
        return preTimeoutRate;
    }

    public void setPreTimeoutRate(String preTimeoutRate) {
        this.preTimeoutRate = preTimeoutRate;
    }
}
