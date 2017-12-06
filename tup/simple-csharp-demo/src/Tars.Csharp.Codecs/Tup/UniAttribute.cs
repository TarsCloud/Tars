using DotNetty.Buffers;
using System;
using System.Collections;
using System.Collections.Generic;
using Tars.Csharp.Codecs.Util;

namespace Tars.Csharp.Codecs.Tup
{
    public class UniAttribute : TarsStruct
    {
        /// <summary>
        /// PACKET_TYPE_TUP类型
        /// </summary>
        protected Dictionary<string, Dictionary<string, byte[]>> data = new Dictionary<string, Dictionary<string, byte[]>>();

        /// <summary>
        /// 精简版tup，PACKET_TYPE_TUP3类型
        /// </summary>
        protected Dictionary<string, byte[]> newData = new Dictionary<string, byte[]>();

        /// <summary>
        /// 存储get后的数据 避免多次解析
        /// </summary>
        private Dictionary<string, object> cachedData = new Dictionary<string, object>(128);

        public short Version { get; set; } = Const.PacketTypeTup2;

        public string EncodeName { get; set; } = "UTF-8";

        public bool IsPacketTypeTup3 => Version == Const.PacketTypeTup3;

        public void ClearCacheData()
        {
            cachedData.Clear();
        }

        public bool IsEmpty()
        {
            return Size == 0;
        }

        public int Size
        {
            get
            {
                return IsPacketTypeTup3
                ? newData.Count
                : data.Count;
            }
        }

        public bool ContainsKey(string key)
        {
            return IsPacketTypeTup3
                ? newData.ContainsKey(key)
                : data.ContainsKey(key);
        }

        public override void ReadFrom(TarsInputStream inputStream)
        {
            if (IsPacketTypeTup3)
            {
                newData = (Dictionary<string, byte[]>)inputStream.ReadMap(newData, 0, false);
            }
            else
            {
                data = (Dictionary<string, Dictionary<string, byte[]>>)inputStream.ReadMap(data, 0, false);
            }
        }

        public override void WriteTo(TarsOutputStream outputStream)
        {
            if (IsPacketTypeTup3)
            {
                outputStream.Write<string, byte[]>(newData, 0);
            }
            else
            {
                outputStream.Write<string, Dictionary<string, byte[]>>(data, 0);
            }
        }

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

            var buf = Unpooled.Buffer(128);
            byte[] sBuffer = null;
            try
            {
                TarsOutputStream _out = new TarsOutputStream(buf);
                _out.SetServerEncoding(EncodeName);
                _out.Write(t, 0);
                sBuffer = _out.ToByteArray();
            }
            finally
            {
                buf.Release();
            }

            if (IsPacketTypeTup3)
            {
                cachedData.Remove(name);

                if (newData.ContainsKey(name))
                {
                    newData[name] = sBuffer;
                }
                else
                {
                    newData.Add(name, sBuffer);
                }
            }
            else
            {
                List<string> listType = new List<string>();
                CheckObjectType(listType, t);
                string className = BasicClassTypeUtil.TransTypeList(listType);

                Dictionary<string, byte[]> pair = new Dictionary<string, byte[]>(1)
                {
                    { className, sBuffer }
                };
                cachedData.Remove(name);

                if (data.ContainsKey(name))
                {
                    data[name] = pair;
                }
                else
                {
                    data.Add(name, pair);
                }
            }
        }

        private Object DecodeData(byte[] data, object proxy, TarsInputStream inputStream)
        {
            Object o = inputStream.Read(proxy, 0, true);
            return o;
        }

        /// <summary>
        /// 获取tup精简版本编码的数据,兼容旧版本tup
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="name"></param>
        /// <param name="proxy"></param>
        /// <returns></returns>
        public T GetByClass<T>(string name, T proxy, TarsInputStream inputStream)
        {
            object obj = null;
            if (IsPacketTypeTup3)
            {
                if (!newData.ContainsKey(name))
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
                        newData.TryGetValue(name, out data);

                        Object o = DecodeData(data, proxy, inputStream);
                        if (null != o)
                        {
                            SaveDataCache(name, o);
                        }
                        return (T)o;
                    }
                    catch (Exception ex)
                    {
                        throw ex;
                        //throw new ObjectCreateException(ex);
                    }
                }
            }
            else //兼容tup2
            {
                return Get<T>(name, inputStream);
            }
        }

        /// <summary>
        /// 获取一个元素,只能用于tup版本2，如果待获取的数据为tup3，则抛异常
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="name"></param>
        /// <returns></returns>
        public T Get<T>(string name, TarsInputStream inputStream)
        {
            if (IsPacketTypeTup3)
            {
                throw new Exception("data is encoded by new version, please use GetByClass(String name,T proxy)");
            }

            object obj = null;

            if (!data.ContainsKey(name))
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
                data.TryGetValue(name, out Dictionary<string, byte[]> pair);

                string strBasicType = "";
                string className = null;
                byte[] d = new byte[0];

                // 找到和T类型对应的数据data
                foreach (KeyValuePair<string, byte[]> e in pair)
                {
                    className = e.Key;
                    d = e.Value;

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

                    obj = DecodeData(d, objtmp, inputStream);
                    if (obj != null)
                    {
                        SaveDataCache(name, obj);
                    }
                    return (T)obj;
                }
                catch (Exception ex)
                {
                    throw ex;
                    //QTrace.Trace(this + " Get Exception: " + ex.Message);
                    //throw new ObjectCreateException(ex);
                }
            }
        }

        /// <summary>
        /// 获取一个元素,tup新旧版本都兼容
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="Name"></param>
        /// <param name="DefaultObj"></param>
        /// <returns></returns>
        public T Get<T>(string Name, T DefaultObj, TarsInputStream inputStream)
        {
            try
            {
                object result = null; ;

                if (IsPacketTypeTup3)
                {
                    result = GetByClass<T>(Name, DefaultObj, inputStream);
                }
                else //tup2
                {
                    result = Get<T>(Name, inputStream);
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

        /// <summary>
        /// 检测传入的元素类型
        /// </summary>
        /// <param name="listTpye"></param>
        /// <param name="o"></param>
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

        //public byte[] Encode()
        //{
        //    TarsOutputStream _os = new TarsOutputStream(0);
        //    _os.SetServerEncoding(EncodeName);
        //    if (IsPacketTypeTup3)
        //    {
        //        _os.Write(newData, 0);
        //    }
        //    else
        //    {
        //        _os.Write(data, 0);
        //    }
        //    return TarsUtil.GetTarsBufArray(_os.GetMemoryStream());
        //}

        public void DecodeTup2(TarsInputStream inputStream, int Index = 0)
        {
            inputStream.SetServerEncoding(EncodeName);
            Version = Const.PacketTypeTup2;
            data = new Dictionary<string, Dictionary<string, byte[]>>();
            inputStream.ReadMap<string, Dictionary<string, byte[]>>(data, 0, false);
        }

        public void DecodeTup3(TarsInputStream inputStream, int Index = 0)
        {
            Version = Const.PacketTypeTup3;
            inputStream.SetServerEncoding(EncodeName);
            newData = new Dictionary<string, byte[]>();
            inputStream.ReadMap<string, byte[]>(newData, 0, false);
        }

        //public void Decode(byte[] buffer, int Index = 0)
        //{
        //    try
        //    {
        //        //try tup2
        //        inputStream.Wrap(buffer, Index);
        //        inputStream.SetServerEncoding(EncodeName);
        //        Version = Const.PacketTypeTup;
        //        data = (Dictionary<string, Dictionary<string, byte[]>>)inputStream.ReadMap(data, 0, false);
        //        return;
        //    }
        //    catch
        //    {
        //        //try tup3
        //        Version = Const.PacketTypeTup3;
        //        inputStream.Wrap(buffer, Index);
        //        inputStream.SetServerEncoding(EncodeName);
        //        newData = (Dictionary<string, byte[]>)inputStream.ReadMap(newData, 0, false);
        //    }
        //}
    }
}