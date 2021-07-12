/****************************************************************************
 * Copyright (C) 2017,2018 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <wups.h>
#include <gx2/swap.h>
#include <vpad/input.h>
#include <utils/logger.h>

#include "utils/voice_swapper.h"
#include "common/c_retain_vars.h"

DECL_FUNCTION(int32_t, AXSetVoiceDeviceMixOld, void *v, int32_t device, uint32_t id, void *mix){
    if(gSwap){ device = !device;}
    if(VOICE_SWAP_LOG == 1){log_printf("AXSetVoiceDeviceMixOld voice: %08X device: %d, mix: %08X\n",v,device,mix);}
    VoiceSwapper_setMix(v,device,mix);
    return real_AXSetVoiceDeviceMixOld(v,device,id,mix);
}

DECL_FUNCTION(int32_t, AXSetVoiceDeviceMix, void *v, int32_t device, uint32_t id, void *mix){
    if(gSwap){ device = !device;}
    if(VOICE_SWAP_LOG == 1){log_printf("AXSetVoiceDeviceMix voice: %08X device: %d, mix: %08X\n",v,device,mix);}
    VoiceSwapper_setMix(v,device,mix);
    return real_AXSetVoiceDeviceMix(v,device,id,mix);
}

DECL_FUNCTION(void *, AXAcquireVoiceExOld, uint32_t prio, void * callback, uint32_t arg){
    void * result = real_AXAcquireVoiceExOld(prio,callback,arg);
    if(VOICE_SWAP_LOG == 1){log_printf("AXAcquireVoiceExOld result: %08X \n",result);}
    VoiceSwapper_acquireVoice(result);
    return result;
}

DECL_FUNCTION(void *, AXAcquireVoiceEx, uint32_t prio, void * callback, uint32_t arg){
    void * result = real_AXAcquireVoiceEx(prio,callback,arg);
    if(VOICE_SWAP_LOG == 1){log_printf("AXAcquireVoiceEx result: %08X \n",result);}
    VoiceSwapper_acquireVoice(result);
    return result;
}

DECL_FUNCTION(void, AXFreeVoiceOld, void *v){
    if(VOICE_SWAP_LOG == 1){log_printf("AXFreeVoiceOld v: %08X \n",v);}
    VoiceSwapper_freeVoice(v);
    real_AXFreeVoiceOld(v);
}

DECL_FUNCTION(void, AXFreeVoice, void *v){
    if(VOICE_SWAP_LOG == 1){log_printf("AXFreeVoice v: %08X \n",v);}
    VoiceSwapper_freeVoice(v);
    real_AXFreeVoice(v);
}

DECL_FUNCTION(void, GX2CopyColorBufferToScanBuffer, GX2ColorBuffer *colorBuffer, int32_t scan_target){
    if(gSwap == G_SWAP_NORMAL) {
        // simply copy as normal
        real_GX2CopyColorBufferToScanBuffer(colorBuffer,scan_target);
    } else if(gSwap == G_SWAP_SWAPPED){
        // swap the target then scan
        if(scan_target == GX2_SCAN_TARGET_TV){
            scan_target = GX2_SCAN_TARGET_DRC;
        } else if(scan_target == GX2_SCAN_TARGET_DRC) {
            scan_target = GX2_SCAN_TARGET_TV;
        } else {
            // not sure what to do here
        }
        real_GX2CopyColorBufferToScanBuffer(colorBuffer,scan_target);
    } else if(gSwap == G_SWAP_MIRROR_TV) {
        // display TV on both
        if(scan_target == GX2_SCAN_TARGET_TV) {
            // copy and scan to both
            real_GX2CopyColorBufferToScanBuffer(colorBuffer,GX2_SCAN_TARGET_TV);
            real_GX2CopyColorBufferToScanBuffer(colorBuffer,GX2_SCAN_TARGET_DRC);
        } else if(scan_target == GX2_SCAN_TARGET_DRC) {
            // drop the target entirely
        } else {
            // not sure what to do here
        }
    } else if(gSwap == G_SWAP_MIRROR_DRC) {
       // display DRC on both
        if(scan_target == GX2_SCAN_TARGET_DRC) {
            // copy and scan to both
            real_GX2CopyColorBufferToScanBuffer(colorBuffer,GX2_SCAN_TARGET_TV);
            real_GX2CopyColorBufferToScanBuffer(colorBuffer,GX2_SCAN_TARGET_DRC);
        } else if(scan_target == GX2_SCAN_TARGET_TV) {
            // drop the target entirely
        } else {
            // not sure what to do here
        }
    } else {
        // Shouldn't ever happen
    }
}

/*
DECL(int32_t, AXSetDefaultMixerSelectOld, uint32_t s){
    int32_t result = real_AXSetDefaultMixerSelectOld(s);
    return result;
}*/


void swapVoices(){
    VoiceSwapper_swapAll();
    for(int32_t i = 0;i<VOICE_INFO_MAX;i++){
        if(gVoiceInfos[i].voice == NULL) continue;

        real_AXSetVoiceDeviceMix(gVoiceInfos[i].voice,0,0,gVoiceInfos[i].mixTV);
        real_AXSetVoiceDeviceMix(gVoiceInfos[i].voice,1,0,gVoiceInfos[i].mixDRC);
        real_AXSetVoiceDeviceMixOld(gVoiceInfos[i].voice,0,0,gVoiceInfos[i].mixTV);
        real_AXSetVoiceDeviceMixOld(gVoiceInfos[i].voice,1,0,gVoiceInfos[i].mixDRC);
    }
}

DECL_FUNCTION(int32_t, VPADRead, int32_t chan, VPADStatus *buffer, uint32_t buffer_size, VPADReadError *error) {
    int32_t result = real_VPADRead(chan, buffer, buffer_size, error);

    if(result > 0 && *error == VPAD_READ_SUCCESS && ((buffer[0].hold & gButtonCombo) == gButtonCombo) && gCallbackCooldown == 0 ){
        gCallbackCooldown = 0x3C;
        if(gAppStatus == WUPS_APP_STATUS_FOREGROUND){
			switch(gSwap) {
                case G_SWAP_NORMAL:
                    gSwap = G_SWAP_SWAPPED;
                    break;
                case G_SWAP_SWAPPED:
                    gSwap = G_SWAP_MIRROR_TV;
                    break;
                case G_SWAP_MIRROR_TV:
                    gSwap = G_SWAP_MIRROR_DRC;
                    break;
                case G_SWAP_MIRROR_DRC:
                    gSwap = G_SWAP_NORMAL;
                    break;
                default:
                    gSwap = G_SWAP_NORMAL;
            }
            swapVoices();
        }
    }
    if(gCallbackCooldown > 0) gCallbackCooldown--;

    return result;
}


WUPS_MUST_REPLACE(GX2CopyColorBufferToScanBuffer,   WUPS_LOADER_LIBRARY_GX2,        GX2CopyColorBufferToScanBuffer);
WUPS_MUST_REPLACE(VPADRead,                         WUPS_LOADER_LIBRARY_VPAD,       VPADRead);
WUPS_MUST_REPLACE(AXAcquireVoiceExOld,              WUPS_LOADER_LIBRARY_SND_CORE,   AXAcquireVoiceEx);
WUPS_MUST_REPLACE(AXFreeVoiceOld,                   WUPS_LOADER_LIBRARY_SND_CORE,   AXFreeVoice);
WUPS_MUST_REPLACE(AXSetVoiceDeviceMixOld,           WUPS_LOADER_LIBRARY_SND_CORE,   AXSetVoiceDeviceMix);
//WUPS_MUST_REPLACE(AXSetDefaultMixerSelectOld, ,   WUPS_LOADER_LIBRARY_SND_CORE,     AXSetDefaultMixerSelect),
WUPS_MUST_REPLACE(AXAcquireVoiceEx,                 WUPS_LOADER_LIBRARY_SNDCORE2,   AXAcquireVoiceEx);
WUPS_MUST_REPLACE(AXFreeVoice,                      WUPS_LOADER_LIBRARY_SNDCORE2,   AXFreeVoice);
WUPS_MUST_REPLACE(AXSetVoiceDeviceMix,              WUPS_LOADER_LIBRARY_SNDCORE2,   AXSetVoiceDeviceMix);
