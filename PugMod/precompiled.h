#pragma once

#ifndef _WIN32
#define _GLIBCXX_USE_CXX11_ABI 0
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _strdup strdup
#define _unlink unlink
#define _vsnprintf vsnprintf
#define _write write
#define _close close
#define _acces access
#define _vsnwprintf vswprintf
#endif

#ifdef _WIN32
#pragma warning(disable: 4100)
#pragma warning(disable: 4996)
#endif

// System
#include <osconfig.h>

// C++
#include <map>
#include <vector>
#include <string>
#include <array>
#include <numeric>

// CSSDK
#include <extdll.h>
#include <eiface.h>

// MetaMod SDK
#include <meta_api.h>

// ReHLDS Api
#include <rehlds_api.h>

// ReGameDLL Api
#include <regamedll_api.h>

// Metamod Includes
#include "MetaMod.h"

// ReAPI & ReGameDLL
#include "ReAPI.h"
#include "ReGameDLL.h"

// Time Format
#include "include/TimeFormat.h"

// Pickup Game Mod
#include "PugMenu.h"
#include "PugAdmin.h"
#include "PugAdminMenu.h"
#include "PugClientCmd.h"
#include "PugCvar.h"
#include "PugDM.h"
#include "PugEngine.h"
#include "PugLeader.h"
#include "PugLO3.h"
#include "PugMapList.h"
#include "PugMod.h"
#include "PugReady.h"
#include "PugRestrictItem.h"
#include "PugServerCmd.h"
#include "PugSpawnEdit.h"
#include "PugTask.h"
#include "PugTimer.h"
#include "PugUtil.h"
#include "PugVoteMap.h"
#include "PugVoteOvertime.h"
#include "PugVoteTeam.h"
#include "PugVoteSwapTeam.h"
