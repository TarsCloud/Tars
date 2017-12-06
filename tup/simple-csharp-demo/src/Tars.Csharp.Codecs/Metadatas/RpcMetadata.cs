using System;
using System.Collections.Generic;
using Tars.Csharp.Codecs.Attributes;

namespace Tars.Csharp.Codecs
{
    public class RpcMetadata
    {
        public Type InterfaceType { get; set; }

        public string Servant { get; set; }

        public CodecAttribute Codec { get; set; }

        public Dictionary<string, RpcMethodMetadata> Methods { get; set; }

        public Type ServantType { get; set; }

        public object ServantInstance { get; set; }

        public Type CodecType { get; set; }
    }
}