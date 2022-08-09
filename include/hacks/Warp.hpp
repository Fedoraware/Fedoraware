#pragma once
#include "settings/Bool.hpp"
class INetMessage;
namespace hacks::tf2::warp
{
extern bool in_rapidfire;
extern bool in_warp;
void SendNetMessage(INetMessage &msg);
void CL_SendMove_hook();
} // namespace hacks::tf2::warp
