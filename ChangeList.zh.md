[Click me switch to English version](ChangeList.md)

# 更新历史(从20191126开始记录)

## 20191126
- 更新tars-web的安装方式, 默认支持权限验证
- 默认创建admin用户, 第一次登陆修改admin密码
- 不允许注册, admin用户来创建用户
- 增加修改密码功能
- 增加check.sh脚本, 监控tarsregistry, tarsAdminRegistry, tarsnode
- 完善安装文档说明


## 20191128
- 更新tars-web 支持在tars-web前端挂载nginx
- 完善安装文档说明

## 20191130
- tars-cpp支持tars2json

## 20191213
- protocol: notify.tars 上报增加格式
- cpp: cgi encode函数优化
- framework: 增加patch, adminregistry, node的notify上报, 记录发布, 重启等错误, 在页面web页面上显示
- web: 部署ssl服务, 支持服务绑定域名
- web: 下线时 检查服务active状态
- web: 服务扩容时, 可以绑定域名
- web: 调整发布界面, 操作更易于理解
- web: 服务管理界面增加刷新
- web: 发布错误的提示, 跳转到服务管理页面

## 20191214
- web(1.3.0): 运维管理界面增加tarsnode列表
- fix web 1.2.0的bug

## 20191216
- web(1.3.1): 运维管理界面增加tarsnode安装逻
- framework(1.3.0): 修改部署脚本, 安装时将tarsnode.tgz copy 到web目录下

## 20191217
- framework(1.3.1): bug fix, mysql 5.7的情况下, 授权有问题,导致安装后web无权限的问题