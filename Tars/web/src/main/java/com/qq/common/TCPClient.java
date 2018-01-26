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

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;

public class TCPClient {

    private final Logger log = LoggerFactory.getLogger(TCPClient.class);

    private static int MAX_PACK_SIZE = 1024 * 1024 * 64;//最大包长64M

    private String ip;

    private int port;

    public TCPClient(String ip, int port) {
        this.ip = ip;
        this.port = port;
    }

    public List<String> sendAndReceive(byte[] request, int timeout) throws IOException {
        Socket socket = null;
        InputStream input = null;
        OutputStream output = null;
        try {
            socket = new Socket();
            InetSocketAddress addr = new InetSocketAddress(ip, port);
            socket.connect(addr, timeout);
            socket.setSoTimeout(timeout);

            input = socket.getInputStream();
            output = socket.getOutputStream();

            output.write(request);
            output.flush();

            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
            List<String> sb = new LinkedList<>();
            String line;
            while ((line = reader.readLine()) != null) {
                sb.add(line);
                if ("endline".equals(line)) {
                    break;
                }
            }
            return sb;
        } finally {
            try {
                if (input != null) input.close();
            } catch (Exception e) {
                log.error("close io exception", e);
            }
            try {
                if (output != null) output.close();
            } catch (Exception e) {
                log.error("close io exception", e);
            }
            try {
                if (socket != null) socket.close();
            } catch (Exception e) {
                log.error("close socket exception", e);
            }
        }
    }
}
