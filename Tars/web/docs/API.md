# 管理平台接口文档

# 接口调用说明
假设部署在localhost，80端口，接口为**tree**
访问的url地址为
```
http://localhost/pages/tree
```

# 业务树
## tree
取业务树
### 参数
无
### 返回值
```javascript
{
    "id": "",                  // 节点ID
    "name": "",                // 节点名称
    "pid": "",                 // 父节点ID
    "open": true,              // 是否展开
    "is_parent": true,         // 是否父节点
    "children": []             // 子节点
}
```

# 服务管理
## server/api/deploy_server
部署服务
### 参数
```javascript
{                                  
    "application": "",             // 应用
    "server_name": "",             // 服务
    "node_name": "",               // 节点
    "server_type": "",             // 服务类型
    "template_name": "",           // 模板名
    "enable_set": true,            // 是否启用Set
    "set_name": "",                // Set名
    "set_area": "",                // Set区
    "set_group": "",               // Set组
    "adapters": [{ 
        "obj_name": "",            // OJB名称
        "bind_ip": "",             // Obj绑定IP
        "port": "",                // 端口
		"port_type": "tcp",        // 端口类型
		"protocol": "tars",        // 协议
        "thread_num": 0,           // 线程数
        "max_connections": 0,      // 最大连接数
        "queuecap": 0,             // 队列最大长度
        "queuetimeout": 0          // 队列超时时间
    }]                             
}
```
### 返回值
```javascript
{                                  
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/update_server
修改服务
### 参数
```javascript
{
	"id": 0,                  // 服务ID
	"isBak": true,            // 是否备机
	"template_name": "",      // 模板名称
	"server_type": "",        // 服务类型
	"enable_set": "",         // 是否启用Set
	"set_name": "",           // Set名
	"ser_area": "",           // Set区
	"set_group": "",          // Set组
	"async_thread_num": 0,    // 异步线程数
	"base_path": "",          // 缺省路径
	"exe_path": "",           // EXE路径
	"start_script_path": "",  // 启动脚本
	"stop_script_path": "",   // 停止脚本
	"monitor_script_path": "",// 监控脚本
	"profile": ""             // 私有模板
}
```
### 返回值
```javascript
{
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/server
取服务
### 参数
```javascript
id // 服务ID
```
### 返回值
```javascript
{
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}
```	

## server/api/server_list
取服务列表
### 参数
```javascript
tree_node_id // 树节点ID
```
### 返回值
```javascript
[{
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}]
```
### 是否支持分页
是

## server/api/inactive_server_list
取设置状态为inactive的服务列表
### 参数
```javascript
application // 应用
server_name // 服务
node_name   // 节点
```
### 返回值
```javascript
[{
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}]
```
### 是否支持分页
是

## server/api/server_notify_list
取服务notify日志列表
### 参数
```javascript
tree_node_id // 树节点ID
```
### 返回值
```javascript
[{
    "notifytime": "",          // 时间
    "server_id": "",           // 服务ID
    "thread_id": "",           // 线程ID
	"command": "",             // 命令
    "result": ""               // 结果
}]
```
### 是否支持分页
是

## server/api/get_realtime_state
取服务实时状态
### 参数
```javascript
id // 服务ID
```
### 返回值
```javascript
{
    "realtime_state": ""       // 实时状态
}
```	

## server/api/load_server
加载服务
### 参数
```javascript
application  // 应用
server_name  // 服务
node_name    // 节点
```
### 返回值
```javascript
"" // 执行结果
```	

## server/api/expand_server_preview
预扩容
### 参数
```javascript
{
    "application": "",        // 应用
    "server_name": "",        // 服务
    "set": "",                // Set
    "node_name": "",          // 节点
    "expand_nodes": [""],     // 扩容节点
    "enable_set": true,       // 是否启用Set
    "set_name": "",           // Set名
    "set_area": "",           // Set区    
    "set_group": "",          // Set组
    "copy_node_config": true  // node_name非空时是否复制节点配置
}
```
### 返回值
```javascript
[{
    "application": "",        // 应用
    "server_name": "",        // 服务
    "set": "",                // Set
    "obj_name": "",           // OBJ名称
    "node_name": "",          // 节点
    "bind_ip": "",            // Obj绑定IP
    "port": 0,                // 端口
    "template_name": "",      // 模板名
    "status": "",             // 状态
}]
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/expand_server
扩容
### 参数
```javascript
{
    "application": "",            // 应用
    "server_name": "",            // 服务
    "set": "",                    // Set
    "node_name": "",              // 节点
    "copy_node_config": true，    // node_name非空时是否复制节点配置
    "expand_preview_servers": [{
        "node_name": "",          // 节点
        "set": "",                // Set
        "obj_name": "",           // OBJ名称
        "bind_ip": "",            // Obj绑定IP
        "port": 0                 // 端口
    }]
}
```
### 返回值
```javascript
[{                                  
    "id": 0,                   // 服务ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "server_type": "",         // 服务类型
    "enable_set": true,        // 是否启用Set
    "set_name": "",            // Set名
    "set_area": "",            // Set区
    "set_group": "",           // Set组
    "setting_state": "",       // 设置状态
    "present_state": "",       // 当前状态
    "bak_flag": true,          // 是否备机
    "template_name": "",       // 模板名称
    "profile": "",             // 私有模板
    "async_thread_num": 0,     // 异步线程数
    "base_path": "",           // 缺省路径
    "exe_path": "",            // EXE路径
    "start_script_path": "",   // 启动脚本
    "stop_script_path": "",    // 停止脚本
    "monitor_script_path": "", // 监控脚本
    "patch_time": "",          // 发布时间
    "patch_version", "",       // 发布版本
    "process_id": "",          // 进程ID
    "posttime": ""             // 更新时间
}]
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/add_adapter_conf
新增Adapter
### 参数
```javascript
{
	"application": "",         // 应用
	"server_name": "",         // 服务
	"node_name": "",           // 节点
	"thread_num": 1,           // 线程数
	"endpoint": "",            // EndPoint
	"max_connections": 0,      // 最大连接数
	"allow_ip": "",            // 允许IP
	"servant": "",             // Servant
	"queuecap": 0,             // 队列长度
	"queuetimeout": 0,         // 队列超时时间
	"protocol": "",            // 协议
	"handlegroup": ""          // 处理组
}
```
### 返回值
```javascript
{
	"id": 0,                   // Adapter ID
	"application": "",         // 应用
	"server_name": "",         // 服务
	"node_name": "",           // 节点
	"adapter_name": "",        // Adapter名
	"thread_num": 1,           // 线程数
	"endpoint": "",            // EndPoint
	"max_connections": 0,      // 最大连接数
	"allow_ip": "",            // 允许IP
	"servant": "",             // Servant
	"queuecap": 0,             // 队列长度
	"queuetimeout": 0,         // 队列超时时间
	"posttime": "",            // 更新时间
	"protocol": "",            // 协议
	"handlegroup": ""          // 处理组
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/delete_adapter_conf
删除Adapter
### 参数
```javascript
id // Adapter ID
```
### 返回值
```javascript
[0] // 删除的Adapter ID
```	

## server/api/update_adapter_conf
修改Adapter
### 参数
```javascript
{
	"id": 0,                   // Adapter ID
	"thread_num": 1,           // 线程数
	"endpoint": "",            // EndPoint
	"max_connections": 0,      // 最大连接数
	"allow_ip": "",            // 允许IP
	"servant": "",             // Servant
	"queuecap": 0,             // 队列长度
	"queuetimeout": 0,         // 队列超时时间
	"protocol": "",            // 协议
	"handlegroup": ""          // 处理组
}
```
### 返回值
```javascript
{
    "id": 0,                   // Adapter ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "adapter_name": "",        // Adapter名
    "thread_num": 1,           // 线程数
    "endpoint": "",            // EndPoint
    "max_connections": 0,      // 最大连接数
    "allow_ip": "",            // 允许IP
    "servant": "",             // Servant
    "queuecap": 0,             // 队列长度
    "queuetimeout": 0,         // 队列超时时间
    "posttime": "",            // 更新时间
    "protocol": "",            // 协议
    "handlegroup": ""          // 处理组
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/adapter_conf
取Adapter
### 参数
```javascript
id // Adapter ID
```
### 返回值
```javascript
{
    "id": 0,                   // Adapter ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "adapter_name": "",        // Adapter名
    "thread_num": 1,           // 线程数
    "endpoint": "",            // EndPoint
    "max_connections": 0,      // 最大连接数
    "allow_ip": "",            // 允许IP
    "servant": "",             // Servant
    "queuecap": 0,             // 队列长度
    "queuetimeout": 0,         // 队列超时时间
    "posttime": "",            // 更新时间
    "protocol": "",            // 协议
    "handlegroup": ""          // 处理组
}
```

## server/api/adapter_conf_list
取Adapter列表
### 参数
```javascript
id // 服务ID
```
### 返回值
```javascript
[{
    "id": 0,                   // Adapter ID
    "application": "",         // 应用
    "server_name": "",         // 服务
    "node_name": "",           // 节点
    "adapter_name": "",        // Adapter名
    "thread_num": 1,           // 线程数
    "endpoint": "",            // EndPoint
    "max_connections": 0,      // 最大连接数
    "allow_ip": "",            // 允许IP
    "servant": "",             // Servant
    "queuecap": 0,             // 队列长度
    "queuetimeout": 0,         // 队列超时时间
    "posttime": "",            // 更新时间
    "protocol": "",            // 协议
    "handlegroup": ""          // 处理组
}]
```

# 服务配置
## server/api/add_config_file
新增配置文件
### 参数
```javascript
{
    "level": 1,         // 层级
    "application": "",  // 应用
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": ""        // 文件内容
}
```
### 返回值
```javascript
{
    "id": 0,            // 配置文件ID
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": "",       // 文件内容
    "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
    "posttime": "",     // 更新时间
}
```

## server/api/delete_config_file
删除配置文件
### 参数
```javascript
id // 配置文件ID
```
### 返回值
```javascript
[0] // 删除的配置文件ID
```

## server/api/update_config_file
修改配置文件
### 参数
```javascript
{
    "id": 0,            // 配置文件ID
    "config": "",       // 文件内容
    "reason": ""        // 备注
}
```
### 返回值
```javascript
{
    "id": 0,            // 配置文件ID
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": "",       // 文件内容
    "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
    "posttime": "",     // 更新时间
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/config_file
取配置文件
### 参数
```javascript
id // 配置文件ID
```
### 返回值
```javascript
{
    "id": 0,            // 配置文件ID
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": "",       // 文件内容
    "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
    "posttime": "",     // 更新时间
}
```

## server/api/config_file_list
取配置文件列表
### 参数
```javascript
level       // 层级,1:应用，2：Set名，3：Set区，4：Set组，5：服务
application // 应用
server_name // 服务
set_name    // Set名
set_area    // Set区
set_group   // Set组
```
### 返回值
```javascript
[{
    "id": 0,            // 配置文件ID
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": "",       // 文件内容
    "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
    "posttime": "",     // 更新时间
}]
```

## server/api/node_config_file_list
取节点配置文件列表
### 参数
```javascript
application // 应用
server_name // 服务
set_name    // Set名
set_area    // Set区
set_group   // Set组
config_id   // 配置文件ID
```
### 返回值
```javascript
[{
    "id": 0,            // 配置文件ID
    "server_name": "",  // 服务
    "node_name": "",    // 节点
    "set_name": "",     // Set名
    "set_area": "",     // Set取
    "set_group": "",    // Set组
    "filename": "",     // 文件名
    "config": "",       // 文件内容
    "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
    "posttime": "",     // 更新时间
}]
```

### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/config_file_history
取配置文件修改记录
### 参数
```javascript
id  // 变更记录ID
```
### 返回值
```javascript
{
    "id": "",        // 变更记录ID
    "config_id": "", // 配置文件ID
    "reason": "",    // 备注
    "content": "",   // 变更内容
    "posttime": "",  // 更新时间
}
```

## server/api/config_file_history_list
取配置文件修改记录
### 参数
```javascript
config_id // 配置文件ID
```
### 返回值
```javascript
[{
    "id": "",        // 变更记录ID
    "config_id": "", // 配置文件ID
    "reason": "",    // 备注
    "content": "",   // 变更内容
    "posttime": "",  // 更新时间
}]
```
### 是否支持分页
是

## server/api/add_config_ref
新增引用
### 参数
```javascript
config_id    // 配置文件ID
reference_id // 引用配置文件ID
```
### 返回值
```javascript
{
    "id": "",           // 引用ID
    "config_id": "",    // 配置文件ID
    "reference_id": ""  // 引用配置文件ID
}
```

## server/api/delete_config_ref
删除引用
### 参数
```javascript
id // 引用ID
```
### 返回值
```javascript
[0] // 删除的引用ID
```

## server/api/config_ref_list
引用列表
### 参数
```javascript
config_id // 配置文件ID
```
### 返回值
```javascript
[{
    "id": 0,                // 引用ID
    "config_id": 0,         // 配置文件ID
    "reference": {
        "id": 0,            // 配置文件ID
        "server_name": "",  // 服务
        "node_name": "",    // 节点
        "set_name": "",     // Set名
        "set_area": "",     // Set取
        "set_group": "",    // Set组
        "filename": "",     // 文件名
        "config": "",       // 文件内容
        "level": 1,         // 层级，1：应用或Set，2：服务，3：节点
        "posttime": "",     // 更新时间
    }
}]
```

## server/api/merged_node_config
合并后节点配置
### 参数
```javascript
id // 配置文件ID
```
### 返回值
```javascript
"" // 配置文件内容
```

## server/api/push_config_file
下发节点配置
### 参数
```javascript
ids // 配置文件ID，用;分隔
```
### 返回值
```javascript
[{
    "application": "",    // 应用
    "server_name": "",    // 服务
    "node_name": "",      // 节点
    "ret_code": 0,        // 执行结果，0成功
    "err_msg": ""         // 错误信息
}]
```	

# 任务管理
> 涵盖启动、停止、发布、下线

## server/api/add_task
新增任务
### 参数
```javascript
{
    "serial": true, // 是否串行
    "items": [{
        "server_id": "",    // 服务
        "command": "",     // 命令字
        "parameters": {     // 参数
        }
    }]
}

命令字包括restart，stop，undeploy_tars，patch_tars
当command!=patch_tars时，parameter为空
当command=patch_tars时，parameter格式为
{
    patch_id: "0",    // 版本号
    update_text: "",  // 备注
    bak_flag: true    // 备机标识，true：备机，false：主机
}
```
### 返回值
```javascript
{
    "task_no": "",               // 任务ID
    "serial": true,              // 是否串行
    "status": 0,                 // 任务状态
    "items":[{
        "task_no": "",
        "item_no": "",           // 子任务ID
        "application": "",       // 应用
        "server_name": "",       // 服务
        "node_name": "",         // 节点
        "command": "",           // 命令
        "parameters": {},        // 参数
        "start_time": "",        // 开始时间
        "end_time": "",          // 结束时间
        "status": "",            // 子任务状态
        "status_info": "",       // 状态信息
        "execute_info": ""       // 执行信息
    }]
}
```
### 备注
只支持POST方式，Header中指定Content-Type:application/json

## server/api/task
取任务及子任务详细信息
### 参数
```javascript
task_no // 任务ID
```
### 返回值
```javascript
{
    "task_no": "",               // 任务ID
    "serial": true,              // 是否串行
    "status": 0,                 // 任务状态
    "items":[{
        "task_no": "",
        "item_no": "",           // 子任务ID
        "application": "",       // 应用
        "server_name": "",       // 服务
        "node_name": "",         // 节点
        "command": "",           // 命令
        "parameters": {},        // 参数
        "start_time": "",        // 开始时间
        "end_time": "",          // 结束时间
        "status": "",            // 子任务状态
        "status_info": "",       // 状态信息
        "execute_info": ""       // 执行信息
    }]
}
```
###	

## server/api/task_list
取任务列表
### 参数
```javascript
application // 应用
server_name // 服务
command     // 命令
from        // 开始日期
to          // 结束日期
```
### 返回值
```javascript
[{
    "task_no": "",               // 任务ID
    "serial": true,              // 是否串行
    "status": 0,                 // 任务状态
    "items":[{
        "task_no": "",
        "item_no": "",           // 子任务ID
        "application": "",       // 应用
        "server_name": "",       // 服务
        "node_name": "",         // 节点
        "command": "",           // 命令
        "parameters": {},        // 参数
        "start_time": "",        // 开始时间
        "end_time": "",          // 结束时间
        "status": "",            // 子任务状态
        "status_info": "",       // 状态信息
        "execute_info": ""       // 执行信息
    }]
}]
```	

# 发布包
## server/api/upload_patch_package
上传发布包
### 参数
```javascript
application // 应用
module_name // 模块名
comment     // 备注
```
### 返回值
```javascript
{
    "id": 0,        // 发布包ID
    "server": "",   // 服务，应用+模块名
    "tgz": "",      // 发布包名称
    "comment": "",  // 备注
    "posttime": ""  // 更新时间
}
```	

## server/api/server_patch_list
取发布版本列表
### 参数
```javascript
application // 应用
module_name // 模块名
```
### 返回值
```javascript
[{
    "id": 0,        // 发布包ID
    "server": "",   // 服务，应用+模块名
    "tgz": "",      // 发布包名称
    "comment": "",  // 备注
    "posttime": ""  // 更新时间
}]
```
### 是否支持分页
是

# 模板
## server/api/add_profile_template
新增模板
### 参数
```javascript
{
	"template_name": "",      // 必填，模板名称
	"parents_name": "",       // 必填，父模板
	"profile": ""             // 必填，模板内容
}
```
### 返回值
```javascript
{
	"id": 0,                  // 模板ID
	"template_name": "",      // 模板名称
	"parents_name": "",       // 父模板
	"profile": "",            // 模板内容
	"posttime": ""            // 更新时间
}
```	

## server/api/delete_profile_template
修改模板
### 参数
```javascript
id  // 模板ID
```
### 返回值
```javascript
[0] // 删除的模板ID
```

## server/api/update_profile_template
修改模板
### 参数
```javascript
{
	"id": "",                 // 必填，模板ID
	"template_name": "",      // 必填，模板名称
	"parents_name": "",       // 必填，父模板
	"profile": ""             // 必填，模板内容
}
```
### 返回值
```javascript
{
	"id": 0,                  // 模板ID
	"template_name": "",      // 模板名称
	"parents_name": "",       // 父模板
	"profile": "",            // 模板内容
	"posttime": ""            // 更新时间
}
```	

## server/api/profile_template
取模板
### 参数
```javascript
template_name // 模板名称
```
### 返回值
```javascript
{
	"id": 0,                  // 模板ID
	"template_name": "",      // 模板名称
	"parents_name": "",       // 父模板
	"profile": "",            // 模板内容
	"posttime": ""            // 更新时间
}
```	

## server/api/query_profile_template
查询模板
### 参数
```javascript
template_name  // 模板名称
parents_name   // 父模板名称
```
### 返回值
```javascript
[{
	"id": 0,                  // 模板ID
	"template_name": "",      // 模板名称
	"parents_name": "",       // 父模板
	"profile": "",            // 模板内容
	"posttime": ""            // 更新时间
}]
```	

# 监控
## server/api/tarsstat_monitor_data
取tarsstat监控数据
### 参数
```javascript
thedate         // 显示日期
predate         // 对比日期
startshowtime   // 开始时间
endshowtime     // 结束时间
master_name     // 主调
slave_name      // 被调
interface_name  // 接口名
master_ip       // 主调IP
slave_ip        // 被调IP
group_by        // 分组
```
### 返回值
```javascript
[{
    "show_date": "",          // 日期
    "show_time": "",          // 时间点
    "master_name": "",        // 主调
    "slave_name": "",         // 被调
    "interface_name": "",     // 接口名
    "master_ip": "",          // 主调IP
    "slave_ip": "",           // 被调IP
    "the_total_count": "",    // 当日总流量
    "pre_total_count": "",    // 对比日总流量
    "total_count_wave": "",   // 流量同比波动
    "the_avg_time": "",       // 当日平均耗时
    "pre_avg_time": "",       // 对比日平均耗时
    "the_fail_rate": "",      // 当日失败率
    "pre_fail_rate": "",      // 对比日失败率
    "the_timeout_rate": "",   // 当日失败率
    "pre_timeout_rate": ""    // 对比日失败率
}]
```

## server/api/tarsproperty_monitor_data
取tarsproperty监控数据
### 参数
```javascript
thedate         // 显示日期
predate         // 对比日期
startshowtime   // 开始时间
endshowtime     // 结束时间
master_name     // 服务名
master_ip       // IP
property_name   // 特性
policy          // 策略
```
### 返回值
```javascript
[{
    "show_date": "",          // 日期
    "show_time": "",          // 时间点
    "master_name": "",        // 服务名
    "master_ip": "",          // IP
    "property_name": "",      // 特性
    "policy": "",             // 策略
    "the_value": "",          // 当日特征值
    "pre_value": ""           // 对比日特征值
}]
```

# 字典
## server/api/server_type_list
取服务类型列表
### 参数
无
### 返回值
```javascript
["tars_cpp"]
```	

## server/api/template_name_list
取模板列表
### 参数
无
### 返回值
```javascript
["tars.default"]
```

## server/api/cascade_select_server
级联选择服务
### 参数
```javascript
level        // 层级，1：应用，2：服务，3：Set，4：节点
application  // 应用，level>1时必填
server_name  // 服务，level>2时必填
set          // Set，level>3时必填，格式：Set名.Set区.Set组
```
### 返回值
```javascript
[""] 对应层级数据
```

# 其他
## server/api/send_command
发送自定义命令
### 参数
```javascript
server_ids // 服务ID
command    // 命令
```
### 返回值
```javascript
[{
	"application": "", // 应用
	"server_name": "", // 服务
	"node_name": "",   // 节点
	"ret_code": "",    // 返回值
	"err_msg": ""      // 错误消息
}]
```	

