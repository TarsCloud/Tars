using System;

namespace Tars.Csharp.Rpc
{
    public class RpcAttribute : Attribute
    {
        public string Servant { get; set; }
    }
}