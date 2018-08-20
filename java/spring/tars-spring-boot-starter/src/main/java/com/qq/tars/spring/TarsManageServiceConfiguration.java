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

package com.qq.tars.spring;

import com.qq.tars.client.Communicator;
import com.qq.tars.client.CommunicatorFactory;
import com.qq.tars.spring.bean.CommunicatorBeanPostProcessor;
import com.qq.tars.spring.bean.ServletContainerCustomizer;
import com.qq.tars.support.config.ConfigHelper;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class TarsManageServiceConfiguration {
    @Bean
    public Communicator communicator() {
        return CommunicatorFactory.getInstance().getCommunicator();
    }

    @Bean
    public CommunicatorBeanPostProcessor communicatorBeanPostProcessor(Communicator communicator) {
        return new CommunicatorBeanPostProcessor(communicator);
    }

    @Bean
    public ConfigHelper configHelper() {
        return ConfigHelper.getInstance();
    }

    @Bean
    public ServletContainerCustomizer servletContainerCustomizer() {
        return new ServletContainerCustomizer();
    }
}
