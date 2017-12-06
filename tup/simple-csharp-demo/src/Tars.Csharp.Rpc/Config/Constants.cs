using System;

namespace Tars.Csharp.Rpc.Config
{
    public class Constants
    {
        public const string SERVER_NODE_CACHE_FILENAME = "tarsnodes.dat";

        public const int INVOKE_STATUS_SUCC = 0;
        public const int INVOKE_STATUS_EXEC = 1;
        public const int INVOKE_STATUS_TIMEOUT = 2;
        public const int INVOKE_STATUS_NETCONNECTTIMEOUT = 3;

        public const int default_max_sample_count = 200;
        public const int default_sample_rate = 1000;

        public const string default_modulename = "tarsproxy";
        public const string default_stat = "tars.tarsstat.StatObj";

        public const int default_connections = 4;
        public const int default_connect_timeout = 3000;
        public const int default_sync_timeout = 3000;
        public const int default_async_timeout = 3000;

        public const int default_refresh_interval = 60 * 1000;
        public const int default_report_interval = 60 * 1000;

        public const int default_background_queuesize = 20000;

        public const string default_charset_name = "UTF-8";

        public const int default_queue_size = 20000;
        public static readonly int default_core_pool_size = Environment.ProcessorCount;
        public static readonly int default_max_pool_size = Environment.ProcessorCount * 2;
        public const int default_keep_alive_time = 120;

        public const int default_check_interval = 60 * 1000;
        public const int default_try_time_interval = 30;
        public const int default_min_timeout_invoke = 20;
        public const int default_frequence_fail_invoke = 50;
        public const float default_frequence_fail_radio = 0.5f;

        public const string TARS_PROTOCOL = "tars";
        public const string HTTP_PROTOCOL = "http";
        public const string PROTO_PROTOCOL = "protobuff";
        public const string TARS_AT = "@";
        public const string TARS_API = "api";
        public const string TARS_JCE_VERSION = "version";

        public const string TARS_CLIENT_CONNECTIONS = "connections";
        public const string TARS_CLIENT_CONNECTTIMEOUT = "connectTimeout";
        public const string TARS_CLIENT_SYNCTIMEOUT = "syncTimeout";
        public const string TARS_CLIENT_ASYNCTIMEOUT = "asyncTimeout";
        public const string TARS_CLIENT_SETDIVISION = "setDivision";
        public const string TARS_CLIENT_TCPNODELAY = "tcpNoDelay";
        public const string TARS_CLIENT_ACTIVE = "active";
        public const string TARS_CLIENT_UDPMODE = "udpMode";
        public const string TARS_CLIENT_CHARSETNAME = "charsetName";

        public const string TARS_HASH = "tars_hash";
        public const string TARS_CONSISTENT_HASH = "taf_consistent_hash";
        public const string TARS_TUP_CLIENT = "tup_client";
        public const string TARS_ONE_WAY_CLIENT = "one_way_client";
        public const string TARS_NOT_CLIENT = "not_tars_client";
        public const string TARS_CLIENT_ENABLEAUTH = "enableAuth";

        public const string TARS_CLIENT_WEIGHT_TYPE = "weightType";
        public const string TARS_CLIENT_WEIGHT = "weight";
        public const string TARS_CLIENT_GRAYFLAG = "taf.framework.GrayFlag";

        public const string PropWaitTime = "req.queue.waitingtime";
        public const string PropHeapUsed = "jvm.memory.heap.used";
        public const string PropHeapCommitted = "jvm.memory.heap.committed";
        public const string PropHeapMax = "jvm.memory.heap.max";
        public const string PropThreadCount = "jvm.thread.num";
        public const string PropGcCount = "jvm.gc.num.";
        public const string PropGcTime = "jvm.gc.time.";
        
        public const string AdminServant = "AdminObj";
    }
}