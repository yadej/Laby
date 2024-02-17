#include "Gardien.h"
#include "Creature.h"
#include "Chasseur.h"
#include "Environnement.h"
#include "Labyrinthe.h"
#include "Mover.h"
#include <cmath>
#include <cstdlib>
#define MAX_HP_GARDIEN 80
#define COOLDOWN_SHOT_GARDIAN 3000
/*
 * Constructor
 **/

Gardien::Gardien (int x, int y, Labyrinthe* l, const char* modele) : Creature(x, y, MAX_HP_GARDIEN,COOLDOWN_SHOT_GARDIAN , l, modele)
{
	_state = PATROUILLE;
}

// Update on the gardian
void Gardien::update (void) 
{
	if(!isAlive())return;
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
	// Shot need to inverse to shot properly
	int shotAngle = (-_angle - 1)   % 360;
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
		message("Touche Player");
		chasseur->getHit(5);
		if(!chasseur->isAlive())partie_terminee(false);
		return false;
	}

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		return true;
	}
	// collision...
	return false;
}

void Gardien::detectChasseur()
{
	double x = _l->_guards[0]->_x;
	double y = _l->_guards[0]->_y;
	// Look first if the Chaser is in range
	// Look if there is any obstacle between the Chaser and the guards
	int distance_by_case = std::abs(_x -x) + std::abs(_y - y);
	for(int i=0; i<distance_by_case; ++i)
	{
		// Put the current position to search
		int posX = (_x + (i * (x - _x) / distance_by_case)) / Environnement::scale;
		int posY = (_y + (i * (y - _y) / distance_by_case)) / Environnement::scale;
        
		// Ensure the calculated position is within the bounds of _data
		if (posX >= 0 && posY >= 0 && posX < _l->width() && posY < _l->height())
		{
			if (_l->data(posX, posY) != EMPTY)
			{
				_state = PATROUILLE;
				return;
			}
		}
		
	}
	_state = ATTAQUE;
}

void Gardien::angleGardien()
{
	// Calculate the distance between the two
	double x = _l->_guards[0]->_x - _x;
	double y = _l->_guards[0]->_y - _y;
	
	//Calculate its angle in radian
	double angleRadians = -atan2(x, y );
	// Then convert it to degree
	int angleDegree = static_cast<int>(angleRadians * (180.0 / M_PI) );
	// when the angleDegree is negative put in positive
	if(angleDegree < 0)angleDegree += 360;

	_angle = angleDegree;
}
