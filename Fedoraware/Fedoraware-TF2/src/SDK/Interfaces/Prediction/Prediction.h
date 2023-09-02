#pragma once
#include "../MoveHelper/MoveHelper.h"

class IPrediction
{
public:
public:
	virtual		 ~IPrediction(void) {};
	virtual void Init(void) = 0;
	virtual void Shutdown(void) = 0;
	virtual void Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	) = 0;

	virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void PostEntityPacketReceived(void) = 0;
	virtual void PostNetworkDataReceived(int commands_acknowledged) = 0;
	virtual void OnReceivedUncompressedPacket(void) = 0;
	virtual void GetViewOrigin(Vec3& org) = 0;
	virtual void SetViewOrigin(Vec3& org) = 0;
	virtual void GetViewAngles(Vec3& ang) = 0;
	virtual void SetViewAngles(Vec3& ang) = 0;
	virtual void GetLocalViewAngles(Vec3& ang) = 0;
	virtual void SetLocalViewAngles(Vec3& ang) = 0;
};

class CPrediction : public IPrediction
{
public:
	virtual		 ~CPrediction();
	virtual void Init();
	virtual void Shutdown();
	virtual void Update
	(
		int startframe,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	);

	virtual void OnReceivedUncompressedPacket();
	virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet);
	virtual void PostEntityPacketReceived();
	virtual void PostNetworkDataReceived(int commands_acknowledged);
	virtual bool InPrediction();
	virtual bool IsFirstTimePredicted();
	virtual int	 GetIncomingPacketNumber();
	virtual void GetViewOrigin(Vec3& org);
	virtual void SetViewOrigin(Vec3& org);
	virtual void GetViewAngles(Vec3& ang);
	virtual void SetViewAngles(Vec3& ang);
	virtual void GetLocalViewAngles(Vec3& ang);
	virtual void SetLocalViewAngles(Vec3& ang);
	virtual void RunCommand(CBaseEntity* player, CUserCmd* ucmd, CMoveHelper* moveHelper);
	virtual void SetupMove(CBaseEntity* player, CUserCmd* ucmd, CMoveHelper* pHelper, CMoveData* move);
	virtual void FinishMove(CBaseEntity* player, CUserCmd* ucmd, CMoveData* move);
	virtual void SetIdealPitch(CBaseEntity* player, const Vec3& origin, const Vec3& angles, const Vec3& viewheight);
	virtual void _Update
	(
		bool received_new_world_update,
		bool validframe,
		int incoming_acknowledged,
		int outgoing_command
	);

	//CHandle< C_BaseEntity > m_hLastGround;
	uint32_t m_hLastGround;
	bool	 m_bInPrediction;
	bool	 m_bFirstTimePredicted;
	bool	 m_bOldCLPredictValue;
	bool	 m_bEnginePaused;
	int		 m_nPreviousStartFrame;
	int		 m_nCommandsPredicted;
	int		 m_nServerCommandsAcknowledged;
	int		 m_bPreviousAckHadErrors;
	int		 m_nIncomingPacketNumber;
	float	 m_flIdealPitch;
};

#define VCLIENT_PREDICTION_INTERFACE_VERSION "VClientPrediction001"