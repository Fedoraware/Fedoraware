#include "NetChannel.h"


#define NETMSG_TYPE_BITS 6
#define NUM_NEW_COMMAND_BITS 4
#define NUM_BACKUP_COMMAND_BITS 3

const char* CLC_Move::ToString(void) const { return "CLC_Move"; }

bool CLC_Move::WriteToBuffer(bf_write& buffer) {
    buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);
    m_nLength = m_DataOut.GetNumBitsWritten();

    buffer.WriteUBitLong(m_nNewCommands, NUM_NEW_COMMAND_BITS);
    buffer.WriteUBitLong(m_nBackupCommands, NUM_BACKUP_COMMAND_BITS);

    buffer.WriteWord(m_nLength);

    return buffer.WriteBits(m_DataOut.GetData(), m_nLength);
}

bool CLC_Move::ReadFromBuffer(bf_read& buffer) {
    m_nNewCommands = buffer.ReadUBitLong(NUM_NEW_COMMAND_BITS);
    m_nBackupCommands = buffer.ReadUBitLong(NUM_BACKUP_COMMAND_BITS);
    m_nLength = buffer.ReadWord();
    m_DataIn = buffer;
    return buffer.SeekRelative(m_nLength);
}

const char* NET_SetConVar::ToString() const { return "(NULL)"; }

bool NET_SetConVar::WriteToBuffer(bf_write& buffer)
{
    buffer.WriteUBitLong(GetType(), 6);
    const int numVars = 1;
    buffer.WriteByte(numVars);
    buffer.WriteString(ConVar.Name);
    buffer.WriteString(ConVar.Value);

    return !buffer.IsOverflowed();
}

bool NET_SetConVar::ReadFromBuffer(bf_read& buffer)
{
	const int numVars = buffer.ReadByte();

    for (int i = 0; i < numVars; i++)
    {
        CVar_t cvar;
        buffer.ReadString(cvar.Name, sizeof(cvar.Name));
        buffer.ReadString(cvar.Value, sizeof(cvar.Value));
    }
    return !buffer.IsOverflowed();
}