#include "Gardien.h"
#include "Creature.h"
#include "Chasseur.h"
#include "Environnement.h"
#include "Labyrinthe.h"
#include "Mover.h"
#include "Sound.h"
#include <cmath>
#include <cstdlib>

// Hp of the gardian
#define MAX_HP_GARDIEN 80
// Cooldown of the shot of the gardian
#define COOLDOWN_SHOT_GARDIAN 3000
// The varying degree where the gardian can shoot
// when his hp are down
#define ANGLE_SHOT_GARDIAN 25
/*
 * Constructor
 * x: x position of the guard
 * y: y position of the guard
 * l: the labyrinth where the guard is
* modele: texture of the guard
 **/

Gardien::Gardien (int x, int y, Labyrinthe* l, const char* modele) : Creature(x, y, MAX_HP_GARDIEN,COOLDOWN_SHOT_GARDIAN , l, modele)
{
	_state = PATROUILLE;
	_guard_die = new Sound("sons/guard_die.wav");
	_guard_hit = new Sound("sons/guard_hit.wav");
	_guard_fire = new Sound("sons/guard_fire.wav");

}

// Update on the gardian
void Gardien::update (void) 
{
	if(!is_alive())return;
	// Put the state of the gardian
	detectChasseur();
	if(_state == PATROUILLE)
	{
		// The direction of the gardian
		double dx=std::cos(-_angle+180),dy=std::sin(-_angle+180);
		
		// If there is an obstacle change the direction of the mob
		if(!move(dx, dy))
		{
			_angle = std::rand() % 360;
		}
	}
	else
	{
		// Look to chasseur
		angleGardien();
		if (_timer.cooldown_finished()) {
			fire(0);
		}
        }
	// regen if not attacked
	regen();
}
/*
 *  Move the gardian in the maze depending 
 *  in some axe
 **/
bool Gardien::move (double dx, double dy)
{
	int tile =_l->data((int)(_x +dx)/Environnement::scale,(int)(_y + dy)/Environnement::scale);
	if(tile == EMPTY && _x + dx > 0 
		&& _y + dy > 0 
		&& _x + dx < _l->width()* Environnement::scale 
		&& _y + dy < _l->height() * Environnement::scale ) 
	{
		_x += dx;
		_y += dy;
		return true;

	}
	return false;

}
/*
 * Shot fire ball
 **/
void Gardien::fire (int angle_vertical) {
	_guard_fire->play();
	// Shot need to inverse to shot properly
	int varying_degree =  (std::rand() % (2 * ANGLE_SHOT_GARDIAN) -  ANGLE_SHOT_GARDIAN) \
	* (1. - (float)_health_point / MAX_HP_GARDIEN);

	int shotAngle = (-_angle - 1 + varying_degree)   % 360;
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, shotAngle);
}	
bool Gardien::process_fireball (float dx, float dy) { 
		
	Chasseur *chasseur = dynamic_cast<Chasseur*>( _l->_guards[0]);
	// distance between the fireball and the chasseur
	float distance = std::sqrt(
		std::pow(_fb->get_x()- chasseur->_x,2) +
		std::pow(_fb->get_y()- chasseur->_y,2)
	);
	if(distance <= 10.0)
	{
		chasseur->get_hit(5);
		chasseur->reset_timer();
		message("Player Hit:  %d/%d", chasseur->_health_point,100 );
		chasseur->_hunter_hit->play();
		if(!chasseur->is_alive())partie_terminee(false);
		return false;
	}
	// Look if the fireball is on bounds
	float new_x = _fb->get_x() + dx;
	float new_y = _fb->get_y() + dy;
	if(new_x < 0 
		|| new_y < 0 
		|| new_x > _l->width() * Environnement::scale
		|| new_y > _l->height() * Environnement::scale)return false;


	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		return true;
	}
	// collision...
	return false;
}

/**
 *  Look if the Chasseur can be seen by the guards
 *  and update the state of the guard accordingly
 **/
void Gardien::detectChasseur()
{
	// Position of the Chasseur
	double x = _l->_guards[0]->_x;
	double y = _l->_guards[0]->_y;
	// Look first if the Chaser is in range
	// Look if there is any obstacle between the Chaser and the guards
	int distance_by_case = std::abs(_x -x) + std::abs(_y - y);
	for(int i=0; i<distance_by_case; ++i)
	{
		// Put the current position to search
		int pos_X = (_x + (i * (x - _x) / distance_by_case)) / Environnement::scale;
		int pos_Y = (_y + (i * (y - _y) / distance_by_case)) / Environnement::scale;
        
		// Ensure the calculated position is within the bounds of _data
		if (pos_X >= 0 && pos_Y >= 0 && pos_X < _l->width() && pos_Y < _l->height())
		{
			if (_l->data(pos_X, pos_Y) != EMPTY)
			{
				_state = PATROUILLE;
				return;
			}
		}
		
	}
	_state = ATTAQUE;
}

/*  
 *   Put the gardian angle towards the player
 **/
void Gardien::angleGardien()
{
	// Calculate the distance between the two
	double x = _l->_guards[0]->_x - _x;
	double y = _l->_guards[0]->_y - _y;
	
	//Calculate its angle in radian
	double angle_radians = -atan2(x, y );
	// Then convert it to degree
	int angle_degree = static_cast<int>(angle_radians * (180.0 / M_PI) );
	// when the angleDegree is negative put in positive
	if(angle_degree < 0)angle_degree += 360;

	_angle = angle_degree;
}
