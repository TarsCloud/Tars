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
#include "interface_analysis.h"

#include <vector>

InterfaceAnalysis::InterfaceAnalysis() {
}

void InterfaceAnalysis::analysis(const vector<InterfacePtr>& interfacePtrs) {
	for (size_t index = 0; index < interfacePtrs.size(); ++index) {
		analysis(interfacePtrs[index]);
	}
}

void InterfaceAnalysis::analysis(const InterfacePtr& interfacePtr) {
	const vector<OperationPtr>& operations = interfacePtr->getAllOperationPtr();

	for (size_t operationIndex = 0; operationIndex < operations.size(); ++operationIndex) {
		const OperationPtr& operation = operations[operationIndex];

		analysis(operation->getReturnPtr()->getTypePtr());

		const vector<ParamDeclPtr>& paramDecls = operation->getAllParamDeclPtr();

		for (size_t paramIndex = 0; paramIndex < paramDecls.size(); paramIndex++) {
			const ParamDeclPtr& paramDeclPtr = paramDecls[paramIndex];
			analysis(paramDeclPtr->getTypeIdPtr()->getTypePtr());
		}
	}
}

void InterfaceAnalysis::analysis(const StructPtr& structPtr) {
	const vector<TypeIdPtr>& allMembersPtr = structPtr->getAllMemberPtr();
	for (size_t index = 0; index < allMembersPtr.size(); ++index) {
		analysis(allMembersPtr[index]->getTypePtr());
	}
}

void InterfaceAnalysis::analysis(const TypePtr& typePtr) {
	VectorPtr vPtr = VectorPtr::dynamicCast(typePtr);
	if (vPtr) {
		analysis(vPtr->getTypePtr());
		return ;
	}

	MapPtr mPtr = MapPtr::dynamicCast(typePtr);
	if (mPtr) {
		analysis(mPtr->getLeftTypePtr());
		analysis(mPtr->getRightTypePtr());
		return ;
	}

	StructPtr sPtr = StructPtr::dynamicCast(typePtr);
	if (sPtr) {
		// 说明已经分析过，并且找到了
		if (mAllStructs.find(sPtr->getSid()) != mAllStructs.end()) {
			return ;
		}

		mAllStructs.insert(std::pair<std::string, StructPtr>(sPtr->getSid(), sPtr));
		analysis(sPtr);
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(typePtr);
	if (ePtr) {
		if (mAllEnums.find(ePtr->getSid()) != mAllEnums.end()) {
			return ;
		}

		mAllEnums.insert(std::pair<std::string, EnumPtr>(ePtr->getSid(), ePtr));
	}
}

const std::map<std::string, StructPtr>& InterfaceAnalysis::getAllStructs() const {
	return mAllStructs;
}

const std::map<std::string, EnumPtr>& InterfaceAnalysis::getAllEnums() const {
	return mAllEnums;
}

const std::map<std::string, ConstPtr>& InterfaceAnalysis::getAllConsts() const {
	return mAllConsts;
}


