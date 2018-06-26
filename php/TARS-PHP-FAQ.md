# TARS-PHP-FAQ

## 常见问题

1. 问题：php服务的业务日志在哪里查看？

答案：/usr/local/app/tars/app_log/$APPNAME/$SERVERNAME/$APPNAME.$SERVERNAME.log 
不要忘记同时查看php_error哈

2. 问题：Class 'TARS_Struct' not found in /usr/local/app/tars/tarsnode/data/PHPTest.PHPHttpServer/bin/src/vendor/phptars/tars-report/src/ServerInfo.php on line 5

答案：请确认一下phptars扩展是否已经被安装，检查命令：php -m | grep phptars。 安装方式请见/php/php_extension下面的auto和auto7脚本内容以及相应文档

3. 问题：搭建了http和tcp的服务，但是tcp服务无法正确回包，是什么问题？

答案：
请检查tcp服务状态是否正常，检查php_error、业务日志中是否有权限导致的服务无法启动问题；
请检查tcp服务是否使用了正确的模板，比如不能指定protocolName为http，因为tcp服务一定要使用tars协议进行通信的，并且一定要指定swoole自动拼包的package_length等配置参数；
使用tcpdump -iany -s0 port tcp服务端口 -XNNnn 进行抓包，看http服务是否正常发包给tcp服务了

4. 问题：我可以不通过平台启动php的服务么？

答案：显然是可以的，直接到服务目录下：/usr/local/app/tars/tarsnode/data/PHPTest.PHPHttpServer/bin
下面会有两个脚本tars_stop.sh和tars_start.sh，只要有权限，是可以直接启停服务的。


5. 问题: 请求部分tcp的方法可以,但是部分不可以,可能什么原因?

答案: 一个可能的原因是你使用了format工具,破坏了server端自动生成的注释的结构(比如examples下面的Tars-tcp-server/src/servant/PHPtest/PHPServer/obj/TestTafServiceServant.php)。下载github上面最新的,或者用tars2php重新生成一下server的interface代码都可以解决这个问题。


## 注意点：

请务必安装phptars扩展
如果使用server，请务必安装swoole扩展