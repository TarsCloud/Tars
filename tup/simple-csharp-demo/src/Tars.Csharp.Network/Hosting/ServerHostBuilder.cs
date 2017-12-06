using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using System;

namespace Tars.Csharp.Network.Hosting
{
    public class ServerHostBuilder
    {
        private IServiceCollection services = new ServiceCollection();

        public ServerHostBuilder ConfigureServices(Action<IServiceCollection> configurer)
        {
            configurer?.Invoke(services);
            return this;
        }

        public ServerHostBuilder ConfigureAppConfiguration(Action<IConfigurationBuilder> configureDelegate)
        {
            var builder = new ConfigurationBuilder();
            configureDelegate?.Invoke(builder);
            services.AddSingleton(i => builder.Build());
            return this;
        }

        public IServerHost Build()
        {
            var provider = services.BuildServiceProvider();
            return provider.GetRequiredService<IServerHost>();
        }
    }
}