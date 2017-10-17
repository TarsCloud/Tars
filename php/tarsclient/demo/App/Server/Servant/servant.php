<?php

namespace App\Server\Servant;

use phptars\TarsAssistant;

use App\Server\Servant\classes\SimpleStruct;
use App\Server\Servant\classes\MiddleStruct;
use App\Server\Servant\classes\NestedStruct;
class servant {
	private $_tarsAssistant;
	private $_servantName = "App.Server.Servant";

	private $_ip;
	private $_port;

	public function __construct($ip="",$port="") {
		$this->_tarsAssistant = new TarsAssistant();
		$this->_ip = $ip;
		$this->_port = $port;
	}

	public function ping() {
		$this->_tarsAssistant->setRequest($this->_servantName,__FUNCTION__,$this->_ip,$this->_port);

		try {


			$this->_tarsAssistant->sendAndReceive();

			return $this->_tarsAssistant->getString("");

		}
		catch (\Exception $e) {
			return array(
				"code" => $e->getCode(),
				"msg" => $e->getMessage(),
			);
		}
	}

	public function singleParam($in1,&$out1) {
		$this->_tarsAssistant->setRequest($this->_servantName,__FUNCTION__,$this->_ip,$this->_port);

		try {
			$this->_tarsAssistant->putString("in1",$in1);


			$this->_tarsAssistant->sendAndReceive();

			$out1 = $this->_tarsAssistant->getDouble("out1");
			return $this->_tarsAssistant->getInt32("");

		}
		catch (\Exception $e) {
			return array(
				"code" => $e->getCode(),
				"msg" => $e->getMessage(),
			);
		}
	}

	public function inAndOut1($param1,$param2,&$out1,&$out2,&$out3) {
		$this->_tarsAssistant->setRequest($this->_servantName,__FUNCTION__,$this->_ip,$this->_port);

		try {
			$param1_map = new \TARS_MAP(\TARS::STRING,new \TARS_VECTOR(new SimpleStruct()));
			foreach($param1 as $singleparam1) {
				$param1_map->pushBack($singleparam1);
			}
			$this->_tarsAssistant->putMap("param1",$param1_map);
			$param2_map = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
			foreach($param2 as $singleparam2) {
				$param2_map->pushBack($singleparam2);
			}
			$this->_tarsAssistant->putMap("param2",$param2_map);


			$this->_tarsAssistant->sendAndReceive();

			$out1 = $this->_tarsAssistant->getMap("out1",new \TARS_MAP(\TARS::STRING,new \TARS_VECTOR(new MiddleStruct())));
			$out2 = $this->_tarsAssistant->getShort("out2");
			$out3 = $this->_tarsAssistant->getVector("out3",new \TARS_VECTOR(new \TARS_VECTOR(new SimpleStruct())));
			return $this->_tarsAssistant->getInt64("");

		}
		catch (\Exception $e) {
			return array(
				"code" => $e->getCode(),
				"msg" => $e->getMessage(),
			);
		}
	}

	public function inAndOut2(SimpleStruct $param1,$param2,$param3,NestedStruct &$out1,&$out2,&$out3,&$out4) {
		$this->_tarsAssistant->setRequest($this->_servantName,__FUNCTION__,$this->_ip,$this->_port);

		try {
			$this->_tarsAssistant->putStruct("param1",$param1);
			$this->_tarsAssistant->putStruct("param2",$param2);
			$this->_tarsAssistant->putUInt8("param3",$param3);


			$this->_tarsAssistant->sendAndReceive();

			$ret = $this->_tarsAssistant->getStruct("out1",$out1);
			$out2 = $this->_tarsAssistant->getVector("out2",new \TARS_VECTOR(new SimpleStruct()));
			$out3 = $this->_tarsAssistant->getVector("out3",new \TARS_VECTOR(new \TARS_MAP(\TARS::INT64,\TARS::INT64)));
			$out4 = $this->_tarsAssistant->getVector("out4",new \TARS_VECTOR(\TARS::INT32));
		}
		catch (\Exception $e) {
			return array(
				"code" => $e->getCode(),
				"msg" => $e->getMessage(),
			);
		}
	}

}

