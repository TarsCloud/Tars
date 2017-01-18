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

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.security.MessageDigest;

public class CodecUtils {

    private static final Logger log = LoggerFactory.getLogger(CodecUtils.class);

    public static String makeMD5(byte[] data) {
        char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        MessageDigest digest;
        try {
            digest = MessageDigest.getInstance("MD5");
        } catch (Exception e) {
            log.error("get message digest instance error", e);
            return null;
        }

        digest.update(data);
        byte[] result = digest.digest();
        char[] rtnAry = new char[16 * 2];
        int t = 0;
        for (int i = 0; i < 16; i++) {
            byte bt = result[i];
            rtnAry[t++] = hexDigits[bt >>> 4 & 0xf];
            rtnAry[t++] = hexDigits[bt & 0xf];
        }
        return new String(rtnAry);
    }
}
