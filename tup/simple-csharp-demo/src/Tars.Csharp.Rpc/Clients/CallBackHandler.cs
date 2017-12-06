using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;
using Tars.Csharp.Codecs.Tup;

namespace Tars.Csharp.Rpc.Clients
{
    public class CallBackHandler<T> : ICallBackHandler<T>
    {
        private ConcurrentDictionary<T, TaskCompletionSource<RequestPacket>> results = new ConcurrentDictionary<T, TaskCompletionSource<RequestPacket>>();
        private int requestId = 0;

        public Task<RequestPacket> AddCallBack(T key, int timeout)
        {
            var source = new TaskCompletionSource<RequestPacket>();
            var tokenSource = new CancellationTokenSource();
            tokenSource.Token.Register(() => source.TrySetCanceled(tokenSource.Token));
            results.AddOrUpdate(key, source, (x, y) => source);
            tokenSource.CancelAfter(timeout);
            return source.Task;
        }

        public int NewRequestId()
        {
            return Interlocked.Increment(ref requestId);
        }

        public void SetResult(T key, RequestPacket result)
        {
            if (results.TryRemove(key, out TaskCompletionSource<RequestPacket> source))
            {
                source.SetResult(result);
            }
        }
    }
}