using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Concurrent;
using System.Threading.Tasks;
using Tars.Csharp.Network.Client;
using Tars.Csharp.Rpc.DynamicProxy;

namespace Tars.Csharp.Rpc.Clients
{
    public class SimpleRpcClientFactory : IRpcClientFactory
    {
        private IServiceProvider provider;
        private IDynamicProxyGenerator generator;
        private ConcurrentDictionary<RpcMode, IRpcClient> clients = new ConcurrentDictionary<RpcMode, IRpcClient>();

        public SimpleRpcClientFactory(IServiceProvider provider, IDynamicProxyGenerator generator)
        {
            this.provider = provider;
            this.generator = generator;
        }

        private IRpcClient GetClient(RpcMode mode)
        {
            return clients.GetOrAdd(mode, m => m == RpcMode.Tcp
            ? (IRpcClient)provider.GetRequiredService<RpcClient<ITcpClient>>()
            : provider.GetRequiredService<RpcClient<IUdpClient>>());
        }

        public T CreateProxy<T>(RpcContext context)
        {
            context.Client = GetClient(context.Mode);
            context.Invoke = (ctx, methodName, parameters) => context.Client.Inovke(context, methodName, parameters);
            return generator.CreateInterfaceProxy<T>(context);
        }

        public async Task ShutdownAsync()
        {
            foreach (var client in clients.Values)
            {
                await client?.ShutdownAsync();
            }
        }
    }
}