using AspectCore.Extensions.Reflection;
using Google.Protobuf;
using System;
using System.Linq;
using System.Threading.Tasks;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Codecs.Attributes
{
    public class ProtobufCodecAttribute : TarsCodecAttribute
    {
        private static readonly MethodReflector MergeFrom = typeof(IMessage).GetType().GetMethods()
            .First(i => i.Name == nameof(IMessage.MergeFrom)
            && i.GetParameters().Length == 1
            && i.GetParameters()[0].ParameterType == typeof(CodedInputStream)).GetReflector();

        public override string Key => "protobuff";

        public override byte[] EncodeReturnValue(object returnValue, object[] parameters, RequestPacket response, RpcMethodMetadata metdata)
        {
            return ((IMessage)returnValue).ToByteArray();
        }

        public override byte[] EncodeMethodParameters(object[] parameters, RequestPacket request, RpcMethodMetadata metdata)
        {
            return ((IMessage)parameters[0]).ToByteArray();
        }

        public override Tuple<object, object[]> DecodeReturnValue(RequestPacket input, RpcMethodMetadata metdata)
        {
            if (metdata.RealReturnType == typeof(void)) return Tuple.Create<object, object[]>(null, null);
            var ins = new CodedInputStream(input.Buffer);
            var result = Activator.CreateInstance(metdata.RealReturnType);
            MergeFrom.Invoke(result, ins);
            return Tuple.Create<object, object[]>(result, null);
        }

        public override object[] DecodeMethodParameters(RequestPacket input, RpcMethodMetadata metdata)
        {
            if (metdata.Parameters.Length == 0) return new object[0];
            var ins = new CodedInputStream(input.Buffer);
            var result = Activator.CreateInstance(metdata.Parameters[0].ParameterType);
            return new object[] { result };
        }
    }
}