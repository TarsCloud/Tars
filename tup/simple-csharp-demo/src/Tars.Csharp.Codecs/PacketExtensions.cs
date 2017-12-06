using DotNetty.Buffers;
using DotNetty.Codecs;
using DotNetty.Transport.Channels;
using Microsoft.Extensions.DependencyInjection;
using System.Linq;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Codecs
{
    public static class PacketExtensions
    {
        public static IChannelPipeline AddLengthFieldHanlder(this IChannelPipeline pipeline, int maxFrameLength, int lengthFieldLength)
        {
            pipeline.AddLast(new LengthFieldBasedFrameDecoder(ByteOrder.BigEndian,
                maxFrameLength, 0, lengthFieldLength, -1 * lengthFieldLength, 0, true));
            return pipeline;
        }

        public static IServiceCollection AddTarsCodec(this IServiceCollection service)
        {
            service
                .AddSingleton<ITupCodecHandler, Tup1CodecHandler>()
                .AddSingleton<ITupCodecHandler, Tup2CodecHandler>()
                .AddSingleton<ITupCodecHandler, Tup3CodecHandler>()
                .AddSingleton(i =>
                {
                    var tups = i.GetServices<ITupCodecHandler>()
                        .GroupBy(j => j.Version)
                        .Select(j => j.First())
                        .ToDictionary(j => j.Version);
                    return new TarsCodecAttribute()
                    {
                        Tups = tups
                    };
                });
            return service;
        }
    }
}