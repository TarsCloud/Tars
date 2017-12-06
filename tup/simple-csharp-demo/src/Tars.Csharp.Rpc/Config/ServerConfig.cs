using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace Tars.Csharp.Rpc.Config
{
    public class ServerConfig
    {
        public ClientConfig ClientConfig { get; set; }
        public string Application { get; set; }
        public string ServerName { get; set; }
        public Endpoint Local { get; set; }
        public string Node { get; set; }
        public string BasePath { get; set; }
        public string DataPath { get; set; }
        public string CharsetName { get; set; }
        public string Config { get; set; }
        public string Notify { get; set; }
        public string Log { get; set; }
        public string LogPath { get; set; }
        public string LogLevel { get; set; }
        public int LogRate { get; set; }
        public string LocalIP { get; set; }
        public int SessionTimeOut { get; set; }
        public int SessionCheckInterval { get; set; }
        public int UdpBufferSize { get; set; }
        public bool TcpNoDelay { get; set; }
        public Dictionary<string, ServantAdapterConfig> ServantAdapterConfMap { get; set; }

        public ServerConfig Load(TarsConfig conf)
        {
            Application = conf.Get("/tars/application/server<app>", "UNKNOWN");
            ServerName = conf.Get("/tars/application/server<server>", null);

            string localStr = conf.Get("/tars/application/server<local>");
            Local = localStr == null || localStr.Length <= 0 ? null : Endpoint
                    .ParseString(localStr);
            Node = conf.Get("/tars/application/server<node>");
            BasePath = conf.Get("/tars/application/server<basepath>");
            DataPath = conf.Get("/tars/application/server<datapath>");

            CharsetName = conf.Get("/tars/application/server<charsetname>", "UTF-8");

            Config = conf.Get("/tars/application/server<config>");
            Notify = conf.Get("/tars/application/server<notify>");

            Log = conf.Get("/tars/application/server<log>");
            LogPath = conf.Get("/tars/application/server<logpath>", null);
            LogLevel = conf.Get("/tars/application/server<loglevel>");
            LogRate = conf.GetInt("/tars/application/server<lograte>", 5);

            LocalIP = conf.Get("/tars/application/server<localip>");

            SessionTimeOut = conf.GetInt(
                    "/tars/application/server<sessiontimeout>", 120000);
            SessionCheckInterval = conf.GetInt(
                    "/tars/application/server<sessioncheckinterval>", 60000);
            UdpBufferSize = conf.GetInt("/tars/application/server<udpbuffersize>",
                    4096);
            TcpNoDelay = conf
                    .GetBool("/tars/application/server<tcpnodelay>", false);

            ServantAdapterConfMap = new Dictionary<string, ServantAdapterConfig>();
            List<String> adapterNameList = conf
                    .GetSubTags("/tars/application/server");
            if (adapterNameList != null)
            {
                foreach (string adapterName in adapterNameList)
                {
                    ServantAdapterConfig config = new ServantAdapterConfig();
                    config.Load(conf, adapterName);
                    ServantAdapterConfMap.Add(config.Servant, config);
                }
            }

            ServantAdapterConfig adminServantAdapterConfig = new ServantAdapterConfig
            {
                Endpoint = Local,
                Servant = $"{Application}.{ServerName}.{Constants.AdminServant}"
            };
            ServantAdapterConfMap.Add(Constants.AdminServant,
                    adminServantAdapterConfig);

            if (Application != null && ServerName != null && LogPath != null)
            {
                LogPath = LogPath + Path.PathSeparator + Application + Path.PathSeparator
                        + ServerName;
            }
            ClientConfig = new ClientConfig().Load(conf);
            if (LogPath != null)
            {
                ClientConfig.LogPath = LogPath;
            }
            ClientConfig.LogLevel = LogLevel;
            ClientConfig.DataPath = DataPath;
            return this;
        }

        public static ServerConfig ParseFrom(string path)
        {
            return new ServerConfig().Load(TarsConfig.ParseFile(path));
        }
    }
}