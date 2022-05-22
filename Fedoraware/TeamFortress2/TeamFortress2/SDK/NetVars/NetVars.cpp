#include "NetVars.h"

#undef GetProp

void CNetVars::Init()
{
	const auto* client_class = I::Client->GetAllClasses();

	while (client_class != nullptr)
	{
		const auto class_info = std::make_shared<node>(0);
		RecvTable* recv_table = reinterpret_cast<RecvTable*>(client_class->m_pRecvTable);
		if (!class_info || !recv_table) {
			continue;
		}
		populate_nodes(recv_table, &class_info->nodes);
		nodes.emplace(recv_table->GetName(), class_info);

		client_class = client_class->m_pNext;
	}
}

void CNetVars::populate_nodes(RecvTable* recv_table, map_type* map)
{
	for (auto i = 0; i < recv_table->GetNumProps(); i++)
	{
		const auto* prop = recv_table->GetPropW(i);
		const auto prop_info = std::make_shared<node>(prop->GetOffset());

		if (prop->GetType() == DPT_DataTable)
			populate_nodes(prop->GetDataTable(), &prop_info->nodes);

		map->emplace(prop->GetName(), prop_info);
	}
}

CNetVars g_NetVars;