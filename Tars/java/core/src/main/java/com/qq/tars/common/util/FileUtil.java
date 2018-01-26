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
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.StringWriter;
import java.util.List;

import com.qq.tars.rpc.exc.TarsException;

public class FileUtil {

    public static String readFileToString(String fileName, String charset) {
        String result = null;
        StringWriter sw = null;
        BufferedReader br = null;
        try {
            sw = new StringWriter();
            br = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), charset));
            int ch = 0;
            while ((ch = br.read()) != -1)
                sw.write(ch);
            result = sw.toString();
        } catch (Throwable t) {
            throw new TarsException("FileUtil|read error", t);
        } finally {
            try {
                if (sw != null) sw.close();
                if (br != null) br.close();
            } catch (Throwable tt) {
                tt.printStackTrace();
            }
        }
        return result;
    }

    public static void writeStringToFile(String fileName, String content, String charset) {
        BufferedWriter bw = null;
        try {
            File file = new File(fileName);
            if (!file.getParentFile().exists()) {
                file.getParentFile().mkdirs();
            }

            bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file), charset));
            bw.write(content);
        } catch (Throwable t) {
            throw new TarsException("FileUtil|write error", t);
        } finally {
            try {
                bw.close();
            } catch (Throwable tt) {
                tt.printStackTrace();
            }
        }
    }

    public static void writeLinesToFile(String fileName, List<String> lines, String charset) {
        BufferedWriter bw = null;
        try {
            bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName), charset));
            for (String line : lines)
                bw.write(line + "\n");
            bw.flush();
        } catch (Throwable t) {
            throw new TarsException("FileUtil|write lines error", t);
        } finally {
            try {
                bw.close();
            } catch (Throwable tt) {
                tt.printStackTrace();
            }
        }
    }

    public static void appendLinesToFile(String fileName, List<String> lines, String charset) {
        BufferedWriter bw = null;
        try {
            bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName, true), charset));
            for (String line : lines)
                bw.write(line.trim() + "\n");
            bw.flush();
        } catch (Throwable t) {
            throw new TarsException("FileUtil|write lines error", t);
        } finally {
            try {
                bw.close();
            } catch (Throwable tt) {
                tt.printStackTrace();
            }
        }
    }

    public static String readFileToString(String fileName) {
        return readFileToString(fileName, "UTF-8");
    }

    public static void writeStringToFile(String fileName, String content) {
        writeStringToFile(fileName, content, "UTF-8");
    }

    public static void appendLinesToFile(String fileName, List<String> lines) {
        appendLinesToFile(fileName, lines, "UTF-8");
    }

    public static void writeLinesToFile(String fileName, List<String> lines) {
        writeLinesToFile(fileName, lines, "UTF-8");
    }

}
