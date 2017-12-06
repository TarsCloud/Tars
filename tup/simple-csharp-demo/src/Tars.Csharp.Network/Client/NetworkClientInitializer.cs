using DotNetty.Transport.Channels;

namespace Tars.Csharp.Network.Client
{
    public abstract class NetworkClientInitializer<T>
    {
        public abstract void Init(IChannel channel);
    }
}