using DotNetty.Codecs;
using DotNetty.Transport.Channels;
using System.Collections.Generic;
using Tars.Csharp.Codecs.Attributes;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Codecs
{
    public class TarsResponseEncoder : MessageToMessageEncoder<RequestPacket>
    {
        private TarsCodecAttribute tars;

        public TarsResponseEncoder(TarsCodecAttribute tars)
        {
            this.tars = tars;
        }

        protected override void Encode(IChannelHandlerContext context, RequestPacket message, List<object> output)
        {
            if (message == null || message.PacketType == Const.OneWay) return;
            var buf = context.Allocator.Buffer(128);
            buf.WriteInt(0);
            tars.EncodeResponse(buf, message);
            var length = buf.WriterIndex;
            buf.SetWriterIndex(0);
            buf.WriteInt(length);
            buf.SetWriterIndex(length);
            output.Add(buf);
        }
    }
}