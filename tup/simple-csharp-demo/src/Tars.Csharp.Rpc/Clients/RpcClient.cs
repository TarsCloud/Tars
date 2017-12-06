using DotNetty.Buffers;
using System.Linq;
using System.Threading.Tasks;
using Tars.Csharp.Codecs;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Codecs.Tup;
using Tars.Csharp.Network.Client;

namespace Tars.Csharp.Rpc.Clients
{
    public class RpcClient<T> : IRpcClient where T : INetworkClient
    {
        protected RpcClientMetadata metadatas;
        protected T client;
        private ICallBackHandler<int> callBackHandler;

        public RpcClient(RpcClientMetadata metadatas, T client, ICallBackHandler<int> callBackHandler)
        {
            this.metadatas = metadatas;
            this.client = client;
            this.callBackHandler = callBackHandler;
        }

        public object Inovke(RpcContext context, string methodName, object[] parameters)
        {
            if (!metadatas.TryGetValue(context.Servant, out RpcMetadata metadata)
                || !metadata.Methods.TryGetValue(methodName, out RpcMethodMetadata methodMetadata)) return null;

            var packet = context.CreatePacket();
            packet.RequestId = callBackHandler.NewRequestId();
            packet.FuncName = methodMetadata.Name;
            packet.Buffer = metadata.Codec.EncodeMethodParameters(parameters, packet, methodMetadata);
            var buf = Unpooled.Buffer(128);
            buf.WriteInt(0);
            metadata.Codec.EncodeRequest(buf, packet);
            var length = buf.WriterIndex;
            buf.SetWriterIndex(0);
            buf.WriteInt(length);
            buf.SetWriterIndex(length);
            var task = client.SendAsync(context.EndPoint, buf);
            return HandleResult(task, methodMetadata, packet.RequestId, context.Timeout, parameters, metadata.Codec);
        }

        private object HandleResult(Task task, RpcMethodMetadata methodMetadata, int requestId, int timeout, object[] parameters, CodecAttribute codec)
        {
            switch (methodMetadata.InvokeStatus)
            {
                case InvokeStatus.SyncCall:
                    return Callback(methodMetadata, requestId, timeout, parameters, codec).Result;

                case InvokeStatus.AsyncCall:
                    return Callback(methodMetadata, requestId, timeout, parameters, codec);

                case InvokeStatus.Oneway:
                default:
                    return null;
            }
        }

        private Task<object> Callback(RpcMethodMetadata methodMetadata, int requestId, int timeout, object[] parameters, CodecAttribute codec)
        {
            return callBackHandler.AddCallBack(requestId, timeout)
                .ContinueWith(t =>
                {
                    if (t.Result.Ret != Const.ServerSuccess)
                        throw new TarsException(t.Result.Ret, t.Result.ResultDesc);
                    var info = codec.DecodeReturnValue(t.Result, methodMetadata);
                    if (info.Item2 == null) return info.Item1;
                    var index = 0;
                    foreach (var item in methodMetadata.Parameters.Where(i => i.ParameterType.IsByRef))
                    {
                        if (index >= info.Item2.Length) break;
                        parameters[item.Position] = info.Item2[index++];
                    }
                    return info.Item1;
                });
        }

        public Task ShutdownAsync()
        {
            return client.ShutdownGracefullyAsync();
        }
    }
}