/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
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


using Tup.Tars;

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace Tup
{
    public class UniAttribute : TarsStruct
    {
        /**
         * PACKET_TYPE_TUP类型
         */
        protected Dictionary<string, Dictionary<string, byte[]>> _data = null;
        /**
         * 精简版tup，PACKET_TYPE_TUP3类型
         */
        protected Dictionary<string, byte[]> _new_data = null;

        /**
         * 存储get后的数据 避免多次解析
         */
        private Dictionary<string, object> cachedData = new Dictionary<string, object>(128);

        protected short _iVer = Const.PACKET_TYPE_TUP;
        public short Version
        {
            get
            {
                return _iVer;
            }
            set
            {
                _iVer = value;
            }
        }

        private string _encodeName = "UTF-8";
        public string EncodeName
        {
            get
            {
                return _encodeName;
            }
            set
            {
                _encodeName = value;
            }
        }

        TarsInputStream _is = new TarsInputStream();

        public UniAttribute()
        {
            _data = new Dictionary<string, Dictionary<string, byte[]>>();
            _new_data = new Dictionary<string, byte[]>();
        }

        /**
         * 清除缓存的解析过的数据
         */
        public void ClearCacheData()
        {
            cachedData.Clear();
        }

        public bool IsEmpty()
        {
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                return _new_data.Count == 0;
            }
            else
            {
                return _data.Count == 0;
            }
        }

        public int Size
        {
            get
            {
                if (_iVer == Const.PACKET_TYPE_TUP3)
                {
                    return _new_data.Count;
                }
                else
                {
                    return _data.Count;
                }
            }
        }

        public bool ContainsKey(string key)
        {
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                return _new_data.ContainsKey(key);
            }
            else
            {
                return _data.ContainsKey(key);
            }
        }

        /**
         * 放入一个元素
         * @param <T>
         * @param name
         * @param t
         */
        public void Put<T>(string name, T t)
        {
            if (name == null)
            {
                throw new ArgumentException("put key can not is null");
            }
            if (t == null)
            {
                throw new ArgumentException("put value can not is null");
            }

            TarsOutputStream _out = new TarsOutputStream();
            _out.setServerEncoding(_encodeName);
            _out.Write(t, 0);
			byte[] sBuffer = TarsUtil.getTarsBufArray(_out.getMemoryStream());

            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                cachedData.Remove(name);

                if (_new_data.ContainsKey(name))
                {
                    _new_data[name] = sBuffer;
                }
                else
                {
                    _new_data.Add(name, sBuffer);
                }
            }
            else
            {
                List<string> listType = new List<string>();
                CheckObjectType(listType, t);
                string className = BasicClassTypeUtil.TransTypeList(listType);

                Dictionary<string, byte[]> pair = new Dictionary<string, byte[]>(1);
                pair.Add(className, sBuffer);
                cachedData.Remove(name);

                if (_data.ContainsKey(name))
                {
                    _data[name] = pair;
                }
                else
                {
                    _data.Add(name, pair);
                }
            }
        }

        private Object decodeData(byte[] data, Object proxy)
        {
            _is.wrap(data);
            _is.setServerEncoding(_encodeName);
            Object o = _is.read(proxy, 0, true);
            return o;
        }

        /**
         * 获取tup精简版本编码的数据,兼容旧版本tup
         * @param <T>
         * @param name
         * @param proxy
         * @return
         * @throws ObjectCreateException
         */
        public T getByClass<T>(string name, T proxy)
        {
            object obj = null;
            if(_iVer == Const.PACKET_TYPE_TUP3)
            {

                if (!_new_data.ContainsKey(name))
                {
                    return (T)obj;
                }
                else if (cachedData.ContainsKey(name))
                {
                    if (!cachedData.TryGetValue(name, out obj))
                    {
                        obj = null;
                    }
                    return (T)obj;
                }
                else
                {
                    try
                    {
                        byte[] data = new byte[0];
                        _new_data.TryGetValue(name, out data);

                        Object o = decodeData(data, proxy);
                        if (null != o)
                        {
                            SaveDataCache(name, o);
                        }
                        return (T)o;
                    }
                    catch (Exception ex)
                    {
                        throw new ObjectCreateException(ex);
                    }
                }
            }
            else //兼容tup2
            {
                return Get<T>(name);
            }
        }


        /**
         * 获取一个元素,只能用于tup版本2，如果待获取的数据为tup3，则抛异常
         * @param <T>
         * @param name
         * @return
         * @throws ObjectCreateException
         */
        public T Get<T>(string name)
        {
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                throw new Exception("data is encoded by new version, please use getTarsStruct(String name,T proxy)");
            }

            object obj = null;

            if (!_data.ContainsKey(name))
            {
                return (T)obj;
            }
            else if (cachedData.ContainsKey(name))
            {
                if (!cachedData.TryGetValue(name, out obj))
                {
                    obj = null;
                }
                return (T)obj;
            }
            else
            {
                Dictionary<string, byte[]> pair;
                _data.TryGetValue(name, out pair);

                string strBasicType = "";
                string className = null;
                byte[] data = new byte[0];

                // 找到和T类型对应的数据data
                foreach (KeyValuePair<string, byte[]> e in pair)
                {
                    className = e.Key;
                    data = e.Value;

                    if (className == null || className == string.Empty)
                    {
                        continue;
                    }

                    // 比较基本类型
                    strBasicType = BasicClassTypeUtil.CS2UniType(typeof(T).ToString());
                    if (className.Length > 0 && className == strBasicType)
                    {
                        break;
                    }
                    if (strBasicType == "map" && className.Length >= 3 && className.Substring(0, 3).ToLower() == "map")
                    {
                        break;
                    }
                    if (typeof(T).IsArray && className.Length > 3 && className.Substring(0, 4).ToLower() == "list")
                    {
                        break;
                    }
                    if (strBasicType == "list" && className.Length > 3 && className.Substring(0, 4).ToLower() == "list")
                    {
                        break;
                    }
                }

                try
                {
                    object objtmp = GetCacheProxy<T>(className);
                    if (objtmp == null)
                    {
                        return (T)objtmp;
                    }
       
                    obj = decodeData(data, objtmp);
                    if (obj != null)
                    {
                        SaveDataCache(name, obj);
                    }
                    return (T)obj;
                }
                catch (Exception ex)
                {
                    QTrace.Trace(this + " Get Exception: " + ex.Message);
                    throw new ObjectCreateException(ex);
                }
            }
        }

        /**
          * 获取一个元素,tup新旧版本都兼容
          * @param Name
          * @param DefaultObj
          * @return
          * @throws ObjectCreateException
          */
        public T Get<T>(string Name, T DefaultObj)
        {
            try
            {
                object result = null; ;
             
                if (_iVer == Const.PACKET_TYPE_TUP3)
                {
                    result = getByClass<T>(Name, DefaultObj);
                    
                }
                else //tup2
                {
                    result = Get<T>(Name);
                }
                
                if (result == null)
                {
                    return DefaultObj;
                }
                return (T)result;
            }
            catch
            {
                return DefaultObj;
            }
        }

        private Object GetCacheProxy<T>(string className)
        {
            return BasicClassTypeUtil.CreateObject<T>();
        }

        private void SaveDataCache(string name, Object o)
        {
            cachedData.Add(name, o);
        }

        /**
         * 检测传入的元素类型
         * 
         * @param listTpye
         * @param o
         */
        private void CheckObjectType(List<string> listTpye, Object o)
        {
            if (o == null)
            {
                throw new Exception("object is null");
            }

            if (o.GetType().IsArray)
            {
                Type elementType = o.GetType().GetElementType();
                listTpye.Add("list");
                CheckObjectType(listTpye, BasicClassTypeUtil.CreateObject(elementType));
            }
            else if (o is IList)
            {
                listTpye.Add("list");

                IList list = (IList)o;
                if (list.Count > 0)
                {
                    CheckObjectType(listTpye, list[0]);
                }
                else
                {
                    listTpye.Add("?");
                }
            }
            else if (o is IDictionary)
            {
                listTpye.Add("map");
                IDictionary map = (IDictionary)o;
                if (map.Count > 0)
                {
                    foreach (object key in map.Keys)
                    {
                        listTpye.Add(BasicClassTypeUtil.CS2UniType(key.GetType().ToString()));
                        CheckObjectType(listTpye, map[key]);
                        break;
                    }
                }
                else
                {
                    listTpye.Add("?");
                    listTpye.Add("?");
                    //throw new ArgumentException("map  can not is empty");
                }
            }
            else
            {
                listTpye.Add(BasicClassTypeUtil.CS2UniType(o.GetType().ToString()));
            }
        }

        public byte[] Encode()
        {
            TarsOutputStream _os = new TarsOutputStream(0);
            _os.setServerEncoding(_encodeName);
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                _os.Write(_new_data, 0);
            }
            else
            {
                _os.Write(_data, 0);
            }
            return TarsUtil.getTarsBufArray(_os.getMemoryStream());
        }

        public void Decode(byte[] buffer, int Index = 0)
        {
            try
            {
                //try tup2
                _is.wrap(buffer, Index);
                _is.setServerEncoding(_encodeName);
                _iVer = Const.PACKET_TYPE_TUP;
                _data = (Dictionary<string, Dictionary<string, byte[]>>)_is.readMap<Dictionary<string, Dictionary<string, byte[]>>>(_data,0, false);
                return;
            }catch
            {
                //try tup3
                _iVer = Const.PACKET_TYPE_TUP3;
                _is.wrap(buffer, Index);
                _is.setServerEncoding(_encodeName);
                _new_data = (Dictionary<string, byte[]>)_is.readMap<Dictionary<string, byte[]>>(_new_data,0, false);     
            }
        }

        public override void WriteTo(TarsOutputStream _os)
        {
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                _os.Write(_new_data, 0);
            }
            else
            {
                _os.Write(_data, 0);
            }
        }

        public override void ReadFrom(TarsInputStream _is)
        {
            if (_iVer == Const.PACKET_TYPE_TUP3)
            {
                _new_data = (Dictionary<string, byte[]>)_is.readMap<Dictionary<string, byte[]>>(_new_data, 0, false); 
            }
            else
            {
                _data = (Dictionary<string, Dictionary<string, byte[]>>)_is.readMap<Dictionary<string, Dictionary<string, byte[]>>>(_data, 0, false);
            }
        }
    }
}
