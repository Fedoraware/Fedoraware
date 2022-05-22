#pragma once
#include "../Interfaces/Interfaces.h"

class CNetVars
{
	struct node;
	using map_type = std::unordered_map<std::string, std::shared_ptr<node>>;

	struct node {
		node(DWORD offset) : offset(offset) {}
		map_type nodes;
		DWORD offset;
	};

	map_type nodes;

public:
	void Init();

private:
	void populate_nodes(class RecvTable* recv_table, map_type* map);

	DWORD get_offset_recursive(map_type& map, int acc, const char* name) {
		return acc + map[name]->offset;
	}

	template<typename ...args_t>
	DWORD get_offset_recursive(map_type& map, int acc, const char* name, args_t ...args) {
		const auto& node = map[name];
		return get_offset_recursive(node->nodes, acc + node->offset, args...);
	}

public:
	template<typename ...args_t>
	DWORD get_offset(const char* name, args_t ...args) {
		const auto& node = nodes[name];
		return get_offset_recursive(node->nodes, node->offset, args...);
	}
};

extern CNetVars g_NetVars;

template<typename T>
class CDynamicNetvar
{
private:
	DWORD dwOffset;

public:
	template<typename... args_t>
	CDynamicNetvar(args_t... a) {
		dwOffset = g_NetVars.get_offset(a...);
	}

	template<typename... args_t>
	CDynamicNetvar(int nOffset, args_t... a) {
		dwOffset = g_NetVars.get_offset(a...) + nOffset;
	}

	T GetValue(void* base) {
		return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(base) + dwOffset);
	}

	void SetValue(void* base, T val) {
		*reinterpret_cast<T*>(reinterpret_cast<DWORD>(base) + dwOffset) = val;
	}
};

#define DYNVAR(name, type, ...) static CDynamicNetvar<type>  ##name( __VA_ARGS__ )
#define DYNVAR_RETURN(type, base, ...) DYNVAR(n, type, __VA_ARGS__); return n.GetValue(base)
#define DYNVAR_SET(type, base, value, ...) DYNVAR(n, type, __VA_ARGS__); n.SetValue(base,value)

#define M_DYNVARGET(name, type, base, ...) __inline type Get##name() \
{ \
	static CDynamicNetvar<type>  Var##name( __VA_ARGS__ ); \
	return Var##name.GetValue(base); \
}

#define M_OFFSETGET(name, type, offset) __inline type Get##name() \
{ \
	return *reinterpret_cast<type*>(this + offset); \
}

#define M_CONDGET(name, conditions, cond) __inline bool Is##name() \
{ \
	return (conditions & cond); \
}

// lol

inline int GetOffset(RecvTable* pTable, const char* szNetVar)
{
	for (int i = 0; i < pTable->m_nProps; i++)
	{
		RecvProp Prop = pTable->m_pProps[i];

		if (std::string_view(Prop.m_pVarName).compare(szNetVar) == 0)
			return Prop.GetOffset();

		if (auto DataTable = Prop.GetDataTable())
		{
			if (auto nOffset = GetOffset(DataTable, szNetVar))
				return nOffset + Prop.GetOffset();
		}
	}

	return 0;
}

inline int GetNetVar(const char* szClass, const char* szNetVar)
{
	CClientClass* pClasses = I::Client->GetAllClasses();

	for (auto pCurrNode = pClasses; pCurrNode; pCurrNode = pCurrNode->m_pNext)
	{
		if (std::string_view(szClass).compare(pCurrNode->m_pNetworkName) == 0)
			return GetOffset(pCurrNode->m_pRecvTable, szNetVar);
	}

	return 0;
}

#define NETVAR(_name, type, table, name) inline type &_name() \
{ \
	static int offset = GetNetVar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset); \
}
