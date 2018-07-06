<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/18
 * Time: 下午11:35.
 */

 /**
  * @param $proto \Taf\TJCE_VECTOR(new \Taf\TJCE_Map(\Taf\TJCE::STRING,new SimpleStruct))
  * 需要递归的实例化出来
  */
 function createInstance($proto)
 {
     if (isBasicType($proto)) {
         return convertBasicType($proto);
     } elseif (!strpos($proto, '(')) {
         $structInst = new $proto();

         return $structInst;
     } else {
         $pos = strpos($proto, '(');
         $className = substr($proto, 0, $pos);
         echo 'className:'.$className."\n";
         if ($className == '\Taf\TJCE_Vector') {
             $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
             echo 'vector next:'.$next."\n";
             $args[] = createInstance($next);
         } elseif ($className == '\Taf\TJCE_Map') {
             $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
             echo 'map next:'.$next."\n";
             $pos = strpos($next, ',');
             $left = substr($next, 0, $pos);
             $right = trim(substr($next, $pos, strlen($next) - $pos), ',');

             echo 'left:'.$left."\n";
             echo 'right:'.$right."\n";

             $args[] = createInstance($left);
             $args[] = createInstance($right);
         } elseif (isBasicType($className)) {
             $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
             echo 'basic next:'.$next."\n";
             $basicInst = createInstance($next);
             $args[] = $basicInst;
         } else {
             $structInst = new $className();
             $args[] = $structInst;
         }
         echo 'args:'.var_export($args, true)."\n";
         $ins = new $className(...$args);
     }

     return $ins;
 }

 function isBasicType($type)
 {
     $basicTypes = [
        '\Taf\TJCE::BOOL',
        '\Taf\TJCE::CHAR',
        '\Taf\TJCE::CHAR',
        '\Taf\TJCE::UINT8',
        '\Taf\TJCE::UINT8',
        '\Taf\TJCE::SHORT',
        '\Taf\TJCE::UINT16',
        '\Taf\TJCE::INT32',
        '\Taf\TJCE::UINT32',
        '\Taf\TJCE::INT64',
        '\Taf\TJCE::FLOAT',
        '\Taf\TJCE::DOUBLE',
        '\Taf\TJCE::STRING',
        '\Taf\TJCE::INT32',
    ];

     return in_array($type, $basicTypes);
 }

function convertBasicType($type)
{
    $basicTypes = [
        '\Taf\TJCE::BOOL' => 1,
        '\Taf\TJCE::CHAR' => 2,
        '\Taf\TJCE::UINT8' => 3,
        '\Taf\TJCE::SHORT' => 4,
        '\Taf\TJCE::UINT16' => 5,
        '\Taf\TJCE::FLOAT' => 6,
        '\Taf\TJCE::DOUBLE' => 7,
        '\Taf\TJCE::INT32' => 8,
        '\Taf\TJCE::UINT32' => 9,
        '\Taf\TJCE::INT64' => 10,
        '\Taf\TJCE::STRING' => 11,
    ];

    return $basicTypes[$type];
}

$str1 = '\Taf\TJCE_Vector(\Taf\TJCE::STRING)';

$str2 = '\Taf\TJCE_Map(\Taf\TJCE::STRING,\Taf\TJCE::STRING)';

$str3 = '\Taf\TJCE_Vector(SimpleStruct)';

$str4 = '\Taf\TJCE_Map(\Taf\TJCE::STRING,SimpleStruct)';

$str5 = '\Taf\TJCE_Vector(\Taf\TJCE_Map(\Taf\TJCE::STRING,SimpleStruct))';

$str6 = '\Taf\TJCE_Map(\Taf\TJCE::STRING,\Taf\TJCE_Vector(SimpleStruct))';

$str7 = '\Taf\TJCE_Vector(\Taf\TJCE_Map(\Taf\TJCE::STRING,\Taf\TJCE_Vector(SimpleStruct)))';

$str8 = '\Taf\TJCE_Map(\Taf\TJCE::STRING,\Taf\TJCE_Vector(\Taf\TJCE_Map(\Taf\TJCE::STRING,SimpleStruct)))';

$inst = createInstance($str2);
var_dump($inst);
