#pragma once
#include <Windows.h>

namespace Minidump
{
	LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS exPtr);
}