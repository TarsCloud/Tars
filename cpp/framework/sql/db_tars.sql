-- MySQL dump 10.13  Distrib 5.6.26, for Linux (x86_64)
--
-- Host: localhost    Database: db_tars
-- ------------------------------------------------------
-- Server version	5.6.26-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_adapter_conf`
--

DROP TABLE IF EXISTS `t_adapter_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_adapter_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(50) DEFAULT '',
  `server_name` varchar(128) DEFAULT '',
  `node_name` varchar(50) DEFAULT '',
  `adapter_name` varchar(100) DEFAULT '',
  `registry_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `thread_num` int(11) DEFAULT '1',
  `endpoint` varchar(128) DEFAULT '',
  `max_connections` int(11) DEFAULT '1000',
  `allow_ip` varchar(255) NOT NULL DEFAULT '',
  `servant` varchar(128) DEFAULT '',
  `queuecap` int(11) DEFAULT NULL,
  `queuetimeout` int(11) DEFAULT NULL,
  `posttime` datetime DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  `protocol` varchar(64) DEFAULT 'tars',
  `handlegroup` varchar(64) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`application`,`server_name`,`node_name`,`adapter_name`),
  KEY `adapter_conf_endpoint_index` (`endpoint`),
  KEY `index_regtime_1` (`registry_timestamp`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_adapter_conf`
--

LOCK TABLES `t_adapter_conf` WRITE;
/*!40000 ALTER TABLE `t_adapter_conf` DISABLE KEYS */;
INSERT INTO `t_adapter_conf` VALUES (1,'tars','tarspatch','192.168.2.131','tars.tarspatch.PatchObjAdapter','2015-11-28 16:16:55',10,'tcp -h 192.168.2.131 -t 60000 -p 10000',200000,'','tars.tarspatch.PatchObj',10000,60000,'2015-11-28 11:16:55','admin','tars',''),(2,'tars','tarsconfig','192.168.2.131','tars.tarsconfig.ConfigObjAdapter','2015-11-28 16:16:26',10,'tcp -h 192.168.2.131 -t 60000 -p 10001',200000,'','tars.tarsconfig.ConfigObj',10000,60000,'2015-11-28 11:16:26','admin','tars',''),(21,'tars','tarsnotify','192.168.2.131','tars.tarsnotify.NotifyObjAdapter','2015-11-29 02:48:13',5,'tcp -h 192.168.2.131 -t 60000 -p 10002',200000,'','tars.tarsnotify.NotifyObj',10000,60000,'2015-11-28 21:48:13','admin','tars','');
/*!40000 ALTER TABLE `t_adapter_conf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ats_cases`
--

DROP TABLE IF EXISTS `t_ats_cases`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ats_cases` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `casename` varchar(20) DEFAULT NULL,
  `retvalue` text,
  `paramvalue` text,
  `interfaceid` int(11) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ats_cases`
--

LOCK TABLES `t_ats_cases` WRITE;
/*!40000 ALTER TABLE `t_ats_cases` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ats_cases` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ats_interfaces`
--

DROP TABLE IF EXISTS `t_ats_interfaces`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ats_interfaces` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `objname` varchar(150) DEFAULT NULL,
  `funcname` varchar(150) DEFAULT NULL,
  `retype` text,
  `paramtype` text,
  `outparamtype` text,
  `interfaceid` int(11) DEFAULT NULL,
  `postime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  `request_charset` varchar(16) NOT NULL,
  `response_charset` varchar(16) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `objname` (`objname`,`funcname`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ats_interfaces`
--

LOCK TABLES `t_ats_interfaces` WRITE;
/*!40000 ALTER TABLE `t_ats_interfaces` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ats_interfaces` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_files`
--

DROP TABLE IF EXISTS `t_config_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server_name` varchar(128) DEFAULT '',
  `set_name` varchar(16) NOT NULL DEFAULT '',
  `set_area` varchar(16) NOT NULL DEFAULT '',
  `set_group` varchar(16) NOT NULL DEFAULT '',
  `host` varchar(20) NOT NULL DEFAULT '',
  `filename` varchar(128) DEFAULT NULL,
  `config` longtext,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  `level` int(11) DEFAULT '2',
  `config_flag` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`server_name`,`filename`,`host`,`level`,`set_name`,`set_area`,`set_group`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_files`
--

LOCK TABLES `t_config_files` WRITE;
/*!40000 ALTER TABLE `t_config_files` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_config_files` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_history_files`
--

DROP TABLE IF EXISTS `t_config_history_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_history_files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `configid` int(11) DEFAULT NULL,
  `reason` varchar(128) DEFAULT '',
  `reason_select` varchar(20) NOT NULL DEFAULT '',
  `content` longtext,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_history_files`
--

LOCK TABLES `t_config_history_files` WRITE;
/*!40000 ALTER TABLE `t_config_history_files` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_config_history_files` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_references`
--

DROP TABLE IF EXISTS `t_config_references`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_references` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `config_id` int(11) DEFAULT NULL,
  `reference_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `config_id` (`config_id`,`reference_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_references`
--

LOCK TABLES `t_config_references` WRITE;
/*!40000 ALTER TABLE `t_config_references` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_config_references` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_group_priority`
--

DROP TABLE IF EXISTS `t_group_priority`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_group_priority` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(128) DEFAULT '',
  `group_list` text,
  `list_order` int(11) DEFAULT '0',
  `station` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_group_priority`
--

LOCK TABLES `t_group_priority` WRITE;
/*!40000 ALTER TABLE `t_group_priority` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_group_priority` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_machine_tars_info`
--

DROP TABLE IF EXISTS `t_machine_tars_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_machine_tars_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(100) NOT NULL DEFAULT '',
  `server_name` varchar(100) NOT NULL DEFAULT '',
  `app_server_name` varchar(50) NOT NULL DEFAULT '',
  `node_name` varchar(50) NOT NULL DEFAULT '',
  `location` varchar(255) NOT NULL DEFAULT '',
  `machine_type` varchar(50) NOT NULL DEFAULT '',
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `update_person` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`application`,`server_name`,`node_name`),
  UNIQUE KEY `id` (`id`),
  KEY `tmachine_i_2` (`node_name`,`server_name`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_machine_tars_info`
--

LOCK TABLES `t_machine_tars_info` WRITE;
/*!40000 ALTER TABLE `t_machine_tars_info` DISABLE KEYS */;
/*INSERT INTO `t_machine_tars_info` VALUES (2,'tars','tarsconfig','tars.tarsconfig','192.168.2.131','','','2015-08-07 16:04:03','admin'),(3,'tars','tarsnotify','tars.tarsnotify','192.168.2.131','','','2015-08-09 02:13:54','admin'),(1,'tars','tarspatch','tars.tarspatch','192.168.2.131','','','2015-08-07 15:57:50','admin');*/
/*!40000 ALTER TABLE `t_machine_tars_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_node_info`
--

DROP TABLE IF EXISTS `t_node_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_node_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `node_name` varchar(128) DEFAULT '',
  `node_obj` varchar(128) DEFAULT '',
  `endpoint_ip` varchar(16) DEFAULT '',
  `endpoint_port` int(11) DEFAULT '0',
  `data_dir` varchar(128) DEFAULT '',
  `load_avg1` float DEFAULT '0',
  `load_avg5` float DEFAULT '0',
  `load_avg15` float DEFAULT '0',
  `last_reg_time` datetime DEFAULT '1970-01-01 00:08:00',
  `last_heartbeat` datetime DEFAULT '1970-01-01 00:08:00',
  `setting_state` enum('active','inactive') DEFAULT 'inactive',
  `present_state` enum('active','inactive') DEFAULT 'inactive',
  `tars_version` varchar(128) NOT NULL DEFAULT '',
  `template_name` varchar(128) NOT NULL DEFAULT '',
  `modify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `group_id` int(11) DEFAULT '-1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `node_name` (`node_name`),
  KEY `indx_node_info_1` (`last_heartbeat`)
) ENGINE=InnoDB AUTO_INCREMENT=68 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_node_info`
--

LOCK TABLES `t_node_info` WRITE;
/*!40000 ALTER TABLE `t_node_info` DISABLE KEYS */;
INSERT INTO `t_node_info` VALUES (67,'192.168.2.131','tars.tarsnode.NodeObj@tcp -h 192.168.2.131 -p 19385 -t 60000','192.168.2.131',19385,'/usr/local/app/tars/tarsnode/data',0,0.12,0.39,'2015-11-29 13:36:07','2015-11-29 22:15:46','active','active','1.0.1_B001','','2015-11-29 14:15:46',-1);
/*!40000 ALTER TABLE `t_node_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_profile_template`
--

DROP TABLE IF EXISTS `t_profile_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_profile_template` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `template_name` varchar(128) DEFAULT '',
  `parents_name` varchar(128) DEFAULT '',
  `profile` text NOT NULL,
  `posttime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `template_name` (`template_name`)
) ENGINE=InnoDB AUTO_INCREMENT=366 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_profile_template`
--

LOCK TABLES `t_profile_template` WRITE;
/*!40000 ALTER TABLE `t_profile_template` DISABLE KEYS */;
INSERT INTO `t_profile_template` VALUES (353,'tars.default','tars.default','<tars>\n    <application>\n    #是否启用SET分组\n    enableset=${enableset}\n    #SET分组的全名.(mtt.s.1)\n    setdivision=${setdivision}\n    <client>\n        #地址\n        locator =${locator}\n        #同步调用超时时间,缺省3s(毫秒)\n        sync-invoke-timeout = 3000\n        #异步超时时间,缺省5s(毫秒)\n        async-invoke-timeout =5000\n        #重新获取服务列表时间间隔(毫秒)\n        refresh-endpoint-interval = 60000\n        #模块间调用服务[可选]\n        stat            = tars.tarsstat.StatObj\n        #属性上报服务[可选]\n        property                    = tars.tarsproperty.PropertyObj\n        #上报间隔时间,默认60s(毫秒)\n        report-interval            = 60000\n        #stat采样比1:n 例如sample-rate为1000时 采样比为千分之一\n         sample-rate = 100000\n        #1分钟内stat最大采样条数\n         max-sample-count = 50\n\n        #网络异步回调线程个数\n        asyncthread      = ${asyncthread}\n        #模块名称\n        modulename      = ${modulename}\n    </client>\n        \n    #定义所有绑定的IP\n    <server>\n        #应用名称\n        app      = ${app}\n        #服务名称\n        server  = ${server}\n        #本地ip\n       localip  = ${localip}\n\n        #本地管理套接字[可选]\n        local  = ${local}\n        #服务的数据目录,可执行文件,配置文件等\n        basepath = ${basepath}\n        #\n        datapath = ${datapath}\n        #日志路径\n        logpath  = ${logpath}\n        #日志大小\n        logsize = 10M\n        #日志数量\n        #   lognum = 10\n        #配置中心的地址[可选]\n        config  = tars.tarsconfig.ConfigObj\n        #信息中心的地址[可选]\n        notify  = tars.tarsnotify.NotifyObj\n        #远程LogServer[可选]\n        log = tars.tarslog.LogObj\n        #关闭服务时等待时间\n         deactivating-timeout = 3000\n        #滚动日志等级默认值\n   logLevel=DEBUG\n    </server>          \n    </application>\n    </tars>','2016-09-22 17:36:01','admin'),(354,'tars.tarspatch','tars.default','<tars>\n    <application>\n    <server>\n        log    = tars.tarslog.LogObj\n        logLevel = DEBUG\n    </server>          \n    </application>\n\n    directory=/usr/local/app/patchs/tars\n    uploadDirectory=/usr/local/app/patchs/tars.upload\n    size=1M\n</tars>','2015-08-07 22:06:46','admin'),(355,'tars.tarsconfig','tars.default','<tars>\n    <application>\n    <server>\n        log    = tars.tarslog.LogObj\n        logLevel = DEBUG\n    </server>          \n    </application>\n    <db>\n        charset=utf8\n        dbhost=db.tars.com\n  dbname=db_tars\n        dbpass=tars2015\n dbport=3306\n dbuser=tars\n\n    </db>\n</tars>','2015-08-07 22:05:24','admin'),(356,'tars.tarsnotify','tars.default','<tars>\n    <application>\n    <server>\n        log    = tars.tarslog.LogObj\n        logLevel = DEBUG\n    </server>          \n    </application>\n\n    <hash> \n min_block=50 \n max_block=200 \n factor=1.5 \n  file_path=./notify \n file_size=50000000 \n  max_page_num=30 \n max_page_size=20 \n </hash> \n <db>\n        charset=utf8\n        dbhost=db.tars.com\n        dbname=db_tars\n        dbpass=tars2015\n        dbport=3306\n        dbuser=tars\n        dbname=db_tars\n\n    </db>\n\n    sql=CREATE TABLE `${TABLE}` (   `id` int(11) NOT NULL AUTO_INCREMENT,  `application` varchar(128) DEFAULT \'\',  `server_name` varchar(128) DEFAULT NULL, `container_name` varchar(128) DEFAULT \'\' , `node_name` varchar(128) NOT NULL DEFAULT \'\',  `set_name` varchar(16) DEFAULT NULL,  `set_area` varchar(16) DEFAULT NULL,  `set_group` varchar(16) DEFAULT NULL,  `server_id` varchar(100) DEFAULT NULL,  `thread_id` varchar(20) DEFAULT NULL,  `command` varchar(50) DEFAULT NULL,  `result` text,  `notifytime` datetime DEFAULT NULL,  PRIMARY KEY (`id`),  KEY `index_name` (`server_name`),  KEY `servernoticetime_i_1` (`notifytime`),  KEY `indx_1_server_id` (`server_id`),  KEY `query_index` (`application`,`server_name`,`node_name`,`set_name`,`set_area`,`set_group`) ) ENGINE\\=InnoDB DEFAULT CHARSET\\=utf8\n</tars>','2016-09-26 15:21:52','admin'),(359,'tars.tarsstat','tars.default','<tars>\n       sql= CREATE TABLE `${TABLE}`( `stattime` timestamp NOT NULL default CURRENT_TIMESTAMP,`f_date` date NOT NULL default \'1970-01-01\', `f_tflag` varchar(8) NOT NULL default \'\',`source_id` varchar(15) NOT NULL default \'\',`master_name` varchar(128) NOT NULL default \'\',`slave_name` varchar(128) NOT NULL default \'\',`interface_name` varchar(128) NOT NULL default \'\',`tars_version` varchar(16) NOT NULL default \'\',`master_ip` varchar(15) NOT NULL default \'\',`slave_ip` varchar(21) NOT NULL default \'\',`slave_port` int(10) NOT NULL default 0,`return_value` int(11) NOT NULL default 0,`succ_count` int(10) unsigned default NULL,`timeout_count` int(10) unsigned default NULL,`exce_count` int(10) unsigned default NULL,`interv_count` varchar(128) default NULL,`total_time` bigint(20) unsigned default NULL,`ave_time` int(10) unsigned default NULL,`maxrsp_time` int(10) unsigned default NULL,`minrsp_time` int(10) unsigned default NULL,PRIMARY KEY (`source_id`,`f_date`,`f_tflag`,`master_name`,`slave_name`,`interface_name`,`master_ip`,`slave_ip`,`slave_port`,`return_value`,`tars_version`),KEY `IDX_TIME` (`stattime`),KEY `IDC_MASTER` (`master_name`),KEY `IDX_INTERFACENAME` (`interface_name`),KEY `IDX_FLAGSLAVE` (`f_tflag`,`slave_name`), KEY `IDX_SLAVEIP` (`slave_ip`),KEY `IDX_SLAVE` (`slave_name`),KEY `IDX_RETVALUE` (`return_value`),KEY `IDX_MASTER_IP` (`master_ip`),KEY `IDX_F_DATE` (`f_date`)) ENGINE\\=InnoDB DEFAULT CHARSET\\=utf8\n  enWeighted=1\n        \n  <masteripGroup>\n   tars.tarsstat;1.1.1.1\n </masteripGroup>\n  <hashmap>\n   masterfile=hashmap_master.txt\n   slavefile=hashmap_slave.txt\n   insertInterval=5\n    enableStatCount=0\n   size=8M\n                countsize=1M\n </hashmap>\n  <reapSql>\n   interval=5\n    insertDbThreadNum=4\n </reapSql>\n        <multidb>\n   <db1>\n     dbhost=db.tars.com\n      dbname=tars_stat\n      tbname=tars_stat_\n     dbuser=tars\n     dbpass=tars2015\n     dbport=3306\n     charset=utf8\n    </db1>\n  </multidb>\n  \n</tars>','2016-09-26 17:25:45','admin'),(365,'tars.tarsjava.default','tars.default','<tars>\n <application>\n   enableset=${enableset}\n    setdivision=${setdivision}\n    <client>\n      locator=${locator}\n      sync-invoke-timeout=20000\n     async-invoke-timeout=20000\n      refresh-endpoint-interval=60000\n     stat=tars.tarsstat.StatObj\n      property=tars.tarsproperty.PropertyObj\n      report-interval=60000\n     modulename=${modulename}\n      sample-rate=100000\n      max-sample-count=50\n   </client>\n   <server>\n      app=${app}\n      server=${server}\n      localip=${localip}\n      local=${local}\n      basepath=${basepath}\n      datapath=${datapath}\n      logpath=${logpath}\n      loglevel=DEBUG\n      logsize=15M\n     log=tars.tarslog.LogObj\n     config=tars.tarsconfig.ConfigObj\n      notify=tars.tarsnotify.NotifyObj\n      mainclass=com.qq.tars.server.startup.Main\n     classpath=${basepath}/conf:${basepath}/WEB-INF/classes:${basepath}/WEB-INF/lib\n      jvmparams=-Dcom.sun.management.jmxremote.ssl\\=false -Dcom.sun.management.jmxremote.authenticate\\=false -Xms2000m -Xmx2000m -Xmn1000m -Xss1000k -XX:PermSize\\=128M -XX:+UseConcMarkSweepGC -XX:CMSInitiatingOccupancyFraction\\=60 -XX:+PrintGCApplicationStoppedTime -XX:+PrintGCDateStamps -XX:+CMSParallelRemarkEnabled -XX:+CMSScavengeBeforeRemark -XX:+UseCMSCompactAtFullCollection -XX:CMSFullGCsBeforeCompaction\\=0 -verbosegc -XX:+PrintGCDetails -XX:ErrorFile\\=${logpath}/${app}/${server}/jvm_error.log\n      sessiontimeout=120000\n     sessioncheckinterval=60000\n      tcpnodelay=true\n     udpbuffersize=8192\n      charsetname=UTF-8\n     backupfiles=conf\n    </server>\n </application>\n</tars>','2016-10-13 17:22:07','admin'),(358,'tars.tarsnode','tars.default','<tars>\n    <application>\n      enableset=n \n      setdivision=NULL\n        <client>\n            modulename=tars.tarsnode\n           locator=${locator}\n            #缺省3s(毫秒)\n            sync-invoke-timeout = 6000\n            asyncthread=3\n        </client>\n        <server>\n            app=tars\n            server=tarsnode\n            localip=${localip}\n            local = tcp -h 127.0.0.1 -p 19385 -t 10000\n            basepath=/usr/local/app/tars/tarsnode/data\n            datapath=/usr/local/app/tars/tarsnode/data\n            logpath=  /usr/local/app/tars/app_log\n            logLevel=DEBUG\n            #配置绑定端口\n            <NodeAdapter>\n                    #监听IP地址\n                    endpoint    = tcp -h ${localip} -p 19385 -t 60000\n                    #允许的IP地址\n                    allow      =\n                    #最大连接数\n                    maxconns    = 1024\n                    #当前线程个数\n                    threads    = 5\n                    #流量限制\n                    queuecap    = 10000\n                    #队列超时时间\n                    queuetimeout= 4000\n                    #处理对象\n                    servant    = tars.tarsnode.NodeObj\n            </NodeAdapter>\n\n            <ServerAdapter>\n                    #监听IP地址\n                    endpoint    = tcp -h  ${localip} -p 19386 -t 60000\n                    #允许的IP地址\n                    allow      =\n                    #最大连接数\n                    maxconns    = 1024\n                    #当前线程个数\n                    threads    = 5\n                    #流量限制\n                    queuecap    = 10000\n                    #队列超时时间\n                    queuetimeout= 4000\n                    #处理对象\n                    servant    = tars.tarsnode.ServerObj\n            </ServerAdapter>\n        </server>\n    </application>\n\n    <node>\n        registryObj = ${registryObj}\n        <keepalive>              \n            #业务心跳超时时间(s) \n            heartTimeout    = 60\n            \n            #监控server状态间隔时间(s) \n            monitorInterval = 2 \n            \n            #跟主控/本地cache同步服务状态间隔时间(s) \n            synStatInterval = 300\n        </keepalive> \n        \n        <hashmap>\n            file            =serversCache.dat\n            minBlock        =500\n            maxBlock        =500\n            factor          =1\n            size            =10M\n        </hashmap>\n    </node>\n</tars>','2015-08-07 22:04:39','admin'),(360,'tars.tarsproperty','tars.default','<tars>\n  sql=CREATE TABLE `${TABLE}` (`stattime` timestamp NOT NULL default CURRENT_TIMESTAMP,`f_date` date NOT NULL default \'1970-01-01\', `f_tflag` varchar(8) NOT NULL default \'\',`master_name` varchar(128) NOT NULL default \'\',`master_ip` varchar(16) default NULL,`property_name` varchar(100) default NULL,`set_name` varchar(15) NOT NULL default \'\',`set_area` varchar(15) NOT NULL default \'\',`set_id` varchar(15) NOT NULL default \'\',`policy` varchar(20) default NULL,`value` varchar(255) default NULL, KEY (`f_date`,`f_tflag`,`master_name`,`master_ip`,`property_name`,`policy`),KEY `IDX_MASTER_NAME` (`master_name`),KEY `IDX_MASTER_IP` (`master_ip`),KEY `IDX_TIME` (`stattime`)) ENGINE=Innodb\n\n <db>\n    charset\n   dbhost=db.tars.com\n    dbname=tars\n   dbport=3306\n   dbuser=tars\n   dbpass=tars2015\n </db>\n <multidb>\n   <db1>\n     dbhost=db.tars.com\n      dbname=tars_property\n      tbname=tars_property_\n     dbuser=tars\n     dbpass=tars2015\n     dbport=3306\n     charset=utf8\n    </db1>\n    <db2>\n     dbhost=db.tars.com\n      dbname=tars_property\n      tbname=tars_property_\n     dbuser=tars\n     dbpass=tars2015\n     dbport=3306\n     charset=utf8\n    </db2>\n  </multidb>\n  <hashmap>\n   factor=1.5\n    file=hashmap.txt\n    insertInterval=5\n    maxBlock=200\n    minBlock=100\n    size=10M\n  </hashmap>\n  <reapSql>\n   Interval=10\n   sql=insert ignore into t_master_property select  master_name, property_name, policy from ${TABLE}  group by  master_name, property_name, policy;\n  </reapSql>\n</tars>','2015-08-25 12:15:52','admin'),(362,'tars.tarslog','tars.default','<tars>\n     <application>\n          <server>\n               logLevel=ERROR\n              </server>\n     </application>\n     <log>\n          logpath=/usr/local/app/tars/remote_app_log\n          logthread=10         \n          <format>\n               hour=xx\n          </format>\n     </log>\n</tars>','2016-10-13 17:31:36','admin'),(366,'tars.tarsquerystat','tars.default','<tars>\n<countdb>\n<db1>\n      dbhost=db.tars.com\n      dbname=tars_stat\n      tbname=tars_stat_\n     dbuser=tars\n     dbpass=tars2015\n     dbport=3306\n     charset=utf8\n    </db1>\n</countdb>\n</tars>','2017-01-04 17:13:40',NULL),(367,'tars.tarsqueryproperty','tars.default','<tars>\n<countdb>\n<db1>\n     dbhost=db.tars.com\n      dbname=tars_property\n      tbname=tars_property_\n     dbuser=tars\n     dbpass=tars2015\n     dbport=3306\n     charset=utf8\n    </db1>\n</countdb>\n</tars>','2017-01-04 17:14:01',NULL);
/*!40000 ALTER TABLE `t_profile_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_registry_info`
--

DROP TABLE IF EXISTS `t_registry_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_registry_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `locator_id` varchar(128) NOT NULL DEFAULT '',
  `servant` varchar(128) NOT NULL DEFAULT '',
  `endpoint` varchar(128) NOT NULL DEFAULT '',
  `last_heartbeat` datetime DEFAULT '1970-01-01 00:08:00',
  `present_state` enum('active','inactive') DEFAULT 'inactive',
  `tars_version` varchar(128) NOT NULL DEFAULT '',
  `modify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `enable_group` char(1) DEFAULT 'N',
  PRIMARY KEY (`id`),
  UNIQUE KEY `locator_id` (`locator_id`,`servant`)
) ENGINE=InnoDB AUTO_INCREMENT=456487 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_registry_info`
--

LOCK TABLES `t_registry_info` WRITE;
/*!40000 ALTER TABLE `t_registry_info` DISABLE KEYS */;
INSERT INTO `t_registry_info` VALUES (456484,'192.168.2.131:17890','tars.tarsregistry.QueryObj','tcp -h 192.168.2.131 -p 17890 -t 10000','2015-11-29 22:16:16','active','1.0.1','2015-11-29 14:16:16','N'),(456485,'192.168.2.131:17890','tars.tarsregistry.RegistryObj','tcp -h 192.168.2.131 -p 17891 -t 30000','2015-11-29 22:16:16','active','1.0.1','2015-11-29 14:16:16','N'),(456486,'192.168.2.131:12000','tars.tarsAdminRegistry.AdminRegObj','tcp -h 192.168.2.131 -p 12000 -t 60000','2015-11-29 22:16:21','active','4.1.0.0_B002','2015-11-29 14:16:21','N');
/*!40000 ALTER TABLE `t_registry_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_conf`
--

DROP TABLE IF EXISTS `t_server_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(128) DEFAULT '',
  `server_name` varchar(128) DEFAULT '',
  `node_group` varchar(50) NOT NULL DEFAULT '',
  `node_name` varchar(50) NOT NULL DEFAULT '',
  `registry_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `base_path` varchar(128) DEFAULT '',
  `exe_path` varchar(128) NOT NULL DEFAULT '',
  `template_name` varchar(128) NOT NULL DEFAULT '',
  `bak_flag` int(11) NOT NULL DEFAULT '0',
  `setting_state` enum('active','inactive') NOT NULL DEFAULT 'inactive',
  `present_state` enum('active','inactive','activating','deactivating','destroyed') NOT NULL DEFAULT 'inactive',
  `process_id` int(11) NOT NULL DEFAULT '0',
  `patch_version` varchar(128) NOT NULL DEFAULT '',
  `patch_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `patch_user` varchar(128) NOT NULL DEFAULT '',
  `tars_version` varchar(128) NOT NULL DEFAULT '',
  `posttime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  `server_type` enum('tars_cpp','not_tars','tars_java', 'tars_nodejs') NOT NULL DEFAULT 'tars_cpp',
  `start_script_path` varchar(128) DEFAULT NULL,
  `stop_script_path` varchar(128) DEFAULT NULL,
  `monitor_script_path` varchar(128) DEFAULT NULL,
  `enable_group` char(1) DEFAULT 'N',
  `enable_set` char(1) NOT NULL DEFAULT 'N',
  `set_name` varchar(16) DEFAULT NULL,
  `set_area` varchar(16) DEFAULT NULL,
  `set_group` varchar(64) DEFAULT NULL,
  `ip_group_name` varchar(64) DEFAULT NULL,
  `profile` text,
  `config_center_port` int(11) NOT NULL DEFAULT '0',
  `async_thread_num` int(11) DEFAULT '3',
  `server_important_type` enum('0','1','2','3','4','5') DEFAULT '0',
  `remote_log_reserve_time` varchar(32) NOT NULL DEFAULT '65',
  `remote_log_compress_time` varchar(32) NOT NULL DEFAULT '2',
  `remote_log_type` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`application`,`server_name`,`node_name`),
  KEY `node_name` (`node_name`),
  KEY `index_i_3` (`setting_state`,`server_type`,`application`,`server_name`,`node_name`),
  KEY `index_regtime` (`registry_timestamp`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_conf`
--

LOCK TABLES `t_server_conf` WRITE;
/*!40000 ALTER TABLE `t_server_conf` DISABLE KEYS */;
INSERT INTO `t_server_conf` VALUES (1,'tars','tarspatch','','192.168.2.131','2015-11-29 11:08:09','','/usr/local/app/tars/tarspatch/bin/tarspatch','tars.tarspatch',0,'active','active',9855,'','0000-00-00 00:00:00','','1.0.1','2015-08-07 23:57:50','admin','tars_cpp','','','','N','N','','','','','',0,3,'0','65','2',0),(2,'tars','tarsconfig','','192.168.2.131','2015-11-29 11:13:02','','/usr/local/app/tars/tarsconfig/bin/tarsconfig','tars.tarsconfig',0,'active','active',9815,'','0000-00-00 00:00:00','','1.0.1','2015-08-08 10:49:37','admin','tars_cpp','','','','N','N','','','','','',0,3,'0','65','2',0),(20,'tars','tarsnotify','','192.168.2.131','2015-11-29 11:13:02','','','tars.tarsnotify',0,'active','active',9838,'47','2015-11-28 22:26:40','admin','1.0.1','2015-11-28 21:48:13','admin','tars_cpp','','','','N','N','','','','','',0,3,'0','65','2',0);
/*!40000 ALTER TABLE `t_server_conf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_group_relation`
--

DROP TABLE IF EXISTS `t_server_group_relation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_group_relation` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(90) NOT NULL DEFAULT '',
  `server_group` varchar(50) DEFAULT '',
  `server_name` varchar(50) DEFAULT '',
  `create_time` datetime DEFAULT NULL,
  `creator` varchar(30) DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `f_unique` (`application`,`server_group`,`server_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_group_relation`
--

LOCK TABLES `t_server_group_relation` WRITE;
/*!40000 ALTER TABLE `t_server_group_relation` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_group_relation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_group_rule`
--

DROP TABLE IF EXISTS `t_server_group_rule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_group_rule` (
  `group_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ip_order` enum('allow_denny','denny_allow') NOT NULL DEFAULT 'denny_allow',
  `allow_ip_rule` text,
  `denny_ip_rule` text,
  `lastuser` varchar(50) DEFAULT NULL,
  `modify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `group_name` varchar(128) DEFAULT '',
  `group_name_cn` varchar(128) DEFAULT '',
  PRIMARY KEY (`group_id`),
  UNIQUE KEY `group_name_index` (`group_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_group_rule`
--

LOCK TABLES `t_server_group_rule` WRITE;
/*!40000 ALTER TABLE `t_server_group_rule` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_group_rule` ENABLE KEYS */;
UNLOCK TABLES;


--
-- Table structure for table `t_server_patchs`
--

DROP TABLE IF EXISTS `t_server_patchs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_patchs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server` varchar(50) DEFAULT NULL,
  `version` varchar(1000) DEFAULT '',
  `tgz` text,
  `update_text` varchar(255) DEFAULT NULL,
  `reason_select` varchar(255) DEFAULT NULL,
  `document_complate` varchar(30) DEFAULT NULL,
  `is_server_group` tinyint(2) NOT NULL DEFAULT '0',
  `publish` tinyint(3) DEFAULT NULL,
  `publish_time` datetime DEFAULT NULL,
  `publish_user` varchar(30) DEFAULT NULL,
  `upload_time` datetime DEFAULT NULL,
  `upload_user` varchar(30) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  `is_release_version` enum('true','false') DEFAULT 'true',
  `package_type` tinyint(4) DEFAULT '0',
  `group_id` varchar(64) NOT NULL DEFAULT '',
  `default_version` tinyint(4) DEFAULT '0',
  `md5` varchar(40) DEFAULT NULL,
  `svn_version` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `server_patchs_server_index` (`server`),
  KEY `index_patchs_i1` (`server`),
  KEY `index_i_2` (`tgz`(50))
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_patchs`
--

LOCK TABLES `t_server_patchs` WRITE;
/*!40000 ALTER TABLE `t_server_patchs` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_patchs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_task`
--

DROP TABLE IF EXISTS `t_task`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_task` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `task_no` varchar(40) DEFAULT NULL,
  `serial` tinyint(1) DEFAULT NULL,
  `user_name` varchar(20) DEFAULT NULL,
  `create_time` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `f_task` (`task_no`)
) ENGINE=InnoDB AUTO_INCREMENT=139 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_task`
--

LOCK TABLES `t_task` WRITE;
/*!40000 ALTER TABLE `t_task` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_task` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_task_item`
--

DROP TABLE IF EXISTS `t_task_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_task_item` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `task_no` varchar(40) DEFAULT NULL,
  `item_no` varchar(40) DEFAULT NULL,
  `application` varchar(30) DEFAULT NULL,
  `server_name` varchar(50) DEFAULT NULL,
  `node_name` varchar(20) DEFAULT NULL,
  `command` varchar(20) DEFAULT NULL,
  `parameters` text,
  `start_time` datetime DEFAULT NULL,
  `end_time` datetime DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `set_name` varchar(20) DEFAULT NULL,
  `log` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `f_uniq` (`item_no`,`task_no`),
  KEY `f_task_no` (`task_no`),
  KEY `f_index` (`application`,`server_name`,`command`)
) ENGINE=InnoDB AUTO_INCREMENT=142 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_task_item`
--

LOCK TABLES `t_task_item` WRITE;
/*!40000 ALTER TABLE `t_task_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_task_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_web_release_conf`
--

DROP TABLE IF EXISTS `t_web_release_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_web_release_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server` varchar(100) NOT NULL DEFAULT '',
  `path` varchar(200) NOT NULL DEFAULT '',
  `server_dir` varchar(200) NOT NULL DEFAULT '',
  `is_server_group` tinyint(2) NOT NULL DEFAULT '0',
  `enable_batch` tinyint(2) NOT NULL DEFAULT '0',
  `user` varchar(200) NOT NULL DEFAULT '*',
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `server` (`server`,`is_server_group`),
  KEY `web_release_conf_server_index` (`server`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_web_release_conf`
--

LOCK TABLES `t_web_release_conf` WRITE;
/*!40000 ALTER TABLE `t_web_release_conf` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_web_release_conf` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2016-11-24 14:28:02
