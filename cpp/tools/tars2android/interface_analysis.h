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

#ifndef INTERFACE_ANALYSIS_H_
#define INTERFACE_ANALYSIS_H_

#include "parse.h"
#include <map>
#include <vector>

class InterfaceAnalysis {
public:
	InterfaceAnalysis();

	const std::map<std::string, StructPtr>& getAllStructs() const;
	const std::map<std::string, EnumPtr>& getAllEnums() const;
	const std::map<std::string, ConstPtr>& getAllConsts() const;

	void analysis(const InterfacePtr& interfacePtr);
	void analysis(const vector<InterfacePtr>& interfacePtrs);

private:
	void analysis(const StructPtr& structPtr);
	void analysis(const TypePtr& typePtr);

private:
	std::map<std::string, StructPtr> mAllStructs;
	std::map<std::string, EnumPtr> mAllEnums;
	std::map<std::string, ConstPtr> mAllConsts;
};



#endif /* INTERFACE_ANALYSIS_H_ */
