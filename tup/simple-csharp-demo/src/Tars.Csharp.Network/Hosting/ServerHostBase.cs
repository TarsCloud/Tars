using DotNetty.Transport.Channels;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using System;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Hosting
{
    public abstract class ServerHostBase : IServerHost
    {
        private IChannelHandler initializer;

        private IConfigurationRoot configuration;

        private ILogger logger;

        public ServerHostBase(IConfigurationRoot configuration, IChannelHandler initializer, ILogger logger)
        {
            this.logger = logger;
            this.initializer = initializer;
            this.configuration = configuration;
        }

        public void Run()
        {
            RunAsync().Wait();
        }

        public async Task RunAsync()
        {
            var eventLoopCount = configuration.GetValue(ServerHostOptions.EventLoopCount, Environment.ProcessorCount);
            var quietPeriod = TimeSpan.FromMilliseconds(configuration.GetValue(ServerHostOptions.ShutdownQuietPeriod, 100));
            var timeout = TimeSpan.FromMilliseconds(configuration.GetValue(ServerHostOptions.ShutdownTimeout, 1000));
            var port = configuration.GetValue(ServerHostOptions.Port, 8080);

            CreateEventLoop(eventLoopCount);
            try
            {
                var boundChannel = await BindAsync(new IPEndPoint(IPAddress.Any, port), initializer);
                logger.LogInformation($"Server start: {port} ... Press any key to stop.");
                Console.ReadKey();
                await boundChannel.CloseAsync();
            }
            catch (Exception ex)
            {
                logger.LogError(ex, "Server stoped!");
            }
            finally
            {
                await StopAsync(quietPeriod, timeout);
            }
        }

        protected abstract Task StopAsync(TimeSpan quietPeriod, TimeSpan timeout);

        protected abstract Task<IChannel> BindAsync(IPEndPoint iPEndPoint, IChannelHandler initializer);

        protected abstract void CreateEventLoop(int eventLoopCount);
    }
}