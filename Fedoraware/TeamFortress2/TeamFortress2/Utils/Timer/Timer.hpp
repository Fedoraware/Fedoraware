#pragma once
#include <chrono>

/*
 *	Credits to cathook (nullifiedcat)
 */

class Timer {
private:
	typedef std::chrono::system_clock Clock;
	std::chrono::time_point<Clock> Last{ };

	void Update()
	{
		Last = Clock::now();
	}

	[[nodiscard]] bool Check(unsigned ms) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - Last).count() >= ms;
	}
public:
	Timer() = default;

	bool Run(unsigned ms)
	{
		if (Check(ms)) {
			Update();
			return true;
		}
		return false;
	}
};