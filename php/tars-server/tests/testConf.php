<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/21
 * Time: 下午1:09.
 */
class Conf
{
    public static function parseText($text)
    {
        $tafAdapters = [];
        $tafServer = [];
        $tafClient = [];
        $objAdapter = [];
        $lines = explode($text, "\r\n");

        $status = 0; //标识在application内
        foreach ($lines as $line) {
            $line = trim($line, " \r\0\x0B\t\n");
            if (empty($line)) {
                continue;
            }
            if ($line[0] == '#') {
                continue;
            }

            switch ($status) {
                // 在server内
                case 1:{
                    if (strstr($line, '=')) {
                        $pos = strpos($line, '=');
                        $name = substr($line, 0, $pos);
                        $value = substr($line, $pos + 1, strlen($line) - $pos);
                        $tafServer[$name] = $value;
                    }
                    // 还要兼容多个adapter的情况(终止的时候兼容即可)
                    elseif (strstr($line, 'Adapter>')) {
                        $status = 2;
                        $adapterName = substr($line, 0, strlen($line) - 1);
                        $adapterName = substr($adapterName, 1, strlen($line) - 1);
                        $objAdapter['adapterName'] = $adapterName;
                    } elseif (strstr($line, '<client>')) {
                        $status = 3;
                    }
                    break;
                }
                // 在adapter内
                case 2: {
                    if (strstr($line, '=')) {
                        $pos = strpos($line, '=');
                        $name = substr($line, 0, $pos);
                        $value = substr($line, $pos + 1, strlen($line) - $pos);
                        $objAdapter[$name] = $value;
                    }
                    // 还要兼容多个adapter的情况(终止的时候兼容即可)
                    elseif (strstr($line, '</')) {
                        // todo 逻辑等待验证
                        //$adapterName = substr($line, 0, strlen($line) - 1);
                        //$adapterName = substr($adapterName, 2, strlen($line) - 2);
                        //$tafAdapters[$adapterName] = $objAdapter;
                        $tafAdapters[] = $objAdapter;
                        $objAdapter = [];
                        $status = 1;
                    }
                    break;
                }
                // 在client内
                case 3: {
                    if (strstr($line, '=')) {
                        $pos = strpos($line, '=');
                        $name = substr($line, 0, $pos);
                        $value = substr($line, $pos + 1, strlen($line) - $pos);
                        $tafClient[$name] = $value;
                    }
                    // 还要兼容多个adapter的情况(终止的时候兼容即可)
                    elseif (strstr($line, '</client>')) {
                        $status = 0;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }
        // 进行一下到Swoole配置的转换
        // todo 需要把可能的swoole的setting补齐
        $tafServer['type'] = $tafAdapters[0]['protocol'] == 'not_taf' ? 'http' : $tafAdapters[0]['protocol'];
        $tafServer['listen'][] = self::getEndpointInfo($tafAdapters[0]['endpoint']);

        $tafServer['entrance'] = isset($tafServer['entrance']) ? $tafServer['entrance'] : $tafServer['basepath'].'src/index.php';
        $setting['worker_num'] = $tafAdapters[0]['threads'];
        $setting['task_worker_num'] = $tafServer['task_worker_num'];
        $setting['dispatch_mode'] = $tafServer['dispatch_mode'];
        $setting['daemonize'] = $tafServer['daemonize'];
        if (isset($tafServer['package_max_length'])) {
            $setting['package_max_length'] = $tafServer['package_max_length'];
        }
        if (isset($tafServer['buffer_output_size'])) {
            $setting['buffer_output_size'] = $tafServer['buffer_output_size'];
        }
        if (isset($tafServer['package_length_type'])) {
            $setting['package_length_type'] = $tafServer['package_length_type'];
        }
        if (isset($tafServer['open_length_check'])) {
            $setting['open_length_check'] = $tafServer['open_length_check'];
        }
        if (isset($tafServer['package_length_offset'])) {
            $setting['package_length_offset'] = $tafServer['package_length_offset'];
        }
        if (isset($tafServer['package_body_offset'])) {
            $setting['package_body_offset'] = $tafServer['package_body_offset'];
        }

        $setting['open_tcp_nodelay'] = $tafServer['open_tcp_nodelay'];
        $setting['open_eof_check'] = $tafServer['open_eof_check'];
        $setting['open_eof_split'] = $tafServer['open_eof_split'];
        $setting['log_file'] = $tafServer['logpath'].$tafServer['app'].'/'.$tafServer['server'].'/'.
            $tafServer['app'].'.'.$tafServer['server'].'.log';
        $setting['log_path'] = $tafServer['logpath'];

        $tafServer['adapters'] = $tafAdapters;
        $tafServer['setting'] = $setting;

        // 把client相关的配置缓存一份
        ClientConf::getInstance();
        ClientConf::init($tafClient);

        return [
            'tafServer' => $tafServer,
            'tafClient' => $tafClient,
        ];
    }

    public static function getLocatorInfo($locatorString)
    {
        $parts = explode('@', $locatorString);
        $locatorName = $parts[0];

        $subParts = explode(':', $parts[1]);
        $infos = [];
        foreach ($subParts as $subPart) {
            $info = self::getEndpointInfo($subPart);
            $infos[] = $info;
        }

        return [
            'locatorName' => $locatorName,
            'routeInfo' => $infos,
        ];
    }

    // 解析出node上报的配置 taf.tafnode.ServerObj@tcp -h 127.0.0.1 -p 2345 -t 10000
    public static function parseNodeInfo($nodeInfo)
    {
        $parts = explode('@', $nodeInfo);
        $objName = $parts[0];
        $subParts = explode('-', $parts[1]);
        $mode = trim($subParts[0]);
        $host = trim($subParts[1], 'h ');
        $port = trim($subParts[2], 'p ');
        $timeout = trim($subParts[3], 't ') / 1000;

        return [
            $objName,
            $mode,
            $host,
            $port,
            $timeout,
        ];
    }

    // tcp -h 10.247.165.120 -b 10.247.165.120 -p 10090 -a 10168 -t 60000
    public static function getEndpointInfo($endpoint)
    {
        $parts = explode('-', $endpoint);
        $sHost = '';
        $sProtocol = '';
        $iPort = '';
        $iTimeout = '';
        $bIp = '';
        foreach ($parts as $part) {
            echo 'part:'.$part;
            if (strstr($part, 'tcp')) {
                $sProtocol = 'tcp';
            } elseif (strstr($part, 'udp')) {
                $sProtocol = 'udp';
            } elseif (strpos($part, 'h') !== false) {
                $sHost = trim($part, " h\t\r");
            } elseif (strpos($part, 'b') !== false) {
                $bIp = trim($part, " b\t\r");
            } elseif (strpos($part, 'p') !== false) {
                $iPort = trim($part, " p\t\r");
            } elseif (strpos($part, 't') !== false) {
                $iTimeout = trim($part, " t\t\r");
            }
        }

        return [
            'sHost' => $sHost,
            'sProtocol' => $sProtocol,
            'iPort' => $iPort,
            'iTimeout' => $iTimeout,
            'bIp' => $bIp,
        ];
    }
}
$text = "<taf>
	<application>
		enableset=n
		setdivision=NULL
		<server>
			node=taf.tafnode.ServerObj@tcp -h 10.247.165.120 -p 19386 -t 60000
			app=Acg
			server=ACGRecomConfigServer
			localip=10.247.165.120
			local=tcp -h 127.0.0.1 -p 10021 -t 3000
			basepath=/usr/local/app/taf/tafnode/data/Acg.ACGRecomConfigServer/bin/
			datapath=/usr/local/app/taf/tafnode/data/Acg.ACGRecomConfigServer/data/
			logpath=/usr/local/app/taf/app_log/
			loglevel=DEBUG
			logsize=15M
			log=taf.taflog.LogObj
			config=taf.tafconfig.ConfigObj
			notify=taf.tafnotify.NotifyObj
			mainclass=com.qq.cloud.taf.server.startup.Main
			classpath=/usr/local/app/taf/tafnode/data/Acg.ACGRecomConfigServer/bin//conf:/usr/local/app/taf/tafnode/data/Acg.ACGRecomConfigServer/bin//lib
			jvmparams=-Dcom.sun.management.jmxremote.ssl\=false -Dcom.sun.management.jmxremote.authenticate\=false -Xms2000m -Xmx2000m -Xmn1000m -Xss1000k -XX:PermSize\=128M -XX:+UseConcMarkSweepGC -XX:CMSInitiatingOccupancyFraction\=60 -XX:+PrintGCApplicationStoppedTime -XX:+PrintGCDateStamps -XX:+CMSParallelRemarkEnabled -XX:+CMSScavengeBeforeRemark -XX:+UseCMSCompactAtFullCollection -XX:CMSFullGCsBeforeCompaction\=0 -verbosegc -XX:+PrintGCDetails -XX:ErrorFile\=/usr/local/app/taf/app_log//Acg/ACGRecomConfigServer/jvm_error.log
			sessiontimeout=120000
			sessioncheckinterval=60000
			tcpnodelay=true
			udpbuffersize=8192
			charsetname=UTF-8
			backupfiles=bak1;bak2;bak3;conf
			<Acg.ACGRecomConfigServer.ACGRecomConfigServantAdapter>
				allow
				endpoint=tcp -h 10.247.165.120 -p 10021 -t 1000
				handlegroup=Acg.ACGRecomConfigServer.ACGRecomConfigServantAdapter
				maxconns=200000
				protocol=taf
				queuecap=10000
				queuetimeout=60000
				servant=Acg.ACGRecomConfigServer.ACGRecomConfigServant
				shmcap=0
				shmkey=0
				threads=5
			</Acg.ACGRecomConfigServer.ACGRecomConfigServantAdapter>
		</server>
		<client>
			locator=taf.tafregistry.QueryObj@tcp -h 10.121.107.80 -p 17890:tcp -h 10.219.140.217 -p 17890:tcp -h 10.224.137.102 -p 17890
			sync-invoke-timeout=20000
			async-invoke-timeout=20000
			refresh-endpoint-interval=60000
			stat=taf.tafstat.StatObj
			property=taf.tafproperty.PropertyObj
			report-interval=60000
			modulename=Acg.ACGRecomConfigServer
			sample-rate=1000000
			max-sample-count=10
			propertyplus=Acg.ACGRecomConfigServer.ACGRecomConfigServant
		</client>
	</application>
</taf>
";

//$result = Conf::parseText($text);

//var_dump($result);

$result = Conf::getEndpointInfo('tcp -h 10.247.165.120 -b 10.247.165.120 -p 10090 -a 10168 -t 60000');

var_dump($result);
$result = Conf::parseNodeInfo('taf.tafnode.ServerObj@tcp -h 127.0.0.1 -p 2345 -t 10000');

var_dump($result);
$result = Conf::getLocatorInfo('taf.tafregistry.QueryObj@tcp -h 10.121.107.80 -p 17890:tcp -h 10.219.140.217 -p 17890:tcp -h 10.224.137.102 -p 17890');

var_dump($result);
