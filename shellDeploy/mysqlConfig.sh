#!/usr/bin/expect
set timeout 5
set username [lindex $argv 0]  
set password [lindex $argv 1]  
#set hostname [lindex $argv 2]  
set tarsDefaultPassword "tars2015"


spawn mysql -u $username -p$password
send "set global validate_password_policy=0;\r"
send "set global validate_password_length=1;\r"
send "set password='$tarsDefaultPassword';\r"
send "grant all on *.* to 'tars'@'%' identified by '$tarsDefaultPassword' with grant option;\r"
send "grant all on *.* to 'tars'@'localhost' identified by '$tarsDefaultPassword' with grant option;\r"
send "flush privileges;\r"
#send "exit"

send "create database db_tars;\r"
send "create database tars_stat;\r"
send "create database tars_property;\r"
send "create database db_tars_web;\r"
send "show databases;\r"
send "use db_tars;\r"
send "source db_tars.sql\r"
send "source tarslog.sql\r"
send "source tarsstat.sql\r"
send "source tarsproperty.sql\r"
send "source tarsqueryproperty.sql\r"
send "source tarsquerystat.sql\r"

#interact

expect "wait 5 seconds:"
send "exit\r"
