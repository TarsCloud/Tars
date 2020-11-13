#!/bin/bash
function LOG_ERROR()
{
	if (( $# < 1 ))
	then
		echo -e "\033[33m usesage: LOG_ERROR msg \033[0m";
	fi
	
	local msg=$(date +%Y-%m-%d" "%H:%M:%S);

    msg="${msg} $@";

	echo -e "\033[31m $msg \033[0m";	
}

function LOG_INFO()
{
	local msg=$(date +%Y-%m-%d" "%H:%M:%S);
	
	for p in $@
	do
		msg=${msg}" "${p};
	done
	
	echo -e "\033[32m $msg \033[0m"  	
}


WORKING_DIR=$(cd $(dirname "$0") && pwd)

# read tag version
tarsTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always)
LOG_INFO "Building framework docker image for ${tarsTag} deploy"
tagVersions=(${tarsTag//./ })
mainVersion=0
subVersion=0
buildNum=0
for ((i=0; i<${#tagVersions[@]}; i++))
do
    version=${tagVersions[$i]}
    if [ ${version: 0: 1} = "v" ]; then
        version=${version: 1}
    elif [ ${version: 0: 1} = "V" ]; then
        version=${version: 1}
    fi
    case $i in
        0)
            mainVersion=$version
            ;;
        1)
            subVersion=$version
            ;;
        2)
            buildNum=$version
            ;;
    esac
done
LOG_INFO "Main version detected: $mainVersion"
LOG_INFO "  Subversion detected: $subVersion"
LOG_INFO "Build number detected: $buildNum"

mkdir -p /tmp/framework-auto-build
# Dockerfile
cp docker/framework/Dockerfile-Deploy /tmp/framework-auto-build/
# TarsFramework
rm -rf /tmp/framework-auto-build/framework
cp -r ./framework /tmp/framework-auto-build/framework

# TarsWeb
rm -rf /tmp/framework-auto-build/web
cp -r ./web /tmp/framework-auto-build/web

# build docker image
cd /tmp/framework-auto-build/
LOG_INFO "change context to /tmp/framework-auto-build/"

LOG_INFO "Building framework docker image tarscloud/framework:$tarsTag"
docker build . --file "Dockerfile-Deploy" --tag tarscloud/framework:$tarsTag
errNo=$(echo $?)
if [ "$errNo" != '0' ]; then
    LOG_ERROR "Failed to build framework docker, tag: $tarsTag"
    exit $errNo
fi

# test docker image
rm -rf /tmp/framework-auto-build/TarsDemo
git clone https://github.com/TarsCloud/TarsDemo
cd /tmp/framework-auto-build/TarsDemo
LOG_INFO "Starting framework image test."
# run TarsDemo to test framework based on local image before docker push
./autorun.sh $tarsTag latest false false
errNo=$(echo $?)
if [ "$errNo" != '0' ]; then
    LOG_ERROR "Framework test failed, tag: $tarsTag"
    exit $errNo
fi
