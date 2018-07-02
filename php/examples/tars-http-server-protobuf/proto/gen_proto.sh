#!/bin/bash
cd $(dirname $0)
project_name=yb_account
../bin/gen_proto  --controller_dir=../src/app/Controllers  $project_name

proto_dir=../src/app/proto
rm -rf $proto_dir/$project_name
mkdir -p    $proto_dir
cp -rf ./php_${project_name}  $proto_dir/$project_name

