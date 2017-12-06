using System;

namespace Tars.Csharp.Codecs
{
    public class TarsDecodeException : Exception
    {
        public TarsDecodeException(string message) : base(message)
        {
        }
    }
}