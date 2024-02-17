#include "Chasseur.h"
#include "Creature.h"
#include "Environnement.h"
#include "Gardien.h"
#include <cmath>

#define MAX_HP_CHASSEUR 100
#define COOLDOWN_SHOT_CHASSEUR 1000
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	char tile =  _l -> data ((int)(_x + dx) / Environnement::scale,(int)((_y + dy) / Environnement::scale));
	if( tile == 'T')
	{
		partie_terminee(true);
	}
	if (tile == EMPTY)
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (int x, int y,Labyrinthe* l) : Creature (x, y, MAX_HP_CHASSEUR, COOLDOWN_SHOT_CHASSEUR, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// Look for each guards if the fireball is at their distance
	for(int indexGuards=1; indexGuards < _l->_nguards; ++indexGuards)
	{

		// Look for the distance between the fireball and the guards
		float distance = std::sqrt(
			std::pow(_fb->get_x()-_l->_guards[indexGuards]->_x,2) +
			std::pow(_fb->get_y()-_l->_guards[indexGuards]->_y,2)
		);
		// If that the case
		if(distance <= 10.0)
		{
			//  Dynamic cast for getting gardian method
			Gardien* gardien = dynamic_cast<Gardien*>(_l->_guards[indexGuards]);
			// Look first if it is alive
			if(!gardien->isAlive())continue;
			// Damage the gardian
			gardien->getHit(20);
			// Action on the hit
			if(gardien->isAlive())gardien->tomber();
			else gardien->rester_au_sol();
			message("Touche %d", indexGuards);
			return false;
		}
	}
	// Empty space
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	if(!_timer.cooldown_finished())return;
	_hunter_fire -> play ();

	// Compute the angle where we will shoot our bullet
	int shotAngle = _angle;

	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, shotAngle);
}

/*
 *	Clic droit: par défaut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions « tomber » et « rester_au_sol »
 */

void Chasseur::right_click (bool shift, bool control)
{
	for(int i=1; i< _l->_nguards;++i)
	{
	if (shift)
		_l -> _guards [i] -> rester_au_sol ();
	else
		_l -> _guards [i] -> tomber ();
	}
}
