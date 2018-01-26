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

package com.qq.tars.client.support;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;

import com.qq.tars.client.util.ClientLogger;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.Loader;
import com.qq.tars.common.util.StringUtils;

public final class ServantCacheManager {

    private final static ServantCacheManager instance = new ServantCacheManager();

    private final Properties props = new Properties();
    private final AtomicBoolean propsInited = new AtomicBoolean();
    private final ReentrantLock lock = new ReentrantLock();

    private ServantCacheManager() {
    }

    public static ServantCacheManager getInstance() {
        return instance;
    }

    public String get(String CommunicatorId, String objName, String dataPath) {
        loadCacheData(dataPath);
        return props.getProperty(makeKey(CommunicatorId, objName));
    }

    private String makeKey(String CommunicatorId, String objName) {
        return objName + "@" + CommunicatorId;
    }

    public void save(String CommunicatorId, String objName, String endpointList, String dataPath) {
        try {
            loadCacheData(dataPath);
            props.remove(objName);
            props.put(makeKey(CommunicatorId, objName), endpointList);
            saveToLocal(dataPath);
        } catch (Throwable e) {
            ClientLogger.getLogger().error("", e);
        }
    }

    private File getCacheFile(String dataPath) throws Exception {
        String path = dataPath;
        if (StringUtils.isEmpty(path)) {
            URL url = Loader.getResource("", true);
            if (url != null) {
                path = url.getFile();
            }
            if (StringUtils.isEmpty(path)) {
                path = System.getProperty("user.dir");
            }
        }
        if (StringUtils.isEmpty(path)) {
            return null;
        }
        File f = new File(path, Constants.SERVER_NODE_CACHE_FILENAME);
        if (!f.exists()) {
            f.createNewFile();
        }
        return f;
    }

    private void saveToLocal(String dataPath) {
        lock.lock();
        OutputStream out = null;
        try {
            File file = getCacheFile(dataPath);
            if (file == null) {
                return;
            }
            out = new BufferedOutputStream(new FileOutputStream(file));
            props.store(out, (new Date()).toString());
            ClientLogger.getLogger().info("save " + file.getAbsolutePath());
        } catch (Exception e) {
            ClientLogger.getLogger().error("save " + Constants.SERVER_NODE_CACHE_FILENAME + " failed", e);
        } finally {
            if (null != out) {
                try {
                    out.close();
                } catch (IOException e) {
                }
            }
            lock.unlock();
        }
    }

    private void loadCacheData(String dataPath) {
        if (propsInited.get()) {
            return;
        }
        if (propsInited.compareAndSet(false, true)) {
            InputStream in = null;
            try {
                File file = getCacheFile(dataPath);
                if (file == null) {
                    return;
                }
                in = new BufferedInputStream(new FileInputStream(file));
                props.load(in);
                ArrayList<String> removeKey = new ArrayList<String>();
                for (Entry<Object, Object> entry : props.entrySet()) {
                    if (entry.getKey().toString().startsWith("<")) {
                        removeKey.add(entry.getKey().toString());
                    }
                }
                for (String key : removeKey) {
                    props.remove(key);
                }
                ClientLogger.getLogger().info("load " + Constants.SERVER_NODE_CACHE_FILENAME);
            } catch (Throwable e) {
                ClientLogger.getLogger().error("read file " + Constants.SERVER_NODE_CACHE_FILENAME + " error.", e);
            } finally {
                if (null != in) {
                    try {
                        in.close();
                    } catch (IOException e) {
                    }
                }
            }
        }
    }
}
