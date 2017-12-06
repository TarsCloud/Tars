using DotNetty.Common.Utilities;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Channels.Sockets;

namespace Tars.Csharp.Network.Hosting
{
    public class ClientUdpHandler : Handler
    {
        public override void ChannelRead(IChannelHandlerContext ctx, object msg)
        {
            if (msg is DatagramPacket packet)
            {
                ctx.FireChannelRead(packet.Content);
            }
            else
            {
                ReferenceCountUtil.Release(msg);
            }
        }

        public override void Flush(IChannelHandlerContext context)
        {
            context.Flush();
        }
    }
}