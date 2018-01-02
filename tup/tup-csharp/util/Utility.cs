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
using System.Net;
using System.IO.IsolatedStorage;
using System.IO;
using System.Text;
using System.Collections.Generic;

namespace Tup.Utility
{
    internal class Util
    {
        static Random rand = new Random();
        public static int Random()
        {
            return rand.Next();
        }

        // 如果是ascii字符，会转为一个字节保存
        public static byte[] CharArray2Bytes(char[] chars)
        {
            byte[] bytes = new byte[chars.Length];
            for (int i = 0; i < chars.Length; i++)
            {
                char ch = chars[i];
                bytes[i] = (byte)ch;
                ch >>= 8;
                if (ch > 0)
                {
                    i++;
                    bytes[i] = (byte)ch;
                }
            }
            return bytes;
        }

        // 用于tars的二进制字符串，其中屏蔽了0x80（消息特有的字符），
        public static string Bytes2String(byte[] bytes)
        {
            string strOutput = "";

            List<int> listIndexes = new List<int>();
            for (int i = 0; i < bytes.Length-1; i++)
            {
                if (((bytes[i] & 0x80) != 0) && ((bytes[i+1] & 0x80) != 0))
                {
                    listIndexes.Add(i);
                }
            }

            if (listIndexes.Count > 0)
            {
                if (listIndexes[0] > 0)
                {
                    strOutput += Encoding.UTF8.GetString(bytes, 0, listIndexes[0]); 
                }

                strOutput += (char)listIndexes[0];
            }

            for (int i = 1; i < listIndexes.Count; i++)
            {
                int num = listIndexes[i] - listIndexes[i - 1] -1;
                if (num > 0)
                {
                    strOutput += Encoding.UTF8.GetString(bytes, listIndexes[i - 1]+1, num); 
                }

                strOutput += (char)listIndexes[i];
            }


            int leftIndex = 0;
            if (listIndexes.Count > 0)
            {
                leftIndex = listIndexes[listIndexes.Count - 1] + 1;
            }
            if (leftIndex < bytes.Length)
            {
                strOutput += Encoding.UTF8.GetString(bytes, leftIndex, bytes.Length - leftIndex); 
            }
            return strOutput;                        
        }

        public static string Hex2String(byte[] buffer)
        {
            string strOutput = "";

            if (buffer != null)
            {
                for (int i = 0; i < buffer.Length; i++)
                {
                    byte bt = buffer[i];
                    strOutput += bt.ToString("x2");
                }
            }

            return strOutput;
        }
                
    }
}
