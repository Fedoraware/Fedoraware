#include "NetVarHk.h"

//	Excuse my pastering
CVar<bool>& RemoveAnims = Vars::Visuals::RemoveTaunts;

bool NetVarHooks::HookNetvar(std::vector<std::string> path, ProxyFnHook& hook, RecvVarProxyFn function)
{
	auto pClass = I::BaseClientDLL->GetAllClasses();
	if (path.size() < 2)
		return false;
	while (pClass)
	{
		// Main class found
		if (!strcmp(pClass->m_pRecvTable->m_pNetTableName, path[0].c_str()))
		{
			RecvTable* curr_table = pClass->m_pRecvTable;
			for (size_t i = 1; i < path.size(); i++)
			{
				bool found = false;
				for (int j = 0; j < curr_table->m_nProps; j++)
				{
					RecvPropRedef* pProp = (RecvPropRedef*)&(curr_table->m_pProps[j]);
					if (!pProp)
						continue;
					if (!strcmp(path[i].c_str(), pProp->m_pVarName))
					{
						// Detect last iteration
						if (i == path.size() - 1)
						{
							hook.init(pProp);
							hook.setHook(function);
							return true;
						}
						curr_table = pProp->m_pDataTable;
						found = true;
					}
				}
				// We tried searching the netvar but found nothing
				if (!found)
				{
					std::string full_path;
					for (auto& s : path)
						full_path += s + "";
					std::string poop("Required member not found: " + full_path);
					char* poop2 = (char*)poop.c_str();
					MessageBoxA(0, poop2, "Hooking netvar failed", MB_ICONERROR);
					//logging::Info("Hooking netvar with path \"%s\" failed. Required member not found.");
					return false;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
	return false;
}

static ProxyFnHook CyoaAnimHookProxy{};
void CyoaView(const CRecvProxyData *data, void *pPlayer, void *out){
    int value      = data->m_Value.m_Int;
    int *value_out = (int*) out;
    if (!RemoveAnims.Value)
    {
        *value_out = value;
        return;
    }
    *value_out = false;
}

void NetVarHooks::Init(){
	
    HookNetvar({"DT_TFPlayer", "m_bViewingCYOAPDA"}, CyoaAnimHookProxy, CyoaView);
}