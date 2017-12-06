using System;

namespace Tars.Csharp.Rpc.Config
{
    public class ClientConfig
    {
        public string Locator { get; set; }
        public string LogPath { get; set; }
        public string LogLevel { get; set; }
        public string DataPath { get; set; }
        public int SyncInvokeTimeout { get; set; }
        public int AsyncInvokeTimeout { get; set; }
        public int RefreshEndpointInterval { get; set; }
        public string Stat { get; set; }
        public string Property { get; set; }
        public int ReportInterval { get; set; }
        public int SampleRate { get; set; }
        public int MaxSampleCount { get; set; }
        public int SendThread { get; set; }
        public int RecvThread { get; set; }
        public int AsyncThread { get; set; }
        public string ModuleName { get; set; }
        public string SetDivision { get; set; }
        public bool EnableSet { get; set; }
        public int Connections { get; set; }
        public int ConnectTimeout { get; set; }
        public int CorePoolSize { get; set; }
        public int MaxPoolSize { get; set; }
        public int KeepAliveTime { get; set; }
        public int QueueSize { get; set; }
        public string CharsetName { get; set; }
        public Type InterfaceType { get; set; }

        public ClientConfig Load(TarsConfig conf)
        {
            Locator = conf.Get("/tars/application/client<locator>");
            LogPath = conf.Get("/tars/application/client<logpath>", null);
            LogLevel = conf.Get("/tars/application/client<loglevel>", "INFO");
            DataPath = conf.Get("/tars/application/client<cdatapath>", null);
            SyncInvokeTimeout = conf.GetInt("/tars/application/client<sync-invoke-timeout>", 3000);
            AsyncInvokeTimeout = conf.GetInt("/tars/application/client<async-invoke-timeout>", 3000);
            RefreshEndpointInterval = conf.GetInt("/tars/application/client<refresh-endpoint-interval>", 60000);
            Stat = conf.Get("/tars/application/client<stat>");
            Property = conf.Get("/tars/application/client<property>");
            ReportInterval = conf.GetInt("/tars/application/client<report-interval>", 60000);
            SampleRate = conf.GetInt("/tars/application/client<sample-rate>", 1000);
            MaxSampleCount = conf.GetInt("/tars/application/client<max-sample-count>", 100);
            SendThread = conf.GetInt("/tars/application/client<sendthread>", 1);
            RecvThread = conf.GetInt("/tars/application/client<recvthread>", 1);
            AsyncThread = conf.GetInt("/tars/application/client<asyncthread>", 1);
            ModuleName = conf.Get("/tars/application/client<modulename>", Constants.default_modulename);
            string enableSetStr = conf.Get("/tars/application<enableset>");
            SetDivision = conf.Get("/tars/application<setdivision>");
            if ("Y".Equals(enableSetStr, StringComparison.OrdinalIgnoreCase))
            {
                EnableSet = true;
            }
            else
            {
                EnableSet = false;
                SetDivision = null;
            }

            Connections = conf.GetInt("/tars/application/client<connections>", Constants.default_connections);
            ConnectTimeout = conf.GetInt("/tars/application/client<connect-timeout>", Constants.default_connect_timeout);
            CorePoolSize = conf.GetInt("/tars/application/client<corepoolsize>", Constants.default_core_pool_size);
            MaxPoolSize = conf.GetInt("/tars/application/client<maxpoolsize>", Constants.default_max_pool_size);
            KeepAliveTime = conf.GetInt("/tars/application/client<keepalivetime>", Constants.default_keep_alive_time);
            QueueSize = conf.GetInt("/tars/application/client<queuesize>", Constants.default_queue_size);
            CharsetName = conf.Get("/tars/application/client<charsetname>", Constants.default_charset_name);
            InterfaceType = Type.GetType(conf.Get("/tars/application/client<interfacetype>"));
            return this;
        }

        public override int GetHashCode()
        {
            int prime = 31;
            int result = 1;
            result = prime * result + ((Locator == null) ? 0 : Locator.GetHashCode());
            result = prime * result + ((ModuleName == null) ? 0 : ModuleName.GetHashCode());
            return result;
        }

        public override bool Equals(object obj)
        {
            if (this == obj) return true;
            if (obj == null) return false;
            if (GetType() != obj.GetType()) return false;
            var other = (ClientConfig)obj;
            if (Locator == null)
            {
                if (other.Locator != null) return false;
            }
            else if (!Locator.Equals(other.Locator)) return false;
            if (ModuleName == null)
            {
                if (other.ModuleName != null) return false;
            }
            else if (!ModuleName.Equals(other.ModuleName)) return false;
            return true;
        }

        public override string ToString()
        {
            return $"CommunicatorConfig [locator={Locator}, syncInvokeTimeout={SyncInvokeTimeout}, asyncInvokeTimeout={AsyncInvokeTimeout}, refreshEndpointInterval={RefreshEndpointInterval}, reportInterval={ReportInterval}, stat={Stat}, property={Property}, sampleRate={SampleRate}, maxSampleCount={MaxSampleCount}, sendThread={SendThread}, recvThread={RecvThread}, asyncThread={AsyncThread}, moduleName={ModuleName}, enableSet={EnableSet}, setDivision={SetDivision}, connections={Connections}, connectTimeout={ConnectTimeout}, corePoolSize={CorePoolSize}, maxPoolSize={MaxPoolSize}, keepAliveTime={KeepAliveTime}, queueSize={QueueSize}, charsetName={CharsetName}, logPath={LogPath}, logLevel={LogLevel}, dataPath={DataPath}]";
        }
    }
}