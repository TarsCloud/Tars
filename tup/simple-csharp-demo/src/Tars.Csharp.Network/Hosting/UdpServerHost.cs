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
    public class UdpServerHost : ServerHostBase
    {
        private MultithreadEventLoopGroup workerGroup;

        public UdpServerHost(IConfigurationRoot configuration, IChannelHandler initializer,
            ILogger<UdpServerHost> logger) : base(configuration, initializer, logger) { }

        protected override Task<IChannel> BindAsync(IPEndPoint iPEndPoint, IChannelHandler initializer)
        {
            var bootstrap = new Bootstrap();
            bootstrap.Group(workerGroup)
                .Channel<SocketDatagramChannel>()
                .Option(ChannelOption.SoBroadcast, true)
                .Handler(initializer);
            return bootstrap.BindAsync(iPEndPoint);
        }

        protected override void CreateEventLoop(int eventLoopCount)
        {
            workerGroup = new MultithreadEventLoopGroup(eventLoopCount);
        }

        protected override Task StopAsync(TimeSpan quietPeriod, TimeSpan timeout)
        {
            return workerGroup.ShutdownGracefullyAsync(quietPeriod, timeout);
        }
    }
}