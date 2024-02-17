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
	~Labyrinthe()
	{
		// Delete the data
		for(int i=0;i<_width;++i)
		{
			delete[] _data[i]; 
		}
		delete [] _data;
		// Delete the walls
		delete[] _walls;

		// Delete the pictures
		delete[] _picts;

		// Delete the boxes
		delete[] _boxes;

		// Delete the marks
		delete[] _marks;

		// Delete the guards and their associated Mover objects
		
		delete[] _guards;
	};
	int width () override { return _width;}	// retourne la largeur du labyrinthe.
	int height () override { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j) override
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
