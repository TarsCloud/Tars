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
using System.Linq;
using System.Text;
using System.Threading;

using Tup.Tars;
using Tup;
using Test;
using System.Net.Sockets;
using System.Net;
using System.IO;
namespace ConsoleApplication
{
    class Program
    {
        /*
         * 结构体赋值并序列化的例子
         */
        public void TestTars(ref TestInfo ti, ref byte[] buffer)
        {
            //给结构TestInfo赋值
            ti.ibegin = 1111;
            ti.ii = 54321;
            ti.li = -1000;
            ti.iend = 9999;
            ti.f = 0.999f;
            ti.d = 123.456f;
            ti.s = "teststring0";
            ti.uii = 123456789;

            //test vector<byte>
            ti.vb = new List<byte> ();

            //////////////////////////////////////////////
            //vector<int> 类型的成员赋值
            ti.vi = new List<int>();
            for (int il = 0; il < 10; il++)
            {
                ti.vi.Add(10000 + il);
            }

            //////////////////////////////////////////////
            //map<int, string> 类型的成员赋值
            ti.mi = new Dictionary<int, string>();
            for (int idl = 0; idl < 10; idl++)
            {
                ti.mi.Add(20000 + idl, "testmap" + idl);
            }


            //////////////////////////////////////////////
            //结构类型的成员赋值
            ti.aa = new A();
            ti.aa.a = 200;
            ti.aa.b = new B();
            ti.aa.b.a = 300;
            ti.aa.b.f = 0.300f;


            //////////////////////////////////////////////
            // vector<struct>类型的成员赋值
            ti.vi2 = new List<A>();
            for (int j = 0; j < 10; j++)
            {
                A a = new A();
                a.a = 200000 + j;
                a.b = new B();
                a.b.a = 300;
                a.b.f = 0.300f;
                ti.vi2.Add(a);

            }
            ///List<float>类型成员赋值
            ti.vf = new List<float>();
            float ff = 1.23f;
            for (int il = 0; il < 5; il++)
            {
                ti.vf.Add(ff);
            }
            //////////////////////////////////////////////
            // map<int, struct>类型的成员赋值
            ti.mi2 = new Dictionary<int, A>();
            for (int il = 0; il < 10; il++)
            {
                A a = new A();
                a.a = 200 + il;
                a.b = new B();
                a.b.a = 300;
                a.b.f = 0.300f;
                ti.mi2.Add(300000 + il, a);
            }


            //////////////////////////////////////////////
            // map<string, vector<struct>>类型的成员赋值
            ti.msv = new Dictionary<string, List<A>>();
            for (int il = 0; il < 10; il++)
            {
                List<A> listA = new List<A>();
                for (int j = 0; j < 2; j++)
                {
                    A a = new A();
                    a.a = 200 + il;
                    a.b = new B();
                    a.b.a = 300;
                    a.b.f = 0.300f;

                    listA.Add(a);
                }
                ti.msv.Add("key" + 400000 + il, listA);
            }
            //结构体赋值结束

             TarsOutputStream os = new TarsOutputStream(0);
             //把结构序列化
             ti.WriteTo(os);

             buffer = os.toByteArray();

             System.IO.File.WriteAllBytes(@"tarsbuffer.txt", buffer);

            TarsInputStream _is = new TarsInputStream();
            _is.wrap(buffer, 0);
            TestInfo stTest = new TestInfo();
            stTest.ReadFrom(_is);

            Console.WriteLine("stTest.ibegin:" + stTest.ibegin);
            Console.WriteLine("stTest.li:" + stTest.li);
            Console.WriteLine("stTest.s:" + stTest.s);
            Console.ReadLine();
        }

        /*
         * 测试UniAttributte
         */
        public void TestUnipack(TestInfo ti)
        {
            byte[] buffer = new byte[8192];
            UniAttribute unipack = new UniAttribute();

            short tests = -100;
            int testi = int.MaxValue;
            long testl = -1000;
            float testf = -100.032f;
            string teststr = "teststring";

            //设置tup版本号，默认是tup2，非精简版
            unipack.Version = Const.PACKET_TYPE_TUP;

            unipack.Put("mystruct", ti);
            unipack.Put("tests", tests);
            unipack.Put("testi", testi);
            unipack.Put("testl", testl);
            unipack.Put("testf", testf);
            unipack.Put("teststr", teststr);

            buffer = unipack.Encode();
            Console.WriteLine("buffer.size:" + buffer.Length);

            UniAttribute de = new UniAttribute();
            TarsInputStream _is = new TarsInputStream(buffer);
            de.ReadFrom(_is);

            TestInfo st = new TestInfo();
            short dtests = 0;
            int dtesti = 0;
            long dtestl = 0;
            float dtestf = 0.0f;
            string dteststr = "";

            //使用新的Get<T>(string Name, T DefaultObj)
            dtests = de.Get<short>("tests", dtests);
            dtesti = de.Get<int>("testi", dtesti);
            dtestl = de.Get<long>("testl", dtestl);
            dtestf = de.Get<float>("testf", dtestf);
            dteststr = de.Get<string>("teststr", dteststr);
            st = de.Get<TestInfo>("mystruct", st);

            Console.WriteLine("dtests:" + dtests);
            Console.WriteLine("dtesti:" + dtesti);
            Console.WriteLine("dtestl:" + dtestl);
            Console.WriteLine("dtestf:" + dtestf);
            Console.WriteLine("dteststr:" + dteststr);
            Console.WriteLine("\nTestInfo get struct:  st->ibegin=" + st.ibegin + " st->li=" + st.li);
            Console.ReadLine();
        }

        /*
         * tup的例子
         */
        public void TestTup(TestInfo ti, ref byte[] buffer)
        {

            UniPacket client = new UniPacket();
            client.setVersion(3);
            client.ServantName = "ServantName";
            client.FuncName = "test";

            short tests = -100;
            int testi = int.MaxValue;
            long testl = -1000;
            float testf = -100.032f;
            string teststr = "teststring";

            client.Put("mystruct", ti);
            client.Put("tests", tests);
            client.Put("testi", testi);
            client.Put("testl", testl);
            client.Put("testf", testf);
            client.Put("teststr", teststr);

            //编码tup
            buffer = client.Encode();
            Console.WriteLine("buffer.size:" + buffer.Length);

            //解码tup
            UniPacket de = new UniPacket();
            de.Decode(buffer);

            TestInfo st = new TestInfo();
            short dtests = 0;
            int dtesti = 0;
            long dtestl = 0;
            float dtestf = 0.0f;
            string dteststr = "";

            //使用新的Get<T>(string Name, T DefaultObj)
            dtests = de.Get<short>("tests", dtests);
            dtesti = de.Get<int>("testi", dtesti);
            dtestl = de.Get<long>("testl", dtestl);
            dtestf = de.Get<float>("testf", dtestf);
            dteststr = de.Get<string>("teststr", dteststr);
            st = de.Get<TestInfo>("mystruct", st);

            Console.WriteLine("dtests:" + dtests);
            Console.WriteLine("dtesti:" + dtesti);
            Console.WriteLine("dtestl:" + dtestl);
            Console.WriteLine("dtestf:" + dtestf);
            Console.WriteLine("dteststr:" + dteststr);
           
            Console.WriteLine("\nTestInfo get struct:  st->ibegin=" + st.ibegin + " st->li=" +　st.li);
            Console.ReadLine();
        }

            
        static public string bin2hex(byte[] value)
        {
            string strValue = "\r\n";
            int i = 0;
            foreach (byte bt in value)
            {
                strValue += string.Format(" {0,02:x2}", bt);
                i++;
                if ((i % 16) == 0)
                {
                    strValue += "\n";
                }
            }
            return strValue;
        }
        static  void assignTestInfo(ref TestInfo ti)
        {
            //给结构TestInfo赋值
            //int 类型赋值
            ti.ibegin = 1111;
            ti.ii = short.MinValue+1;
            ti.iend = 9999;

            //bool类型
            ti.b = true;

            //short类型
            //ti.si = short.MaxValue-1;
            ti.si = -8; 

            //byte类型
            ti.by = 65;

            //long 
            ti.li = -1000;
            ti.uii = uint.MaxValue;

            //float类型赋值
            ti.f = 0.999f;
            
            //double 类型赋值
            ti.d = 123.456f;

            //string类型
            ti.s = "teststring0";

            //////////////////////////////////////////////
            //vector<int> 类型的成员赋值
            ti.vi = new List<int>();
            for (int il = 0; il < 5; il++)
            {
                ti.vi.Add(10000 + il);
            }

            //vector<byte>类型成员赋值
            ti.vb = new List<byte>();
            for (byte il = 97; il < 105; il++)
            {
                ti.vb.Add(il);
            }

            //////////////////////////////////////////////
            //map<int, string> 类型的成员赋值
            ti.mi = new Dictionary<int, string>();
            for (int idl = 0; idl < 5; idl++)
            {
                ti.mi.Add(20000 + idl, "testmap_" + Convert.ToString(idl));
            }


            //////////////////////////////////////////////
            //结构类型的成员赋值
            ti.aa = new A();
            ti.aa.a = 200;
            ti.aa.b = new B();
            ti.aa.b.a = 300;
            ti.aa.b.f = 0.300f;


            //////////////////////////////////////////////
            // vector<struct>类型的成员赋值
            ti.vi2 = new List<A>();
            for (int j = 0; j <5; j++)
            {
                A a = new A();
                a.a = 200000 + j;
                a.b = new B();
                a.b.a = 300;
                a.b.f = 0.300f;
                ti.vi2.Add(a);

            }

            ///List<float>类型成员赋值
            ti.vf = new List<float>();
            float ff = 1.345f;
            for (int il = 0; il < 5; il++)
            {
                ti.vf.Add(ff);
            }
            //////////////////////////////////////////////
            // map<int, struct>类型的成员赋值
            ti.mi2 = new Dictionary<int, A>();
            for (int il = 0; il < 5; il++)
            {
                A a = new A();
                a.a = 200 + il;
                a.b = new B();
                a.b.a = 300;
                a.b.f = 0.300f;
                ti.mi2.Add(300000 + il, a);
            }

            //////////////////////////////////////////////
            // map<string, vector<struct>>类型的成员赋值
            ti.msv = new Dictionary<string, List<A>>();
            for (int il = 0; il < 5; il++)
            {
                List<A> listA = new List<A>();
                for (int j = 0; j < 2; j++)
                {
                    A a = new A();
                    a.a = 200 + il;
                    a.b = new B();
                    a.b.a = 300;
                    a.b.f = 0.300f;

                    listA.Add(a);
                }
                ti.msv.Add("key" + 400000 + il, listA);
            }
            //结构体赋值结束
        }
        public void testbyNetWork()
        {
            try
            {
                tcpClient client = new tcpClient();

                //建立socket
                client.createSocket();
                //连接服务端 
                client.connect("127.0.0.1", 8080);

                TestInfo tiSend = new TestInfo();
                assignTestInfo(ref tiSend);

                //-------------------------encode-----------------------
                UniPacket encodePack = new UniPacket();

                //设置tup版本号,默认是PACKET_TYPE_TUP
                encodePack.ServantName = "TestInfo";
                encodePack.FuncName = "TestInfo";

                short tests = -100;
                int testi = int.MaxValue;
                long testl = -1000;
                float testf = -100.032f;
                string teststr = "teststring";

                encodePack.Put("tests", tests);
                encodePack.Put("testi", testi);
                encodePack.Put("testl", testl);
                encodePack.Put("testf", testf);
                encodePack.Put("teststr", teststr);
                encodePack.Put<TestInfo>("TestInfo", tiSend);

                byte[] bufferS = encodePack.Encode();

                Console.WriteLine("打印发送 buffer");
                int ret = client.send(bufferS);

                StringBuilder sb = new StringBuilder();
                tiSend.Display(sb, 0);
                Console.WriteLine(sb.ToString());
                Console.WriteLine("发送大小: " + ret);

                byte[] bufferR = new byte[8192];

                ret = client.receive(bufferR);

                Console.WriteLine("打印接收 buffer");
                Console.WriteLine("接收大小:" + ret);

                //----------------------decode----------------------------
                if (ret > sizeof(int))
                {
                    UniPacket decodePack = new UniPacket();
                    decodePack.Decode(bufferR);

                    TestInfo tiRecv = new TestInfo();
                    short dtests = 0;
                    int dtesti = 0;
                    long dtestl = 0;
                    float dtestf = 0.0f;
                    string dteststr = "";

                    //使用新的Get<T>(string Name, T DefaultObj)
                    dtests = decodePack.Get<short>("tests", dtests);
                    dtesti = decodePack.Get<int>("testi", dtesti);
                    dtestl = decodePack.Get<long>("testl", dtestl);
                    dtestf = decodePack.Get<float>("testf", dtestf);
                    dteststr = decodePack.Get<string>("teststr", dteststr);
                    tiRecv = decodePack.Get<TestInfo>("TestInfo", tiRecv);

                    sb.Clear();
                    tiRecv.Display(sb, 0);
                    Console.WriteLine("end:" + sb.ToString());
                    Console.WriteLine("dtests:" + dtests);
                    Console.WriteLine("dtesti:" + dtesti);
                    Console.WriteLine("dtestl:" + dtestl);
                    Console.WriteLine("dtestf:" + dtestf);
                    Console.WriteLine("dteststr:" + dteststr);
                    Console.ReadLine();
                }

                client.close();
            }
            catch (Exception e)
            {
                Console.WriteLine("testbyNetWork: " + e.Message);
            }
           
        }
        public void testtupprotocol()
        {
            try
            {
                tcpClient client = new tcpClient();

                //建立socket
                client.createSocket();
                //连接服务端 
                client.connect("127.0.0.1", 8080);

                //-------------------------encode-----------------------
                TarsUniPacket encodePack = new TarsUniPacket();

                encodePack.RequestId = 1;
                //设置tup版本号,默认是PACKET_TYPE_TUP
                encodePack.setTarsVersion(Const.PACKET_TYPE_TUP3);
                encodePack.setTarsPacketType(Const.PACKET_TYPE_TARSNORMAL);
                encodePack.ServantName = "Test.HelloServer.HelloImpObj";
                encodePack.FuncName = "testHello";

                string sHello = "i am a string from csharp";
                encodePack.Put("s", sHello);

                byte[] bufferS = encodePack.Encode();

                Console.WriteLine("打印发送 buffer");
                int ret = client.send(bufferS);

                Console.WriteLine(bin2hex(bufferS));
                Console.WriteLine("发送大小: " + ret);

                byte[] bufferR = new byte[8192];

                ret = client.receive(bufferR);


                //----------------------decode----------------------------
                if (ret > sizeof(int))
                {
                    BinaryReader br = new BinaryReader(new MemoryStream(bufferR));
                    //前4个字节为tup包大小
                    int len = ByteConverter.ReverseEndian(br.ReadInt32());

                    byte[] bufferD = new byte[len];
                    System.Array.Copy(bufferR, 0, bufferD, 0, len);


                    TarsUniPacket respPack = new TarsUniPacket();
                    respPack.Decode(bufferD);

                    Console.WriteLine("打印接收 buffer");
                    Console.WriteLine(bin2hex(bufferD));
                    Console.WriteLine("接收大小:" + ret);

                    if (respPack.getTarsResultCode() == 0)
                    {
                        string sOut = "";
                        sOut = respPack.Get<string>("r", sOut);
                        Console.WriteLine("sOut:" + sOut);
                    }
                    else
                    {
                        Console.WriteLine("tup invoke failure");
                    }
                    Console.ReadLine();
                }

                client.close();
            }
            catch (Exception e)
            {
                Console.WriteLine("testbyNetWork: " + e.Message);
            }
           
        }
        static void Main(string[] args)
        {
            try
            {
                Program p = new Program();
                TestInfo ti = new TestInfo();

                Program.assignTestInfo(ref ti);
                p.TestUnipack(ti);

                byte[] buffer = new byte[8192];
                p.TestTars(ref ti, ref buffer);
                p.TestTup(ti, ref buffer);
                Console.WriteLine("buffer size:" + buffer.Length);
            }catch(Exception e)
            {
                Console.WriteLine("Exception :"+e.StackTrace +"\n" + e.Message);
            }
            Console.ReadLine();
        }
    }
    
    class tcpClient
    {

        public tcpClient()
        {

        }

        public void createSocket()
        {
            try
            {
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            }
            catch (SocketException e)
            {
                Console.WriteLine("create socket exception:" + e.ToString());
            }

        }

        public void connect(string ip, int port)
        {
            try
            {
                s.Connect(IPAddress.Parse(ip), port);
            }
            catch (Exception e)
            {
                Console.WriteLine("Connect socket exception:" + e.ToString());
            }

        }
        public int send(byte[] bytesSendStr)
        {
            return s.Send(bytesSendStr);
        }

        public int receive(byte[] buffer)
        {
            return s.Receive(buffer);
        }
        public void close()
        {
            if (s != null)
            {
                s.Close();
            }
        }
        private Socket s;
    }
}
