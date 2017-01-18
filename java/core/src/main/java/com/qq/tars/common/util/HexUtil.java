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

public class HexUtil {

    private static final char[] digits = new char[] { '0', '1', '2', '3', '4',//
    '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    public static final byte[] emptybytes = new byte[0];

    public static String byte2HexStr(byte b) {
        char[] buf = new char[2];
        buf[1] = digits[b & 0xF];
        b = (byte) (b >>> 4);
        buf[0] = digits[b & 0xF];
        return new String(buf);
    }

    public static String bytes2HexStr(byte[] bytes) {
        if (bytes == null || bytes.length == 0) {
            return null;
        }

        char[] buf = new char[2 * bytes.length];
        for (int i = 0; i < bytes.length; i++) {
            byte b = bytes[i];
            buf[2 * i + 1] = digits[b & 0xF];
            b = (byte) (b >>> 4);
            buf[2 * i + 0] = digits[b & 0xF];
        }
        return new String(buf);
    }

    public static byte hexStr2Byte(String str) {
        if (str != null && str.length() == 1) {
            return char2Byte(str.charAt(0));
        } else {
            return 0;
        }
    }

    public static byte char2Byte(char ch) {
        if (ch >= '0' && ch <= '9') {
            return (byte) (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            return (byte) (ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            return (byte) (ch - 'A' + 10);
        } else {
            return 0;
        }
    }

    public static byte[] hexStr2Bytes(String str) {
        if (str == null || str.equals("")) {
            return emptybytes;
        }

        byte[] bytes = new byte[str.length() / 2];
        for (int i = 0; i < bytes.length; i++) {
            char high = str.charAt(i * 2);
            char low = str.charAt(i * 2 + 1);
            bytes[i] = (byte) (char2Byte(high) * 16 + char2Byte(low));
        }
        return bytes;
    }
}
