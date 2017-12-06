using DotNetty.Transport.Bootstrapping;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Libuv;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using System;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Hosting
{
    public class LibuvTcpServerHost : ServerHostBase
    {
        private MultithreadEventLoopGroup bossGroup;
        private WorkerEventLoopGroup workerGroup;

        public LibuvTcpServerHost(IConfigurationRoot configuration, IChannelHandler initializer,
            ILogger<LibuvTcpServerHost> logger) : base(configuration, initializer, logger) { }

        protected override Task<IChannel> BindAsync(IPEndPoint iPEndPoint, IChannelHandler initializer)
        {
            var bootstrap = new ServerBootstrap();
            bootstrap.Group(bossGroup, workerGroup)
                .Channel<TcpServerChannel>()
                .ChildHandler(initializer);
            return bootstrap.BindAsync(iPEndPoint);
        }

        protected override void CreateEventLoop(int eventLoopCount)
        {
            var dispatcher = new DispatcherEventLoop();
            bossGroup = new MultithreadEventLoopGroup(_ => dispatcher, 1);
            workerGroup = new WorkerEventLoopGroup(dispatcher, eventLoopCount);
        }

        protected override Task StopAsync(TimeSpan quietPeriod, TimeSpan timeout)
        {
            return Task.WhenAll(
                    bossGroup.ShutdownGracefullyAsync(quietPeriod, timeout),
                    workerGroup.ShutdownGracefullyAsync(quietPeriod, timeout));
        }
    }
}