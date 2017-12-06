using System;

namespace Tars.Csharp.Rpc.Config
{
    public class ServantAdapterConfig
    {
        public Endpoint Endpoint { get; set; }
        public string HandleGroup { get; set; }
        public string Protocol { get; set; }
        public int MaxConns { get; set; }
        public int QueueCap { get; set; }
        public int QueueTimeout { get; set; }
        public string Servant { get; set; }
        public int Threads { get; set; }
        public Type ClassType { get; set; }

        public ServantAdapterConfig Load(TarsConfig conf, string adapterName)
        {
            string path = "/tars/application/server/" + adapterName;
            Endpoint = Endpoint.ParseString(conf.Get(path + "<endpoint>"));
            HandleGroup = conf.Get(path + "<handlegroup>", null);
            Protocol = conf.Get(path + "<protocol>", "tars");
            MaxConns = conf.GetInt(path + "<maxconns>", 128);
            QueueCap = conf.GetInt(path + "<queuecap>", 1024);
            QueueTimeout = conf.GetInt(path + "<queuetimeout>", 10000);
            Servant = conf.Get(path + "<servant>");
            Threads = conf.GetInt(path + "<threads>", 1);
            var info = conf.Get(path + "<classtype>");
            ClassType = Type.GetType(info);
            return this;
        }
    }
}