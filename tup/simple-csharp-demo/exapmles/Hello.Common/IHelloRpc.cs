using System.Threading.Tasks;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Rpc;

namespace Hello.Common
{
    [Rpc(Servant = "TestApp.HelloServer.HelloObj")]
    [TarsCodec]
    public interface IHelloRpc
    {
        string Hello(int no, string name);

        void HelloHolder(int no, out string name);

        Task<string> HelloTask(int no, string name);

        ValueTask<string> HelloValueTask(int no, string name);

        [TarsOneway]
        void HelloOneway(int no, string name);
    }
}