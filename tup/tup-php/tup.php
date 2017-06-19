<?php

require_once('tars.php');
require_once('RequestF_tup.php');

class UniPacket
{
	protected $_iVer;
	protected $_data;
	protected $_new_data;
	
	public function __construct()
	{
		$this->_iVer = new c_short;
		
		$this->_iVer->val = 2;
		//v2
		$this->_data = new c_map(new c_string,new c_map(new c_string,new c_vector(new c_char)));
		//v3
		$this->_new_data = new c_map(new c_string,new c_vector(new c_char));
	}
	
	public function setVersion($version)
	{
		$this->_iVer->val = $version;
	}
	
    public function put($name,$mystruct)
    {
        //把struct写到$struct_data里去
		if(method_exists($mystruct,'writeTo'))
		{
			$mystruct->writeTo($struct_data,0);
        }else
		{
			$mystruct->write($struct_data,0);
		}
		
		if($this->_iVer->val == 3)
		{
			$vector = new c_vector(new c_char);
			$vector->push_back($struct_data);
			$this->_new_data->push_back($name,$vector);
		}
		else
		{
			//构造存放map的 data，$vector就是用来存放数据value的地方 $struct则加上了类名作为key
			$vector = new c_vector(new c_char);        	
			$struct = new c_map(new c_string,$vector);	
			//vector加入数据
			$vector->push_back($struct_data);	
			
			//stucte的map，把类名作为key，对应该对象的的vector
			$struct->push_back($mystruct->get_class_name(),$vector);
			
			//最外面一层就是map data把name作为key，value是上面的map
			$this->_data->push_back($name,$struct);
		}		
    }
	
	// 传入name作为key，取得回包struct信息
    public function get($name,&$mystruct)
    {   
		if($this->_iVer->val == 3)
		{
			$struct_vec = $this->_new_data->get_val($name);
			//最后从vector里得到
			if(method_exists($mystruct,'readFrom'))
			{
				$mystruct->readFrom($struct_vec->get_val(),0); 
			}
			else
			{
				$mystruct->read($struct_vec->get_val(),0); 
			}
		}
		else
		{
			//第一层用name为key
			$struct_map = $this->_data->get_val($name);
			//第二层用类名作为key
			$struct_vec = $struct_map->get_val($mystruct->get_class_name()); 
			//最后从vector里得到
			if(method_exists($mystruct,'readFrom'))
			{
				$mystruct->readFrom($struct_vec->get_val(),0); 
			}
			else
			{
				$mystruct->read($struct_vec->get_val(),0); 
			}  
		}     
    }
	public function _encode(&$stream)
	{
		if($this->_iVer->val == 3)
		{
			$this->_new_data->write($stream,0);			
		}
		else
		{
			$this->_data->write($stream,0);
		}
	}
	public function _decode(&$stream)
	{
		//先检查了stream是否为空，这个在请求包不合法的时候，server返回包没有设计body数据的时候会发生
    	if(strlen($stream) == 0 || is_null($stream))
    	{
    		throw new TARSException(__LINE__,STREAM_LEN_ERROR);	
    		return false;
    	}
    	
		if($this->_iVer->val == 3)
		{
			$this->_new_data->clear();
			$this->_new_data->read($stream,0);
		}
		else
		{
			$this->_data->clear();
			$this->_data->read($stream,0);
		}
	}
}

class tup_unipacket extends UniPacket
{
	protected $requestPacket;
	
    public function __construct()
    {
		parent::__construct();
		$this->requestPacket = new RequestPacket;	
		$this->setVersion(2);
    }
	
    public function setVersion($version)
    {
		parent::setVersion($version);
		$this->requestPacket->iVersion->val = $version;
    }
	public function getVersion() {return $this->requestPacket->iVersion;}
    //设置请求包ID
    public function setRequestId($id)
    {	
		$this->requestPacket->iRequestId->val = $id;
    }
	
	public function getRequestId() {return $this->requestPacket->iRequestId;}
	
    //设置服务名字
    public function setServantName($name)
    {
        $this->requestPacket->sServantName->val = $name;
    } 
	public function getServantName() {return $this->requestPacket->sServantName;}
	
    //设置函数名称
    public function setFuncName($name)
    {
        $this->requestPacket->sFuncName->val = $name;
    }
	public function getFuncName() {return $this->requestPacket->sFuncName;}
    //编码过程 返回stream
    public function _encode(&$stream)
    {             
		if($this->_iVer->val == 3)
		{ 
			$this->_new_data->write($data_stream,0); 
		}
		else
		{
			$this->_data->write($data_stream,0);
		} 
		
		//先写入UniPacket数据
		$this->requestPacket->sBuffer->push_back($data_stream);
		//再写入requestPacket
		$this->requestPacket->writeTo($stream,0);
			 
        //在stream的最前面，加上stream的长度（int）。
        $len = strlen($stream)+4;  
        $stream = pack('N',$len).$stream;
    }
    
    public function _decode(&$stream)
    {        
    	//先检查了stream是否为空，这个在请求包不合法的时候，server返回包没有设计body数据的时候会发生
    	if(strlen($stream) == 0 || is_null($stream))
    	{
    		throw new TARSException(__LINE__,STREAM_LEN_ERROR);	
    		return false;
    	}
    	
    	//TUP的包包头前面有tup包的长度
        $len = @unpack('N',$stream);
        $len = $len[1];
		
        if(intval($len) <= 0 )
        {	
        	throw new TARSException(__LINE__,STREAM_LEN_ERROR);
        	return false;
        }
		
		//协议包头读取出去
        $stream = substr($stream,4);
		
		//先解码requestPacket
		$this->requestPacket->readFrom($stream,0);

		$this->_iVer = $this->requestPacket->iVersion;

		if($this->_iVer->val == 3)
		{ 
			$this->_new_data->read($this->requestPacket->sBuffer->get_val(),0); 
		}
		else
		{
			$this->_data->read($this->requestPacket->sBuffer->get_val(),0);
		}
    }  
	public function getResultCode()
	{
		$ret = $this->requestPacket->status->get_val('STATUS_RESULT_CODE');
		return intval($ret->val);
	}
	
	public function getResultDesc()
	{
		$ret = $this->requestPacket->status->get_val('STATUS_RESULT_DESC');
		return $ret->val;
	}
}

class tup_ResponsePacket
{
    public $iVersion;//协议版本号
    public $cPacketType;//调用类型
    public $iRequestId;//请求序列ID号
    public $iMessageType;//消息类型
    public $iRet;
    public $sBuffer;//数据缓存，缓存要发送或者接受的数据，编码后或者解码前的内容
    public $status;//特殊消息的状态值
    public $sResultDesc;//解码的时候用于把buffer复原为可辨识的对象
    public function __construct()
    {
        $this->iVersion     = new c_short;
        $this->cPacketType  = new c_char;
        $this->iMessageType = new c_int;
        $this->iRequestId   = new c_int;
        $this->iRet           = new c_int;
        $this->sBuffer      = new c_vector(new c_char);
        $this->status       = new c_map(new c_string,new c_string);  
        $this->sResultDesc= new c_string;
        $this->iVersion->val    = 1;
        $this->cPacketType->val = 0;
        $this->iMessageType->val = 0;
        $this->iRequestId->val   = 0;
    }
    public function get_version($version)
    {
           return $this->iVersion;
    }
    //设置请求包ID
    public function get_request_id($id)
    {
        return $this->iRequestId;
    }
    //编码过程 返回stream
    public function _encode(&$stream)
    {
    	//把协议的相关变量写入stream
        $this->iVersion->write($stream,1);
        $this->cPacketType->write($stream,2);        
        $this->iMessageType->write($stream,3);        
        $this->iRequestId->write($stream,4);
        $this->iRet->write($stream,5);
        $this->sBuffer->write($stream,6); 
        $this->status->write($stream,7);
        $this->sResultDesc->write($stream,8);
        //在stream的最前面，加上stream的长度（int）。
        $len = strlen($stream)+4;  
        $stream = pack('N',$len).$stream;
    }
    public function _decode(&$stream)
    {        
    	//先检查了stream是否为空，这个在请求包不合法的时候，server返回包没有设计body数据的时候会发生
    	if(strlen($stream) == 0 || is_null($stream))
    	{
    			
    		return false;
    	}
    	//TUP的包包头前面有tup包的长度
        $len = @unpack('N',$stream);
        $len = $len[1];
        if(intval($len) <= 0 )
        {	
        	
        	return false;
        }
        $stream = substr($stream,4);
        $this->iVersion->read($stream,1);
        $this->cPacketType->read($stream,2);        
        $this->iMessageType->read($stream,3);        
        $this->iRequestId->read($stream,4);        
        $this->iRet->read($stream,5);        
        $this->sBuffer->read($stream,6);
        $this->status->read($stream,7);
        $this->sResultDesc->read($stream,8);
    }          
}
?>
