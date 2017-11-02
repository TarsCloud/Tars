# Monitor

`Monitor` 是 `TARS（TUP）` 服务与特性监控上报模块。

它由 2 个子模块构成：

* 服务监控（stat）
* 特性监控（property）

## 安装
`npm install @tars/monitor`

## 初始化

__如果服务通过 [node-agent](https://github.com/tars-node/node-agent "node-agent") （或在TARS平台）运行，则无需执行该方法。__

初始化是通过调用特定模块的 `init(data)` 方法实现。

__data__: 可以为 tars配置文件路径 或 已配置的 `(@tars/utils).Config` 实例。

## 服务监控（stat）

```js
var stat = require('@tars/monitor').stat;
```

服务监控主要统计（上报）每个请求的 `成功、失败、超时的调用量`，并当调用成功时额外搜集 `调用耗时`。

因为其他模块已经集成了本模块， __所以一般情况下，服务脚本无需显式使用此模块。__  

已集成的模块如下：  

* TUP Client & Server 由 [@tars/rpc](https://github.com/tars-node/rpc "@tars/rpc") 进行上报。  
* HTTP(S) Server 由 [node-agent](https://github.com/tars-node/node-agent "node-agent") 进行上报	，但由于 `HTTP(S)` 协议的特性所以：  
	* 不统计超时的调用量，所有请求的超时上报为 0。
	* [response.statusCode >= 400](http://www.nodejs.org/api/http.html#http_response_statuscode "http_response_statuscode") 为失败调用，否则为成功调用。

如您确定要手动进行上报，可通过如下代码进行：

``` js
stat.report(headers, type[, timeout]);
```

__headers__:  
* __masterName__: 主调模块名  
* __slaveName__: 被调模块名  
* __interfaceName__: 被调模块接口名  
* __masterIp__: 主调IP  
* __slaveIp__: 被调IP  
* __slavePort__: 被调端口  
* __bFromClient__: 客户端上报为 `true`，服务端上报为 `false`  
* __returnValue__: 返回值， *默认值为 0*  

如果被调为 `set` 则 `headers` 还需包含如下信息：  
* __slaveSetName__: 被调 `set` 名  
* __slaveSetArea__: 被调 `set` 地区名  
* __slaveSetID__: 被调 `set` 组名  

如果主调为 `set` 则 `headers` 还需包含如下信息：  
* __masterSetInfo__: 主调 `set` 信息（由 setName.setArea.setID 构成）  


参数 `type` 的取值为 `stat.TYPE` 中的一种，如下所示：

__stat.TYPE__:  
* __SUCCESS__: 成功  
* __ERROR__: 失败  
* __TIMEOUT__: 超时  

如果 `type === stat.TYPE.SUCCESS` 必须上报响应耗时 `timeout` _（整型）_

数据上报后，用户可在服务监控选项卡中查看上报的数据。

## 特性监控（property）

```js
var property = require('@tars/monitor').property;
```

特性监控上报的是服务脚本的 `自定义特性`， 它由特性名、特性值、以及统计方法构成 _（key/value pairs）_。

### property.create(name, policies)

调用 `create` 方法，会返回（或创建）一个上报对象，可通过调用返回对象的 `report(value)` 方法进行上报。

其中 `name` 为上报的特性值名，而 `policies` 是统计方法类的实例数组（指定了数据的统计方法）。

```js
property.create('name', [new property.POLICY.Count, new property.POLICY.Max]);
```

_`policies` 数组中的实例对象不能有重复的统计方法。_

__请注意：不要每次上报都调用 `create` 方法获取上报对象，这样会造成性能损耗。__

### obj.report(value)

`property.create` 会返回一个上报对象，可通过调用对象的 `report` 的方法进行上报。

每次调用 `report` 只能上报一次的数据，并且 `value` 在一般情况下必须为数值。

数据上报后，用户可在特性监控中查看上报的数据。

## 统计方法

`特性监控` 所上报的数据（也就是在调用 `create` 时）需要指定一种或者几种统计方法，以便统计在一段时间内的值，这些方法都在 `POLICY` 中定义，它们分别是：

* POLICY.Max：统计最大值
* POLICY.Min：统计最小值
* POLICY.Count：统计一共有多少个数据
* POLICY.Sum：将所有数据进行相加
* POLICY.Avg：计算数据的平均值
* POLICY.Distr：分区间统计

__除了 `property.POLICY.Distr` 其它均不需要传递构造参数__

### property.POLICY.Distr(ranges)

`Distr` 为分区间统计，事先划分区间，在上报时会自动统计落入这个区间的 `value` 数量。

同时，在进行数据展示时，分区间统计展示成为 _饼图类型_。

其中，参数 `ranges` 为数组，其中的每一项为一个数值（Int），并以从小到大的顺序排列。

例如：

``` js
var monitor = property.create('name', [new property.POLICY.Distr([0, 10, 100, 1000])]);
monitor.report(2);
monitor.repott(20);
monitor.report(200);
```

上面的例子统计的统计结果为：
  
> [0 - 10] = 1  
> (10 - 100] = 1  
> (100 - 1000] = 1

_每个区间都包含右侧不包含左侧（除了第一个区间）_

## 上报间隔

在监控上报中，并非每次调用 `report` 方法均会上报数据，模块会搜集一定时间内提交的数据，并进行整合统计后一次性上报（单向调用）。

模块会自动读取TARS配置文件中 `tars.application.client.report-interval` 配置节（单位为 ms）用以配置上报间隔。

__请注意：配置的上报间隔不可低于 10s，亦不可高于 `TARS主控刷新时间`（也就是 `tars.application.client.refresh-endpoint-interval` 配置节）。__

为了防止循环调用，上报模块本身的调用情况由被调方上报（也就是单向调用的上报逻辑）。