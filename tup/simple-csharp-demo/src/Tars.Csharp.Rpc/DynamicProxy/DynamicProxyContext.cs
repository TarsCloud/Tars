using System;

namespace Tars.Csharp.Rpc.DynamicProxy
{
    public class DynamicProxyContext
    {
        public Func<DynamicProxyContext, string, object[], object> Invoke { get; set; }

        public object InvokeProxy(DynamicProxyContext context, string methodName, object[] parameters)
        {
            return Invoke == null ? null : Invoke(context, methodName, parameters);
        }
    }
}