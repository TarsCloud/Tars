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

package com.qq.tars.client;

import java.lang.reflect.Field;

import org.springframework.beans.BeansException;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.config.BeanPostProcessor;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;
import org.springframework.core.annotation.AnnotationUtils;
import org.springframework.util.ReflectionUtils;

import com.qq.tars.client.config.TarsClientProperties;
import com.qq.tars.common.util.BeanAccessor;
import com.qq.tars.register.RegisterHandler;
import com.qq.tars.register.RegisterManager;
import com.qq.tars.server.annotation.TarsClient;

public class TarsClientBeanPostProcessor implements BeanPostProcessor, InitializingBean, ApplicationContextAware {

    @Autowired
    private TarsClientProperties clientProperties;

    private Communicator communicator;

    private ApplicationContext applicationContext;

    public TarsClientBeanPostProcessor() {
    }

    @Override
    public void afterPropertiesSet() throws Exception {
        try {
            RegisterManager.getInstance().setHandler(applicationContext.getBean(RegisterHandler.class));
        } catch (Exception e) {
        }
        communicator = CommunicatorFactory.getInstance().getCommunicator(clientProperties);
        BeanAccessor.setBeanValue(CommunicatorFactory.getInstance(), "communicator", communicator);
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

            Object proxy = communicator.stringToProxy(field.getType(), annotation.name());

            ReflectionUtils.makeAccessible(field);
            ReflectionUtils.setField(field, bean, proxy);
        }
    }

    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        this.applicationContext = applicationContext;
    }
}
