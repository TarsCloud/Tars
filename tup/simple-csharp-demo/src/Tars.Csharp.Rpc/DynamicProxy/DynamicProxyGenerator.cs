using AspectCore.Extensions.Reflection;
using AspectCore.Extensions.Reflection.Emit;
using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;

namespace Tars.Csharp.Rpc.DynamicProxy
{
    public class DynamicProxyGenerator : IDynamicProxyGenerator
    {
        private const string ProxyNameSpace = "Tars.Csharp.Rpc.DynamicProxy";
        private const string ProxyAssemblyName = "Tars.Csharp.Rpc.DynamicProxy.Core";
        private const string ContextName = "Context";
        private readonly ModuleBuilder moduleBuilder;
        private readonly ConcurrentDictionary<string, Type> definedTypes = new ConcurrentDictionary<string, Type>();
        private static readonly ConstructorInfo ObjectCtor = typeof(object).GetTypeInfo().DeclaredConstructors.Single();
        private const MethodAttributes InterfaceMethodAttributes = MethodAttributes.Public | MethodAttributes.Final | MethodAttributes.HideBySig | MethodAttributes.NewSlot | MethodAttributes.Virtual;
        private static readonly MethodInfo InvokeProxy = ReflectionUtils.GetMethod<DynamicProxyContext>(nameof(DynamicProxyContext.InvokeProxy));

        public DynamicProxyGenerator()
        {
            var asmBuilder = AssemblyBuilder.DefineDynamicAssembly(new AssemblyName(ProxyAssemblyName), AssemblyBuilderAccess.RunAndCollect);
            moduleBuilder = asmBuilder.DefineDynamicModule("Impl");
        }

        public T CreateInterfaceProxy<T>(DynamicProxyContext context)
        {
            var proxyType = GetOrCreateInterfaceProxyType(typeof(T));
            return (T)Activator.CreateInstance(proxyType, context);
        }

        public Type GetOrCreateInterfaceProxyType(Type type)
        {
            var name = GetProxyImplTypeFullName(type);
            return definedTypes.GetOrAdd(name, k => CreateProxyInternal(name, type));
        }

        private Type CreateProxyInternal(string name, Type type)
        {
            var interfaces = new Type[] { type };
            var typeBuilder = moduleBuilder.DefineType(name, TypeAttributes.Class | TypeAttributes.Public | TypeAttributes.Sealed, typeof(object), interfaces);
            var fieldBuilder = typeBuilder.DefineField(ContextName, typeof(DynamicProxyContext), FieldAttributes.Public);
            DefineInterfaceProxyConstructor(typeBuilder, fieldBuilder);
            DefineInterfaceProxyMethods(type, typeBuilder, fieldBuilder);
            return typeBuilder.CreateTypeInfo().AsType();
        }

        private void DefineInterfaceProxyMethods(Type type, TypeBuilder typeBuilder, FieldBuilder fieldBuilder)
        {
            foreach (var method in type.GetTypeInfo().DeclaredMethods.Where(x => !x.IsPropertyBinding()))
            {
                var methodBuilder = DefineMethod(method, method.Name, InterfaceMethodAttributes, type, typeBuilder, fieldBuilder);
                typeBuilder.DefineMethodOverride(methodBuilder, method);
            }
        }

        private MethodBuilder DefineMethod(MethodInfo method, string name, MethodAttributes attributes, Type type, TypeBuilder typeBuilder, FieldBuilder fieldBuilder)
        {
            var methodBuilder = typeBuilder.DefineMethod(name, attributes, method.CallingConvention, method.ReturnType, method.GetParameterTypes());
            DefineParameters(method, methodBuilder);

            EmitProxyMethodBody();
            return methodBuilder;

            void EmitProxyMethodBody()
            {
                var ilGen = methodBuilder.GetILGenerator();
                var activatorContext = ilGen.DeclareLocal(typeof(DynamicProxyContext));
                ilGen.EmitThis();
                ilGen.Emit(OpCodes.Ldfld, fieldBuilder);
                ilGen.Emit(OpCodes.Dup);
                ilGen.EmitString(method.GetReflector().DisplayName);
                var parameterTypes = method.GetParameterTypes();
                if (parameterTypes.Length == 0)
                {
                    ilGen.Emit(OpCodes.Ldnull);
                    return;
                }
                ilGen.EmitInt(parameterTypes.Length);
                ilGen.Emit(OpCodes.Newarr, typeof(object));
                var hasRef = false;
                for (var i = 0; i < parameterTypes.Length; i++)
                {
                    ilGen.Emit(OpCodes.Dup);
                    ilGen.EmitInt(i);
                    ilGen.EmitLoadArg(i + 1);
                    if (parameterTypes[i].IsByRef)
                    {
                        hasRef = true;
                        ilGen.EmitLdRef(parameterTypes[i]);
                        ilGen.EmitConvertToObject(parameterTypes[i].GetElementType());
                    }
                    else
                    {
                        ilGen.EmitConvertToObject(parameterTypes[i]);
                    }
                    ilGen.Emit(OpCodes.Stelem_Ref);
                }
                LocalBuilder parameters = null;
                if (hasRef)
                {
                    parameters = ilGen.DeclareLocal(typeof(object[]));
                    ilGen.Emit(OpCodes.Dup);
                    ilGen.Emit(OpCodes.Stloc, parameters);
                }
                ilGen.Emit(OpCodes.Callvirt, InvokeProxy);
                if (hasRef)
                {
                    for (var i = 0; i < parameterTypes.Length; i++)
                    {
                        if (parameterTypes[i].IsByRef)
                        {
                            ilGen.EmitLoadArg(i + 1);
                            ilGen.Emit(OpCodes.Ldloc, parameters);
                            ilGen.EmitInt(i);
                            ilGen.Emit(OpCodes.Ldelem_Ref);
                            ilGen.EmitConvertFromObject(parameterTypes[i].GetElementType());
                            ilGen.EmitStRef(parameterTypes[i]);
                        }
                    }
                }
                if (method.ReturnType == typeof(void))
                {
                    ilGen.Emit(OpCodes.Pop);
                }
                ilGen.Emit(OpCodes.Ret);
            }
        }

        private void DefineParameters(MethodInfo targetMethod, MethodBuilder methodBuilder)
        {
            var parameters = targetMethod.GetParameters();
            if (parameters.Length > 0)
            {
                var paramOffset = 1;   // 1
                for (var i = 0; i < parameters.Length; i++)
                {
                    var parameter = parameters[i];
                    var parameterBuilder = methodBuilder.DefineParameter(i + paramOffset, parameter.Attributes, parameter.Name);
                    if (parameter.HasDefaultValue)
                    {
                        if (!(parameter.ParameterType.GetTypeInfo().IsValueType && parameter.DefaultValue == null))
                            parameterBuilder.SetConstant(parameter.DefaultValue);
                    }
                }
            }

            var returnParamter = targetMethod.ReturnParameter;
            var returnParameterBuilder = methodBuilder.DefineParameter(0, returnParamter.Attributes, returnParamter.Name);
        }

        private void DefineInterfaceProxyConstructor(TypeBuilder typeBuilder, FieldBuilder fieldBuilder)
        {
            var constructorBuilder = typeBuilder.DefineConstructor(MethodAttributes.Public, ObjectCtor.CallingConvention, new Type[] { typeof(DynamicProxyContext) });
            constructorBuilder.DefineParameter(1, ParameterAttributes.None, "context");
            var ilGen = constructorBuilder.GetILGenerator();

            ilGen.EmitThis();
            ilGen.Emit(OpCodes.Call, ObjectCtor);

            ilGen.EmitThis();
            ilGen.EmitLoadArg(1);
            ilGen.Emit(OpCodes.Stfld, fieldBuilder);

            ilGen.Emit(OpCodes.Ret);
        }

        private string GetProxyImplTypeFullName(Type interfaceType)
        {
            return $"{ProxyNameSpace}.{interfaceType.Name}Impl";
        }
    }
}