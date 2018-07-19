# Usage of Future/Promise in Tars framework

It will generate the file.h automatically for the file.tars when using tars2cpp tool to generate the C++ file.
There are four RPC methods for generating your custom interface in the .h file:

*   Synchronous(sync);
*   Asynchronous(async);
*   Future/Promise;
*   Coroutine(coro);

There is sync/async method in the document[samples for use](https://github.com/Tencent/Tars/blob/master/docs/tars_cpp_quickstart.md). 
It may be helpful to see this article for students who don't meet sync/async and then want to use Future/Promise under Tars.
The content and examples in the article are based on the Future/Promise provided under the Tars framework, which is not exactly
the same as the Future/Promise provided by boost, C++11, and other languages.

&nbsp;

## **What is Future/Promise?**

Future and Promise are actually two completely different things:

> Future: Which is used to represent and object that has not yet produced a result, and its behavior that produces this result is asynchronous.
> 
> Promise: The object of Future can be created by using the object of Promise(getFuture). The value which saved by object of Promise can be read 
by the object of Future, and this two objects sharing states are associated. It can be considered that Promise provides a means for the
synchronization of Future results.

In short: **They provide a set of non-blocking parallel operations. Ofcource, you can also block the operation to wait for the Future result to return**

&nbsp;

## **What scenairo do Future/Promise apply to?**

With a virtual example for explaining: **You will contact the intermediary through wechat to check the market and ask for som information, and finally get all the information summarized and then evaluate if you want to buy a house**

If we have intermediaries A, B, C without considering timeout.

**synchronous approach**: First we ask A with wechat, wait for A's reply, then ask B and wait for B's reply, finally ask C and wait for C's reply;

**asynchronous approach**: First we ask A with wechat, we can do other things(such as watching TV, processing work) while waiting for A to reply. Then we ask B, C after A to reply.

**Future/Promise approach**: We ask A, B, C with wechat at the same time, doing other things while getting all the responses.

**According to experience, Future/Promise is the most appropriate method in this scenario.**

For this scenairo, the interrogation agents A, B, C are three tasks without any coupling(a simple understanding is that the order can be disrupted, and there is no dependency between them, A-&gt;B-&gt;C,C-&gt;B-&gt;A is the same result), so the idea of using Future/Promise is most suitable.

&nbsp;

## **The code example of Future/Promise**

Suppose we have a Test application whose TestServant service inside TestServer provides the interface "EchoTest" of the Echo service.

```cpp
//omit the corresponding header file
//callback function
void handleAll(const promise::Future<promise::Tuple<promise::Future<TestServantPrxCallbackPromise::PromisetestPtr>, 
                    promise::Future<TestServantPrxCallbackPromise::PromisetestPtr> > > &result)
{
    promise::Tuple<promise::Future<TestServantPrxCallbackPromise::PromisetestPtr>, promise::Future<TestServantPrxCallbackPromise::PromisetestPtr> > out = result.get();
 
    promise::Future<TestServantPrxCallbackPromise::PromisetestPtr> out1 = out.get<0>();
    const tars::TC_AutoPtr<TestServantPrxCallbackPromise::Promisetest> print1 = out1.get();
 
    promise::Future<TestServantPrxCallbackPromise::PromisetestPtr> out2 = out.get<1>();
    const tars::TC_AutoPtr<TestServantPrxCallbackPromise::Promisetest> print2 = out2.get();
 
    DEBUGLOG("handleAll:" << print1->_ret << "|" << print1->outStr << "|out2:" << print2->_ret << "|" << print2->outStr);
}
 
int main()
{
    map<string, string> ctx;
    TestServantPrx testPrx = Application::getCommunicator()->stringToProxy<TestServantPrx>("Test.TestServer.TestServant");
 
    promise::Future<TestServantPrxCallbackPromise::PromisetestPtr > result1 = testPrx->promise_async_EchoTest("manmanlu1", ctx);
    promise::Future<TestServantPrxCallbackPromise::PromisetestPtr > result2 = testPrx->promise_async_EchoTest("manmanlu2", ctx);
 
    promise::Future<promise::Tuple<promise::Future<TestServantPrxCallbackPromise::PromisetestPtr>, promise::Future<TestServantPrxCallbackPromise::PromisetestPtr> > > r =
        promise::whenAll(result1, result2);
    r.then(tars::TC_Bind(&handleAll));
 
    DEBUGLOG("Test Future-Promise done");
}
```
The output is:
```
Test Future-Promise done
handleAll:0|manmanlu1|out2:0|manmanlu2
```

You can see that the asynchronous callback is triggered normally.
