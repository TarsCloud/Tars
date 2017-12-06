using System;

namespace Tars.Csharp.Rpc
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true, Inherited = true)]
    public class TarsOnewayAttribute : Attribute
    {
    }
}