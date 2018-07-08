#########################################################################
# File Name: update.sh
# Author: berli
# mail: berli@tencent.com
# Created Time: 2018年06月29日 星期五 19时42分10秒
#########################################################################
#!/bin/bash
git remote add upstream https://github.com/Tencent/Tars.git  
git fetch upstream 
git merge upstream/master
git commit -a 
git push
