#ifndef CREATURE_H 
#define CREATURE_H


#include "Mover.h"
#include "Labyrinthe.h"
#include <memory>
#include "Timer.h"
/**
 *  The class creature is used to
 *  add hp interaction to movable
 *  creature
 **/
class Labyrinthe;

class Creature: public Mover
{
	// Cooldown for regen hp
	Timer _health_recovery;
	Timer _cooldown_recovery;

	public:
	// Max hp the creature can have
	int _max_hp;
	// Put hp for movable entity
	int _health_point;
	// Cooldown on shooting
	Timer _timer;
	// Constructor
	Creature(int x,int y, int hp, int cooldown,Labyrinthe* l, const char* model): Mover(x,y, l, model)
	{
		_max_hp = hp;
		_health_point = hp;
		_timer = Timer(cooldown);
		_health_recovery = Timer(500);
		_cooldown_recovery = Timer(5000);
	}
	// delete
	~Creature()
	{
		delete _fb;
		delete _l;
	}

	// Give the number of hp left
	int get_health_point(){return _health_point;}
	// Look if the creature is alive(at least 0 hp) or not
	bool is_alive(){return _health_point > 0;}
	// Creature get damaget
	void get_hit(int damage){_health_point -= damage;}
	// add health back if not attacked	
	void regen()
	{
		if(_health_recovery.cooldown_finished() 
			&&  _health_point < _max_hp 
			&& _cooldown_recovery.cooldown_is_passed() )_health_point++;
	}
	// Reset timer for recovery
	void reset_timer()
	{
		_health_recovery.reset();
		_cooldown_recovery.reset();
	}
};

#endif
