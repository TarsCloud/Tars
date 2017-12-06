using Microsoft.Extensions.DependencyInjection;
using System.Reflection;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Network.Client;
using Tars.Csharp.Rpc.DynamicProxy;

namespace Tars.Csharp.Rpc.Clients
{
    public static class RpcClientExtensions
    {
        public static IServiceCollection UseSimpleRpcClient(this IServiceCollection service, params Assembly[] assemblies)
        {
            return service.AddSingleton(i =>
                {
                    var metadata = new RpcClientMetadata(assemblies);
                    foreach (var item in metadata.metadatas)
                    {
                        item.Value.Codec = i.GetRequiredService(item.Value.CodecType) as CodecAttribute;
                    }
                    return metadata;
                })
                .AddSingleton<ITcpClient, NettyTcpClient>()
                .AddSingleton<IUdpClient, UdpClient>()
                .AddSingleton<RpcClient<ITcpClient>, RpcClient<ITcpClient>>()
                .AddSingleton<RpcClient<IUdpClient>, RpcClient<IUdpClient>>()
                .AddTransient<NetworkClientInitializer<IUdpClient>, RpcUdpClientInitializer>()
                .AddSingleton<IRpcClientFactory, SimpleRpcClientFactory>()
                .AddSingleton<IDynamicProxyGenerator, DynamicProxyGenerator>()
                .AddTransient<NetworkClientInitializer<ITcpClient>, RpcTcpClientInitializer>()
                .AddSingleton<ClientHandler, ClientHandler>()
                .AddSingleton<ICallBackHandler<int>, CallBackHandler<int>>();
        }
    }
}