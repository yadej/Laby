#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <iostream>
#include <ostream>

class Labyrinthe : public Environnement {
private:
	int		_width;
	int		_height;
	char**	_data;

public:
	Labyrinthe (char*);
	~Labyrinthe()
	{
		for(int i=0;i<_width;++i)
		{
			delete _data[i]; 
		}
	};
	int width () { return _width;}	// retourne la largeur du labyrinthe.
	int height () { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j)
	void changePosition(int current_x, int current_y, int new_x, int new_y);
	// print the maze
	void printMaze()
	{
		for(int i=0; i<_width;++i)
		{
			for(int j=0; j<_height;++j)
			{
				std::cout << _data[i][j];
			}
			std::cout << std::endl;
		}
	}
};

#endif
