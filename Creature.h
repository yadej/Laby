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
	int _healthPoint;
	// Cooldown on shooting
	Timer _timer;
	// Constructor
	Creature(int x,int y, int hp, int cooldown,Labyrinthe* l, const char* model): Mover(x,y, l, model)
	{
		_healthPoint = hp;
		_timer = Timer(cooldown);
	}
	// delete
	~Creature()
	{
		delete _fb;
		delete _l;
	}

	// Give the number of hp left
	int getHealthPoint(){return _healthPoint;}
	// Look if the creature is alive(at least 0 hp) or not
	bool isAlive(){return _healthPoint > 0;}
	// Creature get damage
	void getHit(int damage){_healthPoint -= damage;}
};

#endif
