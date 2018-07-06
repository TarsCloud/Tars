# TARS-TIMER-SERVER的说明

## 服务部署guideline

1. 进入运维管理 => 模板管理

平台会提供一份新的针对php的模板,命名为tars.tarsphp.default, !!!!!!!必须首先修改其中php的执行路径!!!!!!!
 
模板相关修改：
    a.新建一个tars.tarsphp.timer服务模板，相比tars.tarsphp.default加一个  isTimer=1 。PHP服务选择tars.tarsphp.timer模板。
    b.或者使用tars.tarsphp.default模板，在编辑服务下面的私有模板里面加入下面内容：
```
<tars>
 <application>
   <server>
      isTimer=1
    </server>
 </application>
</tars>
```


2. 在平台上进入运维管理=>部署服务,填写对应的应用名和服务名称,注意,这个与下文中tars文件夹中的tars.proto.php
需要完全一致。

3. 选择服务类型为tars_php

4. 选择模板为刚刚建立的timer服务模板,默认不启用set

5. 选择可用端口,填写服务器内网ip

6. 端口类型为TCP
!!!!协议类型TIMER服务必须选择非TARS!!!!!!

7. 线程数对应SWOOLE的进程数,即为timer目录下面能启动的定时服务的数量

8. 最大连接数和队列最大长度、队列超时时间,对php服务不生效

9. 点击添加和提交即可,接下来请进入开发guidline

## 开发Guidline

 

 1. 定时任务示例：
	./src/timer/TenSecondsTimer.php

```
<?php
namespace TimerServer\timer;
class TenSecondsTimer {
    public $interval;
    public function __construct()
    {
        $this->interval = 10000; //单位为毫秒
    }
    public function execute() {
        // 你的业务代码
    }
}
```

 2. tars文件夹下面新建tars.proto.php文件, 里面需要包含对你服务本身的说明:
   ```
   <?php
   return array(
       'appName' => 'PHPTest',
       'serverName' => 'PHPTimerServer',
       'objName' => 'obj',
   );
   ```
   这个名称要与在tars平台上面的名称完全一一对应。

 3. 注意事项!!!!
	timer目录下的每个php文件都会和一个worker进程绑定，所以worker的数量一定要不小于定时任务的数量。
	可以在tars管理平台 Servant管理 里面修改线程数（对于php来说就是swoole的worker数）。