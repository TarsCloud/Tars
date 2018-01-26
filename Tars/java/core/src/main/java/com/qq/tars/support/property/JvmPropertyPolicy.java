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

package com.qq.tars.support.property;

import java.lang.management.GarbageCollectorMXBean;
import java.lang.management.ManagementFactory;

public final class JvmPropertyPolicy {

    private static final int MoveBits_ByteToMB = 20;

    public static final class MemoryHeapUsedAvg implements PropertyReportHelper.Policy {

        @Override
        public String desc() {
            return "Avg";
        }

        @Override
        public String get() {
            return String.valueOf(ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getUsed() >> MoveBits_ByteToMB);
        }

        @Override
        public void set(int v) {
        }
    }

    public static final class MemoryHeapCommittedAvg implements PropertyReportHelper.Policy {

        @Override
        public String desc() {
            return "Avg";
        }

        @Override
        public String get() {
            return String.valueOf(ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getCommitted() >> MoveBits_ByteToMB);
        }

        @Override
        public void set(int v) {
        }
    }

    public static final class MemoryHeapMaxAvg implements PropertyReportHelper.Policy {

        @Override
        public String desc() {
            return "Avg";
        }

        @Override
        public String get() {
            return String.valueOf(ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getMax() >> MoveBits_ByteToMB);
        }

        @Override
        public void set(int v) {
        }
    }

    public static final class ThreadNumAvg implements PropertyReportHelper.Policy {

        @Override
        public String desc() {
            return "Avg";
        }

        @Override
        public String get() {
            return String.valueOf(ManagementFactory.getThreadMXBean().getThreadCount());
        }

        @Override
        public void set(int v) {
        }

    }

    public static final class GCNumCount implements PropertyReportHelper.Policy {

        private final String gcName;
        private long lastGCNum = 0;

        public GCNumCount(String gcName) {
            this.gcName = gcName;
        }

        @Override
        public String desc() {
            return "Count";
        }

        @Override
        public String get() {
            for (GarbageCollectorMXBean gcMXBean : ManagementFactory.getGarbageCollectorMXBeans()) {
                if (gcName.equals(gcMXBean.getName())) {
                    long gcNum = gcMXBean.getCollectionCount();
                    long c = gcNum - lastGCNum;
                    lastGCNum = gcNum;
                    return String.valueOf(c);
                }
            }
            return "0";
        }

        @Override
        public void set(int v) {
        }

    }

    public static final class GCTimeSum implements PropertyReportHelper.Policy {

        private final String gcName;
        private long lastGCTime = 0;

        public GCTimeSum(String gcName) {
            this.gcName = gcName;
        }

        @Override
        public String desc() {
            return "Sum";
        }

        @Override
        public String get() {
            for (GarbageCollectorMXBean gcMXBean : ManagementFactory.getGarbageCollectorMXBeans()) {
                if (gcName.equals(gcMXBean.getName())) {
                    long gcTime = gcMXBean.getCollectionTime();
                    long t = gcTime - lastGCTime;
                    lastGCTime = gcTime;
                    return String.valueOf(t);
                }
            }
            return "0";
        }

        @Override
        public void set(int v) {
        }

    }
}
