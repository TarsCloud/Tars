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

package com.qq.tars.spring.schema;

import org.springframework.beans.factory.config.BeanDefinition;
import org.springframework.beans.factory.config.RuntimeBeanReference;
import org.springframework.beans.factory.support.RootBeanDefinition;
import org.springframework.beans.factory.xml.BeanDefinitionParser;
import org.springframework.beans.factory.xml.ParserContext;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;

import com.qq.tars.spring.config.ListenerConfig;
import com.qq.tars.spring.config.ServantConfig;
import com.qq.tars.spring.config.TarsSpringConfigException;

public class TarsBeanDefinitionParser implements BeanDefinitionParser {

    private Class<?> clssze;

    TarsBeanDefinitionParser(Class<?> cls) {
        this.clssze = cls;
    }

    @Override
    public BeanDefinition parse(Element element, ParserContext parserContext) {
        RootBeanDefinition beanDefinition = new RootBeanDefinition();
        beanDefinition.setBeanClass(clssze);
        String id = null;
        id = element.getAttribute("name");
        if (id == null || id.length() == 0) {
            if (clssze == ServantConfig.class) {
                throw new TarsSpringConfigException("Messing servant name");
            } else if (clssze == ListenerConfig.class) {
                id = "listener";

                int counter = 0;
                while (parserContext.getRegistry().containsBeanDefinition(id + counter)) {
                    counter++;
                }

                id = id + counter;
            }
        }

        if (id != null && id.length() > 0) {
            if (parserContext.getRegistry().containsBeanDefinition(id)) {
                throw new TarsSpringConfigException("Duplicate spring bean id " + id);
            }
            parserContext.getRegistry().registerBeanDefinition(id, beanDefinition);
        }

        NamedNodeMap nnm = element.getAttributes();
        for (int i = 0; i < nnm.getLength(); i++) {
            Node node = nnm.item(i);
            String key = node.getLocalName();
            String value = node.getNodeValue();
            if (key.equals("entity")) {
                if (parserContext.getRegistry().containsBeanDefinition(value)) {
                    beanDefinition.getPropertyValues().add(key, parserContext.getRegistry().getBeanDefinition(value));
                } else {
                    beanDefinition.getPropertyValues().add(key, new RuntimeBeanReference(value));
                }
            } else {
                beanDefinition.getPropertyValues().add(key, value);
            }
        }

        return beanDefinition;
    }
}
