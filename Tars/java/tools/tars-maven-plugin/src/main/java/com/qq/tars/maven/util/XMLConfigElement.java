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

import java.util.ArrayList;

import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class XMLConfigElement {

    private Element e;

    XMLConfigElement(Element e) {
        this.e = e;
    }

    public boolean isValid() {
        return e != null;
    }

    public String getStringAttribute(String name) {
        return e != null ? e.getAttribute(name) : null;
    }

    public String getStringAttribute(String name, String def) {
        String s = getStringAttribute(name);
        return s != null && s.length() > 0 ? s : def;
    }

    public boolean isExistAttribute(String name) {
        return getStringAttribute(name) != null;
    }

    public boolean getBooleanAttribute(String name, boolean def) {
        String s = getStringAttribute(name);
        if ("true".equalsIgnoreCase(s) || "yes".equalsIgnoreCase(s)) return true;
        if ("false".equalsIgnoreCase(s) || "no".equalsIgnoreCase(s)) return false;
        return StringUtils.convertBoolean(s, def);
    }

    public int getIntAttribute(String name, int def) {
        String s = getStringAttribute(name);
        return StringUtils.convertInt(s, def);
    }

    public double getDoubleAttribute(String name, double def) {
        String s = getStringAttribute(name);
        return StringUtils.convertDouble(s, def);
    }

    public long getLongAttribute(String name, long def) {
        String s = getStringAttribute(name);
        return StringUtils.convertLong(s, def);
    }

    public short getShortAttribute(String name, short def) {
        String s = getStringAttribute(name);
        return StringUtils.convertShort(s, def);
    }

    public float getFloatAttribute(String name, float def) {
        String s = getStringAttribute(name);
        return StringUtils.convertFloat(s, def);
    }

    public String getName() {
        return e != null ? e.getNodeName() : null;
    }

    public String getContent() {
        return e != null ? e.getTextContent() : null;
    }

    public XMLConfigElement getChildByName(String name) {
        if (e == null) return this;
        NodeList nl = e.getElementsByTagName(name);
        for (int i = 0; i < nl.getLength(); ++i) {
            Node n = nl.item(i);
            Element ee = (Element) n;
            if (ee != null) return new XMLConfigElement(ee);
        }
        return new XMLConfigElement(null);
    }

    public ArrayList<XMLConfigElement> getChildListByName(String name) {
        ArrayList<XMLConfigElement> list = new ArrayList<XMLConfigElement>();
        if (e == null) return list;
        NodeList nl = e.getElementsByTagName(name);
        for (int i = 0; i < nl.getLength(); ++i) {
            Node n = nl.item(i);
            Element ee = (Element) n;
            if (ee != null) list.add(new XMLConfigElement(ee));
        }
        return list;
    }

    public ArrayList<XMLConfigElement> getChildList() {
        ArrayList<XMLConfigElement> list = new ArrayList<XMLConfigElement>();
        if (e == null) return list;
        NodeList nl = e.getChildNodes();
        for (int i = 0; i < nl.getLength(); ++i) {
            Node n = nl.item(i);
            if (n instanceof Element) {
                Element ee = (Element) n;
                list.add(new XMLConfigElement(ee));
            }
        }
        return list;
    }
}
