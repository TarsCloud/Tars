<?php
/**
 * Created by PhpStorm.
 * User: liujingfeng.a
 * Date: 2018/4/25
 * Time: 19:05.
 */
function fromArray($data, &$structObj)
{
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
