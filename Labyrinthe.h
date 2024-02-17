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
	void extractWalls();
	void extractBox();
	void extractTresor();
	void extractPict();
	Wall makePict(int x, int y, char* tmp);

public:
	Labyrinthe (char*);

	~Labyrinthe();
	
	int width () override { return _width;}	// retourne la largeur du labyrinthe.
	int height () override { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j) override
	{
		return _data [i][j];
	}	// retourne la case (i, j)
	// print the maze
	void printMaze()
	{
		for(int i=0; i<_height;++i)
		{
			for(int j=0; j<_width;++j)
			{
				if(_data[j][i] == EMPTY)
				{
					std::cout << " ";
					continue;
				}
				std::cout << _data[j][i];
			}
			std::cout << std::endl;
		}
	}
};

#endif
