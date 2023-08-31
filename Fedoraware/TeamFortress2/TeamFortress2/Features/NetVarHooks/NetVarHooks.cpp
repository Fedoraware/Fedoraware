#include "NetVarHooks.h"
#include <boost/algorithm/string.hpp>

bool NetVarHooks::HookNetvar(const std::vector<std::string>& path, ProxyFnHook& hook, RecvVarProxyFn function)
{
	auto pClass = I::BaseClientDLL->GetAllClasses();

	if (path.size() < 2)
	{
		return false;
	}

	while (pClass)
	{
		// Main class found
		if (!strcmp(pClass->m_pRecvTable->m_pNetTableName, path[0].c_str()))
		{
			const RecvTable* curTable = pClass->m_pRecvTable;
			for (size_t i = 1; i < path.size(); i++)
			{
				bool found = false;
				for (int j = 0; j < curTable->m_nProps; j++)
				{
					const auto pProp = reinterpret_cast<RecvPropRedef*>(&(curTable->m_pProps[j]));
					if (!pProp) { continue; }

					if (path[i] != pProp->m_pVarName)
					{
						// Detect last iteration
						if (i == path.size() - 1)
						{
							hook.init(pProp);
							hook.setHook(function);
							return true;
						}

						curTable = pProp->m_pDataTable;
						found = true;
					}
				}
				// We tried searching the netvar but found nothing
				if (!found)
				{
					const std::string fullPath = boost::algorithm::join(path, "/");
					const std::string stringMessage("Required member not found: " + fullPath);
					const char* errorMessage = stringMessage.c_str();
					MessageBoxA(nullptr, errorMessage, "Hooking netvar failed", MB_ICONERROR);

					return false;
				}
			}
		}

		pClass = pClass->m_pNext;
	}

	return false;
}

ProxyFnHook CyoaAnimHookProxy{};
void CyoaView(const CRecvProxyData* data, void* pPlayer, void* out)
{
	const int value = data->m_Value.m_Int;
	const auto valueOut = static_cast<int*>(out);
	if (!Vars::Visuals::RemoveTaunts.Value)
	{
		*valueOut = value;
		return;
	}
	*valueOut = false;
}

void NetVarHooks::Init()
{
	HookNetvar({"DT_TFPlayer", "m_bViewingCYOAPDA"}, CyoaAnimHookProxy, CyoaView);
}
