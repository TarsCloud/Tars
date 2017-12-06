using DotNetty.Buffers;
using DotNetty.Codecs;
using DotNetty.Transport.Channels;
using System.Collections.Generic;
using Tars.Csharp.Codecs.Attributes;

namespace Tars.Csharp.Codecs
{
    public class TarsRequestDecoder : ByteToMessageDecoder
    {
        private TarsCodecAttribute tars;

        public TarsRequestDecoder(TarsCodecAttribute tars)
        {
            this.tars = tars;
        }

        protected override void Decode(IChannelHandlerContext context, IByteBuffer input, List<object> output)
        {
            while (input.IsReadable())
            {
                output.Add(tars.DecodeRequest(input));
            }
        }
    }
}