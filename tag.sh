# Copyright (c) 2020
# Linux Foundation Projects
#       Link: https://www.linuxfoundation.org/projects/
# TARS Foundation Projects
#       Link: https://github.com/TarsCloud
# All rights reserved.

#!/bin/bash

#update & merge code tools

function show()
{
    branch=$1

    echo "tars:"; git branch -a | grep "*";
    cd framework; echo "framework:"; git branch -a | grep "*"; 
    cd tarscpp; echo "tarscpp:"; git branch -a | grep "*"; 
    cd servant/protocol; echo "protocol:"; git branch -a | grep "*"; 
    cd ../../../..
}


function update()
{
    branch=$1

    git submodule update --remote --recursive

    cd framework; git checkout $branch; git pull --rebase; 
    cd tarscpp; git checkout $branch; git pull --rebase;
    cd servant/protocol; git checkout $branch; git pull --rebase;
    cd ../../../..
}

function merge()
{
    version1=$1
    version2=$2

    cd framework; git checkout $version2; git pull $version2 --rebase; git checkout $version1; git merge $version2;
    cd tarscpp; git checkout $version2; git pull $version2 --rebase; git checkout $version1; git merge  $version2;
    cd servant/protocol; git checkout $version2; git pull $version2 --rebase; git checkout $version1; git merge $version2;
    cd ../../../..
}

function branch()
{
    version1=$1
    cd framework/tarscpp/servant/protocol; git checkout $version1;
    cd ../..; git checkout $version1;
    cd ..; git checkout $version1;
    cd ..; git checkout $version1; 
}

function commit()
{
    version1=$1
    cd framework/tarscpp/servant/protocol; git checkout $version1; git commit -a -m "update merge";
    cd ../..; git checkout $version1; git commit -a -m "update protocol";
    cd ..; git checkout $version1; git commit -a -m "update tarscpp";
    cd ..; git checkout $version1; git commit -a -m "update framework";
}

function push()
{
    version1=$1
    cd framework; git checkout $version1; git push origin $version1;
    cd tarscpp; git checkout $version1; git push origin $version1;
    cd servant/protocol; git checkout $version1; git push origin $version1;
    cd ../../../..;git checkout $version1; git push origin $version1;
}

version=$2
case $1 in
    "show")
        show
        ;;
    "branch")
        branch $2
        show
        ;;
    "update")
        update $2
        ;;
    "commit")
        commit $2
        ;;
    "push")
        push $2
        ;;
    "merge-master")
        merge master release/2.4
        ;;
    "merge-release")
        merge release/2.4 master 
        ;;
    "all")
        show
        update release/2.4
        merge-release
        commit release/2.4    
        push release/2.4
        update master
        merge-master
        commit master
        push master
        ;;
    *)
        echo "$0 show, show all branch"
        echo "$0 branch [branch], checkout to branch"
        echo "$0 update [branch], update branch to new"
        echo "$0 commit [branch], commit branch"
        echo "$0 push [branch], push branch"
        echo "$0 merge-master], checkout master and merge release to master"
        echo "$0 merge-release], checkout relase and merge master to release"
        echo "$0 all, do all"
        ;;
esac

