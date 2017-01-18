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

package com.qq.tars.rpc.ext;

import com.qq.tars.net.protocol.ProtocolDecoder;
import com.qq.tars.net.protocol.ProtocolEncoder;
import com.qq.tars.net.protocol.ProtocolFactory;
import com.qq.tars.rpc.protocol.Codec;

public class ExtendedProtocolFactory implements ProtocolFactory {

    private ExtendedProtocolCodecWrapper codecWrapper = null;

    private static ExtendedProtocolFactory instance = null;

    public static final ExtendedProtocolFactory getInstance() {
        return instance;
    }

    public static final void registerExtendedCodecImpl(Codec codec, String contextName) {
        if (instance == null) {
            instance = buildExtendedProtocolFactory(codec);
        } else {
            if (instance.codecWrapper.codec.getClass() != codec.getClass()) {
                throw new RuntimeException("Found too many codec implementation classes.");
            }
        }
    }

    public static final ExtendedProtocolFactory buildExtendedProtocolFactory(Codec codec) {
        return new ExtendedProtocolFactory(codec);
    }

    private ExtendedProtocolFactory(Codec codec) {
        this.codecWrapper = new ExtendedProtocolCodecWrapper(codec);
    }

    public ProtocolDecoder getDecoder() {
        return this.codecWrapper;
    }

    public ProtocolEncoder getEncoder() {
        return this.codecWrapper;
    }
}
