#pragma once

// Windows Build
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#undef _UNICODE
#endif

// Linux Build
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

// Standard library
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <random>
#include <regex>

// System Includes
#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <direct.h>
#endif

// CSSDK
#include <extdll.h>
#include <eiface.h>

// MetaMod SDK
#include <meta_api.h>

// ReHLDS Api
#include <rehlds_api.h>
#include <rehlds_interfaces.h>

// ReGameDLL Api
#include <regamedll_api.h>
#include <regamedll_const.h>

// Plugin Includes
#include "MetaMod.h"
#include "MetaDLL.h"
#include "MetaEngine.h"

// ReAPI & ReGameDLL
#include "ReAPI.h"
#include "ReGameDLL.h"

// Pickup Game Mod
#include "PugCvar.h"
#include "PugMod.h"
#include "PugMenu.h"
#include "PugDeathmatch.h"
#include "PugPlayer.h"
#include "PugPlayerMenu.h"
#include "PugServerCmd.h"
#include "PugTask.h"
#include "PugTimer.h"
#include "PugTraceLine.h"
#include "PugUtil.h"