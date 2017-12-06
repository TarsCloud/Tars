using DotNetty.Transport.Bootstrapping;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Channels.Sockets;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using System;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Hosting
{
    public class TcpServerHost : ServerHostBase
    {
        private MultithreadEventLoopGroup bossGroup;
        private MultithreadEventLoopGroup workerGroup;

        public TcpServerHost(IConfigurationRoot configuration, IChannelHandler initializer,
            ILogger<TcpServerHost> logger) : base(configuration, initializer, logger) { }

        protected override Task<IChannel> BindAsync(IPEndPoint iPEndPoint, IChannelHandler initializer)
        {
            var bootstrap = new ServerBootstrap();
            bootstrap.Group(bossGroup, workerGroup)
                .Channel<TcpServerSocketChannel>()
                .ChildHandler(initializer);
            return bootstrap.BindAsync(iPEndPoint);
        }

        protected override void CreateEventLoop(int eventLoopCount)
        {
            bossGroup = new MultithreadEventLoopGroup(1);
            workerGroup = new MultithreadEventLoopGroup(eventLoopCount);
        }

        protected override Task StopAsync(TimeSpan quietPeriod, TimeSpan timeout)
        {
            return Task.WhenAll(
                    bossGroup.ShutdownGracefullyAsync(quietPeriod, timeout),
                    workerGroup.ShutdownGracefullyAsync(quietPeriod, timeout));
        }
    }
}