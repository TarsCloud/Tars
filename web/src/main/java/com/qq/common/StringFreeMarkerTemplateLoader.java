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

import freemarker.cache.TemplateLoader;
import org.apache.commons.lang3.StringUtils;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;

public class StringFreeMarkerTemplateLoader implements TemplateLoader {

    private String template;

    public StringFreeMarkerTemplateLoader(String template) {
        this.template = StringUtils.trimToEmpty(template);
    }

    @Override
    public Object findTemplateSource(String name) throws IOException {
        return template;
    }

    @Override
    public long getLastModified(Object templateSource) {
        return -1;
    }

    @Override
    public Reader getReader(Object templateSource, String encoding) throws IOException {
        return new StringReader(new String(template.getBytes(), encoding));
    }

    @Override
    public void closeTemplateSource(Object templateSource) throws IOException {
    }
}
