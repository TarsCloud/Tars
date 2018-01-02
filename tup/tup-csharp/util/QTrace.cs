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

#define DEBUG

using System;
using System.Net;

using System.Collections;


namespace Tup
{
    public class QTrace
    {
        static public void Trace(string value)
        {
#if DEBUG
            string strTrace = DateTime.Now.Hour.ToString("D2") + ":" + DateTime.Now.Minute.ToString("D2") + ":" + DateTime.Now.Second.ToString("D2") + ":" + DateTime.Now.Millisecond.ToString("D3");
            strTrace += "\t" + value;
            System.Diagnostics.Debug.WriteLine(strTrace);
#endif
        }
        static public void Trace(string value, object arg)
        {
#if DEBUG
            string strTrace = string.Format("{0}{1}", value, arg);
            System.Diagnostics.Debug.WriteLine(value);
#endif
        }
        static public string Trace(byte[] value)
        {
            string strValue = "\r\n";
            int i = 0;
            foreach (byte bt in value)
            {
                strValue += string.Format(" {0,02:x}", bt);
                i++;
                if ((i % 16) == 0)
                {
                    strValue += "\n";
                }
            }

            Trace(strValue);
            return strValue;
        }

        static public void Trace(IDictionary dict)
        {
            if (dict == null)
            {
                return;
            }
            string strTrace = " Dictionary: ";
            foreach (object key in dict.Keys)
            {
                strTrace += key.ToString();
                strTrace += "\t\t";
                strTrace += dict[key].ToString();
                strTrace += "\r\n";
            }
            Trace(strTrace);
        }

        static public void Trace(IList list)
        {
            if (list == null)
            {
                return;
            }
            string strTrace = " List: ";
            foreach (object item in list)
            {
                strTrace += item.ToString();
                strTrace += "\r\n";
            }
            Trace(strTrace);
        }

        static public void Assert(bool condition)
        {
            System.Diagnostics.Debug.Assert(condition);
        }

        static public string Output(byte[] value)
        {
            string strValue = "";
#if DEBUG
            int i = 0;
            foreach (byte bt in value)
            {
                strValue += string.Format("0x{0:x},", bt);
                i++;
                if ((i % 16) == 0)
                {
                    strValue += "\n";
                }
            }
#endif
            return strValue;
        }
    }
}
