//====== Copyright Valve Corporation, All rights reserved. ====================
//
// Purpose: misc networking utilities
//
//=============================================================================

#include "CSteamID.h"
#ifndef ISTEAMNETWORKINGUTILS
#define ISTEAMNETWORKINGUTILS
#ifdef _WIN32
#pragma once
#endif

struct SteamDatagramRelayAuthTicket;
struct SteamNetworkingMicroseconds;
struct SteamNetworkPingLocation_t;

struct SteamNetworkingPOPID
{
	unsigned v;
	/* 'out' must point to array with capacity at least 5 */
	inline void ToString(char* out)
	{
		out[0] = char(v >> 16);
		out[1] = char(v >> 8);
		out[2] = char(v);
		out[3] = char(v >> 24);
		out[4] = 0;
	}
};

class ISteamNetworkingUtils
{
public:
	virtual SteamNetworkingMicroseconds GetLocalTimestamp() = 0;
	virtual bool CheckPingDataUpToDate(float flMaxAgeSeconds) = 0;
	virtual float GetLocalPingLocation(SteamNetworkPingLocation_t& result) = 0;
	virtual bool IsPingMeasurementInProgress() = 0;
	virtual int EstimatePingTimeBetweenTwoLocations(const SteamNetworkPingLocation_t& location1, const SteamNetworkPingLocation_t& location2) = 0;
	virtual int EstimatePingTimeFromLocalHost(const SteamNetworkPingLocation_t& remoteLocation) = 0;
	virtual bool ReceivedTicket(const void* pvTicket, int cbTicket, SteamDatagramRelayAuthTicket* pOutParsedTicket) = 0;
	virtual bool HasTicketForServer(CSteamID steamID) = 0;
	virtual uint32 GetIPForServerSteamIDFromTicket(CSteamID steamID) = 0;
	virtual int GetPingToDataCenter(SteamNetworkingPOPID popID, SteamNetworkingPOPID* pViaRelayPoP) = 0;
	virtual int GetDirectPingToPOP(SteamNetworkingPOPID popID) = 0;
	virtual int GetPOPCount() = 0;
	virtual int GetPOPList(SteamNetworkingPOPID* list, int nListSz) = 0;
};
#define STEAMNETWORKINGUTILS_VERSION "SteamNetworkingUtils001"

#endif // ISTEAMNETWORKINGUTILS
