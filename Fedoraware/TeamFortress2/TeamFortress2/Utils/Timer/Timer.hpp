#pragma once
#include <chrono>

/*
 *	Credits to cathook (nullifiedcat)
 */

class Timer {
public:
	typedef std::chrono::system_clock clock;
	std::chrono::time_point<clock> Last{ };

	Timer() = default;

	bool Check(unsigned ms) const {
		return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - Last).count() >= ms;
	}

	bool TestAndSet(unsigned ms) {
		if (Check(ms)) {
			Update();
			return true;
		}
		return false;
	}

	void Update() {
		Last = clock::now();
	}
};