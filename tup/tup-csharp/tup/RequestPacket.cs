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

using Tup;
using System.Collections.Generic;
using System;
using System.Text;
using Tup.Tars;

namespace Tup
{
    public class RequestPacket : Tup.Tars.TarsStruct
    {
        public short iVersion = 0;

        public byte cPacketType = 0;

        public int iMessageType = 0;

        public int iRequestId = 0;

        public string sServantName = null;

        public string sFuncName = null;

        public byte[] sBuffer;

        public int iTimeout = 0;

        public Dictionary<string, string> context;

        public Dictionary<string, string> status;

        public RequestPacket()
        {
        }

        public RequestPacket(short iVersion, byte cPacketType, int iMessageType, int iRequestId, string sServantName, string sFuncName, byte[] sBuffer, int iTimeout, Dictionary<string, string> context, Dictionary<string, string> status)
        {
            this.iVersion = iVersion;
            this.cPacketType = cPacketType;
            this.iMessageType = iMessageType;
            this.iRequestId = iRequestId;
            this.sServantName = sServantName;
            this.sFuncName = sFuncName;
            this.sBuffer = sBuffer;
            this.iTimeout = iTimeout;
            this.context = context;
            this.status = status;
        }

        public override void WriteTo(Tup.Tars.TarsOutputStream _os)
        {
            _os.Write(iVersion, 1);
            _os.Write(cPacketType, 2);
            _os.Write(iMessageType, 3);
            _os.Write(iRequestId, 4);
            _os.Write(sServantName, 5);
            _os.Write(sFuncName, 6);
            _os.Write(sBuffer, 7);
            _os.Write(iTimeout, 8);
            _os.Write(context, 9);
            _os.Write(status, 10);
        }

        static byte[] cache_sBuffer = null;

        public override void ReadFrom(Tup.Tars.TarsInputStream _is)
        {
            try
            {
                iVersion = (short)_is.Read(iVersion, 1, true);
                cPacketType = (byte)_is.Read(cPacketType, 2, true);
                iMessageType = (int)_is.Read(iMessageType, 3, true);
                iRequestId = (int)_is.Read(iRequestId, 4, true);
                sServantName = (string)_is.readString(5, true);
                sFuncName = (string)_is.readString(6, true);
                
                if (null == cache_sBuffer)
                {
                    cache_sBuffer = new byte[] { 0 };
                }
                sBuffer = (byte[])_is.Read<byte[]>(cache_sBuffer, 7, true);
                iTimeout = (int)_is.Read(iTimeout, 8, true);
                
                Dictionary<string, string> cache_context = null;
                context = (Dictionary<string, string>)_is.Read<Dictionary<string, string>>(cache_context, 9, true);
                status = (Dictionary<string, string>)_is.Read<Dictionary<string, string>>(cache_context, 10, true);
            }
            catch (Exception e)
            {
                QTrace.Trace(this + " ReadFrom Exception: " + e.Message);
                throw e;
            }


        }
        
    }
}
