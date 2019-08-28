#!/usr/bin/expect
set timeout 5
set username [lindex $argv 0]  
set password [lindex $argv 1]  
set hostname [lindex $argv 2]  
set tarsDefaultPassword "tars2015"

spawn mysql -u $username -p$password
send "show databases;\r"
send "use db_tars_web;\r"
send "source db_tars_web.sql\r"
#interact
expect "wait 5 seconds:"
send "exit\r"

