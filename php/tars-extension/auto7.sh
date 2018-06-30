#!/bin/sh

phpize=/data/env/runtime/php-7.1.7/bin/phpize
$phpize --clean
$phpize
phpconfig=/data/env/runtime/php-7.1.7/bin/php-config
./configure --enable-phptars --with-php-config=$phpconfig
make
make install
$phpize --clean

