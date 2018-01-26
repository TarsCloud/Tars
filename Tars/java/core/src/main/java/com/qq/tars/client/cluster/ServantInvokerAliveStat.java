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

package com.qq.tars.client.cluster;

import java.math.BigDecimal;
import java.util.Date;
import java.util.concurrent.atomic.AtomicBoolean;

import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.common.util.Constants;

public class ServantInvokerAliveStat {

    private final String identity;
    private AtomicBoolean lastCallSucess = new AtomicBoolean();
    private long timeout_startTime = 0;
    private long frequenceFailInvoke = 0;
    private long frequenceFailInvoke_startTime = 0;
    private long timeoutCount = 0;
    private long failedCount = 0;
    private long succCount = 0;
    private boolean netConnectTimeout = false;
    private boolean alive = true;
    private long lastRetryTime = 0;

    ServantInvokerAliveStat(String identity) {
        this.identity = identity;
    }

    public boolean isAlive() {
        return alive;
    }

    public synchronized void onCallFinished(int ret, ServantProxyConfig config) {
        if (ret == Constants.INVOKE_STATUS_SUCC) {
            frequenceFailInvoke = 0;
            frequenceFailInvoke_startTime = 0;
            lastCallSucess.set(true);
            netConnectTimeout = false;
            succCount++;
        } else if (ret == Constants.INVOKE_STATUS_TIMEOUT) {
            if (!lastCallSucess.get()) {
                frequenceFailInvoke++;
            } else {
                lastCallSucess.set(false);
                frequenceFailInvoke = 1;
                frequenceFailInvoke_startTime = System.currentTimeMillis();
            }
            netConnectTimeout = false;
            timeoutCount++;
        } else if (ret == Constants.INVOKE_STATUS_EXEC) {
            if (!lastCallSucess.get()) {
                frequenceFailInvoke++;
            } else {
                lastCallSucess.set(false);
                frequenceFailInvoke = 1;
                frequenceFailInvoke_startTime = System.currentTimeMillis();
            }
            netConnectTimeout = false;
            failedCount++;
        } else if (ret == Constants.INVOKE_STATUS_NETCONNECTTIMEOUT) {
            netConnectTimeout = true;
        }

        if ((timeout_startTime + config.getCheckInterval()) < System.currentTimeMillis()) {
            timeoutCount = 0;
            failedCount = 0;
            succCount = 0;
            timeout_startTime = System.currentTimeMillis();
        }

        if (alive) {
            long totalCount = timeoutCount + failedCount + succCount;
            if (timeoutCount >= config.getMinTimeoutInvoke()) {
                double radio = div(timeoutCount, totalCount, 2);
                if (radio > config.getFrequenceFailRadio()) {
                    alive = false;
                    ClientLogger.getLogger().info(identity + "|alive=false|radio=" + radio + "|" + toString());
                }
            }

            if (alive) {
                if (frequenceFailInvoke >= config.getFrequenceFailInvoke() && (frequenceFailInvoke_startTime + 5000) > System.currentTimeMillis()) {
                    alive = false;
                    ClientLogger.getLogger().info(identity + "|alive=false|frequenceFailInvoke=" + frequenceFailInvoke + "|" + toString());
                }
            }

            if (alive) {
                if (netConnectTimeout) {
                    alive = false;
                    ClientLogger.getLogger().info(identity + "|alive=false|netConnectTimeout" + "|" + toString());
                }
            }
        } else {
            if (ret == Constants.INVOKE_STATUS_SUCC) {
                alive = true;
            }
        }
    }

    public long getLastRetryTime() {
        return lastRetryTime;
    }

    public void setLastRetryTime(long lastRetryTime) {
        this.lastRetryTime = lastRetryTime;
    }

    public String toString() {
        StringBuilder build = new StringBuilder();
        build.append("lastCallSucc:").append(lastCallSucess.get()).append("|");
        build.append("timeoutCount:").append(timeoutCount).append("|");
        build.append("failedCount:").append(failedCount).append("|");
        build.append("succCount:").append(succCount).append("|");
        build.append("available:").append(alive).append("|");
        build.append("netConnectTimeout:").append(netConnectTimeout).append("|");

        build.append("timeout_startTime:").append(new Date(timeout_startTime)).append("|");
        build.append("frequenceFailInvoke:").append(frequenceFailInvoke).append("|");
        build.append("frequenceFailInvoke_startTime:").append(new Date(frequenceFailInvoke_startTime)).append("|");
        build.append("lastRetryTime:").append(new Date(lastRetryTime));
        return build.toString();
    }

    private double div(double v1, double v2, int scale) {
        if (scale < 0) {
            throw new IllegalArgumentException("The scale must be a positive integer or zero");
        }
        BigDecimal b1 = new BigDecimal(Double.toString(v1));
        BigDecimal b2 = new BigDecimal(Double.toString(v2));
        return b1.divide(b2, scale, BigDecimal.ROUND_HALF_UP).doubleValue();
    }
}
