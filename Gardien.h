#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Creature.h"
#include <chrono>

class Labyrinthe;

enum StateGardien {
	PATROUILLE,
	ATTAQUE
};
class Gardien : public Creature {
private:
	StateGardien _state;
	bool _stateFireball;

	// Look if the Chaser is in range
	void detectChasseur();
	// Position his angle towards the player
	void angleGardien();
	
public:
	Gardien (int x, int y,Labyrinthe* l, const char* modele); 

	void update (void) override;
	bool move (double dx, double dy) override;
	void fire (int angle_vertical) override;
	bool process_fireball (float dx, float dy) override;
	void right_click(bool shift, bool control) override{};
	};

#endif
