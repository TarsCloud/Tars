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

import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.qq.tars.common.util.Constants;

public class ProxyStatBody {

    private AtomicInteger count = new AtomicInteger();
    private AtomicInteger execCount = new AtomicInteger();
    private AtomicInteger timeoutCount = new AtomicInteger();

    private AtomicLong totalRspTime = new AtomicLong();
    private AtomicInteger maxRspTime = new AtomicInteger();
    private AtomicInteger minRspTime = new AtomicInteger();

    public ConcurrentHashMap<Integer, AtomicInteger> intervalCount = new ConcurrentHashMap<Integer, AtomicInteger>();

    ReentrantReadWriteLock intervListlock = new ReentrantReadWriteLock();
    CopyOnWriteArrayList<Integer> timeStatInterval = new CopyOnWriteArrayList<Integer>();

    public ProxyStatBody(List<Integer> interval) {
        setTimeStatInterval(interval);
    }

    public void setTimeStatInterval(List<Integer> interval) {
        timeStatInterval.clear();
        timeStatInterval.addAll(interval);
        for (Integer i : timeStatInterval) {
            if (!intervalCount.containsKey(i)) {
                intervalCount.put(i, new AtomicInteger());
            }
        }
    }

    public void onCallFinished(long costTime, int callStatus) {
        if (callStatus == Constants.INVOKE_STATUS_SUCC) {
            count.incrementAndGet();
            totalRspTime.addAndGet(costTime);
            if (maxRspTime.get() < costTime) {
                maxRspTime.set((int) costTime);
            } else if (costTime > 0 && costTime < minRspTime.get()) {
                minRspTime.set((int) costTime);
            }
        } else if (callStatus == Constants.INVOKE_STATUS_EXEC) {
            execCount.incrementAndGet();
        } else if (callStatus == Constants.INVOKE_STATUS_TIMEOUT) {
            timeoutCount.incrementAndGet();
        }
        for (int i = 0; i < timeStatInterval.size(); i++) {
            if (costTime <= timeStatInterval.get(i)) {
                intervalCount.get(timeStatInterval.get(i)).incrementAndGet();
                break;
            }
        }
    }

    public Object clone() {
        Object o = null;
        try {
            o = super.clone();
        } catch (CloneNotSupportedException ex) {
            assert false;
        }
        return o;
    }

    public void clear() {
        count.set(0);
        execCount.set(0);
        timeoutCount.set(0);
        for (Entry<Integer, AtomicInteger> entry : intervalCount.entrySet()) {
            entry.getValue().set(0);
        }
        totalRspTime.set(0);
        maxRspTime.set(0);
        minRspTime.set(0);
    }

    public int getCount() {
        return count.get();
    }

    public int getExecCount() {
        return execCount.get();
    }

    public int getTimeoutCount() {
        return timeoutCount.get();
    }

    public long getTotalRspTime() {
        return totalRspTime.get();
    }

    public int getMaxRspTime() {
        return maxRspTime.get();
    }

    public int getMinRspTime() {
        return minRspTime.get();
    }
}
