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

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.common.util.Constants;

public class ProxyStat {

    private ConcurrentHashMap<ProxyStatHead, ProxyStatBody> stat = new ConcurrentHashMap<ProxyStatHead, ProxyStatBody>();

    public static final List<Integer> DEFAULT_TIME_STAT_INTERVAL = new ArrayList<Integer>();
    static {
        DEFAULT_TIME_STAT_INTERVAL.add(5);
        DEFAULT_TIME_STAT_INTERVAL.add(10);
        DEFAULT_TIME_STAT_INTERVAL.add(50);
        DEFAULT_TIME_STAT_INTERVAL.add(100);
        DEFAULT_TIME_STAT_INTERVAL.add(200);
        DEFAULT_TIME_STAT_INTERVAL.add(500);
        DEFAULT_TIME_STAT_INTERVAL.add(1000);
        DEFAULT_TIME_STAT_INTERVAL.add(2000);
        DEFAULT_TIME_STAT_INTERVAL.add(3000);
        DEFAULT_TIME_STAT_INTERVAL.add(5000);
        DEFAULT_TIME_STAT_INTERVAL.add(10000);
        DEFAULT_TIME_STAT_INTERVAL.add(100000);
    }

    public void setTimeStatInterval(ProxyStatHead head, List<Integer> timeStatInterval) {
        if (stat.containsKey(head)) {
            stat.get(head).setTimeStatInterval(timeStatInterval);
        }
    }

    private ProxyStatBody getStatBody(ProxyStatHead head) {
        ProxyStatBody body = stat.get(head);
        if (body == null) {
            stat.putIfAbsent(head, new ProxyStatBody(DEFAULT_TIME_STAT_INTERVAL));
            body = stat.get(head);
        }
        return body;
    }

    public void addInvokeTime(ProxyStatHead head, long costTimeMill, int result) {
        if (result == Constants.INVOKE_STATUS_SUCC) {
            getStatBody(head).onCallFinished(costTimeMill, Constants.INVOKE_STATUS_SUCC);
        } else if (result == Constants.INVOKE_STATUS_EXEC) {
            getStatBody(head).onCallFinished(costTimeMill, Constants.INVOKE_STATUS_EXEC);
        } else if (result == Constants.INVOKE_STATUS_TIMEOUT) {
            getStatBody(head).onCallFinished(costTimeMill, Constants.INVOKE_STATUS_TIMEOUT);
        }
    }

    public void addInvokeTime(String moduleName, String objectName, String setDivision, String methodName,
                              String slaveIp, int slavePort, int result, long costTimeMill) {
        ProxyStatHead head = ProxyStatUtils.getHead(moduleName, objectName, methodName, ProxyStatUtils.getLocalIP(), slaveIp, slavePort, result, null, null, null, setDivision);
        addInvokeTime(head, costTimeMill, result);
    }

    public void addInvokeTime(String moduleName, String objectName, String setDivision, String methodName,
                              String masterIp, String slaveIp, int slavePort, int result, long costTimeMill) {
        ProxyStatHead head = ProxyStatUtils.getHead(moduleName, objectName, methodName, masterIp, slaveIp, slavePort, result, null, null, null, setDivision);
        addInvokeTime(head, costTimeMill, result);
    }

    public List<Integer> getStatIntervals() {
        Collection<ProxyStatBody> collection = stat.values();
        if (!collection.isEmpty()) {
            for (ProxyStatBody body : collection) {
                if (body != null) {
                    return body.timeStatInterval;
                }
            }
        }
        return DEFAULT_TIME_STAT_INTERVAL;
    }

    public ConcurrentHashMap<ProxyStatHead, ProxyStatBody> getStats() {
        return this.stat;
    }

    public int size() {
        return this.stat.size();
    }

    public boolean isEmpty() {
        return this.stat.isEmpty();
    }
}
