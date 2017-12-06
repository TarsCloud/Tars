using System.Threading.Tasks;

namespace Tars.Csharp.Rpc.Clients
{
    public interface IRpcClient
    {
        object Inovke(RpcContext context, string methodName, object[] parameters);

        Task ShutdownAsync();
    }
}