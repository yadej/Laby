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

	public:
	// Put hp for movable entity
	int _health_point;
	// Cooldown on shooting
	Timer _timer;
	// Constructor
	Creature(int x,int y, int hp, int cooldown,Labyrinthe* l, const char* model): Mover(x,y, l, model)
	{
		_health_point = hp;
		_timer = Timer(cooldown);
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
	// Creature get damage
	void get_hit(int damage){_health_point -= damage;}
};

#endif
