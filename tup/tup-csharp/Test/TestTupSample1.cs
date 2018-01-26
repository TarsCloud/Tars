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

﻿using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

using System.Collections.Generic;

using Tup;

using friendlist;

namespace TestTup.Test
{
    public class TestOne
    {
        public void Test()
        {
            //TestTool();
            TestBasicType();
            //TestMapList();
            //TestTars();
            //TestArray();
        }

        void TestTool()
        {
            int i = (int)BasicClassTypeUtil.CreateObject<int>();
            uint ui = (uint)BasicClassTypeUtil.CreateObject<uint>();
            float f = (float)BasicClassTypeUtil.CreateObject<float>();
            List<int> list = (List<int>)BasicClassTypeUtil.CreateObject<List<int>>();
            Dictionary<int, string> dict = (Dictionary<int, string>)BasicClassTypeUtil.CreateObject<Dictionary<int, string>>();
            Dictionary<int, List<int>> dictlist = (Dictionary<int, List<int>>)BasicClassTypeUtil.CreateObject<Dictionary<int, List<int>>>();
        }

        public void TestBasicType()
        {
            UniAttribute client = new UniAttribute();
            client.Put("short", (short)-123);
            client.Put("int", -1);
            client.Put("long", -1L);
            client.Put("ushort", (ushort)1234);
            client.Put("uint", (uint)1324234);
            client.Put("ulong", (ulong)123432455);
            client.Put("float", 1.23f);
            client.Put("double", (double)3.1415926);

            byte[] buffer = client.Encode();

            UniAttribute de = new UniAttribute();
            de.Decode(buffer);
            short sh = 0;
            sh = de.Get<short>("short");

            int i = 0;
            i = de.Get<int>("int");

            long l = 0;
            l = de.Get<long>("long");

            ushort ush = 0;
            ush = (ushort)de.Get<ushort>("ushort");

            uint ui = 0;
            ui = de.Get<uint>("uint");

            ulong ul = 0;
            ul = de.Get<ulong>("ulong");

            float f = 0.0f;
            f = de.Get<float>("float");

            double d = 0.0;
            d = de.Get<double>("double");
        }

        public void TestMapList()
        {
            UniPacket client = new UniPacket();
            client.ServantName = "ServantName";
            client.FuncName = "FuncName";
            client.RequestId = 0;

            Dictionary<string, int> dict1 = new Dictionary<string, int>();
            dict1.Add("s1", 2341);

            List<int> list1 = new List<int>();

            for (int il = 0; il < 10; il++)
            {
                list1.Add(il);
            }

            Dictionary<string, List<int>> dictList = new Dictionary<string, List<int>>();
            for (int idl = 0; idl < 3; idl++)
            {
                List<int> listdict = new List<int>();

                for (int il = 0; il < 10; il++)
                {
                    listdict.Add(il + idl);
                }
                dictList.Add("key " + idl, listdict);
            }


            Dictionary<string, Dictionary<string, int>> dictdict = new Dictionary<string, Dictionary<string, int>>();
            for (int idl = 0; idl < 3; idl++)
            {
                Dictionary<string, int> dd = new Dictionary<string, int>();

                for (int il = 0; il < 10; il++)
                {
                    dd.Add("sub key " + il + idl, il + idl);
                }
                dictdict.Add("key " + idl, dd);
            }

            byte[] abtye = new byte[10];
            for (byte ib = 1; ib < abtye.Length; ib++)
            {
                abtye[ib] = ib;
            }

            client.Put("array", abtye);
            client.Put("list", list1);
            client.Put("emptylist", new List<byte>());
            client.Put("emptylistint", new List<int>());
            client.Put("emptylistTars", new List<TestFriendlInfo>());
            client.Put("emptyMapintstring", new Dictionary<int,string>());
            client.Put("emptyMapintTars", new Dictionary<int, TestFriendlInfo>());
            client.Put("map", dict1);
            client.Put("maplist", dictList);
            client.Put("mapmap", dictdict);

            byte[] buffer = client.Encode();

            UniPacket de = new UniPacket();
            de.Decode(buffer);

            abtye = de.Get<byte[]>("array");

            Dictionary<string, int> dict10 = new Dictionary<string, int>();
            dict10 = de.Get<Dictionary<string, int>>("map");
            QTrace.Trace(dict10);

            List<int> list10 = new List<int>();
            list10 = de.Get<List<int>>("list");
            QTrace.Trace(list10);

            List<byte> emptylist = new List<byte>();
            emptylist = de.Get<List<byte>>("emptylist");
            QTrace.Trace(emptylist);

            List<int> emptylistint = new List<int>();
            emptylistint = de.Get<List<int>>("emptylistint");
            QTrace.Trace(emptylistint);

            List<TestFriendlInfo> emptylistTars = de.Get<List<TestFriendlInfo>>("emptylistTars");
            QTrace.Trace(emptylistTars);

            Dictionary<int,string> emptyMapintstring = de.Get<Dictionary<int,string>>("emptyMapintstring");
            QTrace.Trace(emptyMapintstring);

            Dictionary<int, TestFriendlInfo> emptyMapintTars = de.Get<Dictionary<int, TestFriendlInfo>>("emptyMapintTars");
            QTrace.Trace(emptyMapintTars);

            dictList = (Dictionary<string, List<int>>)de.Get<Dictionary<string, List<int>>>("maplist");
            foreach (string key in dictList.Keys)
            {
                QTrace.Trace(key);
                QTrace.Trace(dictList[key]);
            }

            dictdict = (Dictionary<string, Dictionary<string, int>>)de.Get<Dictionary<string, Dictionary<string, int>>>("mapmap");
            QTrace.Trace("Map Map");
            foreach (string key in dictdict.Keys)
            {
                QTrace.Trace(key);
                Dictionary<string, int> subdict = dictdict[key];
                QTrace.Trace(subdict);
            }
        }

        public void TestTars()
        {
            UniAttribute client = new UniAttribute();

            TestFriendlInfo tfi = new TestFriendlInfo();
            tfi.dictTest = new Dictionary<int, string>();
            for (int i = 0; i < 3; i++)
            {
                tfi.dictTest.Add(i, "v" + (i + 1));
            }
            tfi.friend_count = 3;
            tfi.getfriendCount = 4;
            tfi.getgroupCount = 5;
            tfi.group_count = 6;
            tfi.groupid = 7;
            tfi.groupstartIndex = 13;
            tfi.ifGetGroupInfo = 0;
            tfi.ifReflush = 1;
            tfi.reqtype = 4321;
            tfi.startIndex = 32;
            tfi.totoal_friend_count = 3;
            tfi.totoal_group_count = 43;
            tfi.uin = 3214123;
            tfi.vecFriendInfo = new List<FriendInfo>();
            for (int j = 0; j < 3; j++)
            {
                FriendInfo fi = new FriendInfo();
                fi.faceId = (byte)j;
                fi.friendUin = j + 100;
                fi.groupId = (byte)(j + 13);
                fi.remark = "hello " + j;
                fi.sqqtype = (byte)j;
                fi.status = (byte)j;
                tfi.vecFriendInfo.Add(fi);
            }

            client.Put("TestFriendlInfo", tfi);

            byte[] buffer = client.Encode();

            UniAttribute de = new UniAttribute();
            de.Decode(buffer);

            TestFriendlInfo detfi = de.Get<TestFriendlInfo>("TestFriendlInfo");

        }

        public void TestArray()
        {
            UniAttribute client = new UniAttribute();

            byte[] aByte = new byte[]
            {
                1,2,3,4,5,6
            };
            client.Put("aByte", aByte);
            client.Put("aEmpty", new byte[0]);
            client.Put("aint", new int[]{
                11,22,33,44,55,66,77,88,99,10,1111
            });


            byte[] buffer = client.Encode();

            UniAttribute de = new UniAttribute();
            de.Decode(buffer);

            aByte = de.Get<byte[]>("aByte");
            byte[]aEmpty = de.Get<byte[]>("aEmpty");
            int[] aint = de.Get<int[]>("aint");
        }
    }
}
