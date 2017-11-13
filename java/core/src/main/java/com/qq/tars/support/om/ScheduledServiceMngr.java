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

package com.qq.tars.support.om;

import java.util.List;
import java.util.Random;
import java.util.concurrent.TimeUnit;

import com.qq.tars.common.support.ScheduledExecutorManager;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.support.node.NodeHelper;
import com.qq.tars.support.property.PropertyReportHelper;

public class ScheduledServiceMngr {

    private final static ScheduledServiceMngr Instance = new ScheduledServiceMngr();

    private static final int HEART_BEAT_INTERVAL = 10;

    private static final int PROPERTY_REPORT_MIN_INTERVAL = 30000;

    private static int REPORT_INTERVAL = ConfigurationManager.getInstance().getServerConfig().getCommunicatorConfig().getReportInterval();
    private final Random random = new Random(System.currentTimeMillis() / 1000);

    private ScheduledExecutorManager taskExecutorManager = ScheduledExecutorManager.getInstance();

    private ScheduledServiceMngr() {
    }

    public static ScheduledServiceMngr getInstance() {
        return Instance;
    }

    public void start() {
        startHandleService();
    }

    public void shutdown() {
        taskExecutorManager.shutdown();
    }

    public List<Runnable> shutdownNow() {
        return taskExecutorManager.shutdownNow();
    }

    public boolean isShutdown() {
        return taskExecutorManager.isShutdown();
    }

    public boolean isTerminated() {
        return taskExecutorManager.isTerminated();
    }

    class NodeHandleThread implements Runnable {

        @Override
        public void run() {
            NodeHelper.getInstance().keepAlive();
        }

    }

    class PropertyHandleThread implements Runnable {

        @Override
        public void run() {
            PropertyReportHelper.getInstance().report();
        }

    }

    class StatHandleThread implements Runnable {

        @Override
        public void run() {
            ServerStatHelper.getInstance().report();
        }
    }

    private void startHandleService() {
        Runnable nodeHandler = new Thread(new NodeHandleThread(), "HeartBeat");
        taskExecutorManager.scheduleAtFixedRate(nodeHandler, 0, HEART_BEAT_INTERVAL, TimeUnit.SECONDS);

        int initialDelay = REPORT_INTERVAL + (random.nextInt(30) * 1000);
        Runnable statHandler = new Thread(new StatHandleThread(), "ServerStat");
        taskExecutorManager.scheduleAtFixedRate(statHandler, initialDelay, REPORT_INTERVAL, TimeUnit.MILLISECONDS);

        if (REPORT_INTERVAL < PROPERTY_REPORT_MIN_INTERVAL) {
            REPORT_INTERVAL = PROPERTY_REPORT_MIN_INTERVAL;
        }
        initialDelay = REPORT_INTERVAL + (random.nextInt(30) * 1000);
        Runnable propertyHandler = new Thread(new PropertyHandleThread(), "PropertyReport");
        taskExecutorManager.scheduleAtFixedRate(propertyHandler, initialDelay, REPORT_INTERVAL, TimeUnit.MILLISECONDS);
    }
}
