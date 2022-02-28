#pragma once
#include <chrono>

/*
 *	Credits to cathook (nullifiedcat)
 */

class Timer {
private:
	std::chrono::steady_clock::time_point Last;

	[[nodiscard]] bool Check(unsigned ms) const
	{
		const auto currentTime = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - Last).count() >= ms;
	}
public:
	Timer()
	{
		Last = std::chrono::steady_clock::now();
	}

	bool Run(unsigned ms)
	{
		if (Check(ms)) {
			Last = std::chrono::steady_clock::now();
			return true;
		}
		return false;
	}
};