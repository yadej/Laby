#ifndef TIMER_H
#define TIMER_H

/**
 *  The use of Timer is for the cooldown
 *   of shot by the creature
 **/


#include <chrono>
class Timer
{
	std::chrono::steady_clock::time_point _lastShotTime;
	std::chrono::milliseconds cooldownDuration; 
	public:
	/* Constructor */
	Timer()
	{
		_lastShotTime = std::chrono::steady_clock::now();
		cooldownDuration = std::chrono::milliseconds(1000); 
	}

	Timer(int cooldown)
	{
		_lastShotTime = std::chrono::steady_clock::now();
		cooldownDuration = std::chrono::milliseconds(cooldown); 
	}

	/* Look at the current time to see if the creature can do 
	 * his action again
	 **/
	bool cooldown_finished()
	{
		auto currentTime = std::chrono::steady_clock::now();
		if (currentTime - _lastShotTime >= cooldownDuration) {
			_lastShotTime = currentTime; // Update the last shot time
			return true;
		}
		return false;
	}
};

#endif
