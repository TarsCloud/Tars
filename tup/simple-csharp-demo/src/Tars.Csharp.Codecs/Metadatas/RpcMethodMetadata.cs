using AspectCore.Extensions.Reflection;
using System;
using System.Reflection;

namespace Tars.Csharp.Codecs
{
    public class RpcMethodMetadata
    {
        public string Name { get; set; }
        public ParameterInfo ReturnInfo { get; set; }
        public ParameterInfo[] Parameters { get; set; }
        public MethodInfo MethodInfo { get; set; }
        public MethodReflector Reflector { get; set; }
        public InvokeStatus InvokeStatus { get; set; }
        public bool IsValueTask { get; set; }
        public bool IsTaskWithResult { get; set; }
        public Type RealReturnType { get; set; }
        public bool IsTask { get; set; }
        public MethodReflector Invoke { get; set; }
        public string DisplayName { get; set; }
    }
}