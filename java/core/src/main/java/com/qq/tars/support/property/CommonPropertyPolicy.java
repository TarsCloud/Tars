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

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.support.property.PropertyReportHelper.Policy;

public class CommonPropertyPolicy {

    public static class Sum implements Policy {

        private final AtomicInteger n = new AtomicInteger(0);

        @Override
        public String desc() {
            return "Sum";
        }

        @Override
        public String get() {
            return String.valueOf(n.getAndSet(0));
        }

        @Override
        public void set(int v) {
            n.addAndGet(v);
        }
    }

    public static class Min implements Policy {

        private final AtomicInteger n = new AtomicInteger(Integer.MAX_VALUE);

        @Override
        public String desc() {
            return "Min";
        }

        @Override
        public String get() {
            return String.valueOf(n.getAndSet(Integer.MAX_VALUE));
        }

        @Override
        public void set(int v) {
            int old;
            do {
                old = this.n.get();
                if (v >= old) {
                    return;
                }
            } while (!this.n.compareAndSet(old, v));
        }

    }

    public static class Max implements Policy {

        private final AtomicInteger n = new AtomicInteger(Integer.MIN_VALUE);

        @Override
        public String desc() {
            return "Max";
        }

        @Override
        public String get() {
            return String.valueOf(n.getAndSet(Integer.MIN_VALUE));
        }

        @Override
        public void set(int v) {
            int old;
            do {
                old = this.n.get();
                if (v <= old) {
                    return;
                }
            } while (!this.n.compareAndSet(old, v));
        }

    }

    public static class Distr implements Policy {

        private static final Integer[] II = {};
        private Integer[] data;
        private int[] result;

        public Distr(List<Integer> list) {
            if (list == null || list.size() == 0) {
                throw new TarsException("ProperyReport|Distr initial list object can not be empty");
            }

            this.data = new HashSet<Integer>(list).toArray(II);
            Arrays.sort(data);
            result = new int[data.length];
            for (int i = 0; i < result.length; i++) {
                result[i] = 0;
            }
        }

        private synchronized void clear() {
            for (int i = 0; i < result.length; i++) {
                result[i] = 0;
            }
        }

        @Override
        public String desc() {
            return "Distr";
        }

        @Override
        public synchronized String get() {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < data.length; i++) {
                sb.append(i != 0 ? "," : "").append(data[i]).append("|").append(result[i]);
            }

            clear();
            return sb.toString();
        }

        @Override
        public synchronized void set(int value) {
            for (int i = 0; i < data.length; i++) {
                if (data[i] > value) {
                    result[i]++;
                    break;
                }
            }
        }

    }

    public static class Avg implements Policy {

        private final AtomicInteger cnt = new AtomicInteger(0);
        private final AtomicInteger sum = new AtomicInteger(0);

        @Override
        public String desc() {
            return "Avg";
        }

        @Override
        public String get() {
            int cnt = this.cnt.getAndSet(0);
            int sum = this.sum.getAndSet(0);
            if (cnt <= 0) {
                return "0";
            } else {
                return String.valueOf(((double) sum) / cnt);
            }
        }

        @Override
        public void set(int v) {
            cnt.incrementAndGet();
            sum.addAndGet(v);
        }
    }

    public static class Count implements Policy {

        private final AtomicInteger n = new AtomicInteger(0);

        @Override
        public String desc() {
            return "Count";
        }

        @Override
        public String get() {
            return String.valueOf(n.getAndSet(0));
        }

        @Override
        public void set(int v) {
            n.incrementAndGet();
        }

    }
}
