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

package com.qq.tars.common.support;

public class Endpoint {

    private final String type;
    private final String host; 
    private final int port; 

    private final int timeout;
    private final int grid; 
    private final int qos; 
    private final String setDivision; 

    private int hash; 

    public static final Endpoint VOID_ENDPOINT = new Endpoint("tcp", "0.0.0.0", 0, 0, 0, 0, null);

    public Endpoint(String type, String host, int port, int timeout, int grid, int qos, String setDivision) {
        this.type = type;
        this.host = host;
        this.port = port;
        this.timeout = timeout;
        this.grid = grid;
        this.qos = qos;
        this.setDivision = setDivision;
    }

    public String type() {
        return type;
    }

    public String host() {
        return host;
    }

    public int port() {
        return port;
    }

    public int timeout() {
        return timeout;
    }

    public int grid() {
        return grid;
    }

    public int qos() {
        return qos;
    }

    public String setDivision() {
        return setDivision;
    }

    @Override
    public int hashCode() {
        int h = hash;
        if (h == 0) {
            h = 1;
            h = 31 * h + type.hashCode();
            h = 31 * h + host.hashCode();
            h = 31 * h + port;
            hash = h;
        }
        return h;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null) return false;
        if (getClass() != obj.getClass()) return false;
        Endpoint other = (Endpoint) obj;
        if (host == null) {
            if (other.host != null) return false;
        } else if (!host.equals(other.host)) return false;
        if (port != other.port) return false;
        if (type != other.type) return false;
        return true;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(type.toLowerCase());
        sb.append(" -h ").append(host);
        sb.append(" -p ").append(port);
        if (timeout > 0) {
            sb.append(" -t ").append(timeout);
        }
        if (grid > 0) {
            sb.append(" -g ").append(grid);
        }
        if (qos > 0) {
            sb.append(" -q ").append(qos);
        }
        if (setDivision != null && !setDivision.trim().isEmpty()) {
            sb.append(" -s ").append(setDivision);
        }
        return sb.toString();
    }

    public static Endpoint parseString(String node) {
        String type = null;
        String host = null;
        int port = 0;
        int timeout = 3000;
        int grid = 0;
        int qos = 0;
        String setDivision = null;

        String[] fields = node.split("\\s+");
        if ("tcp".equalsIgnoreCase(fields[0])) {
            type = "tcp";
        } else if ("udp".equalsIgnoreCase(fields[0])) {
            type = "udp";
        } else {
            return null;
        }

        int i = 0;
        while (i < fields.length) {
            if ("-h".equals(fields[i]) && i + 1 < fields.length) {
                host = fields[i + 1];
                i += 2;
            } else if ("-p".equals(fields[i]) && i + 1 < fields.length) {
                port = Integer.parseInt(fields[i + 1]);
                i += 2;
            } else if ("-t".equals(fields[i]) && i + 1 < fields.length) {
                timeout = Integer.parseInt(fields[i + 1]);
                i += 2;
            } else if ("-g".equals(fields[i]) && i + 1 < fields.length) {
                grid = Integer.parseInt(fields[i + 1]);
                i += 2;
            } else if ("-q".equals(fields[i]) && i + 1 < fields.length) {
                qos = Integer.parseInt(fields[i + 1]);
                i += 2;
            } else if ("-s".equals(fields[i]) && i + 1 < fields.length) {
                setDivision = fields[i + 1];
                i += 2;
            } else {
                i++;
            }
        }

        if (host == null || host.isEmpty()) {
            return null;
        } else if (host.equals("*")) {
            host = "0.0.0.0";
        }

        if (port < 0 || port > 65535) {
            return null;
        }

        if (timeout <= 0) {
            return null;
        }

        if (grid < 0) {
            return null;
        }
        return new Endpoint(type, host, port, timeout, grid, qos, setDivision);
    }
}
