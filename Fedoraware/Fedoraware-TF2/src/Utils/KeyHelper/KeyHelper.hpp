#pragma once
#include <Windows.h>

class KeyHelper {
	enum class KeyState { None, Down };

	int* Key = nullptr;
	KeyState LastState = KeyState::None;

public:
	explicit KeyHelper(int* vKey)
	{
		Key = vKey;
	}

	// Is the button currently down?
	bool Down()
	{
		if (!*Key)
		{
			LastState = KeyState::None;
			return false;
		}

		const bool isDown = GetAsyncKeyState(*Key) & 0x8000;
		LastState = isDown ? KeyState::Down : KeyState::None;
		return isDown;
	}

	// Was the button just pressed? This will only be true once.
	bool Pressed()
	{
		const bool shouldCheck = LastState == KeyState::None;
		return Down() && shouldCheck;
	}

	// Was the button just released? This will only be true once.
	bool Released()
	{
		const bool shouldCheck = LastState == KeyState::Down;
		return !Down() && shouldCheck;
	}
};