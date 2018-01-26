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

package com.qq.tars.support.log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;
import java.util.Properties;
import java.util.concurrent.atomic.AtomicBoolean;

import com.qq.tars.common.util.StringUtils;

public class LogConfCacheMngr {

    private final static String LOG_CACHE_FILE = "log_conf.dat";
    private final static String KEY_LOG_LEVEL = "level";

    private final static LogConfCacheMngr instance = new LogConfCacheMngr();

    private final Properties props = new Properties();
    private final AtomicBoolean propsInited = new AtomicBoolean();

    private String dataPath;

    private LogConfCacheMngr() {
    }

    public static LogConfCacheMngr getInstance() {
        return instance;
    }

    public void init(String theDataPath) {
        System.out.println("init dataPath=" + theDataPath);
        this.dataPath = theDataPath;
        loadCacheData();
    }

    public String get(String key) {
        loadCacheData();
        return props.getProperty(key);
    }

    public String getLevel() {
        loadCacheData();
        String level = props.getProperty(KEY_LOG_LEVEL);
        System.out.println("level=" + level);
        return level;
    }

    public void set(String key, String value) {
        loadCacheData();
        props.setProperty(key, value);
    }

    public void updateLevel(String level) {
        loadCacheData();
        props.setProperty(KEY_LOG_LEVEL, level);
        saveToLocal();
    }

    private File getCacheFile() throws Exception {
        if (StringUtils.isEmpty(dataPath)) {
            System.out.println("dataPath is null");
            return null;
        }
        File f = new File(dataPath, LOG_CACHE_FILE);
        if (!f.exists()) {
            f.createNewFile();
        }
        return f;
    }

    private void saveToLocal() {
        OutputStream out = null;
        try {
            File file = getCacheFile();
            if (file == null) {
                return;
            }
            out = new BufferedOutputStream(new FileOutputStream(file));
            props.store(out, (new Date()).toString());
        } catch (Exception e) {
            System.out.println("FATAL j4log | save j4log cache file failed: " + e.getMessage());
            e.printStackTrace(System.out);
        } finally {
            if (null != out) {
                try {
                    out.close();
                } catch (IOException e) {
                }
            }
        }
    }

    private void loadCacheData() {
        if (propsInited.get()) {
            return;
        }

        if (StringUtils.isEmpty(dataPath)) {
            return;
        }

        if (propsInited.compareAndSet(false, true)) {
            InputStream in = null;
            try {
                File file = getCacheFile();
                if (file == null) {
                    return;
                }
                in = new BufferedInputStream(new FileInputStream(file));
                props.load(in);

                System.out.println("prop=" + props.toString());
            } catch (Exception e) {
                System.out.println("FATAL j4log | load j4log cache file failed: " + e.getMessage());
                e.printStackTrace(System.out);
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
