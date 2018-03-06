# Tars框架Future/Promise使用

在采用tars2cpp工具自动生成c++文件时，相应的file.tars会自动生成file.h文件。在.h文件里会生成你自定义接口的RPC方法，一共有四种：

*   同步sync方法；
*   异步async方法；
*   Future/Promise方法；
*   协程coco方法；

sync/async方法在文档里都有[使用的样例](https://github.com/Tencent/Tars/blob/master/docs/tars_cpp_quickstart.md)，对于不满足sync/async，然后想在Tars下使用Future/Promise的同学看看此文或许会有帮助。

文章内容、样例都是基于Tars框架下提供的Future/Promise进行分析，与boost、C++11、以及其他语言提供的Future/Promise不完全相同。

&nbsp;

## **Future/Promise是什么？**

Future与Promise其实是二个完全不同的东西：

> Future：用来表示一个尚未有结果的对象，而产生这个结果的行为是异步操作；
> 
> Promise：Future对象可以使用Promise对象来创建（getFuture），创建后，Promise对象保存的值可以被Future对象读取，同时将二个对象共享状态关联起来。可以认Promise为Future结果同步提供了一种手段；

简而言之就是：**他们提供了一套非阻塞并行操作的处理方案，当然，你也可以阻塞操作来等待Future的结果返回。**

&nbsp;

## **Future/Promise适用什么场景？**

通过一个虚拟的例子来说明：**你想买房，然后通过微信联系中介看看行情并询问一些信息，最后拿到所有的信息汇总后再评估。**

我们假如有中介A、B、C，并且不考虑超时情况。

**同步的做法**：我们先微信询问A，等待A的回复，接着询问B，等待B的回复，最后询问C，等待C的回复；

**异步的做法**：我们先微信询问A，在等待A回复的同时，可以干干其他事情（比如看电视，处理工作），等到A回复后再依次询问B，C；

**Future/Promise的做法**同时给A、B、C发消息询问，等待回复的同时干其他事情，一直到所有回复都响应；

**根据经验，在这种场景下Future/Promise才是最合适的做法。**

因为对于这种场景，询问中介A、B、C是三个没有任何耦合的任务（简单理解就是顺序可以打乱的任务，相互之间无依赖，A-&gt;B-&gt;C，C-&gt;B-&gt;A的结果一样），所使用Future/Promise的思想最适合。

&nbsp;

## **Future/Promise代码例子**

假设我们有一Test应用，他的TestServer内部TestServant提供了Echo服务的接口“EchoTest”。

```cpp
//省略了对应头文件
//回调函数
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
输出的结果为：
```
Test Future-Promise done
handleAll:0|manmanlu1|out2:0|manmanlu2
```

可以看到异步回调正常触发。