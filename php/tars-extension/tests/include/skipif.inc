<?php
if (substr(PHP_OS, 0, 3) == 'WIN')
{
    exit("skip for Windows");
}
if(!extension_loaded("phptars")) {
    exit("skip phptars extension not loaded");
}

microtime();