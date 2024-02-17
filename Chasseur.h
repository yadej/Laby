#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Creature.h"
#include "Sound.h"

class Labyrinthe;

class Chasseur : public Creature {
private:
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
public:
	// les sons.
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.
	// constructeur.
	Chasseur (int x, int y,Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) override {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) override {};
	// fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
	bool process_fireball (float dx, float dy) override;
	// tire sur un ennemi.
	void fire (int angle_vertical) override;
	// clic droit.
	void right_click (bool shift, bool control) override;
};

#endif
