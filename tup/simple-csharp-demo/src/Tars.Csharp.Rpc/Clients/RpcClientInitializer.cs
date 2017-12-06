using DotNetty.Transport.Channels;
using Microsoft.Extensions.Configuration;
using Tars.Csharp.Codecs;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Network.Client;
using Tars.Csharp.Network.Hosting;

namespace Tars.Csharp.Rpc.Clients
{
    public class RpcUdpClientInitializer : NetworkClientInitializer<IUdpClient>
    {
        private ClientHandler handler;
        private TarsCodecAttribute tars;

        public RpcUdpClientInitializer(ClientHandler handler, TarsCodecAttribute tars)
        {
            this.handler = handler;
            this.tars = tars;
        }

        public override void Init(IChannel channel)
        {
            channel.Pipeline.AddLast(new ClientUdpHandler(), new TarsResponseDecoder(tars), handler);
        }
    }

    public class RpcTcpClientInitializer : NetworkClientInitializer<ITcpClient>
    {
        private IConfigurationRoot configuration;
        private ClientHandler handler;
        private TarsCodecAttribute tars;

        public RpcTcpClientInitializer(IConfigurationRoot configuration, ClientHandler handler, TarsCodecAttribute tars)
        {
            this.configuration = configuration;
            this.handler = handler;
            this.tars = tars;
        }

        public override void Init(IChannel channel)
        {
            var packetMaxSize = configuration.GetValue(ServerHostOptions.PacketMaxSize, 100 * 1024 * 1024);
            var lengthFieldLength = configuration.GetValue(ServerHostOptions.LengthFieldLength, 4);
            channel.Pipeline.AddLengthFieldHanlder(packetMaxSize, lengthFieldLength);
            channel.Pipeline.AddLast(new TarsResponseDecoder(tars), handler);
        }
    }
}