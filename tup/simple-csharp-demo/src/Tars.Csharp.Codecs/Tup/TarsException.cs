using System;

namespace Tars.Csharp.Codecs.Tup
{
    public class TarsException : Exception
    {
        public int Ret { get; set; }

        public TarsException(int ret, Exception ex) : base(ex.Message, ex)
        {
        }

        public TarsException(int ret, string message) : base(message)
        {
        }
    }
}