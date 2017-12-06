using System;
using System.Threading.Tasks;
using Tars.Csharp.Rpc;
using Tars.Csharp.Rpc.Attributes;
using Tars.Csharp.Rpc.DynamicProxy;

namespace DynamicProxy.Benchmark
{
    [Rpc(Codec = Codec.Tars)]
    public interface IHelloRpc
    {
        Task<string> Hello(int no, string name);
    }

    class Program
    {
        static void Main(string[] args)
        {
            var p = new ProxyInterfaceGenerator();
            p.CreateInterfaceProxyType(typeof(IHelloRpc));
            Console.WriteLine("Hello World!");
        }
    }
}
