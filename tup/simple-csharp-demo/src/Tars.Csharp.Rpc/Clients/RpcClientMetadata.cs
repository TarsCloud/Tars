using AspectCore.Extensions.Reflection;
using System.Collections.Generic;
using System.Reflection;
using Tars.Csharp.Codecs;

namespace Tars.Csharp.Rpc.Clients
{
    public class RpcClientMetadata
    {
        internal IDictionary<string, RpcMetadata> metadatas;

        public RpcClientMetadata(Assembly[] assemblies, bool isGenerateReflector = false)
        {
            metadatas = assemblies.ScanRpcMetadatas(isGenerateReflector, i =>
            {
                var name = i.GetReflector().GetCustomAttribute<RpcAttribute>().Servant;
                return string.IsNullOrWhiteSpace(name) ? i.FullName : name;
            }, i => i.IsInterface && !i.IsGenericType && i.GetReflector().IsDefined<RpcAttribute>());
        }

        public bool TryGetValue(string servant, out RpcMetadata metadata)
        {
            return metadatas.TryGetValue(servant, out metadata);
        }
    }
}