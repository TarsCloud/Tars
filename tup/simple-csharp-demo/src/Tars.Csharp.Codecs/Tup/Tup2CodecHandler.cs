using DotNetty.Buffers;
using System;
using System.Collections.Generic;
using System.Linq;
using Tars.Csharp.Codecs.Util;

namespace Tars.Csharp.Codecs.Tup
{
    public class Tup2CodecHandler : Tup1CodecHandler
    {
        public override short Version => Const.Version2;

        public override void DecodeResponse(TarsInputStream input, RequestPacket response)
        {
            response.PacketType = input.Read((byte)0, 2, true);
            response.MessageType = input.Read(0, 3, true);
            response.RequestId = input.Read(0, 4, true);
            response.ServantName = input.ReadString(5, true);
            response.FuncName = input.ReadString(6, true);
            response.Buffer = input.Read(new byte[0], 7, true);
            response.Timeout = input.Read(0, 8, true);
            response.Status = input.ReadMap<string, string>(new Dictionary<string, string>(), 9, false);
            response.Context = input.ReadMap<string, string>(new Dictionary<string, string>(), 10, false);
            response.GetRetToStatus();
        }

        public override object[] DecodeMethodParameters(byte[] body, RpcMethodMetadata metdata)
        {
            if (metdata.Parameters.Length == 0) return new object[0];
            var buf = Unpooled.WrappedBuffer(body);
            var stream = new TarsInputStream(buf);
            var unaIn = CreateUniAttribute();
            unaIn.DecodeTup2(stream);
            return metdata.Parameters
                .Select(i => unaIn.GetByClass(i.Name, BasicClassTypeUtil.CreateObject(i.ParameterType),
                    stream))
                .ToArray();
        }

        public override Tuple<object, object[]> DecodeReturnValue(byte[] body, RpcMethodMetadata metdata)
        {
            var unaIn = CreateUniAttribute();
            var buf = Unpooled.WrappedBuffer(body);
            var input = new TarsInputStream(buf);
            object result = null;
            if (metdata.RealReturnType != typeof(void))
            {
                result = unaIn.GetByClass(string.Empty, BasicClassTypeUtil.CreateObject(metdata.RealReturnType), input);
            }
            var ps = metdata.Parameters
                .Select(i => unaIn.GetByClass(i.Name, BasicClassTypeUtil.CreateObject(i.ParameterType),
                    input))
                .ToArray();
            return Tuple.Create(result, ps);
        }

        public override byte[] EncodeMethodParameters(object[] parameters, RpcMethodMetadata metdata)
        {
            if (metdata.Parameters.Length == 0) return new byte[0];
            var unaOut = CreateUniAttribute();
            foreach (var item in metdata.Parameters.Where(i => i.ParameterType.IsByRef))
            {
                unaOut.Put(item.Name, parameters[item.Position]);
            }
            var buf = Unpooled.Buffer(128);
            var output = new TarsOutputStream(buf);
            unaOut.WriteTo(output);
            return output.ToByteArray();
        }

        public override byte[] EncodeReturnValue(object returnValue, object[] parameters, RpcMethodMetadata metdata)
        {
            var unaOut = CreateUniAttribute();
            if (returnValue != null && metdata.RealReturnType != typeof(void))
            {
                unaOut.Put(string.Empty, returnValue);
            }
            foreach (var item in metdata.Parameters.Where(i => i.ParameterType.IsByRef))
            {
                unaOut.Put(item.Name, parameters[item.Position]);
            }
            var buf = Unpooled.Buffer(128);
            var output = new TarsOutputStream(buf);
            unaOut.WriteTo(output);
            return output.ToByteArray();
        }

        public override void EncodeResponse(TarsOutputStream output, RequestPacket response)
        {
            response.SetRetToStatus();
            output.Write(response.Version, 1);
            output.Write(response.PacketType, 2);
            output.Write(response.MessageType, 3);
            output.Write(response.RequestId, 4);
            output.Write(response.ServantName, 5);
            output.Write(response.FuncName, 5);
            output.Write(response.Buffer, 7);
            output.Write(response.Timeout, 8);
            output.Write(response.Status, 9);
            output.Write(response.Context, 10);
        }

        protected virtual UniAttribute CreateUniAttribute()
        {
            return new UniAttribute
            {
                Version = Version
            };
        }
    }
}