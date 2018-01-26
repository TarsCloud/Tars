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
#include "tars_filter.h"

TarsFilter::TarsFilter() {
	addSid("TRom::E_ROM_DEVICE_TYPE");
	addSid("TRom::EAPNTYPE");
	addSid("TRom::EIPType");
	addSid("TRom::ELOGINRET");
	addSid("TRom::ENETTYPE");
	addSid("TRom::IPListReq");
	addSid("TRom::IPListRsp");
	addSid("TRom::JoinIPInfo");
	addSid("TRom::LoginReq");
	addSid("TRom::LoginRsp");
	addSid("TRom::RomBaseInfo");
	addSid("TRom::SECPROXY_RETCODE");
	addSid("TRom::SecureReq");
	addSid("TRom::SecureRsp");
}

void TarsFilter::addSid(const std::string& sid) {
	mFilterTarsSidMap.insert(std::pair<std::string, bool>(sid, true));
}

void TarsFilter::filterStructs(
		const std::map<std::string, StructPtr>& orignalMap
		, std::map<std::string, StructPtr>& resultMap) const {
	for (std::map<std::string, StructPtr>::const_iterator it = orignalMap.begin()
			; it != orignalMap.end(); ++it) {
		if (mFilterTarsSidMap.find(it->first) != mFilterTarsSidMap.end()) {
			continue;
		}
		resultMap.insert(std::pair<std::string, StructPtr>(it->first, it->second));
	}
}

void TarsFilter::filterEnums(
		const std::map<std::string, EnumPtr>& orignalMap
		, std::map<std::string, EnumPtr>& resultMap) const {
	for (std::map<std::string, EnumPtr>::const_iterator it = orignalMap.begin()
			; it != orignalMap.end(); ++it) {
		if (mFilterTarsSidMap.find(it->first) != mFilterTarsSidMap.end()) {
			continue;
		}
		resultMap.insert(std::pair<std::string, EnumPtr>(it->first, it->second));
	}
}
