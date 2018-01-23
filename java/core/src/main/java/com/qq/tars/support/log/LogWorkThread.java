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

package com.qq.tars.support.log;

import java.util.Collection;

import com.qq.tars.support.log.Logger.LogType;

final class LogWorkThread implements Runnable {

    private LogType logType;

    public LogWorkThread(LogType logType) {
        this.logType = logType;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            try {
                Thread.sleep(500);

                Collection<Logger> coll = LoggerFactory.getLoggerMap().values();
                for (Logger logger : coll) {
                    if (this.logType.getValue() == logger.getLogType().getValue()) {
                        try {
                            logger.doWriteLog();
                        } catch (Exception e) {
                        }
                    }
                }
            } catch (InterruptedException e) {
                break;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

}
