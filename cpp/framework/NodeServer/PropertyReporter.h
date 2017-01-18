/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
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

#ifndef  __PropertyReporter_H_
#define  __PropertyReporter_H_


#include "servant/Application.h"

//eg1: REPORT_COUNT("socket_error", 1);
//eg2: REPORT_SUM("GB_consume", 100);


//define reporter template
#define REPORT_INSTALL \
template<typename REPORTFUNC> \
PropertyReportPtr creatReport(const string & master, string msg, REPORTFUNC reportFunc) \
{ \
    PropertyReportPtr srp = Application::getCommunicator()->getStatReport()->getPropertyReport(msg); \
    if ( !srp ) \
    { \
        srp = Application::getCommunicator()->getStatReport()->createPropertyReport(msg, reportFunc); \
        srp->setMasterName(master);\
    } \
    return srp; \
};


//count report
#define REPORT_COUNT(master, str,value) \
PropertyReportPtr countReporter = creatReport(master, str, PropertyReport::count()); \
countReporter->report(value);


//sum report
#define REPORT_SUM(master, str,value) \
PropertyReportPtr sumReporter   = creatReport(master, str, PropertyReport::sum()); \
sumReporter->report(value);

//max report
#define REPORT_MAX(master, str,value) \
PropertyReportPtr maxReporter   = creatReport(master, str, PropertyReport::max()); \
maxReporter->report(value);

//set a reporter
REPORT_INSTALL;


#endif // __PropertyReporter_H_

