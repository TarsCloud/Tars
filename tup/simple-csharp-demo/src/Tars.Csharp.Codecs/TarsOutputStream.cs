using DotNetty.Buffers;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Tars.Csharp.Codecs.Util;

namespace Tars.Csharp.Codecs
{
    public class TarsOutputStream
    {
        private IByteBuffer buf;

        public TarsOutputStream(IByteBuffer buf)
        {
            this.buf = buf;
        }

        public byte[] ToByteArray()
        {
            byte[] newBytes = new byte[buf.WriterIndex];
            buf.SetReaderIndex(0);
            buf.ReadBytes(newBytes, 0, newBytes.Length);
            buf.SetReaderIndex(0);
            return newBytes;
        }

        public void Reserve(int len)
        {
            buf.EnsureWritable(len, true);
        }

        // 写入头信息
        public void WriteHead(byte type, int tag)
        {
            if (tag < 15)
            {
                byte b = (byte)((tag << 4) | type);
                buf.WriteByte(b);
            }
            else if (tag < 256)
            {
                byte b = (byte)((15 << 4) | type);
                buf.WriteByte(b);
                buf.WriteByte(tag);
            }
            else
            {
                throw new TarsEncodeException("tag is too large: " + tag);
            }
        }

        public void Write(bool b, int tag)
        {
            byte by = (byte)(b ? 0x01 : 0);
            Write(by, tag);
        }

        public void Write(byte b, int tag)
        {
            Reserve(3);
            if (b == 0)
            {
                WriteHead((byte)TarsStructType.ZeroTag, tag);
            }
            else
            {
                WriteHead((byte)TarsStructType.Byte, tag);
                buf.WriteByte(b);
            }
        }

        public void Write(short n, int tag)
        {
            Reserve(4);
            if (n >= -128 && n <= 127)
            {
                Write((byte)n, tag);
            }
            else
            {
                WriteHead((byte)TarsStructType.Short, tag);
                buf.WriteShort(n);
            }
        }

        public void Write(ushort n, int tag)
        {
            Write((short)n, tag);
        }

        public void Write(int n, int tag)
        {
            Reserve(6);

            if (n >= short.MinValue && n <= short.MaxValue)
            {
                Write((short)n, tag);
            }
            else
            {
                WriteHead((byte)TarsStructType.Int, tag);
                buf.WriteInt(n);
            }
        }

        //public void PutZero()
        //{
        //    buf.WriteInt(0);
        //}

        //public void PutDataLength()
        //{
        //    var len = buf.WriterIndex;
        //    buf.SetWriterIndex(0);
        //    buf.WriteInt(len);
        //    buf.SetWriterIndex(len);
        //}

        public void Write(uint n, int tag)
        {
            Write((long)n, tag);
        }

        public void Write(ulong n, int tag)
        {
            Write((long)n, tag);
        }

        public void Write(long n, int tag)
        {
            Reserve(10);
            if (n >= int.MinValue && n <= int.MaxValue)
            {
                Write((int)n, tag);
            }
            else
            {
                WriteHead((byte)TarsStructType.Long, tag);
                buf.WriteLong(n);
            }
        }

        public void Write(float n, int tag)
        {
            Reserve(6);
            WriteHead((byte)TarsStructType.Float, tag);
            buf.WriteFloat(n);
        }

        public void Write(double n, int tag)
        {
            Reserve(10);
            WriteHead((byte)TarsStructType.Double, tag);
            buf.WriteDouble(n);
        }

        public void WriteStringByte(string s, int tag)
        {
            byte[] by = HexUtil.HexStr2Bytes(s);
            Reserve(10 + by.Length);
            if (by.Length > 255)
            {
                // 长度大于255，为String4类型
                WriteHead((byte)TarsStructType.String4, tag);
                buf.WriteInt(by.Length);
                buf.WriteBytes(by);
            }
            else
            {
                // 长度小于255，位String1类型
                WriteHead((byte)TarsStructType.String1, tag);
                buf.WriteByte(by.Length);
                buf.WriteBytes(by);
            }
        }

        public void WriteByteString(string s, int tag)
        {
            Reserve(10 + s.Length);
            byte[] by = HexUtil.HexStr2Bytes(s);
            if (by.Length > 255)
            {
                WriteHead((byte)TarsStructType.String4, tag);
                buf.WriteInt(by.Length);
                buf.WriteBytes(by);
            }
            else
            {
                WriteHead((byte)TarsStructType.String1, tag);
                buf.WriteByte(by.Length);
                buf.WriteBytes(by);
            }
        }

        public void Write(string s, int tag, bool IsLocalString = false)
        {
            byte[] by;
            by = Encoding.GetEncoding(sServerEncoding).GetBytes(s);
            Reserve(10 + by.Length);
            if (by.Length > 255)
            {
                WriteHead((byte)TarsStructType.String4, tag);
                buf.WriteInt(by.Length);
                buf.WriteBytes(by);
            }
            else
            {
                WriteHead((byte)TarsStructType.String1, tag);
                buf.WriteByte(by.Length);
                buf.WriteBytes(by);
            }
        }

        public void Write<K, V>(IDictionary<K, V> m, int tag)
        {
            Reserve(8);
            WriteHead((byte)TarsStructType.Map, tag);

            Write(m == null ? 0 : m.Count, 0);
            if (m != null)
            {
                foreach (KeyValuePair<K, V> en in m)
                {
                    Write(en.Key, 0);
                    Write(en.Value, 1);
                }
            }
        }

        public void Write(IDictionary m, int tag)
        {
            Reserve(8);
            WriteHead((byte)TarsStructType.Map, tag);
            Write(m == null ? 0 : m.Count, 0);
            if (m != null)
            {
                ICollection keys = m.Keys;
                foreach (object objKey in keys)
                {
                    Write(objKey, 0);
                    Write(m[objKey], 1);
                }
            }
        }

        public void Write(bool[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (bool e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(byte[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8 + nLen);
            WriteHead((byte)TarsStructType.SimpleList, tag);
            WriteHead((byte)TarsStructType.Byte, 0);
            Write(nLen, 0);
            if (l != null)
            {
                buf.WriteBytes(l);
            }
        }

        public void Write(short[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (short e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(int[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (int e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(long[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (long e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(float[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);
            if (l != null)
            {
                foreach (float e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write(double[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (double e in l)
                {
                    Write(e, 0);
                }
            }
        }

        public void Write<T>(T[] l, int tag)
        {
            object o = l;
            WriteArray((object[])o, tag);
        }

        private void WriteArray(object[] l, int tag)
        {
            int nLen = 0;
            if (l != null)
            {
                nLen = l.Length;
            }
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(nLen, 0);

            if (l != null)
            {
                foreach (Object e in l)
                {
                    Write(e, 0);
                }
            }
        }

        // 由于list，应该在第一个位置[0]预置一个元素（可以为空），以方便判断元素类型
        public void WriteList(IList l, int tag)
        {
            Reserve(8);
            WriteHead((byte)TarsStructType.List, tag);
            Write(l == null ? 0 : (l.Count > 0 ? l.Count : 0), 0);
            if (l != null)
            {
                for (int i = 0; i < l.Count; i++)
                {
                    Write(l[i], 0);
                }
            }
        }

        public void Write(TarsStruct o, int tag)
        {
            if (o == null)
            {
                return;
            }
            Reserve(2);
            WriteHead((byte)TarsStructType.StructBegin, tag);
            o.WriteTo(this);
            Reserve(2);
            WriteHead((byte)TarsStructType.StructEnd, 0);
        }

        public void Write(object o, int tag)
        {
            if (o == null)
            {
                return;
            }
            if (o is byte)
            {
                Write(((byte)o), tag);
            }
            else if (o is Boolean)
            {
                Write((bool)o, tag);
            }
            else if (o is short)
            {
                Write(((short)o), tag);
            }
            else if (o is ushort)
            {
                Write(((int)(ushort)o), tag);
            }
            else if (o is int)
            {
                Write(((int)o), tag);
            }
            else if (o is uint)
            {
                Write((long)(uint)o, tag);
            }
            else if (o is long)
            {
                Write(((long)o), tag);
            }
            else if (o is ulong)
            {
                Write(((long)(ulong)o), tag);
            }
            else if (o is float)
            {
                Write(((float)o), tag);
            }
            else if (o is double)
            {
                Write(((double)o), tag);
            }
            else if (o is string)
            {
                string strObj = o as string;
                Write(strObj, tag);
            }
            else if (o is TarsStruct)
            {
                Write((TarsStruct)o, tag);
            }
            else if (o is byte[])
            {
                Write((byte[])o, tag);
            }
            else if (o is bool[])
            {
                Write((bool[])o, tag);
            }
            else if (o is short[])
            {
                Write((short[])o, tag);
            }
            else if (o is int[])
            {
                Write((int[])o, tag);
            }
            else if (o is long[])
            {
                Write((long[])o, tag);
            }
            else if (o is float[])
            {
                Write((float[])o, tag);
            }
            else if (o is double[])
            {
                Write((double[])o, tag);
            }
            else if (o.GetType().IsArray)
            {
                Write((object)(object[])o, tag);
            }
            else if (o is IList)
            {
                WriteList((IList)o, tag);
            }
            else if (o is IDictionary)
            {
                Write((IDictionary)o, tag);
            }
            else
            {
                throw new TarsEncodeException(
                        "write object error: unsupport type. " + o.ToString() + "\n");
            }
        }

        protected string sServerEncoding = "UTF-8";

        public int SetServerEncoding(string se)
        {
            sServerEncoding = se;
            return 0;
        }
    }
}