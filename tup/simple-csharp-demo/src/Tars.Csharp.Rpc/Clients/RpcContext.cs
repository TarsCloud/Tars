using System.Collections.Generic;
using System.Net;
using Tars.Csharp.Codecs.Tup;
using Tars.Csharp.Rpc.DynamicProxy;

namespace Tars.Csharp.Rpc.Clients
{
    public class RpcContext : DynamicProxyContext
    {
        public EndPoint EndPoint { get; set; }
        public IRpcClient Client { get; set; }
        public RpcMode Mode { get; set; }
        public string Servant { get; set; }
        public int Timeout { get; set; } = Const.DefaultTimeout;
        public IDictionary<string, string> Context { get; set; }
        public IDictionary<string, string> Status { get; set; }
        public short Version { get; set; } = Const.Version;
        public int MessageType { get; set; }
        public byte PacketType { get; set; }

        public RequestPacket CreatePacket()
        {
            return new RequestPacket()
            {
                ServantName = Servant,
                Version = Version,
                MessageType = MessageType,
                PacketType = PacketType,
                Timeout = Timeout,
                Context = Context,
                Status = Status
            };
        }
    }
}