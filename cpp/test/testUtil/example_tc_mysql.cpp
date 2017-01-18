/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "util/tc_mysql.h"
#include <iostream>

using namespace tars;

TC_Mysql mysql;

void test()
{
//    cout << mysql.getVariables("character_set_client") << endl;

    TC_Mysql::MysqlData data;
    data = mysql.queryRecord("select * from t_app_users");
    for(size_t i = 0; i < data.size(); i++)
    {
        cout << data[i]["ID"] << endl;
    }
}

void testInsert()
{
    map<string, pair<TC_Mysql::FT, string> > m;
    m["ID"]     = make_pair(TC_Mysql::DB_INT, "2334");
    m["USERID"] = make_pair(TC_Mysql::DB_STR, "abcttt");
    m["APP"]    = make_pair(TC_Mysql::DB_STR, "abcapbbp");
    mysql.insertRecord("t_user_logs", m);
    mysql.replaceRecord("t_user_logs", m);

    mysql.updateRecord("t_user_logs", m, "where ID=2234");
}

int main(int argc, char *argv[])
{
    try
    {
        try{
        mysql.init("172.25.38.21", "pc", "pc@sn", "db_dmqq_system");
        mysql.connect();

        }catch(exception &ex)
        {
            cout << ex.what() << endl;
        }
        mysql.execute("select * from t_app_users");
        test();

//        sleep(10);
//        test();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


