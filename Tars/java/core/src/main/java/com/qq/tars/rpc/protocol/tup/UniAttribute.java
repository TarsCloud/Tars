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

package com.qq.tars.rpc.protocol.tup;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.Set;

import com.qq.tars.protocol.tars.TarsInputStream;
import com.qq.tars.protocol.tars.TarsOutputStream;
import com.qq.tars.protocol.util.TarsUtil;

public class UniAttribute extends OldUniAttribute {

    protected HashMap<String, byte[]> _newData = new HashMap<String, byte[]>();

    private HashMap<String, Object> cachedData = new HashMap<String, Object>();

    TarsInputStream _is = new TarsInputStream();

    public void useVersion3() {
        _newData = new HashMap<String, byte[]>();
    }

    public void clearCacheData() {
        cachedData.clear();
    }

    public Set<String> getKeySet() {
        if (null != _newData) {
            return Collections.unmodifiableSet(_newData.keySet());
        }
        return Collections.unmodifiableSet(_data.keySet());
    }

    public boolean isEmpty() {
        if (null != _newData) {
            return _newData.isEmpty();
        }
        return _data.isEmpty();
    }

    public int size() {
        if (null != _newData) {
            return _newData.size();
        }
        return _data.size();
    }

    public boolean containsKey(String key) {
        if (null != _newData) {
            return _newData.containsKey(key);
        }
        return _data.containsKey(key);
    }

    public <T> void put(String name, T t) {
        if (null != _newData) {
            if (name == null) {
                throw new IllegalArgumentException("put key can not is null");
            }
            if (t == null) {
                throw new IllegalArgumentException("put value can not is null");
            }
            if (t instanceof Set) {
                throw new IllegalArgumentException("can not support Set");
            }
            TarsOutputStream _out = new TarsOutputStream();
            _out.setServerEncoding(encodeName);
            _out.write(t, 0);
            byte[] _sBuffer = TarsUtil.getBufArray(_out.getByteBuffer());
            _newData.put(name, _sBuffer);
        } else {
            super.put(name, t);
        }
    }

    @SuppressWarnings("unchecked")
    public <T> T getStruct(String name, T proxy) throws ObjectCreateException {
        if (!_newData.containsKey(name)) {
            return null;
        } else if (cachedData.containsKey(name)) {
            return (T) cachedData.get(name);
        } else {
            byte[] data = _newData.get(name);
            try {
                Object o = decodeData(data, proxy);
                if (null != o) {
                    saveDataCache(name, o);
                }
                return (T) o;
            } catch (Exception ex) {
                throw new ObjectCreateException(ex);
            }
        }
    }

    public <T> T getByClass(String name, T proxy) throws ObjectCreateException {
        if (null != _newData) {
            if (!_newData.containsKey(name)) {
                return null;
            } else if (cachedData.containsKey(name)) {
                return (T) cachedData.get(name);
            } else {
                byte[] data = _newData.get(name);
                try {
                    Object o = decodeData(data, proxy);
                    if (null != o) {
                        saveDataCache(name, o);
                    }
                    return (T) o;
                } catch (Exception ex) {
                    throw new ObjectCreateException(ex);
                }
            }
        } else {
            if (!_data.containsKey(name)) {
                return null;
            } else if (cachedData.containsKey(name)) {
                return (T) cachedData.get(name);
            } else {
                HashMap<String, byte[]> pair = _data.get(name);
                String className = null;
                byte[] data = new byte[0];
                for (Entry<String, byte[]> e : pair.entrySet()) {
                    className = e.getKey();
                    data = e.getValue();
                    break;
                }
                try {
                    _is.warp(data);
                    _is.setServerEncoding(encodeName);
                    Object o = _is.read(proxy, 0, true);
                    saveDataCache(name, o);
                    return (T) o;
                } catch (Exception ex) {
                    throw new ObjectCreateException(ex);
                }
            }
        }

    }

    public <T> T getByClass(String name, T proxy, T defaultValue) throws ObjectCreateException {
        if (null != _newData) {
            if (!_newData.containsKey(name)) {
                return defaultValue;
            } else if (cachedData.containsKey(name)) {
                return (T) cachedData.get(name);
            } else {
                byte[] data = _newData.get(name);
                try {
                    Object o = decodeData(data, proxy);
                    if (null != o) {
                        saveDataCache(name, o);
                    }
                    return (T) o;
                } catch (Exception ex) {
                    throw new ObjectCreateException(ex);
                }
            }
        } else {
            if (!_data.containsKey(name)) {
                return defaultValue;
            } else if (cachedData.containsKey(name)) {
                return (T) cachedData.get(name);
            } else {
                HashMap<String, byte[]> pair = _data.get(name);
                String className = null;
                byte[] data = new byte[0];
                for (Entry<String, byte[]> e : pair.entrySet()) {
                    className = e.getKey();
                    data = e.getValue();
                    break;
                }
                try {
                    _is.warp(data);
                    _is.setServerEncoding(encodeName);
                    Object o = _is.read(proxy, 0, true);
                    saveDataCache(name, o);
                    return (T) o;
                } catch (Exception ex) {
                    throw new ObjectCreateException(ex);
                }
            }
        }

    }

    @SuppressWarnings("unchecked")
    public <T> T get(String name, T proxy, Object defaultValue) {
        if (!_newData.containsKey(name)) {
            return (T) defaultValue;
        }
        return getByClass(name, proxy);
    }

    private Object decodeData(byte[] data, Object proxy) {
        _is.warp(data);
        _is.setServerEncoding(encodeName);
        Object o = _is.read(proxy, 0, true);
        return o;
    }

    private void saveDataCache(String name, Object o) {
        cachedData.put(name, o);
    }

    public <T> T getStruct(String name) throws ObjectCreateException {
        if (null != _newData) {
            throw new RuntimeException("data is encoded by new version, please use getStruct(String name,T proxy)");
        } else {
            return (T) super.getStruct(name);
        }
    }

    public <T> T get(String name) throws ObjectCreateException {
        if (null != _newData) {
            throw new RuntimeException("data is encoded by new version, please use getByClass(String name, T proxy)");
        } else {
            return (T) super.get(name);
        }
    }

    public <T> T get(String name, Object defaultValue) {
        if (null != _newData) {
            throw new RuntimeException("data is encoded by new version, please use get(String name, T proxy, Object defaultValue)");
        } else {
            return (T) super.get(name, defaultValue);
        }
    }

    public <T> T remove(String name) throws ObjectCreateException {
        if (null != _newData) {
            if (!_newData.containsKey(name)) {
                return null;
            } else {
                _newData.remove(name);
                return null;
            }
        } else {
            return (T) super.remove(name);
        }

    }

    @SuppressWarnings("unchecked")
    public <T> T remove(String name, T proxy) throws ObjectCreateException {
        if (!_newData.containsKey(name)) {
            return null;
        } else {
            if (null != proxy) {
                byte[] data = _newData.remove(name);
                Object o = decodeData(data, proxy);
                return (T) o;
            } else {
                _newData.remove(name);
                return null;
            }
        }
    }

    public byte[] encode() {
        if (null != _newData) {
            TarsOutputStream _os = new TarsOutputStream(0);
            _os.setServerEncoding(encodeName);
            _os.write(_newData, 0);
            return TarsUtil.getBufArray(_os.getByteBuffer());
        }
        return super.encode();
    }

    public void decodeVersion3(byte[] buffer) {
        _is.warp(buffer);
        _is.setServerEncoding(encodeName);
        HashMap<String, byte[]> _tempdata = new HashMap<String, byte[]>(1);
        _tempdata.put("", new byte[0]);
        _newData = (HashMap<String, byte[]>) _is.readMap(_tempdata, 0, false);
    }

    public void decodeVersion2(byte[] buffer) {
        super.decode(buffer);
    }

    public void decode(byte[] buffer) {
        try {
            super.decode(buffer);
            return;
        } catch (Exception e) {
            _is.warp(buffer);
            _is.setServerEncoding(encodeName);
            HashMap<String, byte[]> _tempdata = new HashMap<String, byte[]>(1);
            _tempdata.put("", new byte[0]);
            _newData = (HashMap<String, byte[]>) _is.readMap(_tempdata, 0, false);
        }
    }

}
