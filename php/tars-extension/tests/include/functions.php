<?php
/**
 * Created by PhpStorm.
 * User: liujingfeng.a
 * Date: 2018/4/25
 * Time: 19:05.
 */
if (!function_exists('fromArray')) {
    function fromArray($data, &$structObj) {
        if (!empty($data)) {
            foreach ($data as $key => $value) {
                if (method_exists($structObj, 'set'.ucfirst($key))) {
                    call_user_func_array([$this, 'set'.ucfirst($key)], [$value]);
                } elseif ($structObj->$key instanceof \TARS_Struct) {
                    fromArray($value, $structObj->$key);
                } else {
                    $structObj->$key = $value;
                }
            }
        }
    }
}

if (!function_exists('floatComp')) {
    function floatComp($left_operand, $right_operand, $scale = null) {
        $tmp = 1;
        while ($scale--) {
            $tmp = $tmp / 10;
        }
        if (($left_operand - $right_operand) < $tmp && ($right_operand - $left_operand) < $tmp) {
            return 0;
        } else {
            return ($left_operand > $right_operand) ? 1 : -1;
        }
    }
}
