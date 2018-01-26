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

package com.qq.tars.common.util;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

public class Config {

    private static final String TAG_STARTER = "<";
    private static final String TAG_OVER = "</";
    private static final String TAG_ENDER = "/>";
    private static final String TAG_CLOSE = ">";
    private static final String ATTR_FLAGF = "=";
    private static final String COMMENT = "#";

    private final HashMap<String, List<String>> subTags = new HashMap<String, List<String>>();
    private final HashMap<String, List<String>> subKeys = new HashMap<String, List<String>>();
    private final HashMap<String, String> values = new HashMap<String, String>();
    private final HashMap<String, String> escapedValues = new HashMap<String, String>();

    private Config(String[] lines) {
        LinkedList<String> stack = new LinkedList<String>();
        stack.addLast("");
        subTags.put("", new ArrayList<String>());
        subKeys.put("", new ArrayList<String>());
        for (int i = 0; i < lines.length; ++i) {
            String line = lines[i].trim();
            if (line.length() <= 0 || line.startsWith(COMMENT)) {
                continue;
            }
            if (line.startsWith(TAG_OVER) && line.endsWith(TAG_CLOSE)) {
                String name = line.substring(TAG_OVER.length(), line.length() - TAG_CLOSE.length());
                String path = stack.getLast();
                if (!path.endsWith("/" + name)) {
                    throw new RuntimeException("Invalid tag end : " + name + " at line no " + i);
                }
                stack.removeLast();
            } else if (line.startsWith(TAG_STARTER) && line.endsWith(TAG_ENDER)) {
                String name = line.substring(TAG_STARTER.length(), line.length() - TAG_ENDER.length());
                String path = stack.getLast();
                String newPath = path + "/" + name;
                subTags.get(path).add(name);
                if (!subTags.containsKey(newPath)) {
                    subTags.put(newPath, new ArrayList<String>());
                }
                if (!subKeys.containsKey(newPath)) {
                    subKeys.put(newPath, new ArrayList<String>());
                }
            } else if (line.startsWith(TAG_STARTER) && line.endsWith(TAG_CLOSE)) {
                String name = line.substring(TAG_STARTER.length(), line.length() - TAG_CLOSE.length());
                String path = stack.getLast();
                String newPath = path + "/" + name;
                subTags.get(path).add(name);
                if (!subTags.containsKey(newPath)) {
                    subTags.put(newPath, new ArrayList<String>());
                }
                if (!subKeys.containsKey(newPath)) {
                    subKeys.put(newPath, new ArrayList<String>());
                }
                stack.addLast(newPath);
            } else if (line.startsWith(TAG_STARTER) || line.endsWith(TAG_CLOSE)) {
                throw new RuntimeException("Invalid line : " + line + " at line no " + i);
            } else {
                String key;
                String value;
                int idx = line.indexOf(ATTR_FLAGF);
                if (idx < 0) {
                    key = line.trim();
                    value = "";
                } else {
                    key = line.substring(0, idx).trim();
                    value = line.substring(idx + ATTR_FLAGF.length()).trim();
                }

                String path = stack.getLast();
                String newPath = path + "<" + key + ">";

                subKeys.get(path).add(key);
                values.put(newPath, value);
                escapedValues.put(newPath, unEscape(value));
            }
        }
    }

    private String unEscape(String value) {
        return value.replaceAll("&amp;", "&").replaceAll("&nbsp;", " ").replaceAll("&lt;", "<").replaceAll("&gt;", ">").replaceAll("&quot;", "\"");
    }

    public String get(String path, boolean escape) {
        return escape ? escapedValues.get(path) : values.get(path);
    }

    public String get(String path) {
        return get(path, false);
    }

    public String get(String path, String defaultValue) {
        String value = get(path, false);
        return value == null ? defaultValue : value;
    }

    public int getInt(String path, int defaultValue) {
        String str = get(path);
        if (str == null) {
            return defaultValue;
        }
        try {
            return Integer.parseInt(str);
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    public boolean getBool(String path, boolean defaultValue) {
        String str = get(path);
        if (str == null) {
            return defaultValue;
        }
        return Boolean.parseBoolean(str);
    }

    public List<String> getSubTags(String path) {
        return subTags.get(path);
    }

    public List<String> getSubKeys(String path) {
        return subKeys.get(path);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        toString(sb, "", "");
        return sb.toString();
    }

    private void toString(StringBuilder sb, String indent, String path) {
        for (String key : subKeys.get(path)) {
            String value = values.get(path + "<" + key + ">");
            sb.append(indent).append(key).append(ATTR_FLAGF).append(value).append("\n");
        }
        for (String name : subTags.get(path)) {
            String newPath = path + "/" + name;
            List<String> subTags = this.subTags.get(newPath);
            List<String> subKeys = this.subKeys.get(newPath);
            if (subTags.isEmpty() && subKeys.isEmpty()) {
                sb.append(indent).append(TAG_STARTER).append(name).append(TAG_ENDER).append("\n");
            } else {
                sb.append(indent).append(TAG_STARTER).append(name).append(TAG_CLOSE).append("\n");
                toString(sb, indent + "  ", newPath);
                sb.append(indent).append(TAG_OVER).append(name).append(TAG_CLOSE).append("\n");
            }
        }
    }

    public static Config parse(String content) {
        if (content == null) {
            throw new NullPointerException("content");
        }
        return new Config(content.split("\n"));
    }

    public static Config parse(Reader in) throws IOException {
        List<String> lineList = new ArrayList<String>();
        BufferedReader br = new BufferedReader(in);
        String line = null;
        while ((line = br.readLine()) != null) {
            lineList.add(line);
        }
        return new Config(lineList.toArray(new String[lineList.size()]));
    }

    public static Config parse(InputStream in) throws IOException {
        return parse(new InputStreamReader(in));
    }

    public static Config parse(InputStream in, Charset charset) throws IOException {
        return parse(new InputStreamReader(in, charset));
    }

    public static Config parseFile(String file, Charset charset) throws FileNotFoundException, IOException {
        return parse(new FileInputStream(file), charset);
    }

    public static Config parseFile(String file) throws FileNotFoundException, IOException {
        return parse(new FileInputStream(file));
    }

}
