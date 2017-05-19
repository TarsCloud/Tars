<?php
/*
 *  tars库，定义类型信息。以及基本的header里的tag和type信息的打包和解包实现
 * */
class tars_header
{
    static private $type;
    static function initial()
    {
        //self::$type = new array();
        //每种类型的关键字名字 并对应tag数字
        self::$type['c_char']         = 0;
        self::$type['c_short']        = 1;
        self::$type['c_int']          = 2;
        self::$type['c_int64']        = 3;
        self::$type['c_float']        = 4;
        self::$type['c_double']       = 5;
        self::$type['c_string1']      = 6;
        self::$type['c_string4']      = 7;
        self::$type['c_map']          = 8;
        self::$type['c_list']         = 9;
        self::$type['c_struct_begin'] = 10;
        self::$type['c_struct_end']   = 11;
        self::$type['c_zero']         = 12;
        self::$type['c_simple_list']  = 13;        
        
        //根据tag数字对应出type名字
        self::$type[0] = 'c_char';
        self::$type[1] = 'c_short';
        self::$type[2] = 'c_int';
        self::$type[3] = 'c_int64';
        self::$type[4] = 'c_float';
        self::$type[5] = 'c_double';
        self::$type[6] = 'c_string1';
        self::$type[7] = 'c_string4';
        self::$type[8] = 'c_map';
        self::$type[9] = 'c_list';
        self::$type[10] = 'c_struct_begin';
        self::$type[11] = 'c_struct_end';
        self::$type[12] = 'c_zero';
        self::$type[13] = 'c_simple_list';                        
    }
    
    //输入type和tag，打包到stream里
    static function _pack_header(&$stream,$type,$tag)
    {
    	//$type必须是表示类型名字的字符串 
        if(!is_string($type))
            throw new TARSException(__LINE__, HEADER_TYPE_ERROR);
        //tag必须是数字
        if(!is_int($tag))
            throw new TARSException(__LINE__, HEADER_TAG_ERROR);
        
       	//检查type是否合法。除了c_char 之外的不合法类型名字都会type中取的0值
        if($type != 'c_char' && self::$type[$type] == 0)
            throw new TARSException(__LINE__, HEADER_TYPE_UNKNOWN);       

        //tag大于15 pack方式不一样
        if($tag >= 15)
        {
            $first_tag = 15;  
            /* tag first , type second */ 
            //把tag放到高4位，type对应的类型数字放到低四位                         
            $header = (($first_tag << 4) | (self::$type[$type] & 0x0000000F));                        
            $header = chr($header);        
            $stream.=pack('A',$header); 

            //这里写完整的tag
            $header = chr($tag);            
            $stream.=pack('A',$header); 
                             
        }else{
            /* tag first , type second */ 
        	//	tag小于15的 只需要一个字节 
            $header = (($tag << 4) | (self::$type[$type] & 0x0000000F));                        
            $header = chr($header);        
            $stream.=pack('A',$header);        
        }
    }    
    
   	//从stream里得出tag和type
    static function _unpack_header(&$stream,&$type,&$tag)
    {
        if(strlen($stream) < 1)
		{
            throw new TARSException(__LINE__, STREAM_LEN_ERROR);
		}
        $header = $stream{0};
        $stream = substr($stream,1);
		//按照打包的逆过程         type是的4位 tag是高4位
        $type = self::$type[(ord($header) & 0x0000000F)];
        $tag  = ord($header) >> 4;
        
        //如果发现tag == 15 肯定是tag>15的情况，还需要一个字节去unpack出正确的tag
        if($tag ==  15)
        {
            $tag = $stream{0};
            $stream = substr($stream,1);
            $tag = ord($tag);
        }
    }
    
    //作用类似_unpack_header，只是不改变stream
    static function _peek_header($stream,&$type,&$tag,$isRequire = true)
    {
        $len = strlen($stream);
         if($len < 1){
        	if($isRequire){
            	throw new TARSException(__LINE__, STREAM_LEN_ERROR);
        	}else{
        		return ;
        	}
        }
        $header = $stream{0};                
        $type = self::$type[(ord($header) & 0x0000000F)];
        $tag  = ord($header) >> 4;
                
        if($tag ==  15)
        {
            if($len < 2)
                throw new TARSException(__LINE__, STREAM_LEN_ERROR);
            $tag = $stream{1};
            $tag = ord($tag);
        }
    }    
   //检查stream是否有struct结构内容
   static function _check_struct($stream,&$type,&$tag,$isRequire = true)
   {
        tars_header::_peek_header($stream,$type,$this_tag);
        if($tag != $this_tag)
        {
            if($isRequire == true)
            {
                throw new TarsException(__LINE__ ,TAG_NOT_MATCH);
            }else
            {
                return;
            }
        }
        if($type != 'c_struct_begin')
        {
            throw new TarsException(__LINE__ ,TYPE_NOT_MATCH);
        }
	}
};

tars_header::initial();


?>
