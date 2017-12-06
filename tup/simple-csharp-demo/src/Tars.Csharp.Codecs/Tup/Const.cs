namespace Tars.Csharp.Codecs.Tup
{
    public class Const
    {
        public static string StatusGridKey = "STATUS_GRID_KEY";
        public static string StatusDyedKey = "STATUS_DYED_KEY";
        public static string StatusGridCode = "STATUS_GRID_CODE";
        public static string StatusSampleKey = "STATUS_SAMPLE_KEY";
        public static string StatusResultCode = "STATUS_RESULT_CODE";
        public static string StatusResultDesc = "STATUS_RESULT_DESC";

        public static int InvalidHashCode = -1;
        public static int InvalidGridCode = -1;

        public static byte PacketTypeTarsnormal = 0;
        public static byte PacketTypeTarsoneway = 1;
        public static byte PacketTypeTup2 = 2;
        public static byte PacketTypeTup3 = 3;

        public const short Version = 0x01;
        public const short Version2 = 0x02;
        public const short Version3 = 0x03;

        public const int ServerSuccess = 0;
        public const int ServerDecodeErr = -1;
        public const int ServerEncodeErr = -2;
        public const int ServerNoFuncErr = -3;
        public const int ServerNoServantErr = -4;
        public const int ServerResetGrid = -5;
        public const int ServerQueueTimeout = -6;
        public const int AsyncCallTimeout = -7;
        public const int ProxyConnectErr = -8;
        public const int ServerOverload = -9;
        public const int ServerUnknownErr = -99;

        public const int MessageTypeNull = 0x00;
        public const int MessageTypeHash = 0x01;
        public const int MessageTypeGrid = 0x02;
        public const int MessageTypeDyed = 0x04;
        public const int MessageTypeSample = 0x08;
        public const int MessageTypeAsync = 0x10;
        public const int MessageTypeLoaded = 0x20;

        public const byte Normal = 0x00;
        public const byte OneWay = 0x01;

        public const string TarsCodec = "Tars";
        public const int TarsMaxStringLength = 100 * 1024 * 1024;
        public const int DefaultTimeout = 60000;
    }
}