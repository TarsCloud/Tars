using DotNetty.Buffers;
using System;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Codecs.Attributes
{
    public abstract class CodecAttribute : Attribute
    {
        public abstract string Key { get; }

        public abstract RequestPacket DecodeRequest(IByteBuffer input);

        public abstract RequestPacket DecodeResponse(IByteBuffer input);

        public abstract object[] DecodeMethodParameters(RequestPacket input, RpcMethodMetadata metdata);

        public abstract Tuple<object, object[]> DecodeReturnValue(RequestPacket input, RpcMethodMetadata metdata);

        public abstract void EncodeRequest(IByteBuffer output, RequestPacket request);

        public abstract void EncodeResponse(IByteBuffer output, RequestPacket response);

        public abstract byte[] EncodeMethodParameters(object[] parameters, RequestPacket request, RpcMethodMetadata metdata);

        public abstract byte[] EncodeReturnValue(object returnValue, object[] parameters, RequestPacket response, RpcMethodMetadata metdata);
    }
}