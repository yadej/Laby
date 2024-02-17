#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Creature.h"
#include "Sound.h"
#include <chrono>

class Labyrinthe;
// The state of the gardian
enum StateGardien {
	PATROUILLE,
	ATTAQUE
};

/**
 *  The Gardien class is the enemy of the player
 **/
class Gardien : public Creature {
private:
	// State of the gardian
	StateGardien _state;
	// Look if the Chasseur is in range
	void detectChasseur();
	// Position his angle towards the player
	void angleGardien();
	
public:

	// Sound
	static Sound* _guard_die;
	static Sound* _guard_fire;
	static Sound* _guard_hit;
	// Constructor
	Gardien (int x, int y,Labyrinthe* l, const char* modele); 

	// update of the guard for each tick
	void update (void) override;
	// Movement of the guard
	bool move (double dx, double dy) override;
	// Shooting of the guard
	void fire (int angle_vertical) override;
	// Look at the fireball state and explode if false
	bool process_fireball (float dx, float dy) override;
	// Just to pass the warning
	void right_click(bool shift, bool control) override{};
	};

#endif
