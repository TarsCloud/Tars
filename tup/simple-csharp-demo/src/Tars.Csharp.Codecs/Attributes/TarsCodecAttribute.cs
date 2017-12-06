using DotNetty.Buffers;
using System;
using System.Collections.Generic;
using System.Linq;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Codecs.Attributes
{
    public class TarsCodecAttribute : CodecAttribute
    {
        public override string Key => Const.TarsCodec;

        private IDictionary<short, ITupCodecHandler> tups;

        public IDictionary<short, ITupCodecHandler> Tups
        {
            get { return tups; }
            set
            {
                tups = value;
                VersionHandler = value.Values.First();
            }
        }

        protected ITupCodecHandler VersionHandler { get; private set; }

        protected ITupCodecHandler GetTupByVersion(short version)
        {
            if (!Tups.ContainsKey(version))
            {
                throw new TarsException(Const.ServerUnknownErr, $"un supported protocol, ver={version}");
            }
            return Tups[version];
        }

        protected T TryDecode<T>(Func<T> func)
        {
            return Try(func, Const.ServerDecodeErr);
        }

        protected void TryDecode(Action action)
        {
            Try<object>(() => { action(); return null; }, Const.ServerDecodeErr);
        }

        protected T TryEncode<T>(Func<T> func)
        {
            return Try(func, Const.ServerEncodeErr);
        }

        protected void TryEncode(Action action)
        {
            Try<object>(() => { action(); return null; }, Const.ServerEncodeErr);
        }

        protected T Try<T>(Func<T> func, int code)
        {
            try
            {
                return func();
            }
            catch (Exception ex)
            {
                throw new TarsException(code, ex.Message);
            }
        }

        public override RequestPacket DecodeRequest(IByteBuffer input)
        {
            var inputStream = new TarsInputStream(input);
            var result = new RequestPacket
            {
                Version = TryDecode(() => VersionHandler.DecodeVersion(inputStream))
            };
            var handler = GetTupByVersion(result.Version);
            TryDecode(() => handler.DecodeRequest(inputStream, result));
            input.MarkReaderIndex();
            return result;
        }

        public override RequestPacket DecodeResponse(IByteBuffer input)
        {
            var inputStream = new TarsInputStream(input);
            var result = new RequestPacket
            {
                Version = TryDecode(() => VersionHandler.DecodeVersion(inputStream))
            };
            var handler = GetTupByVersion(result.Version);
            TryDecode(() => handler.DecodeResponse(inputStream, result));
            input.MarkReaderIndex();
            return result;
        }

        public override object[] DecodeMethodParameters(RequestPacket input, RpcMethodMetadata metdata)
        {
            var handler = GetTupByVersion(input.Version);
            return TryDecode(() => handler.DecodeMethodParameters(input.Buffer, metdata));
        }

        public override Tuple<object, object[]> DecodeReturnValue(RequestPacket input, RpcMethodMetadata metdata)
        {
            var handler = GetTupByVersion(input.Version);
            return TryDecode(() => handler.DecodeReturnValue(input.Buffer, metdata));
        }

        public override void EncodeRequest(IByteBuffer output, RequestPacket request)
        {
            var handler = GetTupByVersion(request.Version);
            var outputStream = new TarsOutputStream(output);
            TryEncode(() => handler.EncodeRequest(outputStream, request));
        }

        public override void EncodeResponse(IByteBuffer output, RequestPacket response)
        {
            var handler = GetTupByVersion(response.Version);
            var outputStream = new TarsOutputStream(output);
            TryEncode(() => handler.EncodeResponse(outputStream, response));
        }

        public override byte[] EncodeMethodParameters(object[] parameters, RequestPacket request, RpcMethodMetadata metdata)
        {
            var handler = GetTupByVersion(request.Version);
            return TryEncode(() => handler.EncodeMethodParameters(parameters, metdata));
        }

        public override byte[] EncodeReturnValue(object returnValue, object[] parameters, RequestPacket response, RpcMethodMetadata metdata)
        {
            var handler = GetTupByVersion(response.Version);
            return TryEncode(() => handler.EncodeReturnValue(returnValue, parameters, metdata));
        }
    }
}