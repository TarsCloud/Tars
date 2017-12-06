using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Tars.Csharp.Network.Hosting;
using Tars.Csharp.Rpc;
using Tars.Csharp.Rpc.Config;

namespace HelloServer
{
    public class Program
    {
        private static void Main(string[] args)
        {
            var config = ServerConfig.ParseFrom("testapp.conf");

            new ServerHostBuilder()
                .ConfigureServices(i => i.AddLogging(j => j.AddConsole()
                    .SetMinimumLevel(LogLevel.Trace)))
                .UseRpc(config)
                .Build()
                .Run();
        }
    }
}