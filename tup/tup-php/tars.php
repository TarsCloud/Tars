<?php
/*
 *tars�� .�����˼���tup֧�ֵ���ľ���ʵ�� 
 * */
 
require_once ('tars_exception.php');
require_once ('tars_header.php');
define("TARS_MAXIMUM_TAG",255);

//data_type�����ࡣ�����˱���ʵ�ֵ�read��write�ķ��� �Լ�������������
abstract class data_type
{                                          
	//����ʵ������д��stream��
    abstract public function write(&$stream,$tag);
    //����ʵ�����ݴ�stream�������
    abstract public function read(&$stream,$tag,$isRequire = true);  
    //����������� 
    abstract public function get_class_name();
    
    //unpack��һ���������͵Ĳ�ͬ��һ�����ӵ�ʵ��ϸ�ڡ������е�ʵ��ϸ�ڼ��зŵ�����,����type������val���ȡֵ
    public function _unpack_base(&$stream,$type,&$val)
    {
        if($type == 'c_zero'){
        $val = 0;
            return;
        }else if($type == 'c_char')
        {
        	//char ֻҪһ���ֽ�
            $val = substr($stream,0,1);
            $stream = substr($stream,1);    
        }else if($type == 'c_short'){
        	//short���ͣ���n��
            $val = unpack('n',$stream);
            $val = $val[1];
            $stream = substr($stream,2);    
        }else if($type == 'c_int'){
        	//INT���ͣ���N��
            $val = unpack('N',$stream);
            $val = $val[1];
            $stream = substr($stream,4);  
        }else if($type == 'c_int64'){
        	//INT64���ͣ�������H8��
            $tmp = unpack('H8',$stream);
            $stream = substr($stream,4);                               
            $tmp2 = unpack('H8',$stream);
            $stream = substr($stream,4);               

            //ͨ��ʮ���������������
            $bit[1] = base_convert($tmp[1],16,10);
            $bit[2] = base_convert($tmp2[1],16,10);                                              
            $val = bcadd( bcmul($bit[1],"4294967296"),$bit[2]);
                          
        }else if($type == 'c_string1'){
        	//c_string1 ����һ���ֽ��ܱ�ʾ�ĳ��ȿ�ͷ�Ķ�Ӧ���ȵ��ַ�����
            $len    = ord($stream{0});
            $stream = substr($stream,1);    
            $val = substr($stream,0,$len);
            $stream = substr($stream,$len);
            
        }else if($type == 'c_string4'){  
        	//c_string4 ����4���ֽڵ�int�ܱ�ʾ�ĳ��ȿ�ͷ�Ķ�Ӧ���ȵ��ַ�����          
            $len = unpack('N',$stream);            
            $len = $len[1];
            $stream = substr($stream,4);                                    
            $val = substr($stream,0,$len);
            $stream = substr($stream,$len);            
        }else{
            throw new TARSException(__LINE__, HEADER_TYPE_ERROR);
        }        
    } 

    private function _unpack_length(&$stream)
	{
		tars_header::_unpack_header($stream,$type,$this_tag);
		//length tag must be 0
		if(0 != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }     
                        
        $this->_unpack_base($stream,$type,$val); 
        $len = 0;
        if(is_string($val))
        {
        //�ַ�����int��ת��
            $len = ord($val);  
        }else{
            $len = $val;                        
        } 
        return $len;
	}
	
    private function _skip_vector(&$stream,$type)
    {     
        //���char��simpl list�ʹ������Ԫ�ص�list                            
        $this_tag = 0;
                                
        if($type == 'c_simple_list')
        {
            //here ,unpack c_char tag
            tars_header::_unpack_header($stream,$type,$this_tag);
            $len = 0;            
            $len = $this->_unpack_length($stream); 
	        
            $stream = substr($stream,$len);            
        }else{
            $len = 0;
            $len = $this->_unpack_length($stream);                        
            for($i = 0; $i < $len ;$i++)
            {
                $this->_skip_base($stream);               
            }                        
        }   
    }
    
    private function _skip_map(&$stream)
    {                        
                        
        $len = 0;            
        $len = $this->_unpack_length($stream);  
        $len = $len*2;  
        //�������	         
        for($i = 0;$i < $len;$i++)
        {
            $this->_skip_base($stream);                  
        }      
    }
 	public function _skip_struct(&$stream)
    {
        $struct_type='';  
        do{
        	if(empty($stream))
	    	{
	    		return;
	    	} 
        	tars_header::_peek_header($stream,$struct_type,$this_tag);
        	$this->_skip_base($stream);        	
        }while($struct_type != 'c_struct_end');            
        //$this->_unpack_every_elem($stream);                           
        //tars_header::_unpack_header($stream,$type,$this_tag);  
        //��֤��β�Ƿ���ȷ. tag=0,type=  c_struct_end 
        
        if($this_tag != 0)
            throw new TARSException(__LINE__, TAG_NOT_MATCH);                               
        if($struct_type != 'c_struct_end')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
    }
    
    
    public function _skip_base(&$stream)
    {
    	$this_tag = 0;
    	$type = '';
    	tars_header::_unpack_header($stream,$type,$this_tag);
        if($type == 'c_zero' || $type == 'c_struct_end')
        {      
            return;
        }else if($type == 'c_char')
        {
        	//char ֻҪһ���ֽ�           
            $stream = substr($stream,1);    
        }else if($type == 'c_short'){
        	//short���ͣ���n��         
            $stream = substr($stream,2);    
        }else if($type == 'c_int'){
        	//INT���ͣ���N��         
            $stream = substr($stream,4);  
        }else if($type == 'c_int64'){
        	//INT64���ͣ�������H8��           
            $stream = substr($stream,8); 
                          
        }else if($type == 'c_string1'){
        	//c_string1 ����һ���ֽ��ܱ�ʾ�ĳ��ȿ�ͷ�Ķ�Ӧ���ȵ��ַ�����
            $len    = ord($stream{0});
            $len = intval($len)+1;            
            $stream = substr($stream,$len);            
        }else if($type == 'c_string4'){  
        	//c_string4 ����4���ֽڵ�int�ܱ�ʾ�ĳ��ȿ�ͷ�Ķ�Ӧ���ȵ��ַ�����          
            $len = unpack('N',$stream);            
            $len = intval($len[1])+4;
            $stream = substr($stream,$len);
        }else if($type == 'c_simple_list' || $type == 'c_list'){
        	$this->_skip_vector($stream,$type);
        }else if($type == 'c_map'){
        	$this->_skip_map($stream);
        }else if($type == 'c_struct_begin'){
        	$this->_skip_struct($stream);
        }
        else 
        {        
            throw new TARSException(__LINE__, HEADER_TYPE_ERROR);
        }        
    }   
};        
//�ַ����� 
class c_char extends data_type
{
	//ʵ�ʴ��ֵ�ı���
    private  $val;
    
    public function __construct()
    {
        $this->val = chr(0);
    }

    public function __set($name, $val)
    {
    	//��ħ������ֻ����val�ĸ�ֵ������������׳�VAR_NOT_FOUND�쳣
        if($name == 'val')
        {
        	//ֻ����127��-128��ascii��ͳ���Ψһ���ַ���
            if(is_int($val))
            {
                if($val > 127 || $val < -128)
                    throw new TARSException(__LINE__, OUTOF_RANGE);                
                $this->val = chr($val);    
            }else if(is_null($val)){
                $this->val = chr(0);
            }else if(!is_string($val)) {
                throw new TARSException(__LINE__, TYPE_NOT_MATCH);
            }else{
                if(strlen($val) != 1)
                    throw new TARSException(__LINE__, LEN_NOT_MATCH);
                $this->val = $val;   
            }                                           
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }    
    }    
    public function __get($name)
    {        
        if($name == 'val')
        {
            return $this->val; 
        }else{            
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }        
    }    
    
    public function len () 
    {
    	return 1;  
    }
    
    //��tup֧�ֻ�������
    public function is_base_datatype() { return true; }
    
    public function get_class_name() { return 'char'; }
    
    //дstream����
    public function write(&$stream,$tag)
    {
    	
        if($this->get_as_int() != 0)
        {
        	//�Ȱ�������Ϣ���
            tars_header::_pack_header($stream,'c_char',$tag);
           	//ֱ�Ӱ�ֵ�ŵ�stream����
            $stream.=$this->val;
        }else{
            tars_header::_pack_header($stream,'c_zero',$tag);
        }        
    }
    //��stream���һ��char����
    public function read(&$stream,$tag,$isRequire = true)
    {                
    	//�Ȱ�ͷ��Ϣpeek�������ж��Ƿ�����ȷ��tag��
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true)
            {
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }     
        if($type != 'c_zero' && $type != 'c_char')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);

        //type��tag�����ˣ��������������ȡ
        tars_header::_unpack_header($stream,$type,$this_tag);
        $this->_unpack_base($stream,$type,$val);        
        $this->val = $val;                
    }    
    public function get_as_int()
    {   
    	//ͨ����val�Ƿ��ǡ�\0��,���������           c_zero or c_char   
        return ord($this->val);        
    }
};
//short��tup����
class c_short extends data_type
{
    private  $val;
    public function __construct()
    {
        $this->val = 0;
    }        
    public function __set($name, $val)
    {
        if($name == 'val')
        {      
        	//   -32768    32767 ����short
            if(intval($val) >= -32768 && intval($val) <= 32767)
            {
                $this->val = intval($val);
            }else if(is_null($val)){
                $this->val = 0;
            }else{
                throw new TARSException(__LINE__, OUTOF_RANGE);
            }
            
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
            //echo VAR_NOT_FOUND;
        }    
    }    
    public function __get($name)
    {        
        if($name == 'val')
        {
            return $this->val; 
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }         
    }    
    //SHORT�ǻ���֧������
    public function is_base_datatype() { return true; }
    public function len () { return 2; }
    public function get_class_name() { return 'short'; }

    public function write(&$stream ,$tag)
    {
        if($this->val >= (-128) && $this->val <= 127){            
            $char = new c_char;
            $char->val = $this->val;
            $char->write($stream,$tag);            
        }else{
            tars_header::_pack_header($stream,'c_short',$tag);
            $stream .= pack('n',$this->val);
        }
    }
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }                     
        if($type != 'c_zero' && $type != 'c_char' && $type != 'c_short')
		{
			
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
		}
        //������Ƚ⿪header�ٽ⿪����
        tars_header::_unpack_header($stream,$type,$this_tag);
        $this->_unpack_base($stream,$type,$val);        
        if(is_string($val))
        {
            $this->val = ord($val);  
        }else{
            $this->val = $val;                        
        }
    }
};

class c_int extends data_type
{
    private  $val;
    public function __construct()
    {
        $this->val = 0;
    }                
    public function __set($name, $val)
    {
        if($name == 'val')
        {     //int�ķ�Χ       
            if(intval($val) >= -2147483647 && intval($val) <= 2147483647)
            {
                $this->val = intval($val);
            }else if(is_null($val)){
                $this->val = 0;
            }else{
                throw new TARSException(__LINE__, OUTOF_RANGE);
            }
            
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }    
    }    
    public function __get($name)
    {        
        if($name == 'val')
        {
            return $this->val; 
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }         
    }    
    
    public function len () { return 4; }
    //��������
    public function is_base_datatype() { return true; }
    public function get_class_name() { return 'int32'; }
    
    public function write(&$stream,$tag)
    {
    	//���һ��short���Դ�ã�����short
        if($this->val >= (-32768) && $this->val <= 32767){
            //write((Short) n, tag);
            $short = new c_short;
            $short->val = $this->val;
            $short->write($stream,$tag);            
        }else{
        	//�������int���͵Ĵ��
            tars_header::_pack_header($stream,'c_int',$tag);
            $stream .= pack('N',$this->val);
        }
    }
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }
        if($type != 'c_zero' && $type != 'c_char' && $type != 'c_short' && $type != 'c_int')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        tars_header::_unpack_header($stream,$type,$this_tag);                
        $this->_unpack_base($stream,$type,$val); 
        if(is_string($val))
        {
        	//�ַ�����int��ת��
            $this->val = ord($val);  
        }else{
            $this->val = $val;                        
        }   
    }    
};

class c_int64 extends data_type
{
    private  $val;
    public function __construct()
    {
        $this->val = "0";
    }                
    public function __set($name, $val)
    {
        if($name == 'val')
        {   
        	//���� �������ַ�������ֵ
            if(is_string($val))
            {
                if(!is_numeric($val))
                {
                    throw new TARSException(__LINE__, TYPE_NOT_MATCH);
                }    
                //Returns 0 if the two operands are equal, 1 if the left_operand is larger than the right_operand , -1 otherwise            
                if((bccomp($val,"-9223372036854775807") == 1) && (bccomp($val,"9223372036854775808") <= 0))
                {                    
                    $this->val = $val;
                }else{
                    throw new TARSException(__LINE__, OUTOF_RANGE);
                }
            }else if(is_int($val)){
            	//����Ҳ����Ҫ����Ϊ�ַ���
                $this->val = sprintf("%d",$val);
            }else if(is_null($val)){
                    $this->val = "0";
            }else{
                throw new TARSException(__LINE__, TYPE_NOT_MATCH);
            }             
            
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }    
    }    
    public function __get($name)
    {        
        if($name == 'val')
        {
            return $this->val; 
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }         
    }    
    
    public function len () { return 8; }
    public function is_base_datatype() { return true; }
    public function get_class_name() { return 'int64'; }
    public function write(&$stream,$tag)
    {
        if((bccomp($this->val,"-2147483648") >= 0) && (bccomp($this->val,"2147483647") <= 0))
        {
        	//����Ҫ������������������һ��intҲ���Ա���
            $int = new c_int;
            $int->val = $this->val;
            //����һ��int�ı��棬ʹ��int��Ĳ���ȥ���棬Ҳ�п������ֻ�Ǵ�Ϊһ��char
            $int->write($stream,$tag);            
        }else{
            tars_header::_pack_header($stream,'c_int64',$tag);
            /* 4294967296 = 0x100000000 */                        
            $tmp[0] = sprintf("%08s",base_convert(bcdiv($this->val,"4294967296"),10,16));            
            $tmp[1] = sprintf("%08s",base_convert(bcmod($this->val,"4294967296"),10,16));            
            $stream.= pack('H8',$tmp[0]);
            $stream.= pack('H8',$tmp[1]);                    
        }
    }
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }                
        if($type != 'c_zero' && $type != 'c_char' && $type != 'c_short' && $type != 'c_int' && $type != 'c_int64')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        tars_header::_unpack_header($stream,$type,$this_tag);
        $this->_unpack_base($stream,$type,$val);        
        
        if($type == 'c_char')
        {
            $this->val = ord($val);
        }else if($type == 'c_int64'){
            $this->val = $val;                        
        }else{
            $this->val = sprintf("%d",$val);
        }                
    }            
};

class c_string extends data_type
{
    private  $val;
    public function __construct()
    {
        $this->val = "";
    }                
    public function __set($name, $val)
    {
        if($name == 'val')
        {   //����ֱ���ַ�������int�����ַ���
            if(is_string($val))
            {
                $this->val = $val;
            }else if(is_int($val)){
                $this->val = sprintf("%d",$val);
            }else if(is_null($val)){
                $this->val = "";
            }else{
            	throw new TARSException(__LINE__, TYPE_NOT_MATCH);
            }                         
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }    
    }    
    public function __get($name)
    {        
        if($name == 'val')
        {
            return $this->val; 
        }else{
            throw new TARSException(__LINE__, VAR_NOT_FOUND);
        }         
    }    
    
    public function len () { return strlen($this->val); }
    public function is_base_datatype() { return true; }
    public function get_class_name() { return 'string'; }
    public function write(&$stream,$tag)
    {
        $len = strlen($this->val);
        
        if($len > 255)
        {
        	//��4�ֽڱ�ʾ�ַ�������
            tars_header::_pack_header($stream,'c_string4',$tag);            
            $stream.= pack("N",$len);
            $stream.= pack("A*",$this->val);
            
        }else{
        	//��һ���ֽڱ�ʾ�ַ�������
            tars_header::_pack_header($stream,'c_string1',$tag);
            $stream.= pack("A",chr($len));
            $stream.= pack("A*",$this->val);
        }               
    }
    public function read(&$stream,$tag,$isRequire = true)
    {
		
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }
		 
        if($type != 'c_string1' && $type !='c_string4' )
		{
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
		}
        tars_header::_unpack_header($stream,$type,$this_tag);
        $this->_unpack_base($stream,$type,$val);        
        $this->val = $val;                        
    }                
};

class c_vector extends data_type
{
	//������ݵ�����
    private $vector;
    ////vector��ŵĶ��������
    private $class_name;
    //vector��ŵĶ����ԭ������
    private $proto_type;
    public  function __construct($proto)
    {
	    //	���캯������ָ��������ԭ������
        if(is_null($proto))
        {
            throw new TARSException(__LINE__, PROTO_TYPE_UNKNOWN);
        }
        
        if(is_int($proto))
        {
            $proto = new c_int;
            $this->class_name = 'c_int';
            $this->proto_type = clone $proto;
        }else if(is_string($proto))
        {
            $proto = new c_char;
            $this->class_name = 'c_char';
            $this->proto_type = clone $proto;
        }else{
            $this->class_name = get_class($proto);
            $this->proto_type = clone $proto;            
        }
        //ֻ��c_char ����Ҫarray����ʽ�����vector����Ϊc_char������ϳ�string
        if($this->class_name != 'c_char')
        {
            $this->vector = array();
        }
    }
    //��¡��������Ҫ�Ǵ���object��clone���Լ�ԭ�͵ĸ���
    public function __clone()
    {        
        if(is_array($this->vector) && count($this->vector) > 0)
        {
            foreach($this->vector as $key => &$val)
            {
                if(is_object($val))
                {
                    $val = clone $val;
                }
            }
        }
        
        if(is_object($this->proto_type))
        {
            $this->proto_type = clone $this->proto_type;
        }
    }
    
    //�չ�C++����Ա��ϰ�ߣ�ʵ�ֵ�push_back������ͬC++-STL��vector��ͬ������������
    public function push_back($val)
    {           
    	//�õ�push�Ķ��������             
        if(is_object($val))
        {            
            $class_name = get_class($val);     
        }else if(is_string($val)){
            $class_name = 'c_char';
        }else if(is_int($val))
        {
            $class_name = 'c_int';
        }else{                        
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        }
        
        //����Ƿ�ƥ�䣬��vectorֻ�ܴ��ͬһ���͵ı���
        if($class_name != $this->class_name)
        {  
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        }
            
        //char��vector,���ǰ��ַ���ӵ��ַ�������
        if($class_name == 'c_char')
        {
            $this->vector .= $val;
        }else{          
            if(is_int($val))
            {            
                $item = new c_int;
                $item->val = $val;
                $val = $item;
            }  
            array_push($this->vector,$val);         
        }                           
    }
    //SIZE
    public function size()
    {
        if($this->class_name == 'c_char')
            return strlen($this->vector);
        return count($this->vector);
    }
    
    public function get_val()
    {        
        return $this->vector;
    }
    public function is_base_datatype() { return false; }
    
    //VECTOR������
    public function get_class_name() 
    { 
        $type = new $this->class_name;
        $name = 'list<'.$type->get_class_name().'>';
        return $name; 
    }
    public function write(&$stream,$tag)
    {
        $cnt = count($this->vector);                
        if($this->class_name == 'c_char')
        {
        	//char��vector-type��c_simple_list��
            tars_header::_pack_header($stream,'c_simple_list',$tag);
            tars_header::_pack_header($stream,'c_char',0);
            $len = new c_int;
            $len->val = strlen($this->vector);
            $len->write($stream,0);                      
            $stream.=$this->vector;                                
        }else{
        	//type=c_list
            tars_header::_pack_header($stream,'c_list',$tag);
            $len = new c_int;
            $len->val = $cnt;
            $len->write($stream,0);            
            for($i = 0;$i < $cnt;$i++)
            {                
                $this->vector[$i]->write($stream,0);                
            }            
        }        
    }
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }        
        //���char��simpl list�ʹ������Ԫ�ص�list                            
        if($type != 'c_simple_list' && $type != 'c_list')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        tars_header::_unpack_header($stream,$type,$this_tag);    
        $this->vector = array();
                                
        if($type == 'c_simple_list')
        {
            //here ,unpack c_char tag
            tars_header::_unpack_header($stream,$type,$this_tag);
            $len = new c_int;
            $len->read($stream,0);            
            $this->vector  = substr($stream,0,$len->val);            
            $stream = substr($stream,$len->val);            
        }else{
            $len = new c_int;
            $len->read($stream,0);            
                                    
            for($i = 0; $i < $len->val ;$i++)
            {
                $item = clone $this->proto_type;                                
                $item->read($stream,0);                
                $this->vector[$i] = $item;                
            }                        
        }                         
    }     
};
//Map�ṹ
class c_map extends data_type
{
    private $map;
    private $key_class;
    private $key_proto;
    private $val_class;
    private $val_proto;
    
    public  function __construct($key_proto,$val_proto)
    {        
        if(is_null($key_proto) || is_null($val_proto))
        {
            throw new TARSException(__LINE__, PROTO_TYPE_UNKNOWN);
        }
        $this->map = array();
        //KEY��value����Ҫ��ź����ǵ������ֺ�ԭ������
        if(is_int($key_proto))
        {
            $key_proto = new c_int;
            $this->key_class = 'c_int';
            $this->key_proto = clone $key_proto;
        }else if(is_string($key_proto))
        {
            $key_proto = new c_string;
            $this->key_class = 'c_string';
            $this->key_proto = clone $key_proto;
        }else{            
            $this->key_class = get_class($key_proto);
            $this->key_proto = clone $key_proto;
        }
        
        if(is_int($val_proto))
        {
            $val_proto = new c_int;
            $this->val_class = 'c_int';
            $this->val_proto = clone $val_proto;
        }else if(is_string($val_proto))
        {
            $val_proto = new c_string;
            $this->val_proto = 'c_string';
            $this->val_proto = clone $val_proto;
        }else{
            $this->val_class = get_class($val_proto);
            $this->val_proto = clone $val_proto;            
        }        
    }
    public function __clone()
    {
        if(count($this->map) > 0)
        {
            foreach($this->map as $key=>&$val)
            {
                if(is_object($val['key']))
                {
                    $val['key'] = clone $val['key'];
                }
                if(is_object($val['val']))
                {
                    $val['val'] = clone $val['val'];
                }
            }
        }
        if(is_object($this->key_proto))
        {
            $this->key_proto = clone $this->key_proto;
        }
        if(is_object($this->val_proto))
        {
            $this->val_proto = clone $this->val_proto;
        }
    }
    //׷��һ��Ԫ�ظ�map
    public function push_back($key,$val)
    {
        if(is_int($key))
        {
            $key_class = 'c_int';
        }else if(is_string($key))
        {
            $key_class = 'c_string';
        }else if(is_object($key)){
            $key_class = get_class($key);
        }else{
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        }
        
        if(is_int($val))
        {
            $val_class = 'c_int';
        }else if(is_string($val))
        {
            $val_class = 'c_string';
        }else if(is_object($val)){
            $val_class = get_class($val);
        }else{                
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        }    

        //ͬһ��map��ֻ��һ��key���ͺ�value����
        if($this->key_class != $key_class || $this->val_class != $val_class)
        {
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        }
        
        $this->_set_data($key,$val);                 
    }
    
    private function _set_data($key,$val)
    {
    	//map��ÿ��key value����һ��array
        $pair = array();
        if(is_int($key))
        {
            $int_key = new c_int;
            $int_key->val = $key;
            $pair['key'] = $int_key;
        }else if(is_string($key)){
            $str_key = new c_string;
            $str_key->val = $key;
            $pair['key'] = $str_key;
        }else{
            $pair['key'] = $key;
        }        
        if(is_int($val))
        {
            $int_val = new c_int;
            $int_val->val = $val;
            $pair['val'] = $int_val;
        }else if(is_string($val)){
            $str_val = new c_string;
            $str_val->val = $val;
            $pair['val'] = $str_val;
        }else{
            $pair['val'] = $val;
        }
        //ARRAY����key ��value����Ԫ��
        array_push($this->map,$pair);
    }
    public function is_base_datatype() { return false; }
    public function get_class_name()
    {
        $key_type = new $this->key_class;
        $val_type = new $this->val_class;        
        $name = 'map<'.$key_type->get_class_name().','.$val_type->get_class_name().'>';
        return $name; 
    }
    //�õ�map�Ĵ洢���ݣ��������ⲿʹ��
    public function get_map()
    {
    	return $this->map;
    }
    public function get_val($key)
    {
        if(is_string($key))
        {
            $item_key = new c_string;
            $item_key->val = $key;
        }else if(is_int($key)){
            $item_key = new c_int;
            $item_key->val = $key;
        }
        
        //�������飬�ҵ���Ӧ��key���ڵ�������ȡ��value��Ӧ��ֵ
        if(count($this->map) > 0)
        {
            foreach($this->map as $map_key=>$map_val)
            {
                if($map_val['key'] == $item_key)
                {
                    return $map_val['val'];
                }
            }
        }
        return new $this->val_class;
    }
    
    public function write(&$stream,$tag)
    {
        $cnt = count($this->map);        
        tars_header::_pack_header($stream,'c_map',$tag);
        
        $len = new c_int;
        $len->val = $cnt;
        //д��key value�Ե���Ŀ
        $len->write($stream,0); 
        //��һ��д��û��key-val��
        for($i = 0;$i < $cnt;$i++)
        {
            $this->map[$i]['key']->write($stream,0);
            $this->map[$i]['val']->write($stream,1);    
        }              
    }    
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true){
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else{
                return;
            }
        }                          
        if($type != 'c_map')
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
        tars_header::_unpack_header($stream,$type,$this_tag);              
        $this->map = array();    
        $len = new c_int;        
        $len->read($stream,0);    
        //�������	         
        for($i = 0;$i < $len->val;$i++)
        {
            $this->map[$i] = array();
                            
            $key = clone $this->key_proto;
            $key->read($stream,0);
            $this->map[$i]['key'] = clone $key;
                        
            $val = clone $this->val_proto;
            $val->read($stream,1);
            $this->map[$i]['val'] = clone $val;            
        }        
    }  
	public function clear()
	{
		$this->map = array();
	}
};

abstract class c_struct extends data_type
{
    abstract function __clone();
    
    public function __set($name,$val)
    {
        foreach($this as $item_key=>$item)
        {            
            if($item_key == $name)
            {
                if(is_null($this->$name))
                {                        
                    throw new TARSException(__LINE__, TYPE_NOT_MATCH);
                }                    
                if(!is_object($val))
                {
                    if($this->$name->is_base_datatype() == true)
                        $this->$name->val = $val;                        
                }else{
                    if(get_class($val) != get_class($this->$name))
                        throw new TARSException(__LINE__, TYPE_NOT_MATCH);
                    $this->$name = $val;                            
                }                                                            
                return;
            }            
        }
        throw new TARSException(__LINE__, OBJECT_NOT_FOUND);               
    }
    
    public function is_base_datatype() { return false; }
    
    //���δ��ÿ����Ա
    public function _pack_every_elem(&$stream) 
    { 
        $id = 0;
        foreach($this as $key=>$item)
        {
            $item->write($stream,$id);
            $id++;
        }
    }
    //���ν��ÿ����Ա
    public function _unpack_every_elem(&$stream) 
    {
    	$tagId = 0;    	
        foreach($this as $key=>$item)
        {              	
            if(false === $this->skipToTag($stream,$tagId))
            {
            	return ;
            }
            else
            {            
            	$item->read($stream,$tagId);
        	}
        	$tagId++;
        }
    }
    public function write(&$stream,$tag)
    {
    	//struct�� c_struct_begin��ʼ c_struct_end��β
        tars_header::_pack_header($stream,'c_struct_begin',$tag);
        $this->_pack_every_elem($stream); 
        //tag������0
        tars_header::_pack_header($stream,'c_struct_end',0);
        
    }
    
    public function skipToTag(&$stream,$tag)
    {    	
    	$tagMax = $tag;
    	while(1)
    	{
    		if($tagMax >= TARS_MAXIMUM_TAG)
    		{
    			throw new TARSException(__LINE__, TAG_NOT_MATCH);   
    		}
    		tars_header::_peek_header($stream,$type,$this_tag);
    		if($type == 'c_struct_end')
    		{
    			return false;
    		}
    		if($tag <= $this_tag)
	        {
	        	//ok found
	            return $this_tag;
	        } 
	        else
	        {
	        	$this->_skip_base($stream);
	        	$tagMax++;
	        }      
    	}
         
    }
    
    public function read(&$stream,$tag,$isRequire = true)
    {
        tars_header::_peek_header($stream,$type,$this_tag,$isRequire);
        if($tag != $this_tag)
        {
            if($isRequire == true)
			{
                throw new TARSException(__LINE__, TAG_NOT_MATCH);   
            }else
			{
                return;
            }
        } 
		
        if($type != 'c_struct_begin')
		{
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);    
		}	
		
        tars_header::_unpack_header($stream,$type,$this_tag);  
		
        $this->_unpack_every_elem($stream);        
		
        tars_header::_unpack_header($stream,$type,$this_tag);  
        //��֤��β�Ƿ���ȷ. tag=0,type=  c_struct_end 
        if($this_tag != 0)
		{
            throw new TARSException(__LINE__, TAG_NOT_MATCH);   
		}			
        if($type != 'c_struct_end')
		{
            throw new TARSException(__LINE__, TYPE_NOT_MATCH);
		}
			
    }    
};
?>
