#pragma once
#include <Windows.h>

class KeyHelper {
private:
	enum class KeyState { NONE, DOWN };

	int* Key = nullptr;
	KeyState LastState = KeyState::NONE;

public:
	KeyHelper(int* vKey)
	{
		Key = vKey;
	}

	// Is the button currently down?
	bool IsDown()
	{
		LastState = KeyState::DOWN;
		return GetAsyncKeyState(*Key) & 0x8000;
	}

	// Was the button just pressed? This will only be true once.
	bool IsPressed()
	{
		return GetAsyncKeyState(*Key) & 0x1;
	}

	// Was the button just released? This will only be true once.
	bool IsReleased()
	{
		if (const bool keyDown = IsDown())
		{
			return false;
		}
		else
		{
			const bool released = !keyDown && LastState == KeyState::DOWN;
			LastState = KeyState::NONE;
			return released;
		}
	}
};