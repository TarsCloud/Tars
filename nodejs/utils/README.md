# @tars/utils
TARS 框架辅助工具集合

## Installation
```sh
$ npm install @tars/utils
```

## 01. 配置文件解析器
```js
var Config = require('@tars/utils').Config;
```
### API
#### parseFile(sFilePath, [encoding, callback])
解析指定文件
* `sFilePath`: 文件名
* `encoding`: 文件编码类型。 (默认值: utf8)
* `callback`: 回调函数，回调函数的格式 function callback(ret, config){}, 其中ret为对象{code: 返回码，成功为0， 失败为-1, message: 描述, exception:如果成功为undefined，如果失败为事件对象}， config为解析器本身

### parseText(sText)
解析字符串，并将解析的结果存于内部的_data属性中，可以通过get方法获取相应的值
* `sText`: 字符串
* `return`: true：解析成功, false: 解析失败

#### get(key, defaultValue)
文件被解析之后，会将结果存储到一个对象中，通过get方法可以获取制定的值。注：如果配置文件/字符串中有相同的key，则get获取key对应的值时，不会获取所有的值，而是获取该key最后对应的那个值，也可以理解为对应相同的key后面的值覆盖前面的值。
* `key`: 需要取值的key值，格式为x1.x2.x3，其中x1，x2，x3依次为深层次的key，注：如果key值本身为x1.x2格式，取该key对应的值时需要写成<x1.x2>,具体使用参见例子。
* `defaultValue`: 取不到结果的默认值

#### getDomain(key, defaultValue)
获取key对应的值中类型为Object的属性数组
* `key`: key值。
* `defaultValue`: 取不到结果的默认值

#### getDomainValue(key, defaultValue)
获取key对应的值中类型为Object的属性值数组
* `key`: key值。
* `defaultValue`: 取不到结果的默认值

#### getDomainLine(key, defaultValue)
获取key对应路径下的所有非空行
* `key`: key值。
* `defaultValue`: 取不到结果的默认值
* `return`: 数组

#### data
通过该属性，可以获取文件解析的结果

## example

```js
var Config = require('@tars/utils').Config;

var config = new Config();
config.parseFile('./config.conf', 'utf8');

var data = config.data;
console.log('data: ', data);
console.log('get: tars.application.server.local: ', config.get('tars.application.server.local'));
console.log('getDomain: tars.application.server: ', config.getDomain('tars.application.server'));
console.log('getDomainValue: tars.application.server: ', config.getDomainValue('tars.application.server'));
```

具体例子参见examples目录下的test-config.js文件

## 02. Endpoint工具
```js
var Endpoint = require('@tars/utils').Endpoint;
```
### API
#### Class方法：parse(desc)
从字符串中解析出Endpoint信息
* `desc`: 字符串，例如：'tcp -h 127.0.0.1 -p 10000 -t 60000'
* `return`: 返回Endpoint实例。

#### toString()
Endpoint信息转化成字符串

#### copy()
拷贝Endpoint实例

## example

```js
var Endpoint = require('@tars/utils').Endpoint;

var endpoint = Endpoint.parse('tcp -h 127.0.0.1 -p 10000 -t 60000');
console.log('endpoint: ' + endpoint.toString());
console.log('endpoint.copy: ' + endpoint.copy().toString());
```

具体例子参见examples目录下的test-endpoint.js文件

## 03. timeProvider工具
```js
var timeProvider = require('@tars/utils').timeProvider;
```
### API
#### nowTimestamp()
采用Date.now()的方式获取时间，此种方式效率最高，Date.now()的方式的效率大概是new Date().getTime()的2倍，是process.hrtime()方式的4倍。
* `return`: 返回对象

```js
{
    hrtime: // 数组类型，[秒, 纳秒],
    timestamp: // 单位ms
}
```

#### diff(oTime)
当前时间相对于oTime的时间间隔
* `oTime`:  相对时间，nowTimestamp函数返回的对象类型
* `return`: 浮点类型，时间间隔，单位毫秒
* 注：nowTimestamp和diff配对使用

#### dateTimestamp()
获取当前的时间戳, 即机器从启动到当前的时间（process.hrtime）
* `return`: 返回对象

```js
{
    hrtime: // 数组类型，[秒, 纳秒],
    timestamp: // 单位ms
}
```

#### dateTimestampDiff(oTime)
当前时间相对于oTime的时间间隔
* `oTime`:  相对时间，dateTimestamp函数返回的对象类型
* `return`: 浮点类型，时间间隔，单位毫秒
* 注：dateTimestamp和dateTimestampDiff配对使用

## example

```js
var timeProvider = require('@tars/utils').timeProvider;

var i = 0, count = 10000000;
var tt1, tt2, interval = 0;
var t1 = new Date().getTime();
var t2 = t1;

tt1 = timeProvider.nowTimestamp();
for(i = 0; i < count; i++) {
    tt2 = timeProvider.diff(tt1);
}
t2 = new Date().getTime();
console.log('【hrTime】interval: ' + (t2 - t1));

t1 = new Date().getTime();
tt1 = timeProvider.dateTimestamp();
for(i = 0; i < count; i++) {
    tt2 = timeProvider.dateTimestampDiff(tt1);
}
t2 = new Date().getTime();
console.log('【hrTime】interval: ' + (t2 - t1));
```

具体例子参见examples目录下的test-timer.js文件

## 03. Promise库
```js
var Promise = require('@tars/utils').Promise;
```

为 TARS 应用提供一个方便统一的Promise库。开发 TARS 应用时推荐大家使用此库而不是自己选择Promise库，当出现更好的promise方案时，我们可以直接替换此模块中的实现，直接对所有应用生效。

```javascript
var Promise=require("@tars/utils").Promise;
var promise=new Promise(function(resolve,reject){
    setTimeout(function(){
        resolve(666)
    },3000);
});
promise.then(function(data){
    console.log(data);
});
```

目前 TARS 中的Promise是基于bluebird库实现的，在q、bluebird、原生Promise中bluebird性能最好。