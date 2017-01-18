
1. Tars C++开发环境如何搭建？
> * 参考tars_install.md

2. Tars C++如何快速上手？
> * 文档参考tars_cpp_quickstart.md，相关示例代码参考cpp/examples目录

3. tars/tup协议是什么？
> * 具体参考tars_tup.md

4. Tars C++是否支持自定义协议（比如:http）？
> * Tars C++除了支持tars/tup协议之外，还支持业务自定义协议，具体参考tars_cpp_thirdprotocol.md

5. Tars C++如何拉取业务配置文件？
> * Tars C++支持通过使用addConfig方法获取指定的业务配置文件，业务配置相关远离，可以参考tars_config.md

6. 在Tars框架中运行的服务是如何被监控的？
> * 具体参考tars_server_monitor.md

7. Tars C++通讯器如何创建？
> * 如果服务基于TAF框架,请直接从Applicatin获取,不要自己创建。例如: 
```
Application::getCommunicator()->stringToProxy<NotifyPrx>(ServerConfig::Notify);
```
> * 如果服务非基于TAF框架,只是TAF客户端，可以用new Communicator(conf) 初始化通信器。例如:
``` 
TC_Config conf(“config.conf”);
CommunicatorPtr c = new Communicator(conf);
```

8. Tars C++调用的超时时间如何设置？
> * 具体参考tars_cpp_user_guide.md

9. 服务发布后但没有运行怎么查看原因？
> * 是否服务配置文件没有正确获取。例如：在web平台上配置的配置文件名与在程序add下载的配置文件名不一致。
> * 查找问题时，首先看web上"服务当前报告"是否提示正常，例如:配置文件有没下载成功，服务启动时下载的配置文件。
> * 再查找服务自己打印log日志。日志一般在/usr/local/app/tars/app_log/应用名/服务名/目录下。
> * 若仍有问题请查看node日志。

10. core文件如何打开，生成在什么地方？
> * core文件是在tafnode启动脚本添加ulimite -c unlimited 打开，目前core文件生成在 /user/local/app/tars/app_log下.

11. 主控Registry故障是否会影响业务服务的正常访问？
> * 不会影响业务服务正常访问，框架底层会缓存后端服务的ip列表。