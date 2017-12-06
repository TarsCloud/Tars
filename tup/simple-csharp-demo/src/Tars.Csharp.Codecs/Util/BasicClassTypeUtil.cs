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

using System;
using System.Collections.Generic;
using System.Text;

namespace Tars.Csharp.Codecs.Util
{
    internal class BasicClassTypeUtil
    {
        /**
         * 将嵌套的类型转成字符串
         * @param listTpye
         * @return
         */

        public static string TransTypeList(List<string> listTpye)
        {
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < listTpye.Count; i++)
            {
                listTpye[i] = CS2UniType(listTpye[i]);
            }

            listTpye.Reverse();

            for (int i = 0; i < listTpye.Count; i++)
            {
                string type = (string)listTpye[i];

                if (type == null)
                {
                    continue;
                }

                if (type.Equals("list"))
                {
                    listTpye[i - 1] = "<" + listTpye[i - 1];
                    listTpye[0] = listTpye[0] + ">";
                }
                else if (type.Equals("map"))
                {
                    listTpye[i - 1] = "<" + listTpye[i - 1] + ",";
                    listTpye[0] = listTpye[0] + ">";
                }
                else if (type.Equals("Array"))
                {
                    listTpye[i - 1] = "<" + listTpye[i - 1];
                    listTpye[0] = listTpye[0] + ">";
                }
            }
            listTpye.Reverse();
            foreach (string s in listTpye)
            {
                sb.Append(s);
            }
            return sb.ToString();
        }

        public static object CreateObject<T>()
        {
            return CreateObject(typeof(T));
        }

        public static object CreateObject(Type t)
        {
            try
            {
                var type = t.IsByRef ? t.GetElementType() : t;

                // String类型没有缺少构造函数，
                if (type.ToString() == "System.String")
                {
                    return "";
                }
                else if (type == typeof(byte[]))
                {
                    return new byte[0];
                }
                else if (type == typeof(short[]))
                {
                    return new short[0];
                }
                else if (type == typeof(ushort[]))
                {
                    return new ushort[0];
                }
                else if (type == typeof(int[]))
                {
                    return new int[0];
                }
                else if (type == typeof(uint[]))
                {
                    return new uint[0];
                }
                else if (type == typeof(long[]))
                {
                    return new long[0];
                }
                else if (type == typeof(ulong[]))
                {
                    return new ulong[0];
                }
                else if (type == typeof(char[]))
                {
                    return new char[0];
                }

                return Activator.CreateInstance(type);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static object CreateListItem(Type typeList)
        {
            Type[] itemType = typeList.GetGenericArguments();
            if (itemType == null || itemType.Length == 0)
            {
                return null;
            }
            return CreateObject(itemType[0]);
        }

        public static string CS2UniType(string srcType)
        {
            if (srcType.Equals("System.Int16"))
            {
                return "short";
            }
            else if (srcType.Equals("System.UInt16"))
            {
                return "ushort";
            }
            else if (srcType.Equals("System.Int32"))
            {
                return "int32";
            }
            else if (srcType.Equals("System.UInt32"))
            {
                return "uint32";
            }
            else if (srcType.Equals("System.Boolean"))
            {
                return "bool";
            }
            else if (srcType.Equals("System.Byte"))
            {
                return "char";
            }
            else if (srcType.Equals("System.Double"))
            {
                return "double";
            }
            else if (srcType.Equals("System.Single"))
            {
                return "float";
            }
            else if (srcType.Equals("System.Int64"))
            {
                return "int64";
            }
            else if (srcType.Equals("System.UInt64"))
            {
                return "uint64";
            }
            else if (srcType.Equals("System.String"))
            {
                return "string";
            }
            else if (srcType.IndexOf("System.Collections.Generic.QDictionary") == 0)
            {
                return "map";
            }
            else if (srcType.IndexOf("System.Collections.Generic.List") == 0)
            {
                return "list";
            }
            else
            {
                return srcType;
            }
        }

        public static bool IsQDictionary(string cls)
        {
            return cls.IndexOf("System.Collections.Generic.QDictionary") == 0;
        }
    }
}