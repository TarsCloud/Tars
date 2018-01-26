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

import java.net.URL;

import com.qq.tars.rpc.exc.TarsException;

public class Loader {

    public static URL getResource(String resource) {
        return getResource(resource, false);
    }

    public static URL getResource(String resource, boolean ignoreTCL) {
        ClassLoader classLoader = null;
        URL url = null;
        try {
            if (!ignoreTCL) {
                classLoader = Thread.currentThread().getContextClassLoader();
                if (classLoader != null) {
                    url = classLoader.getResource(resource);
                    if (url != null) {
                        return url;
                    }
                }
            }
            classLoader = Loader.class.getClassLoader();
            if (classLoader != null) {
                url = classLoader.getResource(resource);
                if (url != null) {
                    return url;
                }
            }
        } catch (Exception t) {
            throw new TarsException("caught Exception while in Loader.getResource. This may be innocuous");
        }
        return ClassLoader.getSystemResource(resource);
    }
}
