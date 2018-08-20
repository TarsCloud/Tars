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

import com.qq.tars.client.Communicator;
import com.qq.tars.client.ServantProxyConfig;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.protocol.annotation.Servant;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.spring.annotation.TarsClient;
import org.springframework.beans.BeansException;
import org.springframework.beans.factory.config.BeanPostProcessor;
import org.springframework.core.annotation.AnnotationUtils;
import org.springframework.util.ReflectionUtils;

import java.lang.reflect.Field;

public class CommunicatorBeanPostProcessor implements BeanPostProcessor {
    private final Communicator communicator;

    public CommunicatorBeanPostProcessor(Communicator communicator) {
        this.communicator = communicator;
    }

    @Override
    public Object postProcessBeforeInitialization(Object bean, String beanName) throws BeansException {
        Class clazz = bean.getClass();
        processFields(bean, clazz.getDeclaredFields());
        return bean;
    }

    @Override
    public Object postProcessAfterInitialization(Object bean, String beanName) throws BeansException {
        return bean;
    }

    private void processFields(Object bean, Field[] declaredFields) {
        for (Field field : declaredFields) {
            TarsClient annotation = AnnotationUtils.getAnnotation(field, TarsClient.class);
            if (annotation == null) {
                continue;
            }

            if (field.getType().getAnnotation(Servant.class) == null) {
                throw new RuntimeException("[TARS] autoware client failed: target field is not TAF client");
            }

            String objName = annotation.name();

            if (StringUtils.isEmpty(annotation.value())) {
                throw new RuntimeException("[TARS] autoware client failed: objName is empty");
            }

            ServantProxyConfig config = new ServantProxyConfig(objName);

            config.setModuleName(ConfigurationManager.getInstance().getserverConfig()
                    .getCommunicatorConfig().getModuleName());
            config.setSetDivision(annotation.setDivision());
            config.setConnections(annotation.connections());
            config.setConnectTimeout(annotation.connectTimeout());
            config.setSyncTimeout(annotation.syncTimeout());
            config.setAsyncTimeout(annotation.asyncTimeout());
            config.setEnableSet(annotation.enableSet());
            config.setTcpNoDelay(annotation.tcpNoDelay());
            config.setCharsetName(annotation.charsetName());

            Object proxy = communicator.stringToProxy(field.getType(), config);

            ReflectionUtils.makeAccessible(field);
            ReflectionUtils.setField(field, bean, proxy);
        }
    }
}
