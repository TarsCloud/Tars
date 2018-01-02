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

package com.qq.tars.rpc.protocol;

import com.qq.tars.common.util.Constants;
import com.qq.tars.net.protocol.ProtocolDecoder;
import com.qq.tars.net.protocol.ProtocolEncoder;
import com.qq.tars.net.protocol.ProtocolException;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;

import java.lang.reflect.InvocationTargetException;

public abstract class Codec implements ProtocolEncoder, ProtocolDecoder {

    protected String charsetName = Constants.default_charset_name;

    public Codec(String charsetName) {
        this.setCharsetName(charsetName);
    }

    public String getCharsetName() {
        return charsetName;
    }

    public void setCharsetName(String charsetName) {
        this.charsetName = charsetName;
    }

    public abstract String getProtocol();
}
