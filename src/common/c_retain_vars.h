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
#ifndef C_RETAINS_VARS_H_
#define C_RETAINS_VARS_H_

#include "utils/voice_info.h"
extern uint8_t gSwap;
#define G_SWAP_NORMAL     0
#define G_SWAP_SWAPPED    1
#define G_SWAP_MIRROR_TV  2
#define G_SWAP_MIRROR_DRC 3
extern uint8_t gCallbackCooldown;
extern uint8_t gAppStatus;
extern uint32_t gButtonCombo;
extern VoiceInfo gVoiceInfos[VOICE_INFO_MAX];

#endif // C_RETAINS_VARS_H_
