<?php
require_once('tars.php');
require_once('tup.php');
require_once('Hello_tup.php');

function mybin2hex($str) {
	$hex = "";
	$i = 0;
	do {
		$hex .= sprintf("%02x ", ord($str{$i}));
		$i++;
		if ($i % 16 == 0)
		{
			printf("%s", $hex);
			$hex = "";
			printf("\n");
		}
	} while ($i < strlen($str));
	printf("%s\n", $hex);
	return $hex;
}

function testpack()
{
	$tup = new UniPacket;
	$helloworld = new HelloWorld;

	$helloworld->nId->val = 23;
	$helloworld->sMsg->val ="aron";
	$helloworld->a->b->val = 234;

	$tup->put('hello',$helloworld);

	$tup->_encode($Buffer);
	
	$len = strlen($Buffer);

	//printf("len of buffer:%d\n",$len);

	$tupdec = new UniPacket;
	$tupdec->_decode($Buffer);

	$helloworlddec = new HelloWorld;

	$tupdec->get('hello',$helloworlddec);
	echo "test UniPacket: ";
	print_r($helloworlddec->sMsg);
	print_r($helloworlddec->a);
	
}
function testtup_unipacket()
{
	$tup = new tup_unipacket;
	
	static $id = 0;
	$id++;
	
	$name = 'Test.HelloServer.HelloImpObj';
	$funcName = 'testPerson'; 
   
	$tup->setRequestId($id);
	$tup->setServantName($name);
	$tup->setFuncName($funcName);
	
	$helloworld = new HelloWorld;

	$helloworld->nId->val = 23;
	$helloworld->sMsg->val ="aron";
	$helloworld->a->b->val = 234;

	$tup->put('hello',$helloworld);

	$tup->_encode($Buffer);
	
	
	$len = strlen($Buffer);

	//printf("len of enbuffer:%d\n",$len);

	$tupdec = new tup_unipacket;
	$tupdec->_decode($Buffer);

	$helloworlddec = new HelloWorld;

	$tupdec->get('hello',$helloworlddec);
	echo "test tup_unipacket: ";
	print_r($helloworlddec->sMsg);
	
}
testpack();
testtup_unipacket();

function testHello() {	
	 
        //����Զ�̵��ú������������
        //�����ͻ���tup����,���ñ����ķ�������Զ�̵��ú�������
        $tup = new tup_unipacket();    
		
        static $id = 1;
        $id++;
		
        $name = 'Test.HelloServer.HelloImpObj';
        $funcName = 'testHello'; 
		
        $tup->setVersion(3);
		
        $tup->setRequestId($id);
        $tup->setServantName($name);
        $tup->setFuncName($funcName);
        
        //////////////////////////////////////////
	   //����Զ�̵��ú������������
	   $hello = new c_string;
	   
	   $hello->val = '123456789';

       $tup->put('s',$hello);

	   $sh = new c_short;
	   $sh->val = -238;
	   
	   $vsh = new c_vector(new c_short);
	   $vsh->push_back($sh);
	   $vsh->push_back($sh);
	   $vsh->push_back($sh);
	   $vsh->push_back($sh);
	   $vsh->push_back($sh);
	   $vsh->push_back($sh);
	   
	   $tup->put('vsh',$vsh);
	   
       $tup->_encode($sendBuffer);
		
        //mybin2hex($sendBuffer);
		
		//printf("\n");
        //echo "encode success\n";
        
       // $fp = fsockopen("10.6.208.181", 45688, $errno, $errstr, 3);
	    $fp = fsockopen("10.12.22.10", 45688, $errno, $errstr, 3);
		$respBuffer = '';
		if(!$fp){
			echo "$errstr ($errno)<br>n";
		}
		else {
			fputs($fp, $sendBuffer);
            //echo "fputs success\n";
			
			while ($line = fgets($fp)) 
			{
				$respBuffer .= $line; 
			}
		}
		
		fclose($fp);     
		//echo "\nreceive success:\n";
		//mybin2hex($respBuffer);
		//////////////////////////////////////////
		//�½�tup����"tupResp"�����뷵�ذ�		
		$tupResp = new tup_unipacket();
		
        $tupResp->_decode($respBuffer);
		
		if($tupResp->getResultCode() == 0)
		{
			$sOut = new c_string;
			
			$tupResp->get('r',$sOut);
			echo "test testHello: ";
			print_r($sOut);
		}
		else
		{
			echo "error: " . ($tupResp->getResultDesc()) ."\n";
		}
}

function testPerson() {	
	 
        //����Զ�̵��ú������������
        //�����ͻ���tup����,���ñ����ķ�������Զ�̵��ú�������
        $tup = new tup_unipacket();    
		
        static $id = 0;
        $id++;
		
        $name = 'Test.HelloServer.HelloImpObj';
        $funcName = 'testPerson'; 
		
        $tup->setVersion(3);
	   
        $tup->setRequestId($id);
        $tup->setServantName($name);
        $tup->setFuncName($funcName);
        
        //////////////////////////////////////////
	   //����Զ�̵��ú������������
	   $helloPerson = new HelloPerson;
	   
	   $helloPerson->nId->val = 2;
	   $helloPerson->sName->val = "aron";
	   $helloPerson->bMan->val = 1;

	   //�ṹ�帳ֵ
	   $helloPerson->shello->nId->val = -4321;
	   $helloPerson->shello->sMsg->val = "i am from php client";
	   
	   $str1 = new c_string;
	   $str1->val = "A";
	   
	   $helloPerson->shello->vNews->push_back($str1);
	   
	   $str2 = new c_string;
	   $str2->val = "B";
	   
	   $helloPerson->shello->vNews->push_back($str2);
	   
	   $helloPerson->shello->mAddr->push_back($helloPerson->nId, $helloPerson->nId);
	   
	   $helloPerson->shello->a->b->val = 1111;
	   
       $tup->put('stPersonIn',$helloPerson);
		
       $tup->_encode($sendBuffer);
		
        //mybin2hex($sendBuffer);
		
		//printf("\n");
        //echo "encode success\n";
        
	    $fp = fsockopen("10.12.22.10", 45688, $errno, $errstr, 3);
		$respBuffer = '';
		if(!$fp){
			echo "$errstr ($errno)<br>n";
		}
		else {
			fputs($fp, $sendBuffer);
           // echo "fputs success:\n";
			
			while ($line = fgets($fp)) 
			{
				//mybin2hex($line);
				$respBuffer .= $line; 
			}
		}
		fclose($fp);     
		//echo "\nreceive success:\n";
		//mybin2hex($respBuffer);
		//////////////////////////////////////////
		//�½�tup����"tupResp"�����뷵�ذ�		
		$tupResp = new tup_unipacket();
		
        $tupResp->_decode($respBuffer);
		//�жϵ����Ƿ�ɹ�	
		if($tupResp->getResultCode() === 0)
		{
			$sOut = new HelloWorld;
			//����get��name����Ϊtars�ļ��ж��������������ƣ�Ҫ����һ��
			$tupResp->get('stPersonOut',$sOut);
			echo "test testPerson: ";
			print_r($sOut->sMsg);
		}
		else
		{
			echo "error: " . ($tupResp->getResultDesc()) ."\n";
		}
		
}
while(true)
{
testpack();

testtup_unipacket();
testHello();

testPerson();
}
?>

