#pragma once

#include "../../Includes/Includes.h"

class CBaseEntity;

class IPredictionSystem
{
public:
	IPredictionSystem()
	{
		m_pNextSystem = g_pPredictionSystems;
		g_pPredictionSystems = this;

		m_bSuppressEvent = false;
		m_pSuppressHost = NULL;

		m_nStatusPushed = 0;
	};

	virtual ~IPredictionSystem() {};

	IPredictionSystem* GetNext()
	{
		return m_pNextSystem;
	}

	void SetSuppressEvent(bool state)
	{
		m_bSuppressEvent = state;
	}

	void SetSuppressHost(CBaseEntity* host)
	{
		m_pSuppressHost = host;
	}

	CBaseEntity const* GetSuppressHost(void)
	{
		if (DisableFiltering())
		{
			return NULL;
		}

		return m_pSuppressHost;
	}

	bool CanPredict(void) const
	{
		if (DisableFiltering())
		{
			return false;
		}

		return !m_bSuppressEvent;
	}

	static IPredictionSystem* g_pPredictionSystems;

	static void SuppressEvents(bool state)
	{
		IPredictionSystem* sys = g_pPredictionSystems;
		while (sys)
		{
			sys->SetSuppressEvent(state);
			sys = sys->GetNext();
		}
	}

	static void SuppressHostEvents(CBaseEntity* host)
	{
		IPredictionSystem* sys = g_pPredictionSystems;
		while (sys)
		{
			sys->SetSuppressHost(host);
			sys = sys->GetNext();
		}
	}

private:

	static void Push(void)
	{
		IPredictionSystem* sys = g_pPredictionSystems;
		while (sys)
		{
			sys->_Push();
			sys = sys->GetNext();
		}
	}

	static void Pop(void)
	{
		IPredictionSystem* sys = g_pPredictionSystems;
		while (sys)
		{
			sys->_Pop();
			sys = sys->GetNext();
		}
	}

	void _Push(void)
	{
		++m_nStatusPushed;
	}
	void _Pop(void)
	{
		--m_nStatusPushed;
	}

	bool DisableFiltering(void) const
	{
		return (m_nStatusPushed > 0) ? true : false;
	}

	IPredictionSystem* m_pNextSystem;
	bool				m_bSuppressEvent;
	CBaseEntity* m_pSuppressHost;

	int					m_nStatusPushed;

	friend class CDisablePredictionFiltering;
};

class CDisablePredictionFiltering
{
public:
	CDisablePredictionFiltering(bool disable = true)
	{
		m_bDisabled = disable;
		if (m_bDisabled)
		{
			IPredictionSystem::Push();
		}
	}

	~CDisablePredictionFiltering(void)
	{
		if (m_bDisabled)
		{
			IPredictionSystem::Pop();
		}
	}
private:
	bool	m_bDisabled;
};


class IEffects : public IPredictionSystem
{
public:
	virtual void Beam(const Vector & Start, const Vector & End, int nModelIndex, int nHaloIndex, unsigned char frameStart, unsigned char frameRate, float flLife, unsigned char width, unsigned char endWidth, unsigned char fadeLength, unsigned char noise, unsigned char red, unsigned char green, unsigned char blue, unsigned char brightness, unsigned char speed) = 0;
	virtual void Smoke(const Vector& origin, int modelIndex, float scale, float framerate) = 0;
	virtual void Sparks(const Vector& position, int nMagnitude = 1, int nTrailLength = 1, const Vector* pvecDir = NULL) = 0;
	virtual void Dust(const Vector& pos, const Vector& dir, float size, float speed) = 0;
	virtual void MuzzleFlash(const Vector& vecOrigin, const QAngle& vecAngles, float flScale, int iType) = 0;
	virtual void MetalSparks(const Vector& position, const Vector& direction) = 0;
	virtual void EnergySplash(const Vector& position, const Vector& direction, bool bExplosive = false) = 0;
	virtual void Ricochet(const Vector& position, const Vector& direction) = 0;
	virtual float Time() = 0;
	virtual bool IsServer() = 0;
	virtual void SuppressEffectsSounds(bool bSuppress) = 0;
};



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_RecipientFilter : public IRecipientFilter
{
public:
	C_RecipientFilter();
	virtual			~C_RecipientFilter();

	virtual bool	IsReliable(void) const;

	virtual int		GetRecipientCount(void) const;
	virtual int		GetRecipientIndex(int slot) const;

	virtual bool	IsInitMessage(void) const { return false; };

public:

	void			CopyFrom(const C_RecipientFilter& src);

	void			Reset(void);

	void			MakeReliable(void);

	void			AddAllPlayers(void);
	void			AddRecipientsByPVS(const Vector& origin);
	void			AddRecipientsByPAS(const Vector& origin);
	void			AddRecipient(CBaseEntity* player);
	void			RemoveRecipient(CBaseEntity* player);
	void			AddRecipientsByTeam(CBaseEntity* team);
	void			RemoveRecipientsByTeam(CBaseEntity* team);

	void			UsePredictionRules(void);
	bool			IsUsingPredictionRules(void) const;

	bool			IgnorePredictionCull(void) const;
	void			SetIgnorePredictionCull(bool ignore);

	void			AddPlayersFromBitMask(void* playerbits);

	//private:

	bool				m_bReliable;
	bool				m_bInitMessage;
	CUtlVector< int >	m_Recipients;
	// If using prediction rules, the filter itself suppresses local player
	bool				m_bUsingPredictionRules;
	// If ignoring prediction cull, then external systems can determine
	//  whether this is a special case where culling should not occur
	bool				m_bIgnorePredictionCull;
};


class CEffectsClient : public IEffects
{
public:
	CEffectsClient();
	virtual ~CEffectsClient();

	// Members of the IEffect interface
	virtual void Beam(const Vector& Start, const Vector& End, int nModelIndex,
		int nHaloIndex, unsigned char frameStart, unsigned char frameRate,
		float flLife, unsigned char width, unsigned char endWidth, unsigned char fadeLength,
		unsigned char noise, unsigned char red, unsigned char green,
		unsigned char blue, unsigned char brightness, unsigned char speed);
	virtual void Smoke(const Vector& origin, int modelIndex, float scale, float framerate);
	virtual void Sparks(const Vector& position, int nMagnitude = 1, int nTrailLength = 1, const Vector* pvecDir = NULL);
	virtual void Dust(const Vector& pos, const Vector& dir, float size, float speed);
	virtual void MuzzleFlash(const Vector& origin, const QAngle& angles, float fScale, int type);
	virtual void MetalSparks(const Vector& position, const Vector& direction);
	virtual void EnergySplash(const Vector& position, const Vector& direction, bool bExplosive = false);
	virtual void Ricochet(const Vector& position, const Vector& direction);

	// FIXME: Should these methods remain in this interface? Or go in some 
	// other client-server neutral interface?
	virtual float Time();
	virtual bool IsServer();
	virtual void SuppressEffectsSounds(bool bSuppress);

private:
	//-----------------------------------------------------------------------------
	// Purpose: Returning true means don't even call TE func
	// Input  : filter - 
	//			*suppress_host - 
	// Output : static bool
	//-----------------------------------------------------------------------------
	bool SuppressTE(C_RecipientFilter& filter)
	{
		if (!CanPredict())
			return true;

		if (!filter.GetRecipientCount())
		{
			// Suppress it
			return true;
		}

		// There's at least one recipient
		return false;
	}

	bool m_bSuppressSound;
};

#define IEFFECTS_INTERFACE_VERSION	"IEffects001"