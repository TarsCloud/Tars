using System.Threading.Tasks;

namespace Tars.Csharp.Rpc.Clients
{
    public interface IRpcClientFactory
    {
        T CreateProxy<T>(RpcContext context);

        Task ShutdownAsync();
    }
}