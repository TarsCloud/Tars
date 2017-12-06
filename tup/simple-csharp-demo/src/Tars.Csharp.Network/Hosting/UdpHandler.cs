using DotNetty.Buffers;
using DotNetty.Common.Utilities;
using DotNetty.Transport.Channels;
using DotNetty.Transport.Channels.Sockets;
using System.Net;
using System.Threading.Tasks;

namespace Tars.Csharp.Network.Hosting
{
    public class UdpHandler : Handler
    {
        private EndPoint endPoint;
        private EndPoint sender;

        public override void ChannelRead(IChannelHandlerContext ctx, object msg)
        {
            if (msg is DatagramPacket packet)
            {
                endPoint = packet.Sender;
                sender = packet.Recipient;
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

        public override Task WriteAsync(IChannelHandlerContext context, object message)
        {
            return base.WriteAsync(context, new DatagramPacket(message as IByteBuffer, sender, endPoint));
        }
    }
}