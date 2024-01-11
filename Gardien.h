#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

class Labyrinthe;

class Gardien : public Mover {
public:
	Gardien (Labyrinthe* l, const char* modele); 

	// mon gardien pense très mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);
};

#endif
