using DotNetty.Buffers;
using DotNetty.Transport.Bootstrapping;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Channels.Sockets;
using System;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Client
{
    public class UdpClient : IUdpClient
    {
        private IChannel channel;

        public UdpClient(NetworkClientInitializer<IUdpClient> initializer)
        {
            Build(initializer);
        }

        private MultithreadEventLoopGroup group = new MultithreadEventLoopGroup();
        private Bootstrap bootstrap = new Bootstrap();

        public void Build(NetworkClientInitializer<IUdpClient> initializer)
        {
            bootstrap
                .Group(group)
                .Channel<SocketDatagramChannel>()
                .Option(ChannelOption.SoBroadcast, true)
                .Handler(new ActionChannelInitializer<IChannel>(channel => initializer.Init(channel)));
        }

        public async Task<IChannel> ConnectAsync(EndPoint endPoint)
        {
            if (channel == null || !channel.Active)
            {
                channel = await bootstrap.BindAsync(8787);
            }
            return channel;
        }

        public Task ShutdownGracefullyAsync()
        {
            return group.ShutdownGracefullyAsync(TimeSpan.FromMilliseconds(100), TimeSpan.FromSeconds(1));
        }

        public async Task SendAsync(EndPoint endPoint, IByteBuffer request)
        {
            var channel = await ConnectAsync(endPoint);
            await channel.WriteAndFlushAsync(new DatagramPacket(request, channel.LocalAddress, endPoint));
        }
    }
}