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

package com.qq.tars.client.util;

import com.qq.tars.support.log.Logger;
import com.qq.tars.support.log.LoggerFactory;

public final class ClientLogger {

    private static final String CLIENT_LOG_NAME = "tars_client.log";

    public static void init(String logPath, String logLevel) {
        if (logLevel == null) {
            logLevel = "INFO";
        }
        LoggerFactory.config(logLevel, logPath);
    }

    public static Logger getLogger() {
        return Logger.getLogger(CLIENT_LOG_NAME);
    }
}
