#include "Chasseur.h"
#include "Creature.h"
#include "Environnement.h"
#include "Gardien.h"
#include <cmath>

// player health at the start of the game
#define MAX_HP_CHASSEUR 100
// shooting cooldown
#define COOLDOWN_SHOT_CHASSEUR 250
// Miss angle when hp is low
#define DAMAGE_SHOT_ANGLE 10
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	if(_x + dx < 0 
		|| _y + dy < 0 
		|| _x + dx > _l->width() * Environnement::scale 
		|| _y + dy > _l->height() * Environnement::scale )return false;
	char tile =  _l -> data ((int)(_x + dx) / Environnement::scale,(int)((_y + dy) / Environnement::scale));
	// Look for treasure if true finish the game
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
 *	x: x position of the player
 *	y: y position of the player
 *	l: labyrinth where the player is
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
 *	Move the fireball
 *	dx: direction x of the fireball 
 *	dy: direction y of the fireball
 *
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
			if(!gardien->is_alive())continue;
			// Damage the gardian
			gardien->get_hit(20);
			gardien->reset_timer();
			gardien->_guard_hit->play();
			// Action on the hit
			if(gardien->is_alive())gardien->tomber();
			else 
			{
				gardien->rester_au_sol();
				gardien->_guard_die->play();
			}
			message("Touche %d, Health left: %d", indexGuards, gardien->get_health_point());
			return false;
		}
	}

	// Look if the fireball is on bounds
	float new_x = _fb->get_x() + dx;
	float new_y = _fb->get_y() + dy;
	if(new_x < 0 
		|| new_y < 0 
		|| new_x > _l->width() * Environnement::scale
		|| new_y > _l->height() * Environnement::scale)return false;

	// Empty space
	if (EMPTY == _l -> data ((int)(new_x / Environnement::scale),
							 (int)(new_y / Environnement::scale)))
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

	// Compute the degree of shooting depending on the health
	int varying_degree =  (std::rand() %(2 * DAMAGE_SHOT_ANGLE) - DAMAGE_SHOT_ANGLE) \
	* (1. - (float)_health_point / MAX_HP_CHASSEUR);
	// Compute the angle where we will shoot our bullet
	int shot_angle = _angle + varying_degree;
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, shot_angle);
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
