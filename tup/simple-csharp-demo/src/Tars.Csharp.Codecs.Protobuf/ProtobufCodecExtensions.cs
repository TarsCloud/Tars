using Microsoft.Extensions.DependencyInjection;
using Tars.Csharp.Codecs.Attributes;

namespace Tars.Csharp.Codecs.Protobuf
{
    public static class ProtobufCodecExtensions
    {
        public static IServiceCollection AddProtobufCodec(this IServiceCollection service)
        {
            return service.AddTarsCodec()
                .AddSingleton(i =>
                {
                    var tups = i.GetRequiredService<TarsCodecAttribute>().Tups;
                    return new ProtobufCodecAttribute()
                    {
                        Tups = tups
                    };
                });
        }
    }
}