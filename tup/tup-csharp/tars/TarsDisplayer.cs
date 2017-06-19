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
using System.Text;
using System.Collections.Generic;
using System.Collections;
using Tup;

namespace Tup.Tars
{
    /**
     * 格式化输出tars结构的所有属性
     * 主要用于调试或打日志
     */
    public class TarsDisplayer
    {
        private StringBuilder sb;
        private int _level = 0;

        private void ps(string fieldName)
        {
            for (int i = 0; i < _level; ++i)
            {
                sb.Append('\t');
            }

            if (fieldName != null)
            {
                sb.Append(fieldName).Append(": ");
            }
        }

        public TarsDisplayer(StringBuilder sb, int level)
        {
            this.sb = sb;
            this._level = level;
        }

        public TarsDisplayer(StringBuilder sb)
        {
            this.sb = sb;
        }

        public TarsDisplayer Display(bool b, string fieldName)
        {
            ps(fieldName);
            sb.Append(b ? 'T' : 'F').Append('\n');
            return this;
        }

        public TarsDisplayer Display(byte n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(char n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(short n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(int n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(long n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(float n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(double n, string fieldName)
        {
            ps(fieldName);
            sb.Append(n).Append('\n');
            return this;
        }

        public TarsDisplayer Display(string s, string fieldName)
        {
            ps(fieldName);
            if (null == s)
            {
                sb.Append("null").Append('\n');
            }
            else
            {
                sb.Append(s).Append('\n');
            }

            return this;
        }

        public TarsDisplayer Display(byte[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (byte o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(char[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (char o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(short[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (short o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(int[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (int o in v)
                jd.Display(o, null);
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(long[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (long o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(float[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (float o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display(double[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (double o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display<K, V>(Dictionary<K, V> m, string fieldName)
        {
            ps(fieldName);
            if (null == m)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (m.Count == 0)
            {
                sb.Append(m.Count).Append(", {}").Append('\n');
                return this;
            }
            sb.Append(m.Count).Append(", {").Append('\n');
            TarsDisplayer jd1 = new TarsDisplayer(sb, _level + 1);
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 2);
            foreach (KeyValuePair<K, V> en in m)
            {
                jd1.Display('(', null);
                jd.Display(en.Key, null);
                jd.Display(en.Value, null);
                jd1.Display(')', null);
            }
            Display('}', null);
            return this;
        }

        public TarsDisplayer Display<T>(T[] v, string fieldName)
        {
            ps(fieldName);
            if (null == v)
            {
                sb.Append("null").Append('\n');
                return this;
            }
            if (v.Length == 0)
            {
                sb.Append(v.Length).Append(", []").Append('\n');
                return this;
            }
            sb.Append(v.Length).Append(", [").Append('\n');
            TarsDisplayer jd = new TarsDisplayer(sb, _level + 1);
            foreach (T o in v)
            {
                jd.Display(o, null);
            }
            Display(']', null);
            return this;
        }

        public TarsDisplayer Display<T>(List<T> v, string fieldName)
        {
            if (null == v)
            {
                ps(fieldName);
                sb.Append("null").Append('\n');
                return this;
            }
            else
            {
                for (int i = 0; i < v.Count; i++)
                {
                    Display(v[i], fieldName);
                }
                return this;
            }
        }

        ////@SuppressWarnings("unchecked")
        public TarsDisplayer Display<T>(T o, string fieldName)
        {
            object oObject = o;

            if (null == o)
            {
                sb.Append("null").Append('\n');
            }

            else if (o is byte)
            {
                Display(((byte)oObject), fieldName);
            }
            else if (o is bool)
            {
                Display(((bool)oObject), fieldName);
            }
            else if (o is short)
            {
                Display(((short)oObject), fieldName);
            }
            else if (o is int)
            {
                Display(((int)oObject), fieldName);
            }
            else if (o is long)
            {
                Display(((long)oObject), fieldName);
            }
            else if (o is float)
            {
                Display(((float)oObject), fieldName);
            }
            else if (o is Double)
            {
                Display(((Double)oObject), fieldName);
            }
            else if (o is string)
            {
                Display((string)oObject, fieldName);
            }
            else if (o is TarsStruct)
            {
                Display((TarsStruct)oObject, fieldName);
            }
            else if (o is byte[])
            {
                Display((byte[])oObject, fieldName);
            }
            else if (o is bool[])
            {
                Display((bool[])oObject, fieldName);
            }
            else if (o is short[])
            {
                Display((short[])oObject, fieldName);
            }
            else if (o is int[])
            {
                Display((int[])oObject, fieldName);
            }
            else if (o is long[])
            {
                Display((long[])oObject, fieldName);
            }
            else if (o is float[])
            {
                Display((float[])oObject, fieldName);
            }
            else if (o is double[])
            {
                Display((double[])oObject, fieldName);
            }
            else if (o.GetType().IsArray)
            {
                Display((Object[])oObject, fieldName);
            }
            else if (o is IList)
            {
                IList list = (IList)oObject;

                List<object> tmplist = new List<object>();
                foreach (object obj in list)
                {
                    tmplist.Add(obj);
                }
                Display(tmplist, fieldName);
            }
            else
            {
                throw new TarsEncodeException("write object error: unsupport type.");
            }

            return this;
        }
        public TarsDisplayer Display(TarsStruct v, string fieldName)
        {
            Display('{', fieldName);
            if (null == v)
            {
                sb.Append('\t').Append("null");
            }
            else
            {
                v.Display(sb, _level + 1);
            }

            Display('}', null);
            return this;
        }
    }

}
