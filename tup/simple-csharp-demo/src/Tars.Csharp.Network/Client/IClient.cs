using DotNetty.Buffers;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Client
{
    public interface INetworkClient
    {
        Task ShutdownGracefullyAsync();

        Task SendAsync(EndPoint endPoint, IByteBuffer request);
    }
}