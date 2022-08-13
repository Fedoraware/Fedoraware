#pragma once

class WUserMessage {
	int MessageType{ };
	bf_read& MessageData;

	bool CanRead(int size)
	{
		return MessageData.GetNumBitsLeft() > size;
	}

public:
	explicit WUserMessage(int msgType, bf_read& msgData) : MessageType(msgType), MessageData(msgData)
	{
		MessageData.Seek(0);
	}

	int GetType()
	{
		return MessageType;
	}

	void Reset()
	{
		MessageData.Reset();
	}

	void Seek(int bit)
	{
		MessageData.Seek(bit);
	}

	int GetDataBits()
	{
		return MessageData.m_nDataBits;
	}

	int GetNumBitsLeft()
	{
		return MessageData.GetNumBitsLeft();
	}

	int ReadByte()
	{
		if (!CanRead(sizeof(byte))) { return 0; }
		return MessageData.ReadByte();
	}

	float ReadFloat()
	{
		if (!CanRead(sizeof(float))) { return 0; }
		return MessageData.ReadFloat();
	}

	long ReadLong()
	{
		if (!CanRead(sizeof(long))) { return 0; }
		return MessageData.ReadLong();
	}

	const char* ReadString(int length)
	{
		char strBuffer[256];
		if (MessageData.ReadString(strBuffer, length))
		{
			return strBuffer;
		}

		return "";
	}
};