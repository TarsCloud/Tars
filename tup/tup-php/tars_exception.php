<?php

define ('TYPE_NOT_MATCH',     28001);
define ('LEN_NOT_MATCH',      28002);
define ('VAR_NOT_FOUND',      28003);
define ('OUTOF_RANGE',        28004);
define ('STRING_TYPE_UNKNOWN',28005);
define ('HEADER_TYPE_UNKNOWN',28006);
define ('HEADER_TYPE_ERROR',  28007);
define ('HEADER_TAG_ERROR',   28008);
define ('OBJECT_NOT_FOUND',   28009);
define ('STREAM_LEN_ERROR',   28010);
define ('TAG_NOT_MATCH',      28011);
define ('UNKNOWN_SERVICE',    28012);
define ('UNKNOWN_FUNCTION',   28013);
define ('PROTO_TYPE_UNKNOWN', 28014);

class TARSException extends Exception
{
    function __construct($msg = "", $code = ECODE_UNKOWN)
    {
        parent::__construct($msg, intval($code));
    }
    public function NormalJsonOutput()
    {
    }
    public function CrossDomainJsonOutpon()
    {
    }
}; 

?>