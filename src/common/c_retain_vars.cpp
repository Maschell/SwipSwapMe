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
#include "common/c_retain_vars.h"

uint8_t gSwap __attribute__((section(".data"))) = G_SWAP_NORMAL;
uint8_t gCallbackCooldown __attribute__((section(".data"))) = 0;
uint8_t gAppStatus __attribute__((section(".data"))) = 0;
uint32_t gButtonCombo __attribute__((section(".data"))) = 0;

VoiceInfo gVoiceInfos[VOICE_INFO_MAX] __attribute__((section(".data")));
