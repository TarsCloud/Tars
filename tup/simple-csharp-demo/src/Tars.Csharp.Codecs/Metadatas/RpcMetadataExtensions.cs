using AspectCore.Extensions.Reflection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using Tars.Csharp.Codecs;
using Tars.Csharp.Codecs.Attributes;

namespace Tars.Csharp.Rpc
{
    public static class RpcMetadataExtensions
    {
        private static readonly MethodInfo FromResult = typeof(Task).GetTypeInfo().GetMethod(nameof(Task.FromResult));
        private static readonly MethodInfo _InvokeTask = typeof(RpcMetadataExtensions).GetTypeInfo().GetMethod(nameof(RpcMetadataExtensions.InvokeTask));
        private static readonly MethodInfo _InvokeTaskResult = typeof(RpcMetadataExtensions).GetTypeInfo().GetMethod(nameof(RpcMetadataExtensions.InvokeTaskResult));
        private static readonly MethodInfo _InvokeValueTask = typeof(RpcMetadataExtensions).GetTypeInfo().GetMethod(nameof(RpcMetadataExtensions.InvokeValueTask));

        public static IDictionary<string, RpcMetadata> ScanRpcMetadatas(this Assembly[] assembly, bool isGenerateReflector, Func<Type, string> getKey, Func<Type, bool> predicate)
        {
            var metadatas = new Dictionary<string, RpcMetadata>();

            foreach (var item in assembly.SelectMany(i => i.GetExportedTypes().Where(predicate)).Distinct())
            {
                var reflector = item.GetReflector();
                if (!reflector.IsDefined<CodecAttribute>())
                {
                    throw new Exception($"{item.FullName} no CodecAttribute.");
                }

                var metadata = new RpcMetadata()
                {
                    InterfaceType = item,
                    Servant = getKey(item),
                    CodecType = reflector.GetCustomAttribute<CodecAttribute>().GetType(),
                    Methods = new Dictionary<string, RpcMethodMetadata>(StringComparer.OrdinalIgnoreCase)
                };
                SetMethodMetaDatas(metadata, isGenerateReflector);
                metadatas.Add(metadata.Servant, metadata);
            }

            return metadatas;
        }

        private static void SetMethodMetaDatas(RpcMetadata metadata, bool isGenerateReflector)
        {
            foreach (var method in metadata.InterfaceType.GetMethods())
            {
                var returnInfo = method.ReturnType.GetTypeInfo();
                var m = new RpcMethodMetadata()
                {
                    DisplayName = method.GetReflector().DisplayName,
                    Name = HandleFuncName(method.Name),
                    ReturnInfo = method.ReturnParameter,
                    Parameters = method.GetParameters(),
                    MethodInfo = method,
                    Reflector = isGenerateReflector ? method.GetReflector() : null,
                    InvokeStatus = GetMethodInvokeStatus(method, returnInfo),
                    IsValueTask = returnInfo.IsValueTask(),
                    IsTaskWithResult = returnInfo.IsTaskWithResult(),
                    IsTask = returnInfo.IsTask(),
                };
                var testName = method.GetReflector().DisplayName;
                SetConvertReturnType(returnInfo, m);
                metadata.Methods.Add(m.DisplayName, m);
            }
        }

        private static string HandleFuncName(string name)
        {
            var funcName = name.ToLower();
            return funcName.EndsWith("async") ? funcName.Replace("async", "") : funcName;
        }

        private static void SetConvertReturnType(TypeInfo returnInfo, RpcMethodMetadata metadata)
        {
            if (returnInfo.IsTaskWithResult())
            {
                var returnType = returnInfo.GetGenericArguments().Single();
                var invoke = _InvokeTaskResult.MakeGenericMethod(returnType).GetReflector();
                metadata.Invoke = invoke;
                metadata.RealReturnType = returnType;
            }
            else if (returnInfo.IsValueTask())
            {
                var returnType = returnInfo.GetGenericArguments().Single();
                var valueTaskType = typeof(ValueTask<>).MakeGenericType(returnType).GetTypeInfo();
                var invoke = _InvokeValueTask.MakeGenericMethod(returnType).GetReflector();
                metadata.Invoke = invoke;
                metadata.RealReturnType = returnType;
            }
            else if (returnInfo.IsTask())
            {
                var invoke = _InvokeTask.GetReflector();
                metadata.Invoke = invoke;
                metadata.RealReturnType = typeof(void);
            }
            else
            {
                metadata.RealReturnType = returnInfo.AsType();
            }
        }

        public static TResult InvokeTaskResult<TResult>(object obj)
        {
            var task = (Task<TResult>)obj;
            if (!task.IsCompleted)
            {
                task.GetAwaiter().GetResult();
            }
            return task.Result;
        }

        public static TResult InvokeValueTask<TResult>(object obj)
        {
            var task = (ValueTask<TResult>)obj;
            if (!task.IsCompleted)
            {
                task.GetAwaiter().GetResult();
            }
            return task.Result;
        }

        public static object InvokeTask(object obj)
        {
            var task = (Task)obj;
            if (!task.IsCompleted)
            {
                task.GetAwaiter().GetResult();
            }
            return null;
        }

        private static InvokeStatus GetMethodInvokeStatus(MethodInfo method, TypeInfo returnInfo)
        {
            var reflector = method.GetReflector();
            if (reflector.IsDefined<TarsOnewayAttribute>())
            {
                return InvokeStatus.Oneway;
            }
            else if (returnInfo.IsTask() || returnInfo.IsTaskWithResult() || returnInfo.IsValueTask())
            {
                return InvokeStatus.AsyncCall;
            }
            else
            {
                return InvokeStatus.SyncCall;
            }
        }
    }
}