# TARS-TIMER-SERVER的说明


## 开发Guidline

 0. 平台会提供一份新的针对php的模板,命名为tars.tarsphp.default, !!!!!!!必须首先修改其中php的执行路径!!!!!!!
 
 1. 模板相关修改：
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

 2. 定时任务示例：
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

 3. tars文件夹下面新建tars.proto.php文件, 里面需要包含对你服务本身的说明:
   ```
   <?php
   return array(
       'appName' => 'PHPTest',
       'serverName' => 'PHPTimerServer',
       'objName' => 'obj',
   );
   ```
   这个名称要与在tars平台上面的名称完全一一对应。

 4. 注意事项!!!!
	timer目录下的每个php文件都会和一个worker进程绑定，所以worker的数量一定要不小于定时任务的数量。
	可以在tars管理平台 Servant管理 里面修改线程数（对于php来说就是swoole的worker数）。