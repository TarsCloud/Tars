using System.Threading.Tasks;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Rpc.Clients
{
    public interface ICallBackHandler<T>
    {
        Task<RequestPacket> AddCallBack(T key, int timeout);

        void SetResult(T key, RequestPacket result);

        int NewRequestId();
    }
}