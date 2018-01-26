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

import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

public final class InvokeStatHelper {

    private final static InvokeStatHelper instance = new InvokeStatHelper();

    private volatile ConcurrentHashMap<String, ProxyStat> proxyStats = new ConcurrentHashMap<String, ProxyStat>();
    private volatile CopyOnWriteArrayList<Integer> timeStatInterval = new CopyOnWriteArrayList<Integer>();

    private InvokeStatHelper() {
        timeStatInterval.addAll(ProxyStat.DEFAULT_TIME_STAT_INTERVAL);
    }

    public static InvokeStatHelper getInstance() {
        return instance;
    }

    public CopyOnWriteArrayList<Integer> getStatIntervals() {
        return timeStatInterval;
    }

    public ProxyStat addProxyStat(String objectName) {
        ProxyStat statTool = proxyStats.get(objectName);
        if (statTool == null) {
            proxyStats.putIfAbsent(objectName, new ProxyStat());
            statTool = proxyStats.get(objectName);
        }
        return statTool;
    }

    public ProxyStat getProxyStat(String objectName) {
        ProxyStat tool = proxyStats.get(objectName);
        if (tool == null) {
            tool = addProxyStat(objectName);
        }
        return tool;
    }

    public ConcurrentHashMap<String, ProxyStat> getProxyStats() {
        return proxyStats;
    }

    public void setTimeStatInterval() {
        for (Entry<String, ProxyStat> entry : proxyStats.entrySet()) {
            setTimeStatInterval(entry.getKey());
        }
    }

    public void setTimeStatInterval(String objectName) {
        for (Entry<ProxyStatHead, ProxyStatBody> entry : getProxyStat(objectName).getStats().entrySet()) {
            entry.getValue().setTimeStatInterval(timeStatInterval);
        }
    }

    public void resetTimeStatInterval() {
        timeStatInterval.clear();
        timeStatInterval.addAll(ProxyStat.DEFAULT_TIME_STAT_INTERVAL);
        setTimeStatInterval();
    }

    public void addTimeStatInterval(int interval) {
        boolean add = false;
        for (int i = 0; i < timeStatInterval.size(); i++) {
            if (interval == timeStatInterval.get(i)) {
                return;
            }
            if (interval < timeStatInterval.get(i)) {
                timeStatInterval.add(i, interval);
                add = true;
                break;
            }
        }
        if (!add) {
            timeStatInterval.add(interval);
        }
        setTimeStatInterval();
    }
}
