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

import freemarker.template.Configuration;
import freemarker.template.DefaultObjectWrapper;
import freemarker.template.Template;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.StringWriter;
import java.io.Writer;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public abstract class CodeEnableRuntimeException extends RuntimeException implements CodeEnabled {

    private final Logger log = LoggerFactory.getLogger(CodeEnableException.class);

    private Map<String, String> context = new HashMap<>();

    public CodeEnableRuntimeException(Throwable cause) {
        super(cause);
    }

    public CodeEnableRuntimeException() {
    }

    public CodeEnableRuntimeException(String message) {
        super(message);
    }

    public CodeEnableRuntimeException(String message, Throwable cause) {
        super(message, cause);
    }

    /**
     * 设置上下文变量
     *
     * @param key   变量key，对应消息模板中的变量
     * @param value 变量值
     * @return 当前对象，用户链式调用
     */
    public CodeEnableRuntimeException setContextParameter(String key, String value) {
        context.put(key, value);
        return this;
    }

    /**
     * 设置上下文变量
     *
     * @param parameters 变量，key对应消息模板中的变量
     * @return 当前对象，用户链式调用
     */
    public CodeEnableRuntimeException setContextParameter(Map<String, String> parameters) {
        parameters.keySet().forEach(key -> setContextParameter(key, parameters.get(key)));
        return this;
    }

    /**
     * 获取展现给用户的消息
     *
     * @return 模板填充变量后的结果，如果填充模板异常则只返回错误码
     */
    @Override
    public String getMessageForUser() {
        String fullCode = getFullCode();
        try {
            String messageTemplate = ExceptionMessageTemplateManager.getExceptionMessageTemplate(getCode());

            // 根据模板，填充上下文信息
            Configuration freemakerCfg = new Configuration();
            freemakerCfg.setTemplateLoader(new StringFreeMarkerTemplateLoader(messageTemplate));
            freemakerCfg.setEncoding(Locale.CHINA, Charsets.UTF8);
            freemakerCfg.setObjectWrapper(new DefaultObjectWrapper());

            Template template = freemakerCfg.getTemplate("", Locale.CHINA);
            template.setEncoding(Charsets.UTF8);

            context.put("sub_err_type", getCode().substring(4));

            Writer out = new StringWriter();
            template.process(context, out);
            out.flush();
            out.close();

            return String.format("%s, %s", fullCode, out.toString());
        } catch (Exception e) {
            log.error(String.format("build message from template error, code=%s", getCode()), e);
            return fullCode;
        }
    }

    /**
     * 获取完整的错误码
     *
     * @return 完整错误码，格式为：模块ID长度 + 模块ID + 错误码
     */
    @Override
    public String getFullCode() {
        // 模块ID长度不固定，加前置长度标记
        return String.format("%d%s%s", 1, 0, getCode());
    }

    /**
     * 获取错误码
     *
     * @return 错误码，不包含模块信息
     */
    public abstract String getCode();
}
