# @tars/logs

基于 [winston](https://github.com/flatiron/winston) 与 [winston-tars](https://github.com/tars-node/winston-tars) 开发，符合 TARS 框架规范的日志组件，其中包含染色日志、滚动（大小、时间）日志。


## 安装
`npm install @tars/logs`


## 例子

输出滚动日志
``` js
var logger = new tarsLogs('TarsRotate');
```

输出名为 access 的按天日志
``` js
var logger = new tarsLogs('TarsDate', 'access');
```

输出名为 access 的按小时日志
``` js
var logger = new tarsLogs('TarsDate', 'access', {  
	format : tarsLogs.DateFormat.LogByHour
});
```

输出名为 access 的按 20分钟 滚动的日志，文件名为 2015-01-01_10:00 这样的格式
``` js
var logger = new tarsLogs('TarsDate', 'access', {
	format : new (tarsLogs.DateFormat.LogByMinute)(20, '%Y-%m-%d_%H:%M')
});
```

指定输出 `INFO` 级别的日志信息
``` js
logger.info('data');
logger.info('data1', 'data2', 'data3');
```

指定当前 `INFO` 级别的日志输出需要染色
``` js
logger.info('data', logger.getDyeingObj(true));
```

## 初始化

__如果服务在 `TARS平台` 上运行，则无需配置此项__

__如果服务在 `本地环境` 中调试，所有日志类型都将输出至 Console__

可通过调用 `tarsLogs.setConfig(data)` 静态方法进行初始化

`data(String|Object)` 可以为 tars配置文件路径 或 已配置的 `(@tars/utils).Config` 实例。

## 使用

### 实例化

```js
var tarsLogs = require('@tars/logs');

var logger = new tarsLogs(type, [name, options]);
```

`type(String)` 日志类型：

* __TarsRotate__: 按大小滚动日志  
* __TarsDate__: 按时间滚动日志   

`name(String)` 用户自定义的文件名（可不填）

`options(Object)` 根据不同的日志类型，存在不同的参数，但下面的参数是各类型共享的：
  
* __hasSufix__: 日志文件名是否带.log后缀， *默认值为 true*  
* __hasAppNamePrefix__: 是否允许框架在日志文件名上增加业务相关的标识， *默认值为 true*  
* __concatStr__: 日志文件名中字符间的连接符， *默认值为 _*  
* __separ__: 日志内容项之间的分隔符， *默认值为 |*  

`options` 中的其它参数，请详见不同日志类型的说明项。

__在正常情况下，相同一个日志文件应该共享相同的一个 `logger`，而不是多次进行实例化__

### 日志输出

在 `TARS` 中存在有 4 种日志级别 `INFO`、`DEBUG`、`WARN`、`ERROR` 可用对应的方法进行输出

``` js
logger.info([data], [...]);
logger.debug([data], [...]);
logger.warn([data], [...]);
logger.error([data], [...]);
```

方法支持多重参数， 详情可见 [util.format()](http://www.nodejs.org/api/util.html#util_util_format_format)

如需要染色，请详见 `染色` 节

### 文件名与行号

默认情况下，输出的日志中包含调用输出方法的代码所在的 `文件名:行号`。

可通过如下示例进行自定义（用于封装模块）或者关闭（以提高性能）。

关闭文件名与行号输出：

``` js
logger.info('data', {
	lineno : false
});
```

自定义文件名与行号输出：

``` js
logger.info('data', {
	lineno : 'app.js:123'
});
```

更多详细信息，请参考 [@tars/winston-tars.Metadata](http://https://github.com/tars-node/winston-tars#metadata)

### 日志级别

日志级别的优先级为： `INFO` < `DEBUG` < `WARN` < `ERROR` < `NONE`

其中，除了按大小滚动日志（TarsRotate）默认级别为 `DEBUG`，其它均为 `INFO`。

如需变更日志级别，可调用 `logger.setLevel(level)` 方法，传入需要的日志级别即可：

``` js
logger.setLevel('info');
logger.setLevel（'none'); // none 为一种特殊的日志级别，所有日志均不输出
```

如果服务在 `TARS平台` 上运行：

* 模块会接收 `日志级别变更` 的管理命令，自动变更当前日志级别。
* 模块会读取 TARS 配置文件中 `tars.application.server.logLevel` 节，以配置日志级别。
* 以上两项日志级别的配置仅针对 `按大小滚动日志（TarsRotate）` 生效。

### 日志内容格式（Formatter）

模块提供精简 `Formatter.Simple()` 与复杂 `Formatter.Detail()` 两种日志处理方法：

* __复杂__： `日期 时间|PID|日志级别|文件名与行号|内容`
* __精简__： `日期 时间|内容`

默认情况下，不同日志类型会使用不同的处理方法。

关于 `Formatter` 的详情，请访问 [@tars/winston-tars.Formatter](https://github.com/tars-node/winston-tars)

## 按大小滚动日志（TarsRotate）

在初始化类型为 `TarsRotate` 的 `logger` 时， `options` 还接受如下参数：

* __maxFiles__: 最大的文件总数（也就是例子中的 n）， *默认值为 10*  
* __maxSize__: 单文件最大大小（单位为 bytes）， *默认值为 10M*   
* __formatter__: 定义日志内容格式化方法， *默认值为 Formatter.Detail()*

关于 `TarsRotate` 的详情，请访问 [@tars/winston-tars.TarsRotate](https://github.com/tars-node/winston-tars#tarsrotate)

## 时间相关（DateFormat）

定义了与时间相关的日志（`TarsDate`）滚动的处理方法：

* 按1天日志：LogByDay([interval, pattern])
* 按1小时日志：LogByHour([interval, pattern])
* 按10分钟日志：LogByMinute([interval, pattern])
* 自定义格式日志：LogByCustom(pattern)

其中， `interval` 为日志滚动间隔， `pattern` 为日志文件名中时间的格式

一般情况下，可直接使用字面量：

```js
var logger = new tarsLogs('TarsDate', 'access', {
	format : tarsLogs.DateFormat.LogByDay
});
```

但如果需要自定义间隔或日志文件名，则需要实例化：

```js
var logger = new tarsLogs('TarsDate', 'access', {
	format : new tarsLogs.DateFormat.LogByDay(3, '%Y-%m-%d');
});
```

关于 `DateFormat` 的详情，请访问 [@tars/winston-tars.DateFormat](https://github.com/tars-node/winston-tars#dateformat)

## 按时间滚动日志（TarsDate）

在初始化类型为 `TarsDate` 的 `logger` 时， `options` 还接受如下参数：

* __format__: 创建新文件的间隔，为 DateFormat 对象， *默认值为 FORMAT.LogByDay*
* __formatter__: 定义日志内容格式化方法， *默认值为 Formatter.Simple()*

关于 `TarsDate` 的详情，请访问 [@tars/winston-tars.TarsDate](https://github.com/tars-node/winston-tars#tarsdate)

## 染色

在每一条日志写入时可以指定是否需要对此日志进行染色（也就是说，染色的开关并不在初始化的时而在日志写入时）。

仅有 `按大小滚动（TarsRotate）` 与 `按时间滚动（TarsDate）` 输出的日志可以进行染色。

染色的日志不仅按照之前逻辑进行输出，也会按照相同的逻辑会多输出一份放在 `$LOG_PATH$/tars_dyeing/` 目录下。

染色日志总是全量输出（忽略当前日志级别进行输出）。

### 染色对象

染色对象标识了当前染色的状态（是否需要染色 及 附加信息）。

染色对象需要通过 [@tars/dyeing](https://github.com/tars-node/dyeing) 提供的方法生成。

为了便于使用，本模块封装了染色对象的生成方式。可通过 `logger.getDyeingObj()` 来获取染色对象。 

打开染色
``` js
logger.getDyeingObj(true);
```

打开染色并设置染色的 `val` 为 `guid`，`key` 为 `guid|sn`
```js
logger.getDyeingObj(true, 'guid', 'guid|sn');
```

__实际应用中，不应调用此方法生成染色对象，而应直接使用其他模块提供的染色对象__

符合染色标准的其它模块，均会提供 `Object.getDyeingObj()` 方法，可通过调用它获取染色对象，而不是使用此模块的方法。

关于染色的详细信息，请访问 [@tars/dyeing](https://github.com/tars-node/dyeing) 获取。

### 使用

需要对某条日志进行染色时，需在调用 `logger` 特定方法（日志级别）的最后一个参数传入 `染色对象`。

输出日志内容为 `data1 data2` 并 强制染色的日志
``` js
logger.info('data1', 'data2', logger.getDyeingObj(true));
```

输出日志内容为 `data` 并根据 `rpc.server` 调用链上的染色信息进行染色的日志
```js
tars.TestImp.prototype.echo = function (current, i) {
   logger.info('data', current.getDyeingObj());
}
```
_`rpc` 具体获取染色对象的方式，请详见 [@tars/rpc](https://github.com/tars-node/rpc)_