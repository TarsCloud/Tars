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
using System.Text;
using System.Collections.Generic;
using System.IO;

namespace Tup
{
    public class UniPacket : UniAttribute
    {
        public static readonly int UniPacketHeadSize = 4;

        protected RequestPacket _package = new RequestPacket();

        /**
         * 获取请求的service名字
         * 
         * @return
         */
        public string ServantName
        {
            get
            {
                return _package.sServantName;
            }
            set
            {
                _package.sServantName = value;
            }
        }

        /**
         * 获取请求的函数名字
         * 
         * @return
         */
        public string FuncName
        {
            get
            {
                return _package.sFuncName;
            }
            set
            {
                _package.sFuncName = value;
            }
        }

        /**
         * 获取消息序列号
         * 
         * @return
         */
        public int RequestId
        {
            get
            {
                return _package.iRequestId;
            }
            set
            {
                _package.iRequestId = value;
            }
        }
        public UniPacket()
        {
            _package.iVersion = 2;
        }
        public void setVersion(short iVer)
        {
            _iVer = iVer;
            _package.iVersion = iVer;
        }
        public short getVersion()
        {
            return _package.iVersion;
        }
        /**
         * 将put的对象进行编码
         */
        public new byte[] Encode()
        {
            if (_package.sServantName.Equals(""))
            {
                throw new ArgumentException("servantName can not is null");
            }
            if (_package.sFuncName.Equals(""))
            {
                throw new ArgumentException("funcName can not is null");
            }

            TarsOutputStream _os = new TarsOutputStream(0);
            _os.setServerEncoding(EncodeName);
            if (_package.iVersion == Const.PACKET_TYPE_TUP)
            {
                _os.Write(_data, 0);
            }
            else
            {
                _os.write(_new_data, 0);
            }

            _package.sBuffer = TarsUtil.getTarsBufArray(_os.getMemoryStream());
            
            _os = new TarsOutputStream(0);
            _os.setServerEncoding(EncodeName);
            this.WriteTo(_os);
            byte[] bodys = TarsUtil.getTarsBufArray(_os.getMemoryStream());
            int size = bodys.Length;

            MemoryStream ms = new MemoryStream(size + UniPacketHeadSize);

            using (BinaryWriter bw = new BinaryWriter(ms))
            {
                // 整个数据包长度
                bw.Write(ByteConverter.ReverseEndian(size + UniPacketHeadSize));
                bw.Write(bodys);
            }

            return ms.ToArray();
        }

        /**
         * 对传入的数据进行解码 填充可get的对象
         */

        public new void Decode(byte[] buffer, int Index = 0)
        {
            if (buffer.Length < UniPacketHeadSize)
            {
                throw new ArgumentException("Decode namespace must include size head");
            }

            try
            {
                TarsInputStream _is = new TarsInputStream(buffer, UniPacketHeadSize + Index);
                _is.setServerEncoding(EncodeName);
                //解码出RequestPacket包
                this.ReadFrom(_is);

                //设置tup版本
                _iVer = _package.iVersion;

                _is = new TarsInputStream(_package.sBuffer);
                _is.setServerEncoding(EncodeName);

                if (_package.iVersion == Const.PACKET_TYPE_TUP)
                {
                    _data = (Dictionary<string, Dictionary<string, byte[]>>)_is.readMap<Dictionary<string, Dictionary<string, byte[]>>>(0, false);
                }
                else
                {
                    _new_data = (Dictionary<string, byte[]>)_is.readMap<Dictionary<string, byte[]>>(0, false);  
                }
            }
            catch (Exception e)
            {
                QTrace.Trace(this + " Decode Exception: " + e.Message);
                throw (e);
            }
        }

        public override void WriteTo(TarsOutputStream _os)
        {
            _package.WriteTo(_os);
        }

        public override void ReadFrom(TarsInputStream _is)
        {
            _package.ReadFrom(_is);
        }
        
        public int OldRespIRet { get; set; }

        /**
         * 为兼容最早发布的客户端版本解码使用 iret字段始终为0
         * 
         * @return
         */
        public byte[] CreateOldRespEncode()
        {
            TarsOutputStream _os = new TarsOutputStream(0);
            _os.setServerEncoding(EncodeName);
            _os.Write(_data, 0);
            byte[] oldSBuffer = TarsUtil.getTarsBufArray(_os.getMemoryStream());
            _os = new TarsOutputStream(0);
            _os.setServerEncoding(EncodeName);
            _os.Write(_package.iVersion, 1);
            _os.Write(_package.cPacketType, 2);
            _os.Write(_package.iRequestId, 3);
            _os.Write(_package.iMessageType, 4);
            _os.Write(OldRespIRet, 5);
            _os.Write(oldSBuffer, 6);
            _os.Write(_package.status, 7);
            return TarsUtil.getTarsBufArray(_os.getMemoryStream());
        }
    }
}