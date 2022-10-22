#include "ConVar.h"
#include "../Interfaces.h"

ConCommandBase* ConCommandBase::s_pConCommandBases = nullptr;
static int	s_nCVarFlag = 0;
static int	s_nDLLIdentifier = -1;
static bool	s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase* pVar)
	{
		I::Cvar->RegisterConCommand(pVar);
		return true;
		//return false;
	}
};

static CDefaultAccessor s_DefaultAccessor;
IConCommandBaseAccessor* ConCommandBase::s_pAccessor = &s_DefaultAccessor;

void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor* pAccessor)
{
	if (s_bRegistered)
		return;
	s_bRegistered = true;
	s_nCVarFlag = nCVarFlag;
	s_nDLLIdentifier = I::Cvar->AllocateDLLIdentifier();
	ConCommandBase* pCur, * pNext;
	ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
	pCur = ConCommandBase::s_pConCommandBases;
	while (pCur)
	{
		pNext = pCur->m_pNext;
		pCur->AddFlags(s_nCVarFlag);
		pCur->Init();
		pCur = pNext;
	}
	ConCommandBase::s_pConCommandBases = NULL;
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags)
{
	Create(pName, pDefaultValue, flags);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString)
{
	Create(pName, pDefaultValue, flags, pHelpString);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
	Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, FnChangeCallback_t callback)
{
	Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
}

ConVar::ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
	Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
}

ConVar::~ConVar(void)
{
	if (m_Value.m_pszString)
	{
		delete[] m_Value.m_pszString;
		m_Value.m_pszString = NULL;
	}
}

bool ConVar::IsFlagSet(int flag) const
{
	return (flag & m_pParent->m_nFlags) ? true : false;
}

const char* ConVar::GetHelpText(void) const
{
	return m_pParent->m_pszHelpString;
}

void ConVar::AddFlags(int flags)
{
	m_pParent->m_nFlags |= flags;
}

int ConVar::GetFlags(void) const
{
	return m_pParent->m_nFlags;
}

bool ConVar::IsRegistered(void) const
{
	return m_pParent->m_bRegistered;
}

bool ConVar::IsCommand(void) const
{
	return false;
}

const char* ConVar::GetName(void) const
{
	return m_pParent->m_pszName;
}

const char* ConVar::GetBaseName(void) const
{
	return m_pParent->m_pszName;
}

float ConVar::GetFloat(void) const
{
	return m_pParent->m_Value.m_fValue;
}

int ConVar::GetInt(void) const
{
	return m_pParent->m_Value.m_nValue;
}

bool ConVar::GetBool(void) const
{
	return m_pParent->m_Value.m_nValue == 1;
}

const char* ConVar::GetString(void) const
{
	return m_pParent->m_Value.m_pszString;
}

int ConVar::GetSplitScreenPlayerSlot(void) const
{
	return 0;
}

void ConVar::InternalSetValue(const char* value)
{
	float fNewValue;
	char  tempVal[32];
	char* val;

	float flOldValue = m_Value.m_fValue;

	val = (char*)value;
	fNewValue = (float)atof(value);

	if (ClampValue(fNewValue))
	{
		val = tempVal;
	}

	// Redetermine value
	m_Value.m_fValue = fNewValue;
	m_Value.m_nValue = (int)(fNewValue);

	if (!(m_nFlags & (int)EConVarFlags::FCVAR_NEVER_AS_STRING))
	{
		ChangeStringValue(val, flOldValue);
	}
}

void ConVar::ChangeStringValue(const char* tempVal, float flOldValue)
{
	UNREFERENCED_PARAMETER(flOldValue);
	int len = strlen(tempVal) + 1;

	if (len > m_Value.m_StringLength)
	{
		if (m_Value.m_pszString)
		{
			delete[] m_Value.m_pszString;
		}

		m_Value.m_pszString = new char[len];
		m_Value.m_StringLength = len;
	}

	memcpy(m_Value.m_pszString, tempVal, len);

}

bool ConVar::ClampValue(float& value)
{
	if (m_bHasMin && (value < m_fMinVal))
	{
		value = m_fMinVal;
		return true;
	}

	if (m_bHasMax && (value > m_fMaxVal))
	{
		value = m_fMaxVal;
		return true;
	}

	return false;
}

void ConVar::InternalSetFloatValue(float fNewValue)
{
	if (fNewValue == m_Value.m_fValue)
		return;
	// Check bounds
	ClampValue(fNewValue);

	// Redetermine value
	float flOldValue = m_Value.m_fValue;
	m_Value.m_fValue = fNewValue;
	m_Value.m_nValue = (int)fNewValue;

	if (!(m_nFlags & (int)EConVarFlags::FCVAR_NEVER_AS_STRING))
	{
		char tempVal[32];
		ChangeStringValue(tempVal, flOldValue);
	}
}

void ConVar::InternalSetIntValue(int nValue)
{
	if (nValue == m_Value.m_nValue)
		return;

	float fValue = (float)nValue;
	if (ClampValue(fValue))
	{
		nValue = (int)(fValue);
	}

	float flOldValue = m_Value.m_fValue;
	m_Value.m_fValue = fValue;
	m_Value.m_nValue = nValue;

	if (!(m_nFlags & (int)EConVarFlags::FCVAR_NEVER_AS_STRING))
	{
		char tempVal[32];
		ChangeStringValue(tempVal, flOldValue);
	}
}

void ConVar::InternalSetColorValue(DWORD cValue)
{
	int color = (int)cValue;
	InternalSetIntValue(color);
}

void ConVar::Create(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
	static const char* empty_string = "";

	m_pParent = this;

	// Name should be static data
	m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

	m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
	m_Value.m_pszString = new char[m_Value.m_StringLength];
	memcpy(m_Value.m_pszString, m_pszDefaultValue, m_Value.m_StringLength);

	m_bHasMin = bMin;
	m_fMinVal = fMin;
	m_bHasMax = bMax;
	m_fMaxVal = fMax;

	m_fnChangeCallback = callback;
	m_Value.m_fValue = (float)atof(m_Value.m_pszString);
	m_Value.m_nValue = (int)m_Value.m_fValue;

	BaseClass::Create(pName, pHelpString, flags);
}

void ConVar::SetValue(const char* value)
{
	m_pParent->InternalSetValue(value);
}

void ConVar::SetValue(float value)
{
	m_pParent->InternalSetFloatValue(value);
}

void ConVar::SetValue(int value)
{
	m_pParent->InternalSetIntValue(value);
}

void ConVar::SetValue(DWORD value)
{
	m_pParent->InternalSetColorValue(value);
}

const char* ConVar::GetDefault(void) const
{
	return m_pParent->m_pszDefaultValue;
}

ConCommandBase::ConCommandBase(void)
{
	m_bRegistered = false;
	m_pszName = NULL;
	m_pszHelpString = NULL;

	m_nFlags = 0;
	m_pNext = NULL;
}

ConCommandBase::ConCommandBase(const char* pName, const char* pHelpString, int flags)
{
	Create(pName, pHelpString, flags);
}

ConCommandBase::~ConCommandBase(void)
{
}

bool ConCommandBase::IsCommand(void) const
{
	return true;
}

int ConCommandBase::GetDLLIdentifier() const
{
	return s_nDLLIdentifier;
}

void ConCommandBase::Create(const char* pName, const char* pHelpString, int flags)
{
	static const char* empty_string = "";

	m_bRegistered = false;

	m_pszName = pName;
	m_pszHelpString = pHelpString ? pHelpString : empty_string;

	m_nFlags = flags;

	if (!(m_nFlags & (int)EConVarFlags::FCVAR_UNREGISTERED))
	{
		m_pNext = s_pConCommandBases;
		s_pConCommandBases = this;
	}

	else
	{
		m_pNext = NULL;
	}

	Init();
}

void ConCommandBase::Init()
{
	if (s_pAccessor)
	{
		s_pAccessor->RegisterConCommandBase(this);
	}
}

const char* ConCommandBase::GetName(void) const
{
	return m_pszName;
}

void ConVar::InstallChangeCallback(FnChangeCallback_t callback)
{
	m_pParent->m_fnChangeCallback = callback;

	if (m_pParent->m_fnChangeCallback)
	{
		m_pParent->m_fnChangeCallback(this, m_Value.m_pszString, m_Value.m_fValue);
	}
}

bool ConCommandBase::IsFlagSet(int flag) const
{
	return (flag & m_nFlags) ? true : false;
}

void ConCommandBase::AddFlags(int flags)
{
	m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
	m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags(void) const
{
	return m_nFlags;
}

const char* ConCommandBase::GetHelpText(void) const
{
	return m_pszHelpString;
}

bool ConCommandBase::IsRegistered(void) const
{
	return m_bRegistered;
}

static characterset_t s_BreakSet;
static bool s_bBuiltBreakSet = false;

//-----------------------------------------------------------------------------
void CharacterSetBuild(characterset_t* pSetBuffer, const char* pszSetString)
{
	int i = 0;

	// Test our pointers
	if (!pSetBuffer || !pszSetString)
		return;

	memset(pSetBuffer->set, 0, sizeof(pSetBuffer->set));

	while (pszSetString[i])
	{
		pSetBuffer->set[(unsigned)pszSetString[i]] = 1;
		i++;
	}

}

CCommand::CCommand()
{
	if (!s_bBuiltBreakSet)
	{
		s_bBuiltBreakSet = true;
		CharacterSetBuild(&s_BreakSet, "{}()':");
	}

	Reset();
}

CCommand::CCommand(int nArgC, const char** ppArgV)
{
	if (!s_bBuiltBreakSet)
	{
		s_bBuiltBreakSet = true;
		CharacterSetBuild(&s_BreakSet, "{}()':");
	}

	Reset();

	char* pBuf = m_pArgvBuffer;
	char* pSBuf = m_pArgSBuffer;
	m_nArgc = nArgC;
	for (int i = 0; i < nArgC; ++i)
	{
		m_ppArgv[i] = pBuf;
		int nLen = strlen(ppArgV[i]);
		memcpy(pBuf, ppArgV[i], nLen + 1);
		if (i == 0)
		{
			m_nArgv0Size = nLen;
		}
		pBuf += nLen + 1;

		bool bContainsSpace = strchr(ppArgV[i], ' ') != NULL;
		if (bContainsSpace)
		{
			*pSBuf++ = '\"';
		}
		memcpy(pSBuf, ppArgV[i], nLen);
		pSBuf += nLen;
		if (bContainsSpace)
		{
			*pSBuf++ = '\"';
		}

		if (i != nArgC - 1)
		{
			*pSBuf++ = ' ';
		}
	}
}

void CCommand::Reset()
{
	m_nArgc = 0;
	m_nArgv0Size = 0;
	m_pArgSBuffer[0] = 0;
}

characterset_t* CCommand::DefaultBreakSet()
{
	return &s_BreakSet;
}

//bool CCommand::Tokenize(const char* pCommand, characterset_t* pBreakSet)
//{
//	Reset();
//	if (!pCommand)
//		return false;
//
//	// Use default break set
//	if (!pBreakSet)
//	{
//		pBreakSet = &s_BreakSet;
//	}
//
//	// Copy the current command into a temp buffer
//	// NOTE: This is here to avoid the pointers returned by DequeueNextCommand
//	// to become invalid by calling AddText. Is there a way we can avoid the memcpy?
//	int nLen = strlen(pCommand);
//	if (nLen >= COMMAND_MAX_LENGTH - 1)
//	{
//		return false;
//	}
//
//	memcpy(m_pArgSBuffer, pCommand, nLen + 1);
//
//	// Parse the current command into the current command buffer
//	CUtlBuffer bufParse(m_pArgSBuffer, nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
//	int nArgvBufferSize = 0;
//	while (bufParse.IsValid() && (m_nArgc < COMMAND_MAX_ARGC))
//	{
//		char* pArgvBuf = &m_pArgvBuffer[nArgvBufferSize];
//		int nMaxLen = COMMAND_MAX_LENGTH - nArgvBufferSize;
//		int nStartGet = bufParse.TellGet();
//		int	nSize = bufParse.ParseToken(pBreakSet, pArgvBuf, nMaxLen);
//		if (nSize < 0)
//			break;
//
//		// Check for overflow condition
//		if (nMaxLen == nSize)
//		{
//			Reset();
//			return false;
//		}
//
//		if (m_nArgc == 1)
//		{
//			// Deal with the case where the arguments were quoted
//			m_nArgv0Size = bufParse.TellGet();
//			bool bFoundEndQuote = m_pArgSBuffer[m_nArgv0Size - 1] == '\"';
//			if (bFoundEndQuote)
//			{
//				--m_nArgv0Size;
//			}
//			m_nArgv0Size -= nSize;
//			Assert(m_nArgv0Size != 0);
//
//			// The StartGet check is to handle this case: "foo"bar
//			// which will parse into 2 different args. ArgS should point to bar.
//			bool bFoundStartQuote = (m_nArgv0Size > nStartGet) && (m_pArgSBuffer[m_nArgv0Size - 1] == '\"');
//			Assert(bFoundEndQuote == bFoundStartQuote);
//			if (bFoundStartQuote)
//			{
//				--m_nArgv0Size;
//			}
//		}
//
//		m_ppArgv[m_nArgc++] = pArgvBuf;
//		if (m_nArgc >= COMMAND_MAX_ARGC)
//		{
//			Warning("CCommand::Tokenize: Encountered command which overflows the argument buffer.. Clamped!\n");
//		}
//
//		nArgvBufferSize += nSize + 1;
//		Assert(nArgvBufferSize <= COMMAND_MAX_LENGTH);
//	}
//
//	return true;
//}


//-----------------------------------------------------------------------------
// Helper function to parse arguments to commands.
//-----------------------------------------------------------------------------
const char* CCommand::FindArg(const char* pName) const
{
	int nArgC = ArgC();
	for (int i = 1; i < nArgC; i++)
	{
		if (!_stricmp(Arg(i), pName))
			return (i + 1) < nArgC ? Arg(i + 1) : "";
	}
	return 0;
}

int CCommand::FindArgInt(const char* pName, int nDefaultVal) const
{
	const char* pVal = FindArg(pName);
	if (pVal)
		return atoi(pVal);
	else
		return nDefaultVal;
}