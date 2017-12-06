using DotNetty.Transport.Channels;
using System;

namespace Tars.Csharp.Network.Hosting
{
    public class Handler : ChannelHandlerAdapter
    {
        public override void ChannelReadComplete(IChannelHandlerContext context)
        {
            context.Flush();
        }

        public override void ExceptionCaught(IChannelHandlerContext context, Exception exception)
        {
            context.CloseAsync();
        }
    }
}