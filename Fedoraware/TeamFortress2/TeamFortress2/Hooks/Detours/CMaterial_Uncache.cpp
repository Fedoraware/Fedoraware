#include "../Hooks.h"

MAKE_HOOK( CMaterial_Uncache, g_Pattern.Find( L"MaterialSystem.dll", L"55 8B EC 81 EC 04 01 ? ? 56 8B F1 8B 0d 54 D0 11 10" ), void, __fastcall,
	IMaterial* ecx, void* edx, bool bPreserveVars ) {
	if ( ecx ) {
		const std::string materialName = ecx->GetName( );
		if ( materialName.find( "m_pmat" ) != std::string::npos ) {
			if ( materialName.find( "dme_mat_" ) == std::string::npos ) {
				I::CVars->ConsolePrintf( "Preventing material %s from being uncached\n", ecx->GetName( ) );
				return;
			}
		}
	}

	if ( ecx ) {
		Hook.Original<FN>( )( ecx, edx, bPreserveVars );
	}
}