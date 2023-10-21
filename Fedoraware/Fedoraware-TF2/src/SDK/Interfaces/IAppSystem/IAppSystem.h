#pragma once

class IAppSystem
{
public:
	virtual bool Connect(void* factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* pInterfaceName) = 0;
	virtual int Init() = 0;
	virtual void Shutdown(char* reason) = 0;
};