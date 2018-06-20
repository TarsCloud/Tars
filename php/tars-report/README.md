进行keepAlive上报的组件

1、组件说明
```
	tarsphp框架在master进程启动的时候会上报服务存活状态(主进程存活&管理进程存活)。
	在第一个worker进程启动的时候，会开启一个timer来定时上报存活状态。
```

2、文件说明：
```
	├── composer.json
	├── src
	│ ├── NodeF.tars //协议文件
	│ ├── ServerFAsync.php // 异步上报
	│ ├── ServerFSync.php // 同步上报
	│ └── ServerInfo.php  //ServerInfo
```

3、使用实例(假如服务为 PHPTest.test.obj)：
```
        $serverInfo = new ServerInfo();
        $serverInfo->adapter = 'PHPTest.test.objAdapter';
        $serverInfo->application = 'PHPTest';
        $serverInfo->serverName = 'test';
        $serverInfo->pid = $masterPid;

        $serverF = new ServerFSync($host, $port, $objName);
        $serverF->keepAlive($serverInfo); //主进程存活

        $adminServerInfo = new ServerInfo();
        $adminServerInfo->adapter = 'AdminAdapter';
        $adminServerInfo->application = 'PHPTest';
        $adminServerInfo->serverName = 'test';
        $adminServerInfo->pid = $masterPid;
        $serverF->keepAlive($adminServerInfo);//管理进程存活
```