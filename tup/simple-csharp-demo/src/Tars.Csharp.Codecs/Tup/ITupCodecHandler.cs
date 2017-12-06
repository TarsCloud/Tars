using System;

namespace Tars.Csharp.Codecs.Tup
{
    public interface ITupCodecHandler
    {
        short Version { get; }

        short DecodeVersion(TarsInputStream input);

        void DecodeRequest(TarsInputStream input, RequestPacket request);

        void DecodeResponse(TarsInputStream input, RequestPacket response);

        object[] DecodeMethodParameters(byte[] body, RpcMethodMetadata metdata);

        Tuple<object, object[]> DecodeReturnValue(byte[] body, RpcMethodMetadata metdata);

        void EncodeRequest(TarsOutputStream output, RequestPacket request);

        void EncodeResponse(TarsOutputStream output, RequestPacket response);

        byte[] EncodeMethodParameters(object[] parameters, RpcMethodMetadata metdata);

        byte[] EncodeReturnValue(object returnValue, object[] parameters, RpcMethodMetadata metdata);
    }
}