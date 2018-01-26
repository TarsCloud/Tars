1. Tars java开发环境如何搭建？
> - 参考tars_install.md

2. 如何在本地启动和开发调试tars
> - 首先在工程目录下执行 mvn tars:build -Dapp=TestApp -Dserver=HelloJavaServer -DjvmParams="-Xms1024m -Xmx1024m -Dcom.sun.management.jmxremote.ssl=false -Dcom.sun.management.jmxremote.authenticate=false -Xdebug -Xrunjdwp:transport=dt_socket,address=9000,server=y,suspend=n"
> - 在工程目录target/tars/bin/tars_start 启动服务 
