#include "../Hooks.h"

std::hash<std::string_view> mHash;

class IInterpolatedVar
{
public:
	virtual ~IInterpolatedVar() = 0;
	virtual void Setup(void* pValue, int type) = 0;
	virtual void SetInterpolationAmount(float seconds) = 0;
	virtual void NoteLastNetworkedValue() = 0;
	virtual bool NoteChanged(float changetime, bool bUpdateLastNetworkedValue) = 0;
	virtual void Reset() = 0;
	virtual int Interpolate(float currentTime) = 0;
	virtual int  GetType() const = 0;
	virtual void RestoreToLastNetworked() = 0;
	virtual void Copy(IInterpolatedVar* pSrc) = 0;
	virtual const char* GetDebugName() = 0;
	virtual void SetDebugName(const char* pName) = 0;
	virtual void SetDebug(bool bDebug) = 0;
};

MAKE_HOOK(C_BaseEntity_AddVar, S::C_BaseEntity_AddVar(), void, __fastcall, void* ecx, void* edx, void* data, IInterpolatedVar* watcher, int type, bool bSetup) {
	if (watcher && Vars::Misc::DisableInterpolation.Value){
		const size_t hash = mHash(watcher->GetDebugName());

		static const size_t m_iv_vecVelocity = mHash("C_BaseEntity::m_iv_vecVelocity");
		static const size_t m_iv_angEyeAngles = mHash("C_TFPlayer::m_iv_angEyeAngles");
		static const size_t m_iv_flPoseParameter = mHash("C_BaseAnimating::m_iv_flPoseParameter");
		static const size_t m_iv_flCycle = mHash("C_BaseAnimating::m_iv_flCycle");
		static const size_t m_iv_flMaxGroundSpeed = mHash("CMultiPlayerAnimState::m_iv_flMaxGroundSpeed");

		if (hash == m_iv_vecVelocity ||
			hash == m_iv_flPoseParameter ||
			hash == m_iv_flCycle ||
			hash == m_iv_flMaxGroundSpeed ||
			(hash == m_iv_angEyeAngles && ecx == g_EntityCache.GetLocal())) {
			return;
		}
	}

	return Hook.Original<FN>()(ecx, edx, data, watcher, type, bSetup);
}
