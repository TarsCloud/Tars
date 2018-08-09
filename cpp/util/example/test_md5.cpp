/*************************************************************************
    > File Name: test_md5.cpp
    > Author: berli
    > Mail: berli@tencent.com 
    > Created Time: 2018年08月09日 星期四 14时52分33秒
 ************************************************************************/

#include<iostream>
#include "util/tc_common.h"
#include "util/tc_md5.h"
using namespace std;
using namespace tars;

int main(int argc, char*argv[])
{
	string lsTemp(argv[1]);
	lsTemp = TC_Common::trimleft(lsTemp);
	lsTemp = TC_Common::trimright(lsTemp);
	cout<<"src="<<lsTemp<<" md5="<<TC_MD5::md5str(lsTemp)<<endl;
	
}
