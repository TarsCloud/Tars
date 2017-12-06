namespace Tars.Csharp.Rpc.DynamicProxy
{
    public interface IDynamicProxyGenerator
    {
        T CreateInterfaceProxy<T>(DynamicProxyContext context);
    }
}