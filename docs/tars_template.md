# 目录
> * [1 模版配置介绍] (#main-chapter-1)
> * [2 模版配置]     (#main-chapter-2)
>>* [2.1 tars.default 模板](#chapter-1)
>>* [2.2 tars.tarsconfig 模板] (#chapter-2)
>>* [2.3 tars.tarslog 模板] (#chapter-3)
>>* [2.4 tars.tarsnotify 模板] (#chapter-4)
>>* [2.5 tars.tarspatch 模版] (#chapter-5)
>>* [2.6 tars.tarsproperty 模版] (#chapter-6)
>>* [2.7 tars.tarsstat 模版] (#chapter-7)
>>* [2.8 tars.tarsquerystat 模版] (#chapter-8)
>>* [2.9 tars.tarsqueryproperty 模版] (#chapter-9)
>>* [2.10 tars.tarsjava.default 模版] (#chapter-10)

# 1. 模版配置介绍 <a id="main-chapter-1"></a>

每个Tars服务启动运行时，必须指定一个模版配置文件，在Tars web管理系统中部署的服务的模版配置由node进行组织生成，若不是在web管理系统上，则需要自己创建一个模版文件。

框架的模版配置文件由父模版、主模版、私有模版配置来组织。

父模版和主模版的配置信息，存放在db_tars/t_profile_template数据表里，表结构如下：

![tars-template](images/tars_template_table1.png)

私有模版信息，存放在db_tars/t_server_conf数据表的profile字段里面。
# 2. 模板配置  <a id="main-chapter-2"></a>

注意模版中有db.tars.com时，需要修改成部署数据库的机器ip

## 2.1. tars.default 模板 <a id="chapter-1"></a>

模版名称:tars.default(所有服务的模版都直接或者间接继承这个模版)

内容:
```xml
<tars>
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			stat=tars.tarsstat.StatObj
			property=tars.tarsproperty.PropertyObj
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
</tars>
```

## 2.2. tars.tarsconfig模版 <a id="chapter-2"></a>

模版名称:tars.tarsconfig(继承tars.default)

内容:
```xml
<tars>
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
	<db>
		charset=utf8
		dbhost=db.tars.com
		dbname=db_tars
		dbpass=tars2015
		dbport=3306
		dbuser=tars
	</db>
</tars>
```

## 2.3. tars.tarslog模版 <a id="chapter-3"></a>

模版名称:tars.tarslog(继承tars.default)

内容:
```xml
<tars>
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			stat=tars.tarsstat.StatObj
			property=tars.tarsproperty.PropertyObj
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=100000000
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=ERROR
		</server>
	</application>
	<log>
		logpath=/usr/local/app/tars/remote_app_log
		logthread=10
		<format>
			hour=xx
		</format>
	</log>
</tars>

```
## 2.4. tars.tarsnotify模版 <a id="chapter-4"></a>

模版名称:tars.tarsnotify(继承tars.default)

内容:
```xml
<tars>
	sql=CREATE TABLE `${TABLE}` (   `id` int(11) NOT NULL AUTO_INCREMENT,  `application` varchar(128) DEFAULT '',  `server_name` varchar(128) DEFAULT NULL, `container_name` varchar(128) DEFAULT '' , `node_name` varchar(128) NOT NULL DEFAULT '',  `set_name` varchar(16) DEFAULT NULL,  `set_area` varchar(16) DEFAULT NULL,  `set_group` varchar(16) DEFAULT NULL,  `server_id` varchar(100) DEFAULT NULL,  `thread_id` varchar(20) DEFAULT NULL,  `command` varchar(50) DEFAULT NULL,  `result` text,  `notifytime` datetime DEFAULT NULL,  PRIMARY KEY (`id`),  KEY `index_name` (`server_name`),  KEY `servernoticetime_i_1` (`notifytime`),  KEY `indx_1_server_id` (`server_id`),  KEY `query_index` (`application`,`server_name`,`node_name`,`set_name`,`set_area`,`set_group`) ) ENGINE\=InnoDB DEFAULT CHARSET\=utf8
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
	<db>
		charset=utf8
		dbhost=db.tars.com
		dbpass=tars2015
		dbport=3306
		dbuser=tars
		dbname=db_tars
	</db>
	<hash>
		min_block=50
		max_block=200
		factor=1.5
		file_path=./notify
		file_size=50000000
		max_page_num=30
		max_page_size=20
	</hash>
</tars>

```
## 2.5. tars.tarspatch模版 <a id="chapter-5"></a>

模版名称:tars.tarspatch(继承tars.default)

内容:
```xml
<tars>
	directory=/usr/local/app/patchs/tars
	uploadDirectory=/usr/local/app/patchs/tars.upload
	size=100M
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
</tars>
```

## 2.6. tars.tarsproperty模版 <a id="chapter-6"></a>

模版名称:tars.tarsproperty(继承tars.default)

内容:
```xml
<tars>
	sql=CREATE TABLE `${TABLE}` (`stattime` timestamp NOT NULL default CURRENT_TIMESTAMP,`f_date` date NOT NULL default '1970-01-01', `f_tflag` varchar(8) NOT NULL default '',`master_name` varchar(128) NOT NULL default '',`master_ip` varchar(16) default NULL,`property_name` varchar(100) default NULL,`set_name` varchar(15) NOT NULL default '',`set_area` varchar(15) NOT NULL default '',`set_id` varchar(15) NOT NULL default '',`policy` varchar(20) default NULL,`value` varchar(255) default NULL, KEY (`f_date`,`f_tflag`,`master_name`,`master_ip`,`property_name`,`policy`),KEY `IDX_MASTER_NAME` (`master_name`),KEY `IDX_MASTER_IP` (`master_ip`),KEY `IDX_TIME` (`stattime`)) ENGINE\=Innodb
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
	<db>
		charset
		dbhost=db.tars.com
		dbname=tars
		dbport=3306
		dbuser=tars
		dbpass=tars2015
	</db>
	<multidb>
		<db1>
			dbhost=db.tars.com
			dbname=tars_property
			tbname=tars_property_
			dbuser=tars
			dbpass=tars2015
			dbport=3306
			charset=utf8
		</db1>
		<db2>
			dbhost=db.tars.com
			dbname=tars_property
			tbname=tars_property_
			dbuser=tars
			dbpass=tars2015
			dbport=3306
			charset=utf8
		</db2>
	</multidb>
	<hashmap>
		factor=1.5
		file=hashmap.txt
		insertInterval=5
		maxBlock=200
		minBlock=100
		size=10M
	</hashmap>
	<reapSql>
		Interval=10
		sql=insert ignore into t_master_property select  master_name, property_name, policy from ${TABLE}  group by  master_name, property_name, policy;
	</reapSql>
</tars>

```
 
## 2.7. tars.tarsstat模版 <a id="chapter-7"></a>

模版名称:tars.tarsstat(继承tars.default)

内容:
```xml
<tars>
	sql=CREATE TABLE `${TABLE}`( `stattime` timestamp NOT NULL default CURRENT_TIMESTAMP,`f_date` date NOT NULL default '1970-01-01', `f_tflag` varchar(8) NOT NULL default '',`source_id` varchar(15) default NULL,`master_name` varchar(64) default NULL,`slave_name` varchar(64) default NULL,`interface_name` varchar(64) default NULL,`tars_version` varchar(16) NOT NULL default '',`master_ip` varchar(15) default NULL,`slave_ip` varchar(21) default NULL,`slave_port` int(10) default NULL,`return_value` int(11) default NULL,`succ_count` int(10) unsigned default NULL,`timeout_count` int(10) unsigned default NULL,`exce_count` int(10) unsigned default NULL,`interv_count` varchar(128) default NULL,`total_time` bigint(20) unsigned default NULL,`ave_time` int(10) unsigned default NULL,`maxrsp_time` int(10) unsigned default NULL,`minrsp_time` int(10) unsigned default NULL,PRIMARY KEY (`source_id`,`f_date`,`f_tflag`,`master_name`,`slave_name`,`interface_name`,`master_ip`,`slave_ip`,`slave_port`,`return_value`,`tars_version`),KEY `IDX_TIME` (`stattime`),KEY `IDC_MASTER` (`master_name`),KEY `IDX_INTERFACENAME` (`interface_name`),KEY `IDX_FLAGSLAVE` (`f_tflag`,`slave_name`), KEY `IDX_SLAVEIP` (`slave_ip`),KEY `IDX_SLAVE` (`slave_name`),KEY `IDX_RETVALUE` (`return_value`),KEY `IDX_MASTER_IP` (`master_ip`),KEY `IDX_F_DATE` (`f_date`)) ENGINE\=MyISAM DEFAULT CHARSET\=utf8
	enWeighted=1
	useolddatabase=0
	time_out=600
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			report-interval=60000
			sample-rate=100000
			max-sample-count=50
			asyncthread=${asyncthread}
			modulename=${modulename}
		</client>
		<server>
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			logsize=10M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			logLevel=DEBUG
		</server>
	</application>
	<masteripGroup>
		tars.tarsstat;1.1.1.1
	</masteripGroup>
	<hashmap>
		masterfile=hashmap_master.txt
		slavefile=hashmap_slave.txt
		insertInterval=5
		enableStatCount=0
		size=8M
		countsize=1M
	</hashmap>
	<reapSql>
		interval=5
		insertDbThreadNum=4
	</reapSql>
	<multidb>
        <db1>
			dbhost=db.tars.com
			dbname=tars_stat
			tbname=tars_stat_
			dbuser=tars
			dbpass=tars2015
			dbport=3306
			charset=utf8
        </db1>
	</multidb>
</tars>

```
## 2.8. tars.tarsquerystat模版 <a id="chapter-8"></a>
模版名称:tars.tarsquerystat(继承tars.default)
``` 
<tars>
    <application>
    #是否启用SET分组
    enableset=${enableset}
    #SET分组的全名.(mtt.s.1)
    setdivision=${setdivision}
    <client>
        #地址
        locator =${locator}
        #同步调用超时时间,缺省3s(毫秒)
        sync-invoke-timeout = 3000
        #异步超时时间,缺省5s(毫秒)
        async-invoke-timeout =5000
        #重新获取服务列表时间间隔(毫秒)
        refresh-endpoint-interval = 60000
        #模块间调用服务[可选]
        stat            = tars.tarsstat.StatObj
        #属性上报服务[可选]
        property                    = tars.tarsproperty.PropertyObj
        #上报间隔时间,默认60s(毫秒)
        report-interval            = 60000
        #stat采样比1:n 例如sample-rate为1000时 采样比为千分之一
         sample-rate = 100000
        #1分钟内stat最大采样条数
         max-sample-count = 50

        #网络异步回调线程个数
        asyncthread      = ${asyncthread}
        #模块名称
        modulename      = ${modulename}
    </client>
        
    #定义所有绑定的IP
    <server>
        #应用名称
        app      = ${app}
        #服务名称
        server  = ${server}
        #本地ip
       localip  = ${localip}

        #本地管理套接字[可选]
        local  = ${local}
        #服务的数据目录,可执行文件,配置文件等
        basepath = ${basepath}
        #
        datapath = ${datapath}
        #日志路径
        logpath  = ${logpath}
        #日志大小
        logsize = 10M
        #日志数量
        #   lognum = 10
        #配置中心的地址[可选]
        config  = tars.tarsconfig.ConfigObj
        #信息中心的地址[可选]
        notify  = tars.tarsnotify.NotifyObj
        #远程LogServer[可选]
        log = tars.tarslog.LogObj
        #关闭服务时等待时间
         deactivating-timeout = 3000
        #滚动日志等级默认值
   logLevel=DEBUG
    </server>          
    </application>
    <countdb>
		<db1>
			dbhost=db.tars.com
		   	dbname=tars_stat
			tbname=tars_stat_
			dbuser=tars
			dbpass=tars2015
			dbport=3306
			charset=utf8
		</db1>
    </countdb>
    </tars>
>
```

## 2.9.tars.tarsqueryproperty 模版 <a id="chapter-9"></a>
模版名称:tars.tarsqueryproperty(继承tars.default)
``` 
<tars>
    <application>
    #是否启用SET分组
    enableset=${enableset}
    #SET分组的全名.(mtt.s.1)
    setdivision=${setdivision}
    <client>
        #地址
        locator =${locator}
        #同步调用超时时间,缺省3s(毫秒)
        sync-invoke-timeout = 3000
        #异步超时时间,缺省5s(毫秒)
        async-invoke-timeout =5000
        #重新获取服务列表时间间隔(毫秒)
        refresh-endpoint-interval = 60000
        #模块间调用服务[可选]
        stat            = tars.tarsstat.StatObj
        #属性上报服务[可选]
        property                    = tars.tarsproperty.PropertyObj
        #上报间隔时间,默认60s(毫秒)
        report-interval            = 60000
        #stat采样比1:n 例如sample-rate为1000时 采样比为千分之一
         sample-rate = 100000
        #1分钟内stat最大采样条数
         max-sample-count = 50

        #网络异步回调线程个数
        asyncthread      = ${asyncthread}
        #模块名称
        modulename      = ${modulename}
    </client>
        
    #定义所有绑定的IP
    <server>
        #应用名称
        app      = ${app}
        #服务名称
        server  = ${server}
        #本地ip
       localip  = ${localip}

        #本地管理套接字[可选]
        local  = ${local}
        #服务的数据目录,可执行文件,配置文件等
        basepath = ${basepath}
        #
        datapath = ${datapath}
        #日志路径
        logpath  = ${logpath}
        #日志大小
        logsize = 10M
        #日志数量
        #   lognum = 10
        #配置中心的地址[可选]
        config  = tars.tarsconfig.ConfigObj
        #信息中心的地址[可选]
        notify  = tars.tarsnotify.NotifyObj
        #远程LogServer[可选]
        log = tars.tarslog.LogObj
        #关闭服务时等待时间
         deactivating-timeout = 3000
        #滚动日志等级默认值
   logLevel=DEBUG
    </server>          
    </application>
    <countdb>
		<db1>
			dbhost=10.121.108.158
			dbname=tars_property
			tbname=tars_property_
			dbuser=tars
			dbpass=tars2015
			dbport=3306
			charset=utf8
		</db1>
    </countdb>
    </tars>
>
```
## 2.10. tars.tarsjava.default模版 <a id="chapter-10"></a>

模版名称:.tars.tarsjava.default(继承tars.default，所有tarsjava服务的模版都直接或者间接继承这个模版)

内容:
```xml
<tars>
	<application>
		enableset=${enableset}
		setdivision=${setdivision}
		<client>
			asyncthread=${asyncthread}
			locator=${locator}
			sync-invoke-timeout=3000
			async-invoke-timeout=5000
			refresh-endpoint-interval=60000
			stat=tars.tarsstat.StatObj
			property=tars.tarsproperty.PropertyObj
			report-interval=60000
			modulename=${modulename}
			sample-rate=1000000
			max-sample-count=10
		</client>
		<server>
			deactivating-timeout=2000
			openthreadcontext=0
			threadcontextnum=10000
			threadcontextstack=32768
			logLevel=DEBUG
			app=${app}
			server=${server}
			localip=${localip}
			local=${local}
			basepath=${basepath}
			datapath=${datapath}
			logpath=${logpath}
			loglevel=DEBUG
			logsize=15M
			log=tars.tarslog.LogObj
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			mainclass=com.qq.cloud.tars.server.startup.Main
			classpath=${basepath}/conf:${basepath}/lib
			jvmparams=-Dcom.sun.management.jmxremote.ssl\=false -Dcom.sun.management.jmxremote.authenticate\=false -Xms2000m -Xmx2000m -Xmn1000m -Xss1000k -XX:PermSize\=128M -XX:+UseConcMarkSweepGC -XX:CMSInitiatingOccupancyFraction\=60 -XX:+PrintGCApplicationStoppedTime -XX:+PrintGCDateStamps -XX:+CMSParallelRemarkEnabled -XX:+CMSScavengeBeforeRemark -XX:+UseCMSCompactAtFullCollection -XX:CMSFullGCsBeforeCompaction\=0 -verbosegc -XX:+PrintGCDetails -XX:ErrorFile\=${logpath}/${app}/${server}/jvm_error.log
			sessiontimeout=120000
			sessioncheckinterval=60000
			tcpnodelay=true
			udpbuffersize=8192
			charsetname=UTF-8
			backupfiles=bak1;bak2;bak3;conf
		</server>
	</application>
</tars>

```
