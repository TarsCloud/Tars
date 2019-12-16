[点击切换到中文文档](ChangeList.zh.md)

# Update History(From 20191126)

## 20191126
- Update the installation mode of tar web, and support permission verification by default
- Create the admin user by default, and change the admin password for the first login
- Registration not allowed, admin user to create user
- Add password modification function
- Add check.sh script to monitor tarsregistry, tarsadminregistry, tarsnode
- Complete installation documentation

## 20191128
- Update tars-web,  Support to mount nginx on the front end of tars Web
- Complete installation documentation

## 20191130
- tars-cpp support tars2json

## 20191213
- protocol: notify.tars 上报增加格式
- cpp: cgi encode fix
- framework: patch, adminregistry, node add notify report, show error message in web
- web: deploy ssl server, support server bind dns
- web: undeploy check server state
- web: expand server can bind dns
- web: fix deploy page
- web: manage page add refresh
- web: deploy message, goto manage page

## 20191214
- web(1.3.0): add tarsnode list
- fix web 1.2.0 bug

## 20191216
- web(1.3.1): add tarsnode install online
- framework(1.3.0): fix deploy script, during deploy , copy tarsnode.tgz to web/dist/static