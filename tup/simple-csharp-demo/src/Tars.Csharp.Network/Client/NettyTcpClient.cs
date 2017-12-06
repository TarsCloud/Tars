using DotNetty.Buffers;
using DotNetty.Transport.Bootstrapping;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Channels.Sockets;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Concurrent;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Client
{
    public class NettyTcpClient : ITcpClient
    {
        private ILogger<NettyTcpClient> logger;

        public NettyTcpClient(ILogger<NettyTcpClient> logger, NetworkClientInitializer<ITcpClient> initializer)
        {
            this.logger = logger;
            Build(initializer);
        }

        private MultithreadEventLoopGroup group = new MultithreadEventLoopGroup();
        private Bootstrap bootstrap = new Bootstrap();
        private ConcurrentDictionary<EndPoint, IChannel> channels = new ConcurrentDictionary<EndPoint, IChannel>();

        public NettyTcpClient Build(NetworkClientInitializer<ITcpClient> initializer)
        {
            bootstrap
                .Group(group)
                .Channel<TcpSocketChannel>()
                .Option(ChannelOption.TcpNodelay, true)
                .Handler(new ActionChannelInitializer<IChannel>(channel => initializer.Init(channel)));

            return this;
        }

        private async Task<IChannel> TryConnectAsync(EndPoint endPoint)
        {
            var channel = await bootstrap.ConnectAsync(endPoint);
            channels.AddOrUpdate(endPoint, channel, (x, y) => channel);
            return channel;
        }

        public Task<IChannel> ConnectAsync(EndPoint endPoint)
        {
            if (channels.TryGetValue(endPoint, out IChannel channel)
                && channel.Active) return Task.FromResult(channel);
            else return TryConnectAsync(endPoint);
        }

        internal bool RemoveChannel(EndPoint point)
        {
            return channels.TryRemove(point, out IChannel channel);
        }

        public Task ShutdownGracefullyAsync()
        {
            return group.ShutdownGracefullyAsync(TimeSpan.FromMilliseconds(100), TimeSpan.FromSeconds(1));
        }

        public async Task SendAsync(EndPoint endPoint, IByteBuffer request)
        {
            var channel = await ConnectAsync(endPoint);
            await channel.WriteAndFlushAsync(request);
        }
    }
}