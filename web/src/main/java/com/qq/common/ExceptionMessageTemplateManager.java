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

package com.qq.common;

import com.google.common.cache.CacheBuilder;
import com.google.common.cache.CacheLoader;
import com.google.common.cache.LoadingCache;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * 消息模板管理器，访问服务器获取消息模板并管理消息模版缓存
 */
public class ExceptionMessageTemplateManager {

    private static final Logger log = LoggerFactory.getLogger(ExceptionMessageTemplateManager.class);

    private static LoadingCache<String, String> cache = CacheBuilder.newBuilder()
            .maximumSize(100)
            .refreshAfterWrite(30000, TimeUnit.MILLISECONDS)
            .build(new CacheLoader<String, String>() {
                @Override
                public String load(String key) throws Exception {
                    List<ExceptionMessageTempldate> configs = new LinkedList<>();
                    if (configs.isEmpty()) {
                        log.info("no exception config found, code={}", key);
                        return "没有错误码详细说明";
                    } else {
                        ExceptionMessageTempldate config = configs.get(0);
                        return String.format("%s, %s", config.getName(), config.getMessage());

                    }
                }
            });

    /**
     * 根据错误码获取配置的异常信息模板<br/>
     * <span style="color:red">1.取缓存；2.取远程</span>
     *
     * @param code 错误码（类型）
     * @return 异常模板
     * @throws Exception
     */
    public static String getExceptionMessageTemplate(String code) throws Exception {
        return cache.get(code);
    }
}

