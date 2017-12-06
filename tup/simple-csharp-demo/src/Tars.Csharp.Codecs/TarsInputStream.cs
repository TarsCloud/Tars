using DotNetty.Buffers;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Tars.Csharp.Codecs.Tup;
using Tars.Csharp.Codecs.Util;

namespace Tars.Csharp.Codecs
{
    public class TarsInputStream
    {
        public const byte StructEnd = (byte)TarsStructType.StructEnd;

        public class HeadData
        {
            public byte Type;
            public int Tag;

            public void Clear()
            {
                Type = 0;
                Tag = 0;
            }
        }

        private IByteBuffer buffer;

        protected Encoding sServerEncoding = Encoding.GetEncoding("utf-8");

        public void SetServerEncoding(string se)
        {
            sServerEncoding = Encoding.GetEncoding(se);
        }

        public TarsInputStream(IByteBuffer input)
        {
            buffer = input;
        }

        /// <summary>
        /// 读取数据头
        /// </summary>
        /// <param name="hd"></param>
        /// <param name="bb"></param>
        /// <returns></returns>
        public static int ReadHead(HeadData hd, IByteBuffer bb)
        {
            byte b = bb.ReadByte();
            hd.Type = (byte)(b & 15);
            hd.Tag = ((b & (15 << 4)) >> 4);
            if (hd.Tag == 15)
            {
                hd.Tag = bb.ReadByte() & 0x00ff;
                return 2;
            }
            return 1;
        }

        public int ReadHead(HeadData hd)
        {
            return ReadHead(hd, buffer);
        }

        /// <summary>
        /// 读取头信息，但不移动缓冲区的当前偏移
        /// </summary>
        /// <param name="hd"></param>
        /// <returns></returns>
        private int PeakHead(HeadData hd)
        {
            int len = ReadHead(hd);
            Skip(-1 * len);
            return len;
        }

        /// <summary>
        /// 跳过若干字节
        /// </summary>
        /// <param name="len"></param>
        private void Skip(int len)
        {
            buffer.SetReaderIndex(buffer.ReaderIndex + len);
        }

        /// <summary>
        /// 跳到指定的tag的数据之前
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public bool SkipToTag(int tag)
        {
            try
            {
                var hd = new HeadData();
                while (true)
                {
                    int len = PeakHead(hd);
                    if (hd.Type == StructEnd) return false;
                    if (tag <= hd.Tag)
                    {
                        return tag == hd.Tag;
                    }
                    Skip(len);
                    SkipField(hd.Type);
                }
            }
            catch (IndexOutOfRangeException e)
            {
            }
            catch (TarsDecodeException e)
            {
                //QTrace.Trace(e.Message);
            }
            return false;
        }

        /// <summary>
        /// 跳到当前结构的结束位置
        /// </summary>
        public void SkipToStructEnd()
        {
            var hd = new HeadData();
            do
            {
                ReadHead(hd);
                SkipField(hd.Type);
            } while (hd.Type != StructEnd);
        }

        /// <summary>
        /// 跳过一个字段
        /// </summary>
        private void SkipField()
        {
            var hd = new HeadData();
            ReadHead(hd);
            SkipField(hd.Type);
        }

        private void SkipField(byte type)
        {
            switch (type)
            {
                case (byte)TarsStructType.Byte:
                    Skip(1);
                    break;

                case (byte)TarsStructType.Short:
                    Skip(2);
                    break;

                case (byte)TarsStructType.Int:
                    Skip(4);
                    break;

                case (byte)TarsStructType.Long:
                    Skip(8);
                    break;

                case (byte)TarsStructType.Float:
                    Skip(4);
                    break;

                case (byte)TarsStructType.Double:
                    Skip(8);
                    break;

                case (byte)TarsStructType.String1:
                    {
                        int len = buffer.ReadByte();
                        if (len < 0)
                            len += 256;
                        Skip(len);
                        break;
                    }
                case (byte)TarsStructType.String4:
                    {
                        Skip(buffer.ReadInt());
                        break;
                    }
                case (byte)TarsStructType.Map:
                    {
                        int size = Read(0, 0, true);
                        for (int i = 0; i < size * 2; ++i)
                        {
                            SkipField();
                        }
                        break;
                    }
                case (byte)TarsStructType.List:
                    {
                        int size = Read(0, 0, true);
                        for (int i = 0; i < size; ++i)
                        {
                            SkipField();
                        }
                        break;
                    }
                case (byte)TarsStructType.SimpleList:
                    {
                        var hd = new HeadData();
                        ReadHead(hd);
                        if (hd.Type != (byte)TarsStructType.Byte)
                        {
                            throw new TarsDecodeException("skipField with invalid type, type value: " + type + ", " + hd.Type);
                        }
                        int size = Read(0, 0, true);
                        Skip(size);
                        break;
                    }
                case (byte)TarsStructType.StructBegin:
                    SkipToStructEnd();
                    break;

                case (byte)TarsStructType.StructEnd:
                case (byte)TarsStructType.ZeroTag:
                    break;

                default:
                    throw new TarsDecodeException("invalid type.");
            }
        }

        public bool Read(bool b, int tag, bool isRequire)
        {
            byte c = Read((byte)0x0, tag, isRequire);
            return c != 0;
        }

        public byte Read(byte c, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        c = 0x0;
                        break;

                    case (byte)TarsStructType.Byte:
                        {
                            c = buffer.ReadByte();
                            break;
                        }
                    default:
                        {
                            throw new TarsDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return c;
        }

        public short Read(short n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Byte:
                        {
                            n = (short)buffer.ReadByte();
                            break;
                        }
                    case (byte)TarsStructType.Short:
                        {
                            n = buffer.ReadShort();
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public ushort Read(ushort n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Byte:
                        {
                            n = buffer.ReadByte();
                            break;
                        }
                    case (byte)TarsStructType.Short:
                        {
                            n = buffer.ReadUnsignedShort();
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public int Read(int n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Byte:
                        n = (int)buffer.ReadByte();
                        break;

                    case (byte)TarsStructType.Short:
                        n = buffer.ReadShort();
                        break;

                    case (byte)TarsStructType.Int:
                        n = buffer.ReadInt();
                        break;

                    case (byte)TarsStructType.Long:
                        n = (int)buffer.ReadInt();
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public uint Read(uint n, int tag, bool isRequire)
        {
            return (uint)Read((long)n, tag, isRequire);
        }

        public long Read(long n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Byte:
                        {
                            n = (long)buffer.ReadByte();
                        }
                        break;

                    case (byte)TarsStructType.Short:
                        {
                            n = buffer.ReadShort();
                        }
                        break;

                    case (byte)TarsStructType.Int:
                        {
                            n = buffer.ReadShort();
                        }
                        break;

                    case (byte)TarsStructType.Long:
                        {
                            n = buffer.ReadLong();
                        }
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public ulong Read(ulong n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Byte:
                        {
                            n = buffer.ReadByte();
                        }
                        break;

                    case (byte)TarsStructType.Short:
                        {
                            n = buffer.ReadUnsignedShort();
                        }
                        break;

                    case (byte)TarsStructType.Int:
                        {
                            n = buffer.ReadUnsignedInt();
                        }
                        break;

                    case (byte)TarsStructType.Long:
                        {
                            n = (ulong)buffer.ReadLong();
                        }
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public float Read(float n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Float:
                        {
                            n = buffer.ReadFloat();
                        }
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public double Read(double n, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.ZeroTag:
                        n = 0;
                        break;

                    case (byte)TarsStructType.Float:
                        {
                            n = buffer.ReadFloat();
                        }
                        break;

                    case (byte)TarsStructType.Double:
                        {
                            n = buffer.ReadDouble();
                        }
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return n;
        }

        public string ReadByteString(string s, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.String1:
                        {
                            {
                                int len = 0;
                                len = buffer.ReadByte();
                                if (len < 0)
                                {
                                    len += 256;
                                }

                                byte[] ss = new byte[len];
                                buffer.ReadBytes(ss);
                                s = HexUtil.Bytes2HexStr(ss);
                            }
                        }
                        break;

                    case (byte)TarsStructType.String4:
                        {
                            {
                                int len = 0;
                                len = buffer.ReadInt();

                                if (len > Const.TarsMaxStringLength || len < 0)
                                {
                                    throw new TarsDecodeException("string too long: " + len);
                                }

                                byte[] ss = buffer.ReadBytes(len).Array;
                                s = HexUtil.Bytes2HexStr(ss);
                            }
                        }
                        break;

                    default:
                        {
                            throw new TarsDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return s;
        }

        public string Read(string s, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.String1:
                        {
                            int len = 0;
                            len = buffer.ReadByte();
                            if (len < 0)
                            {
                                len += 256;
                            }

                            byte[] ss = new byte[len];
                            buffer.ReadBytes(ss);
                            try
                            {
                                s = sServerEncoding.GetString(ss);
                            }
                            catch
                            {
                                s = Encoding.UTF8.GetString(ss);
                            }
                        }
                        break;

                    case (byte)TarsStructType.String4:
                        {
                            int len = 0;
                            len = buffer.ReadInt();
                            if (len > Const.TarsMaxStringLength || len < 0)
                            {
                                throw new TarsDecodeException("string too long: " + len);
                            }

                            byte[] ss = new byte[len];
                            buffer.ReadBytes(ss);
                            try
                            {
                                s = sServerEncoding.GetString(ss);
                            }
                            catch
                            {
                                s = Encoding.UTF8.GetString(ss);
                            }
                        }
                        break;

                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return s;
        }

        public string ReadString(int tag, bool isRequire)
        {
            string str = null;
            return Read(str, tag, isRequire);
        }

        public string[] Read(string[] s, int tag, bool isRequire)
        {
            return ReadArray(s, tag, isRequire);
        }

        public IDictionary<K, V> ReadMap<K, V>(IDictionary<K, V> m, int tag, bool isRequire)
        {
            if (m == null)
            {
                return null;
            }

            var mk = default(K);
            var mv = default(V);

            if (SkipToTag(tag))
            {
                HeadData hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.Map:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new TarsDecodeException("size invalid: " + size);
                            }
                            for (int i = 0; i < size; ++i)
                            {
                                mk = (K)Read(mk, 0, true);
                                mv = (V)Read(mv, 1, true);

                                if (mk != null)
                                {
                                    if (m.ContainsKey(mk))
                                    {
                                        m[mk] = mv;
                                    }
                                    else
                                    {
                                        m.Add(mk, mv);
                                    }
                                }
                            }
                        }
                        break;

                    default:
                        {
                            throw new TarsDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return m;
        }

        public IDictionary ReadMap<T>(int tag, bool isRequire)
        {
            T m = (T)BasicClassTypeUtil.CreateObject<T>();
            return ReadMap<T>(m, tag, isRequire);
        }

        public IDictionary ReadMap<T>(T arg, int tag, bool isRequire)
        {
            IDictionary m = arg as IDictionary;
            if (m == null)
            {
                return null;
            }

            Type type = m.GetType();
            Type[] argsType = type.GetGenericArguments();
            if (argsType == null || argsType.Length < 2)
            {
                return null;
            }

            var mk = BasicClassTypeUtil.CreateObject(argsType[0]);
            var mv = BasicClassTypeUtil.CreateObject(argsType[1]);

            if (SkipToTag(tag))
            {
                HeadData hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.Map:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new TarsDecodeException("size invalid: " + size);
                            }
                            for (int i = 0; i < size; ++i)
                            {
                                mk = Read<object>(mk, 0, true);
                                mv = Read<object>(mv, 1, true);

                                if (mk != null)
                                {
                                    if (m.Contains(mk))
                                    {
                                        m[mk] = mv;
                                    }
                                    else
                                    {
                                        m.Add(mk, mv);
                                    }
                                }
                            }
                        }
                        break;

                    default:
                        {
                            throw new TarsDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return m;
        }

        public bool[] Read(bool[] l, int tag, bool isRequire)
        {
            bool[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new bool[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public byte[] ReadBytes(int tag, bool isRequire)
        {
            byte[] cache_sBuffer = null;
            return Read(cache_sBuffer, tag, isRequire);
        }

        public byte[] Read(byte[] l, int tag, bool isRequire)
        {
            byte[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.SimpleList:
                        {
                            var hh = new HeadData();
                            ReadHead(hh);
                            if (hh.Type != (byte)TarsStructType.Byte)
                            {
                                throw new TarsDecodeException("type mismatch, tag: " + tag + ", type: " + hd.Type + ", " + hh.Type);
                            }

                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new TarsDecodeException("invalid size, tag: " + tag + ", type: " + hd.Type + ", " + hh.Type + ", size: " + size);
                            }

                            try
                            {
                                lr = new byte[size];
                                buffer.ReadBytes(lr);
                            }
                            catch (Exception e)
                            {
                                //QTrace.Trace(e.Message);
                                return null;
                            }
                            break;
                        }
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new byte[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public short[] Read(short[] l, int tag, bool isRequire)
        {
            short[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new short[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public int[] Read(int[] l, int tag, bool isRequire)
        {
            int[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new int[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public long[] Read(long[] l, int tag, bool isRequire)
        {
            long[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new long[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public float[] Read(float[] l, int tag, bool isRequire)
        {
            float[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new float[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public double[] Read(double[] l, int tag, bool isRequire)
        {
            double[] lr = null;
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                                throw new TarsDecodeException("size invalid: " + size);
                            lr = new double[size];
                            for (int i = 0; i < size; ++i)
                                lr[i] = Read(lr[0], 0, true);
                            break;
                        }
                    default:
                        throw new TarsDecodeException("type mismatch.");
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return lr;
        }

        public T[] ReadArray<T>(T[] l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null || l.Length == 0)
                throw new TarsDecodeException("unable to get type of key and value.");
            return (T[])ReadArrayImpl(l[0], tag, isRequire);
        }

        public IList ReadList<T>(T l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null)
            {
                return null;
            }

            IList list = BasicClassTypeUtil.CreateObject(l.GetType()) as IList;
            if (list == null)
            {
                return null;
            }

            object objItem = BasicClassTypeUtil.CreateListItem(list.GetType());

            Array array = ReadArrayImpl(objItem, tag, isRequire);

            if (array != null)
            {
                list.Clear();
                foreach (object obj in array)
                {
                    list.Add(obj);
                }

                return list;
            }

            return null;
        }

        public List<T> ReadArray<T>(List<T> l, int tag, bool isRequire)
        {
            // 生成代码时已经往List里面添加了一个元素，纯粹用来作为类型识别，否则java无法识别到List里面放的是什么类型的数据
            if (l == null || l.Count == 0)
            {
                return new List<T>();
            }
            T[] tt = (T[])ReadArrayImpl(l[0], tag, isRequire);
            if (tt == null) return null;
            List<T> ll = new List<T>();
            for (int i = 0; i < tt.Length; ++i)
            {
                ll.Add(tt[i]);
            }
            return ll;
        }

        private Array ReadArrayImpl<T>(T mt, int tag, bool isRequire)
        {
            if (SkipToTag(tag))
            {
                var hd = new HeadData();
                ReadHead(hd);
                switch (hd.Type)
                {
                    case (byte)TarsStructType.List:
                        {
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new TarsDecodeException("size invalid: " + size);
                            }

                            Array lr = Array.CreateInstance(mt.GetType(), size);
                            for (int i = 0; i < size; ++i)
                            {
                                T t = (T)Read(mt, 0, true);
                                lr.SetValue(t, i);
                            }
                            return lr;
                        }

                    case (byte)TarsStructType.SimpleList:
                        {
                            var hh = new HeadData();
                            ReadHead(hh);
                            if (hh.Type == (byte)TarsStructType.ZeroTag)
                            {
                                return Array.CreateInstance(mt.GetType(), 0);
                            }
                            if (hh.Type != (byte)TarsStructType.Byte)
                            {
                                throw new TarsDecodeException("type mismatch, tag: " + tag + ", type: " + hd.Type + ", " + hh.Type);
                            }
                            int size = Read(0, 0, true);
                            if (size < 0)
                            {
                                throw new TarsDecodeException("invalid size, tag: " + tag + ", type: " + hd.Type + ", size: " + size);
                            }

                            T[] lr = new T[size];

                            try
                            {
                                byte[] lrtmp = new byte[size];
                                buffer.ReadBytes(lrtmp);
                                for (int i = 0; i < lrtmp.Length; i++)
                                {
                                    object obj = lrtmp[i];
                                    lr[i] = (T)obj;
                                }

                                return lr;
                            }
                            catch (Exception e)
                            {
                                //QTrace.Trace(e.Message);
                                return null;
                            }
                        }
                    default:
                        {
                            throw new TarsDecodeException("type mismatch.");
                        }
                }
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return null;
        }

        public TarsStruct DirectRead(TarsStruct o, int tag, bool isRequire)
        {
            //o必须有一个无参的构造函数
            TarsStruct reff = null;
            if (SkipToTag(tag))
            {
                try
                {
                    reff = (TarsStruct)BasicClassTypeUtil.CreateObject(o.GetType());
                }
                catch (Exception e)
                {
                    throw new TarsDecodeException(e.Message);
                }

                var hd = new HeadData();
                ReadHead(hd);
                if (hd.Type != (byte)TarsStructType.StructBegin)
                {
                    throw new TarsDecodeException("type mismatch.");
                }
                reff.ReadFrom(this);
                SkipToStructEnd();
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }

            return reff;
        }

        public TarsStruct Read(TarsStruct o, int tag, bool isRequire)
        {
            //o必须有一个无参的构造函数
            TarsStruct reff = null;
            if (SkipToTag(tag))
            {
                try
                {
                    // 必须重新创建一个，否则，会导致在同一个对象上赋值，这是由于C#的引用引起的
                    reff = (TarsStruct)BasicClassTypeUtil.CreateObject(o.GetType());
                }
                catch (Exception e)
                {
                    throw new TarsDecodeException(e.Message);
                }

                var hd = new HeadData();
                ReadHead(hd);
                if (hd.Type != (byte)TarsStructType.StructBegin)
                {
                    throw new TarsDecodeException("type mismatch.");
                }
                reff.ReadFrom(this);
                SkipToStructEnd();
            }
            else if (isRequire)
            {
                throw new TarsDecodeException("require field not exist.");
            }
            return reff;
        }

        public TarsStruct[] Read(TarsStruct[] o, int tag, bool isRequire)
        {
            return ReadArray(o, tag, isRequire);
        }

        internal object Read(object proxy, int tag, bool isRequired)
        {
            return Read<object>(proxy, tag, isRequired);
        }

        public object Read<T>(T o, int tag, bool isRequire)
        {
            if (o == null)
            {
                o = (T)BasicClassTypeUtil.CreateObject<T>();
            }
            if (o is Byte || o is Char)
            {
                return (Read((byte)0x0, tag, isRequire));
            }
            else if (o is char)
            {
                return (Read((char)0x0, tag, isRequire));
            }
            else if (o is Boolean)
            {
                return (Read(false, tag, isRequire));
            }
            else if (o is short)
            {
                return (Read((short)0, tag, isRequire));
            }
            else if (o is ushort)
            {
                return (Read((ushort)0, tag, isRequire));
            }
            else if (o is int)
            {
                return Read((int)0, tag, isRequire);
            }
            else if (o is uint)
            {
                return Read((uint)0, tag, isRequire);
            }
            else if (o is long)
            {
                return (Read((long)0, tag, isRequire));
            }
            else if (o is ulong)
            {
                return (Read((ulong)0, tag, isRequire));
            }
            else if (o is float)
            {
                return (Read((float)0, tag, isRequire));
            }
            else if (o is Double)
            {
                return (Read((double)0, tag, isRequire));
            }
            else if (o is string)
            {
                return (ReadString(tag, isRequire));
            }
            else if (o is TarsStruct)
            {
                object oo = o;
                return Read((TarsStruct)oo, tag, isRequire);
            }
            else if (o != null && o.GetType().IsArray)
            {
                if (o is byte[] || o is Byte[])
                {
                    return Read((byte[])null, tag, isRequire);
                }
                else if (o is bool[])
                {
                    return Read((bool[])null, tag, isRequire);
                }
                else if (o is short[])
                {
                    return Read((short[])null, tag, isRequire);
                }
                else if (o is int[])
                {
                    return Read((int[])null, tag, isRequire);
                }
                else if (o is long[])
                {
                    return Read((long[])null, tag, isRequire);
                }
                else if (o is float[])
                {
                    return Read((float[])null, tag, isRequire);
                }
                else if (o is double[])
                {
                    return Read((double[])null, tag, isRequire);
                }
                else
                {
                    object oo = o;
                    return ReadArray((Object[])oo, tag, isRequire);
                }
            }
            else if (o is IList)
            {
                return ReadList<T>(o, tag, isRequire);
            }
            else if (o is IDictionary)
            {
                return ReadMap<T>(o, tag, isRequire);
            }
            else
            {
                throw new TarsDecodeException("read object error: unsupport type." + o.ToString());
            }
        }
    }
}