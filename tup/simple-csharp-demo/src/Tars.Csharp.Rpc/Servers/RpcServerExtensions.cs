using DotNetty.Transport.Channels;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Tars.Csharp.Codecs;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Network.Hosting;
using Tars.Csharp.Rpc.Clients;
using Tars.Csharp.Rpc.Config;

namespace Tars.Csharp.Rpc
{
    public static class RpcServerExtensions
    {
        public static ServerHostBuilder UseRpc(this ServerHostBuilder builder, ServerConfig config)
        {
            var kv = new Dictionary<string, string>()
            {
                { ServerHostOptions.Port, config.Local.Port.ToString() }
            };

            var assemblies = config.ServantAdapterConfMap.Values
                .Where(i=> i.ClassType != null)
                .Select(i => i.ClassType.Assembly)
                .Union(new Assembly[] { config.ClientConfig.InterfaceType.Assembly }).ToArray();

            return builder.ConfigureAppConfiguration(i => i.AddInMemoryCollection(kv))
                .ConfigureServices(i => i.UseSimpleRpcClient(assemblies)
                    .AddTarsCodec())
                .UseRpc(config.Local.Type == "tcp" ? RpcMode.Tcp : RpcMode.Udp, true, config, assemblies);
        }

        public static ServerHostBuilder UseRpc(this ServerHostBuilder builder, RpcMode mode = RpcMode.Tcp, bool isLibuv = true, ServerConfig config = null, params Assembly[] assemblies)
        {
            var ms = new ServerRpcMetadata(assemblies);
            UseTarsCodec(builder);
            builder.ConfigureServices(i =>
            {
                var services = i.AddSingleton<ServerHandler, ServerHandler>();
                foreach (var item in ms.metadatas)
                {
                    services.AddSingleton(item.Value.InterfaceType, item.Value.ServantType);
                }
                services.AddSingleton(j =>
                {
                    foreach (var item in ms.metadatas)
                    {
                        var configMetadata = config?.ServantAdapterConfMap.Values.FirstOrDefault(x => x.ClassType == item.Value.ServantType);

                        if (configMetadata != null)
                        {
                            item.Value.Servant = configMetadata.Servant;
                        }
                        item.Value.ServantInstance = j.GetRequiredService(item.Value.InterfaceType);
                        item.Value.Codec = j.GetRequiredService(item.Value.CodecType) as CodecAttribute;
                    }
                    return ms.ChangeRpcMetadataKeyForConfig();
                });
            });
            ConfigHost(builder, mode, isLibuv);
            return builder;
        }

        private static void ConfigHost(ServerHostBuilder builder, RpcMode mode, bool isLibuv)
        {
            void action(IServiceProvider i, IChannelPipeline j, bool hasLengthField)
            {
                var config = i.GetRequiredService<IConfigurationRoot>();
                var codec = i.GetRequiredService<TarsCodecAttribute>();
                var handler = i.GetRequiredService<ServerHandler>();
                if (hasLengthField)
                {
                    var packetMaxSize = config.GetValue(ServerHostOptions.PacketMaxSize, 100 * 1024 * 1024);
                    var lengthFieldLength = config.GetValue(ServerHostOptions.LengthFieldLength, 4);
                    j.AddLengthFieldHanlder(packetMaxSize, lengthFieldLength);
                }
                j.AddLast(new TarsRequestDecoder(codec), new TarsResponseEncoder(codec), handler);
            }

            switch (mode)
            {
                case RpcMode.Udp:
                    builder.UseUdp((x, y) => action(x, y, false));
                    break;

                case RpcMode.Tcp when isLibuv:
                    builder.UseLibuvTcp((x, y) => action(x, y, true));
                    break;

                default:
                    builder.UseTcp((x, y) => action(x, y, true));
                    break;
            }
        }

        private static void UseTarsCodec(ServerHostBuilder builder)
        {
            builder.ConfigureServices(i =>
            {
                i.AddSingleton<IDictionary<string, CodecAttribute>>(j =>
                {
                    var dict = new Dictionary<string, CodecAttribute>(StringComparer.OrdinalIgnoreCase);
                    var tars = j.GetRequiredService<TarsCodecAttribute>();
                    dict.Add(tars.Key, tars);
                    foreach (var item in j.GetServices<CodecAttribute>())
                    {
                        dict.Add(item.Key, item);
                    }
                    return dict;
                });
            });
        }
    }
}