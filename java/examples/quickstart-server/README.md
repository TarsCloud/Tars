## 本地IDE启动方法

1. 使用ide打开项目。
2. 配置启动Main Class为com.qq.tars.server.startup.Main。
3. 修改resources目录下的TestApp.HelloJavaServer.config.conf，指定其中的basepath、datapath和logpath。
4. 配置启动参数。
    > -Dconfig=(TestApp.HelloJavaServer.config.conf路径)
5. 通过ide启动。

注：此处的TestApp.HelloJavaServer.config.conf模版文件只是用于本地启动零时使用，在线上环境该文件会由TarsNode自动生成并添加到启动参数中。
