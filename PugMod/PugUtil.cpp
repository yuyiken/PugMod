#include "precompiled.h"

CPugUtil gPugUtil;

cvar_t* CPugUtil::CvarRegister(std::string Name, std::string Value)
{
	cvar_t* Pointer = g_engfuncs.pfnCVarGetPointer(Name.c_str());

	if (!Pointer)
	{
		this->m_CvarData[Name].name = Name.c_str();

		this->m_CvarData[Name].flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

		g_engfuncs.pfnCVarRegister(&this->m_CvarData[Name]);

		Pointer = g_engfuncs.pfnCVarGetPointer(this->m_CvarData[Name].name);

		if (Pointer)
		{
			g_engfuncs.pfnCvar_DirectSet(Pointer, Value.c_str());
		}
	}

	return Pointer;
}