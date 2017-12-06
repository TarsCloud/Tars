using System;

namespace Tars.Csharp.Codecs
{
    public class TarsEncodeException : Exception
    {
        public TarsEncodeException(string message) : base(message)
        {
        }
    }
}