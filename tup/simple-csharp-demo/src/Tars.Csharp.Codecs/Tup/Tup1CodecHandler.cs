using DotNetty.Buffers;
using System;
using System.Collections.Generic;
using System.Linq;
using Tars.Csharp.Codecs.Util;

namespace Tars.Csharp.Codecs.Tup
{
    public class Tup1CodecHandler : ITupCodecHandler
    {
        public virtual short Version => Const.Version;

        public virtual void DecodeRequest(TarsInputStream input, RequestPacket request)
        {
            request.PacketType = input.Read(request.PacketType, 2, true);
            request.MessageType = input.Read(request.MessageType, 3, true);
            request.RequestId = input.Read(request.RequestId, 4, true);
            request.ServantName = input.ReadString(5, true);
            request.FuncName = input.ReadString(6, true);
            request.Buffer = input.ReadBytes(7, true);
            request.Timeout = input.Read(request.Timeout, 8, true);
            request.Context = input.ReadMap<string, string>(new Dictionary<string, string>(), 9, false);
            request.Status = input.ReadMap<string, string>(new Dictionary<string, string>(), 10, false);
        }

        public virtual void DecodeResponse(TarsInputStream input, RequestPacket response)
        {
            response.PacketType = input.Read((byte)0, 2, true);
            response.RequestId = input.Read(0, 3, true);
            response.MessageType = input.Read(0, 4, true);
            response.Ret = input.Read(0, 5, true);
            response.Buffer = input.Read(new byte[0], 6, true);
            response.Status = input.ReadMap<string, string>(new Dictionary<string, string>(), 7, false);
            response.ResultDesc = input.ReadString(8, false);
            response.Context = input.ReadMap<string, string>(new Dictionary<string, string>(), 9, false);
        }

        public virtual object[] DecodeMethodParameters(byte[] body, RpcMethodMetadata metdata)
        {
            if (metdata.Parameters.Length == 0) return new object[0];

            var buf = Unpooled.WrappedBuffer(body);
            var stream = new TarsInputStream(buf);
            return metdata.Parameters
                .Select(i => stream.Read(BasicClassTypeUtil.CreateObject(i.ParameterType), i.Position + 1, false))
                .ToArray();
        }

        public virtual Tuple<object, object[]> DecodeReturnValue(byte[] body, RpcMethodMetadata metdata)
        {
            var buf = Unpooled.WrappedBuffer(body);
            var stream = new TarsInputStream(buf);
            object result = metdata.RealReturnType == typeof(void) ? null
                : stream.Read(BasicClassTypeUtil.CreateObject(metdata.RealReturnType), metdata.ReturnInfo.Position + 1, true);
            var ps = metdata.Parameters
                .Where(i => i.ParameterType.IsByRef)
                .Select(i => stream.Read(BasicClassTypeUtil.CreateObject(i.ParameterType), i.Position + 1, false))
                .ToArray();
            return Tuple.Create(result, ps);
        }

        public virtual short DecodeVersion(TarsInputStream input)
        {
            return input.Read((short)0, 1, true);
        }

        public virtual void EncodeRequest(TarsOutputStream output, RequestPacket request)
        {
            output.Write(request.Version, 1);
            output.Write(request.PacketType, 2);
            output.Write(request.MessageType, 3);
            output.Write(request.RequestId, 4);
            output.Write(request.ServantName, 5);
            output.Write(request.FuncName, 6);
            output.Write(request.Buffer, 7);
            output.Write(request.Timeout, 8);
            output.Write(request.Context, 9);
            output.Write(request.Status, 10);
        }

        public virtual void EncodeResponse(TarsOutputStream output, RequestPacket response)
        {
            output.Write(response.Version, 1);
            output.Write(response.PacketType, 2);
            output.Write(response.RequestId, 3);
            output.Write(response.MessageType, 4);
            output.Write(response.Ret, 5);
            output.Write(response.Buffer, 6);
            if (response.Status != null)
            {
                output.Write(response.Status, 7);
            }
            if (response.Ret != Const.ServerSuccess)
            {
                output.Write(response.ResultDesc ?? "", 8);
            }
            if (response.Context != null)
            {
                output.Write(response.Context, 9);
            }
        }

        public virtual byte[] EncodeMethodParameters(object[] parameters, RpcMethodMetadata metdata)
        {
            if (metdata.Parameters.Length == 0) return new byte[0];
            var buf = Unpooled.Buffer(128);
            var output = new TarsOutputStream(buf);
            foreach (var item in metdata.Parameters)
            {
                output.Write(parameters[item.Position], item.Position + 1);
            }
            return output.ToByteArray();
        }

        public virtual byte[] EncodeReturnValue(object returnValue, object[] parameters, RpcMethodMetadata metdata)
        {
            var buf = Unpooled.Buffer(128);
            var output = new TarsOutputStream(buf);
            if (metdata.RealReturnType != typeof(void))
            {
                output.Write(returnValue, 0);
            }
            if (parameters != null)
            {
                foreach (var item in metdata.Parameters.Where(i => i.ParameterType.IsByRef))
                {
                    output.Write(parameters[item.Position], item.Position + 1);
                }
            }
            return output.ToByteArray();
        }
    }
}