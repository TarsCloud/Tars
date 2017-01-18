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
package com.qq.tars.maven.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

public class XMLConfigFile {

    private Document doc;

    public void parse(InputStream is) throws SAXException, IOException, ParserConfigurationException {
        DocumentBuilderFactory docBuilderFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder docBuilder = docBuilderFactory.newDocumentBuilder();
        docBuilder.setEntityResolver(new EntityResolver() {

            public InputSource resolveEntity(String publicId, String systemId) throws SAXException, IOException {
                if (systemId.contains("http://java.sun.com/dtd/web-app_2_3.dtd")) {
                    return new InputSource(new StringReader(""));
                } else {
                    return null;
                }
            }
        });

        doc = docBuilder.parse(is);
    }

    public XMLConfigElement getRootElement() {
        Element root = doc.getDocumentElement();
        return new XMLConfigElement(root);
    }
}
