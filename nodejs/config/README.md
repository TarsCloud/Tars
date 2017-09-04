# @tars/config

## 安装
``` js
npm install @tars/config
``` 

## 使用
``` js
var tarsConfigHelper = require("@tars/config");
var tarsConfig = new tarsConfigHelper(options);
```

### options

__如果服务通过 [node-agent](http://git.oa.com/tars/node-agent/tree/master "node-agent") （或在TARS平台）运行，则无需填写。__

* __app__: tars app名称
* __server__: 服务名称

# 方法调用

事先说明，以下几个方法调用，configOptions可以是一个object,包含以下参数。

struct ConfigInfo
{
	//业务名称
	0 require string appname;
	//服务名称
	1 require string servername;
	//配置文件名称
	2 require string filename;
	//是否只获取应用配置,默认为false,如果为true则servername可以为空
	3 require bool bAppOnly=false;
	//服务所在节点(ip)
	4 optional string host;
	//set分组名称
	5 optional string setdivision;
};

所有的方法调用均返回 [Promise] 对象。

## loadConfig(configOptions[, configFormat])

获取配置文件内容

__configOptions__: {string/object} tars配置文件名或者一个Object(见上面configOptions说明)
__configFormat__: {string} 转换格式，如果不传，每个配置项直接返回字符串。"json":自动转为json格式，"c":c++格式的配置项

``` js
tarsConfig.loadConfig("Server.conf").then(function (data) {
    console.log("loadConfig back", data);
}, function (err) {
    console.error("loadConfig err", err);
});
```

## getConfigList(configOptions)

获取配置文件列表

__configOptions__: {string/object} tars配置文件名或者一个Object(见上面configOptions说明)，也可以不传，那么就直接使用默认的app和server。

``` js
tarsConfig.getConfigList().then(function(configList) {
    console.log("examples:getConfigList back", configList);
},
function(err) {
    console.error("getConfigList error", err);
});
```


## getAllConfigData([configOptions, configFormat])

获取配置文件列表以及配置项的内容

__configOptions__: {string/object} tars配置文件名或者一个Object(见上面configOptions说明)，可缺省。
__configFormat__: {string} 转换格式，默认配置项直接返回字符串。"json":自动转为json格式，"c":c++格式的配置项

返回的对象内，key：文件名，value：文件内容

``` js
tarsConfig.getAllConfigData().then(function(configDatas) {
    console.log("examples:getAllConfigData back", configDatas);
},
function(err) {
    console.error("getAllConfigData error", err);
});
```

## setTimeout(iTimeout)

设置调用超时时间，默认是30s，参数单位为 ms

## loadServerObject(configFormat)

加载服务默认的配置项，并且转换为json

__configFormat__: {string} "c":c++格式的配置项（默认值），"json"：json格式配置项

返回的对象内，key：文件名，value：文件内容

``` js
tarsConfig.loadServerObject().then(function(serverObject) {
    console.log("serverObject",serverObject);
});
```

# 事件支持

## configPushed

从TARS配置平台push配置文件的时候，将触发此事件

``` js
tarsConfig.on("configPushed", function(filename, content) {
    console.log("config pushed", filename, content);
});
```