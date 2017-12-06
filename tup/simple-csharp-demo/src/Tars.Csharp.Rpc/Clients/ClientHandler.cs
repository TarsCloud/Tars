using DotNetty.Transport.Channels;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Rpc.Clients
{
    public class ClientHandler : SimpleChannelInboundHandler<RequestPacket>
    {
        public override bool IsSharable => true;

        private ICallBackHandler<int> callBackHandler;

        public ClientHandler(ICallBackHandler<int> callBackHandler)
        {
            this.callBackHandler = callBackHandler;
        }

        protected override void ChannelRead0(IChannelHandlerContext ctx, RequestPacket msg)
        {
            callBackHandler.SetResult(msg.RequestId, msg);
        }
    }
}