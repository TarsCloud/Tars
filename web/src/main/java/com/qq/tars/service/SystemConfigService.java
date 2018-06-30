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

package com.qq.tars.service;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.core.io.ClassPathResource;
import org.springframework.core.io.Resource;
import org.springframework.core.io.support.PropertiesLoaderUtils;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.util.Properties;

@Service
public class SystemConfigService {

    private final Logger log = LoggerFactory.getLogger(SystemConfigService.class);

    public String getUploadTgzPath() {
        Resource resource = new ClassPathResource("/app.config.properties");
        Properties props;
        try {
            props = PropertiesLoaderUtils.loadProperties(resource);
        } catch (IOException e) {
            log.error("load app.config.properties error");
            throw new RuntimeException("load app.config.properties error", e);
        }
        return props.getProperty("upload.tgz.path");
    }
    
    public String getProperty(String key) {
        Resource resource = new ClassPathResource("/app.config.properties");
        Properties props;
        try {
            props = PropertiesLoaderUtils.loadProperties(resource);
        } catch (IOException e) {
            log.error("load app.config.properties error");
            throw new RuntimeException("load app.config.properties error", e);
        }
        return props.getProperty(key);
    }
}
