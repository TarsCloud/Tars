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

package com.qq.tars.spring.bean;

import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.spring.annotation.RemotePropertySource;
import com.qq.tars.support.config.ConfigHelper;
import org.springframework.boot.context.event.ApplicationEnvironmentPreparedEvent;
import org.springframework.context.ApplicationListener;
import org.springframework.core.env.PropertiesPropertySource;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class PropertiesListener implements ApplicationListener<ApplicationEnvironmentPreparedEvent> {
    @Override
    public void onApplicationEvent(ApplicationEnvironmentPreparedEvent event) {
        RemotePropertySource sources = event.getSpringApplication()
                .getMainApplicationClass().getAnnotation(RemotePropertySource.class);

        String configPath = ConfigurationManager.getInstance().getserverConfig().getBasePath() + "/conf/";
        if (sources != null) {
            for (String name : sources.value()) {
                try {
                    ConfigHelper.getInstance().loadConfig(name);
                    File config = new File(configPath + name);

                    if (config.exists()) {
                        Properties properties = new Properties();
                        properties.load(new FileInputStream(config));
                        event.getEnvironment().getPropertySources().addFirst(new PropertiesPropertySource(name, properties));
                    } else {
                        throw new RuntimeException("[TARS] load config failed file not exists");
                    }
                } catch (IOException e) {
                    System.err.println("[TARS] load config failed: " + name);
                    e.printStackTrace();
                }
            }
        }
    }
}
