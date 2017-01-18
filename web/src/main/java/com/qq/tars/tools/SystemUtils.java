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

package com.qq.tars.tools;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.commons.lang3.tuple.Pair;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;

public class SystemUtils {

    private static final Logger log = LoggerFactory.getLogger(SystemUtils.class);

    public static Pair<Integer, Pair<String, String>> exec(String command) {
        log.info("start to exec shell, command={}", command);

        try {
            Process process = Runtime.getRuntime().exec("/bin/sh");

            OutputStream os = process.getOutputStream();
            os.write(command.getBytes());
            os.close();

            final StringBuilder stdout = new StringBuilder(1024);
            final StringBuilder stderr = new StringBuilder(1024);
            final BufferedReader stdoutReader = new BufferedReader(new InputStreamReader(process.getInputStream(), "GBK"));
            final BufferedReader stderrReader = new BufferedReader(new InputStreamReader(process.getErrorStream(), "GBK"));

            new Thread(() -> {
                String line;
                try {
                    while (null != (line = stdoutReader.readLine())) {
                        stdout.append(line).append("\n");
                    }
                } catch (IOException e) {
                    log.error("read stdout error", e);
                }
            }).start();

            new Thread(() -> {
                String line;
                try {
                    while (null != (line = stderrReader.readLine())) {
                        stderr.append(line).append("\n");
                    }
                } catch (IOException e) {
                    log.error("read stderr error", e);
                }
            }).start();

            int ret = process.waitFor();

            stdoutReader.close();
            stderrReader.close();

            Pair<String, String> output = Pair.of(stdout.toString(), stderr.toString());
            return Pair.of(ret, output);
        } catch (Exception e) {
            return Pair.of(-1, Pair.of("", ExceptionUtils.getStackTrace(e)));
        }
    }
}
