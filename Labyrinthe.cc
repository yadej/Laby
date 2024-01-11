#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Environnement.h"
#include "Gardien.h"
#include "Mover.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream> 
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.
/*
 *	Pour que l'interface puisse appeler le constucteur de
 *	la classe Labyrinthe sans en avoir les sources...
 */

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}
// Put the different type of guard here
const std::vector<const char *> gardien_png = {"Lezard","Blade","Samourai", "Serpent"};


std::vector<Wall> extractWalls(char ** data,const int height,const int width);
std::vector<Box> extractBox(char ** data,const int height,const int width);
Box extractTresor(char ** data,const int height,const int width);

/**
 * Constructor via filename
 *
 **/
Labyrinthe::Labyrinthe(char* filename)
{
	std::unordered_map<std::string, std::string> texture_affiche;
	std::ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	_height = 0;
	_width = 0;
	_npicts = 0;
	_nguards = 1;
	std::string line;
	while (std::getline(file, line))
	{
		if(line.empty()) continue;
		
		if( line.find('|') != std::string::npos ||
		    line.find('+') != std::string::npos ||
		    line.find('-') != std::string::npos )
		{
			_height++;
			_width = std::max(_width ,(int)line.size());
			_nguards += std::count(line.begin(), line.end(), 'G'); 
			for (const auto& pair: texture_affiche) {
				_npicts += std::count(line.begin(), line.end(), (char) pair.first[0]);
			}

		}
		else 
		{	
			std::string key,value;
			// We use stringstream to find the 2 first words
			// which is the letter and its texture
			std::stringstream new_line(line);
			new_line >> key >> value;
			if(key.find('#') != std::string::npos )continue;
			texture_affiche[key] = value;
			std::cout << key << " " << value << "\n";
		}
	}
	// Allocate memory for the labyrinth data
	char** data = new char*[_width];
	_data = new char*[_width];
	for (int i = 0; i < _width; ++i) 
	{
		data[i] = new char[_height];
		_data[i] = new char[_height];

	}

	file.close();

	file.open(filename);
	
	int current_line = 0;
	// Put character on _data
	while (std::getline(file, line))
	{
		if(line.empty() )continue;
		if( line.find('|') != std::string::npos || line.find('+') != std::string::npos ||
		    line.find('-') != std::string::npos )
		{
			for(std::size_t i=0; i < line.size();++i)
			{
				data[i][current_line] = (char)line[i];
			}
			for(std::size_t i = line.size(); i <(std::size_t)_width;++i)
			{
				data[i][current_line] = ' ';
			}
			current_line++;
		}


	}
	// See _data if it works properly
	
	for(std::size_t i =0; i < (std::size_t)_height; ++i)
	{
		for(std::size_t j = 0; j < (std::size_t) _width; ++j)
		{
			std::cout << data[j][i];
		}
		std::cout << std::endl;
	}
	
	std::cout<< "extract Walls\n";
	// Make the wall on the maze
	std::vector<Wall> walls = extractWalls(data, _height, _width);
	std::cout<< "finish Walls\n";
	char tmp[128];
	sprintf (tmp, "%s/%s", texture_dir, "brickwall.jpg");

	_nwall = walls.size();
	_walls = new Wall[_nwall];
	std::copy(walls.begin(),walls.end(), _walls);

	std::cout << "extract Box\n";
	// Make the box of the maze
	std::vector<Box> boxs = extractBox(data , _height, _width);
	std::cout << "finish Box\n";
	sprintf (tmp, "%s/%s", texture_dir, "boite2.jpg");
	for(Box &box:boxs)
	{
		box._ntex = wall_texture(tmp);
	}
	_nboxes = boxs.size();
	_boxes = new Box[_nboxes];
	std::copy(boxs.begin(), boxs.end(), _boxes); Box treasor = extractTresor(data, _height, _width);
	_treasor._x = treasor._x;
	_treasor._y = treasor._y;

	_guards = new Mover*[_nguards];
	_guards[0] = new Chasseur(this);
	//Extract Guards  From the maze 
	//Put guards number through an accumulator
	int accumulator = 1;
	// Instead to choose randomly we change the texture of the guards each time
	int turn = 0;
	for(int i=0; i<_width; ++i)
	{
		for(int j=0; j<_height;++j)
		{
			// Case for the guards
			if(data[i][j] == 'G')
			{
				_guards[accumulator] = new Gardien(this, gardien_png[turn]);
				_guards[accumulator]->_x = (float)i * scale;
				_guards[accumulator]->_y = (float)j * scale;
				++accumulator;
				turn = (turn+1) % gardien_png.size();
			}
			// Case for chaser
			if(data[i][j] == 'C')
			{
				_guards[0]->_x = (float)i * scale;
				_guards[0]->_y = (float)j * scale;
			}
		}
	}
	std::cout << "Finish Extract Guards\n";
	// Create the maze via _data
	for(int i=0; i<_width; ++i)
	{
		for(int j=0; j<_height;++j)
		{
			_data[i][j] = EMPTY;
		}
	}
	std::cout << "Put wall on maze\n";
	// Put wall in _data
	for (const Wall wall: walls) {
	
		for(int i=wall._y1; i<wall._y2+1;++i)
		{
			for(int j=wall._x1; j<wall._x2+1;++j)
			{
				if(i >= 0 && i < _height && j >= 0 && j < _width)_data[j][i] = 1;
			}
		}
	}
	std::cout << "Put box on maze\n";
	// Put Box on the maze
	for(const Box box: boxs)
	{
		_data[box._x][box._y] = 1;
	}

	// Treasor on the maze
	_data[_treasor._x][_treasor._y] = 1;
	// Put Guards on the maze
	std::cout << "Put guards on maze\n";
	for(int i=0; i<_nguards;++i)
	{
		_data[(int)(_guards[i]->_x/scale)][(int)(_guards[i]->_y/scale)] = 2;
	}
	file.close();
	_nmarks = 0;
	_marks = nullptr;
	_npicts = 0;
	_picts = 0;
	std::cout << "Finish init maze\n";
}

/*
 * Extract the Walls for the maze
 **/
std::vector<Wall> extractWalls(char ** data,const int height,const int width)
{
	// Doing like that is quite ugly
	std::vector<Wall> allWalls;
	for(int i=0; i < height;++i)
	{
		for (int j=0; j < width;++j) 
		{
			Wall wall;
			if(data[j][i] == '+')
			{
				if(j+1 < width && data[j+1][i] != ' ')
				{
					int endWall = j+1;
					while (endWall < width && data[endWall][i] != '+') 
					{
						endWall++;
					}
					wall._x1 = j;
					wall._y1 = i;
					wall._x2 = endWall;
					wall._y2 = i;
					wall._ntex = 0;
					allWalls.push_back(wall);
				}
				if(i+1 < height && data[j][i+1] != ' ' )
				{
					int endWall = i+1;
					while (endWall < height && data[j][endWall] != '+')
					{
						endWall++;
					}
					wall._x1 = j;
					wall._x2 = j;
					wall._y2 = endWall;
					wall._y1 = i;
					wall._ntex = 0;
					allWalls.push_back(wall);
				}
				
			}
		}
	}
	return allWalls;
}

std::vector<Box> extractBox(char ** data,const int height,const int width)
{
	std::vector<Box> allBox;
	for(int i=0; i < height;++i)
	{
		for (int j=0; j < width;++j) 
		{
			Box box;
			if(data[j][i] == 'X')
			{
				box._x = j;
				box._y = i;
				box._ntex = 0;
				allBox.push_back(box);
			}
		}
	}
	return allBox;
}
Box extractTresor(char ** data,const int height,const int width)
{
	Box box;
	for(int i=0; i < height; ++i)
	{
		for (int j=0; j < width; ++j) 
		{
			if(data[j][i] == 'T')
			{
				box._x = j;
				box._y = i;
				return box;
			}
		}
	}

	return box;
}

// For the mob if they move we need to updates the maze
void Labyrinthe::changePosition(int current_x, int current_y, int new_x, int new_y)
{
	if(current_x != new_x && current_y != new_y)
	{
		_data[new_x][new_y] = _data[current_x][current_y];
		_data[current_x][current_y] = EMPTY;
	}
}


/*
 *	EXEMPLE de labyrinthe � taille fixe.
 */
/*
#define	LAB_WIDTH	80
#define	LAB_HEIGHT
25

Labyrinthe::Labyrinthe (char* filename) : _width(LAB_WIDTH), _height(LAB_HEIGHT)
{
	// 1. allouer les tableaux pour les murs, les affiches et les caisses.

	// les 4 murs.
	_nwall = 4;
	_walls = new Wall [_nwall];
	// deux affiches.
	_npicts = 2;
	_picts = new Wall [_npicts];
	// 3 caisses.
	_nboxes = 3;
	_boxes = new Box [_nboxes];
	// 2 marques au sol.
	_nmarks = 2;
	_marks = new Box [_nmarks];

	// 2. placer les 4 murs.
	_walls [0]._x1 = 0; _walls [0]._y1 = 0;
	_walls [0]._x2 = width ()-1; _walls [0]._y2 = 0; 
	_walls [0]._ntex = 0; 

	_walls [1]._x1 = width ()-1; _walls [1]._y1 = 0;
	_walls [1]._x2 = width ()-1; _walls [1]._y2 = height ()-1; 
	_walls [1]._ntex = 0; 

	_walls [2]._x1 = width ()-1; _walls [2]._y1 = height ()-1;
	_walls [2]._x2 = 0; _walls [2]._y2 = height ()-1; 
	_walls [2]._ntex = 0; 

	_walls [3]._x1 = 0; _walls [3]._y1 = height ()-1;
	_walls [3]._x2 = 0; _walls [3]._y2 = 0; 
	_walls [3]._ntex = 0; 

	// 3. placer les affiches; Attention: pour des raisons de rapport d'aspect,
	// les affiches doivent faire 2 de long)
	_picts [0]._x1 = 4; _picts [0]._y1 = 0;
	_picts [0]._x2 = 6; _picts [0]._y2 = 0; 
	_picts [0]._ntex = 0; 

	// la deuxieme a une texture differente.
	_picts [1]._x1 = 8; _picts [1]._y1 = 0;
	_picts [1]._x2 = 10; _picts [1]._y2 = 0; 

	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);

	// 4. placer les caisses.
	_boxes [0]._x = 70; _boxes [0]._y = 12; _boxes [0]._ntex = 0; 
	_boxes [1]._x = 10; _boxes [1]._y = 5; _boxes [1]._ntex = 0; 

	// mettre une autre texture a la troisieme caisse. sprintf (tmp, "%s/%s", texture_dir, "boite.jpg"); _boxes [2]._x = 65; _boxes [2]._y = 22; _boxes [2]._ntex = wall_texture (tmp);

	// 5. mettre deux marques au sol.
	_marks [0]._x = 50; _marks [0]._y = 14;
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	_marks [0]._ntex = wall_texture (tmp);

	_marks [1]._x = 20; _marks [1]._y = 15;
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	_marks [1]._ntex = wall_texture (tmp);


	// 6. creer la matrice representant le sol du labyrinthe.

	// 6.1 allouer un tableau de pointeurs de lignes.
	_data = new char* [width ()];
	// 6.2 faire pointer chaque ligne sur son tableau de colonnes.
	for (int i = 0; i < width (); ++i)
		_data [i] = new char [height ()];

	// 7. indiquer l'emplacement des murs en marquant les cases du sol.
	for (int i = 0; i < width (); ++i)
		for (int j = 0; j < height (); ++j) {
			if (i == 0 || i == width ()-1 || j == 0 || j == height ()-1)
				_data [i][j] = 1;
			else
				_data [i][j] = EMPTY;
		}

	// 7.1 indiquer l'emplacement des caisses au sol.
	_data [_boxes [0]._x][_boxes [0]._y] = 1;
	_data [_boxes [1]._x][_boxes [1]._y] = 1;
	_data [_boxes [2]._x][_boxes [2]._y] = 1;

	// 8. le tresor.
	_treasor._x = 10;
	_treasor._y = 10;
	// 8.1 indiquer l'emplacement du tresor au sol.
	_data [_treasor._x][_treasor._y] = 1;

	// 9. allouer le chasseur et les 4 gardiens.
	_nguards = 5;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "Lezard");
	_guards [2] = new Gardien (this, "Blade"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Serpent"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "Samourai"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;

	// 9.1 indiquer l'emplacement des gardiens au sol.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}
*/
