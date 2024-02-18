#ifndef TIMER_H
#define TIMER_H

/**
 *  The use of Timer is for the cooldown
 *   of shot by the creature
 **/


#include <chrono>
class Timer
{
	// Variable to put the time the action was last done
	std::chrono::steady_clock::time_point _last_shot_time;
	// Cooldown of the action
	std::chrono::milliseconds _cooldown_duration; 
	public:
	/* Constructor */
	Timer()
	{
		_last_shot_time = std::chrono::steady_clock::now();
		_cooldown_duration = std::chrono::milliseconds(1000); 
	}
	/* Constructor
	*  cooldown: cooldown of the action
	**/
	Timer(int cooldown)
	{
		_last_shot_time = std::chrono::steady_clock::now();
		_cooldown_duration = std::chrono::milliseconds(cooldown); 
	}

	/* Look at the current time to see if the creature can do 
	 * his action again
	 **/
	bool cooldown_finished()
	{
		auto currentTime = std::chrono::steady_clock::now();
		if (currentTime - _last_shot_time >= _cooldown_duration) {
			_last_shot_time = currentTime; // Update the last shot time
			return true;
		}
		return false;
	}
	bool cooldown_is_passed()
	{
		auto currentTime = std::chrono::steady_clock::now();
		return currentTime - _last_shot_time >= _cooldown_duration;
	}
	// Reset the clock if needed
	void reset(){_last_shot_time = std::chrono::steady_clock::now();}
};

#endif
