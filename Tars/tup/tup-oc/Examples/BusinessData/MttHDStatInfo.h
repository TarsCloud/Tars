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

//
//  MttHDStatInfo.h
//  MttHD
//
//  Created by 易 壬俊 on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDTupManager.h"
#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttSTStat.h>

#pragma mark - MttHDStatPVType

enum {
    MttHDStatPVType_WAP,
    MttHDStatPVType_WEB,
    MttHDStatPVType_RES,
    MttHDStatPVType_ERR,
};
#define MttHDStatPVType NSInteger

#pragma mark - MttHDStatApn

#define MttHDStatApn NSString*
#define MttHDStatApn_WAP    @"wap"
#define MttHDStatApn_NET    @"net"
#define MttHDStatApn_WIFI   @"wifi"
#define MttHDStatApn_NONE   @""

#pragma mark - MttHDUserBehavior

#define MttHDUserBehavior NSString*

#define MttHDUserBehavior_MaxTabel                      @"windmax"
#define MttHDUserBehavior_ForwardBtn                    @"bottomf"
#define MttHDUserBehavior_BackBtn                       @"bottomb"
#define MttHDUserBehavior_HomeBtn                       @"bottomp"
#define MttHDUserBehavior_BookmarkBtn                   @"bottomh"
#define MttHDUserBehavior_ReloadBtn                     @"bottomr"
#define MttHDUserBehavior_StopBtn                       @"cancel"
#define MttHDUserBehavior_AddBookmarkBtn                @"bkmark"
#define MttHDUserBehavior_AddQuicklinkBtn               @"toobarqmark"
#define MttHDUserBehavior_AddQuicklinkCell              @"startpageqmark”"
#define MttHDUserBehavior_AddDownloadTask               @"dltimes"
#define MttHDUserBehavior_FileManager                   @"filemanager"
#define MttHDUserBehavior_Login                         @"login"
#define MttHDUserBehavior_Logout                        @"logout"
#define MttHDUserBehavior_QQMusic                       @"qqmusic"
#define MttHDUserBehavior_FImport                       @"fimport"
#define MttHDUserBehavior_FExport                       @"fexport"
#define MttHDUserBehavior_FSync                         @"fsync"
#define MttHDUserBehavior_NewPageButton                 @"newpage"
#define MttHDUserBehavior_AdvancedFunctionPanel         @"panelclick"
#define MttHDUserBehavior_Fullscreen                    @"fullscreen"
#define MttHDUserBehavior_FullscreenExit                @"fullscreen_exit"
#define MttHDUserBehavior_FullscreenExitIn              @"fullscreen_exit_in"
#define MttHDUserBehavior_FullscreenBack                @"fullscreen_back"
#define MttHDUserBehavior_FullscreenForward             @"fullscreen_forward"
#define MttHDUserBehavior_FullscreenRefresh             @"fullscreen_refresh"
#define MttHDUserBehavior_QuickCloseFocusedTab          @"quickclose"
#define MttHDUserBehavior_LightSet                      @"lightset"
#define MttHDUserBehavior_PrivacyMode                   @"privacymode"
#define MttHDUserBehavior_Setting                       @"setting"
#define MttHDUserBehavior_GestureDoubleClose            @"gdctab"
#define MttHDUserBehavior_GestureTwoFingerLeftSleek     @"g2fls"
#define MttHDUserBehavior_GestureTwoFingerRightSleek    @"g2frs"
#define MttHDUserBehavior_GestureTwoFingerTapTop        @"g2fhc"
#define MttHDUserBehavior_GestureTwoFingerTapBottom     @"g2fbc"
#define MttHDUserBehavior_AdvancedFunctionPanelSearch   @"searchpage_by_cp"
#define MttHDUserBehavior_TwoFingerOpenLinkInBackground @"two_hand_c_h_ob"
#define MttHDUserBehavior_GestureTwoFingerUpSleek       @"g2fus"
#define MttHDUserBehavior_GestureTwoFingerDownSleek     @"g2fds"
#define MttHDUserBehavior_QuickNewFocusedTab            @"new_t_c_fp"
#define MttHDUserBehavior_QuickReload                   @"reload_c_fp"
#define MttHDUserBehavior_QuickHome                     @"home_c_fp"
#define MttHDUserBehavior_QuickBack                     @"back_clicked_fp"
#define MttHDUserBehavior_AutoAddQuickLink              @"atsdbs"
#define MttHDUserBehavior_DownloadSuccessCount          @"dlotimes"
#define MttHDUserBehavior_IpaDownlaodPV                 @"dlipa"
#define MttHDUserBehavior_QuickTurnPage                 @"menu_fpage"
#define MttHDUserBehavior_QuickFontSize                 @"menu_fsize"
#define MttHDUserBehavior_CrashRecovery                 @"recovery"
#define MttHDUserBehavior_SettingMostVisit              @"A50"
#define MttHDUserBehavior_SettingLastClose              @"A43"
#define MttHDUserBehavior_QuikcLinkRemove               @"qlldel"
#define MttHDUserBehavior_QuikcLinkEdit                 @"qlledit"
#define MttHDUserBehavior_QuikcLinkNavAdd               @"nav_adql"
#define MttHDUserBehavior_ShareQQWeibo                  @"menu_share_qqmb"
#define MttHDUserBehavior_ShareSinaWeibo                @"menu_share_snmb"
#define MttHDUserBehavior_ShareRenRen                   @"menu_share_rrmb"
#define MttHDUserBehavior_TransService                  @"trans"
#define MttHDUserBehavior_StartPageSwitchTabClick       @"C12"
#define MttHDUserBehavior_Searchassociation             @"Searchasso"
#define MttHDUserBehavior_UrlImportdassociation         @"importasso"
#define MttHDUserBehavior_TabClickClose                 @"clickclose"

#pragma mark - MttHDStatInfo

@interface MttHDStatInfo : MttHDPersistentObject 
{
    NSMutableDictionary *_totalPV;
    NSMutableDictionary *_outerPV;
    NSMutableDictionary *_innerPV;
    NSMutableDictionary *_entryPV;
}

@property (readonly) NSInteger           statValue;
@property (readonly) NSInteger           useTime;
@property (retain)   NSArray             *totalPV;
@property (retain)   NSArray             *outerPV;
@property (retain)   NSArray             *innerPV;
@property (retain)   NSArray             *entryPV;
@property (retain)   NSMutableDictionary *behaviors;
@property (assign)   NSInteger           maximumTabs;
@property (retain)   MttHDStatApn        apn;           // TODO:renjunyi bad smell

+ (MttHDStatInfo *)theSpecial;
+ (MttHDStatInfo *)clearTheSpecial;

- (NSString *)protocol;

- (void)addPV:(NSString *)domain type:(MttHDStatPVType)type apn:(MttHDStatApn)apn flow:(NSUInteger)flow;
- (void)addEntryPV:(NSString *)url type:(MttEntryType)type;
- (void)statUserBehavior:(MttHDUserBehavior)behavior;
- (void)setMaximumTabs:(NSInteger)maximumTabs;

@end
