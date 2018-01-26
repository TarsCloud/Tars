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

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.PropertyNamingStrategy;
import com.github.pagehelper.Page;
import com.github.pagehelper.PageInfo;
import org.springframework.http.MediaType;
import org.springframework.http.converter.json.AbstractJackson2HttpMessageConverter;
import org.springframework.http.converter.json.Jackson2ObjectMapperBuilder;

import java.io.IOException;
import java.util.List;

public class ResponseBodyMessageConverter extends AbstractJackson2HttpMessageConverter {

    public ResponseBodyMessageConverter() {
        this(Jackson2ObjectMapperBuilder.json().build());
    }

    public ResponseBodyMessageConverter(ObjectMapper objectMapper) {
        super(objectMapper, new MediaType("application", "json", DEFAULT_CHARSET),
                new MediaType("application", "*+json", DEFAULT_CHARSET));

        getObjectMapper().setPropertyNamingStrategy(PropertyNamingStrategy.CAMEL_CASE_TO_LOWER_CASE_WITH_UNDERSCORES);
    }

    @Override
    protected void writePrefix(JsonGenerator generator, Object object) throws IOException {
        if (object instanceof ExceptionResponse) {
            return;
        }

        String jsonPrefix = "{\n" +
                "    \"ret_code\": 200,\n";

        if (object instanceof Page) {
            PageInfo pageInfo = new PageInfo((List) object);
            jsonPrefix += "    \"cur_page\": " + pageInfo.getPageNum() + ",\n";
            jsonPrefix += "    \"page_size\": " + pageInfo.getPageSize() + ",\n";
            jsonPrefix += "    \"total\": " + pageInfo.getTotal() + ",\n";
        }

        jsonPrefix += "    \"data\": ";
        generator.writeRaw(jsonPrefix);
    }

    @Override
    protected void writeSuffix(JsonGenerator generator, Object object) throws IOException {
        if (object instanceof ExceptionResponse) {
            return;
        }

        generator.writeRaw("\n}");
    }
}
