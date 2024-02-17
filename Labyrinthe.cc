#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Environnement.h"
#include "Gardien.h"
#include "Mover.h"
#include "Sound.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <ostream> 
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.
Sound*  Gardien::_guard_die;    // sound guard die
Sound*  Gardien::_guard_hit;    // sound hit of the guard
Sound*  Gardien::_guard_fire;   // sound fire of the guard
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
std::unordered_map<char, char*> texture_affiche;


/**
 * Constructor via filename
 *
 **/

Labyrinthe::Labyrinthe(char* filename)
{
	// Test file
	std::ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	// Declaration of variable even not necessary
	// I know that in gerenal variable not declared
	// take a base value which is 0
	_height = 0;
	_width = 0;
	_npicts = 0;
	_nguards = 1;
	_nboxes = 0;
	// Looping through the file
	std::string line;
	while (std::getline(file, line))
	{
		if(line.empty()) continue;
		
		// If there is a wall it means it is in the labyrinth
		if( line.find('|') != std::string::npos ||
		    line.find('+') != std::string::npos ||
		    line.find('-') != std::string::npos )
		{
			_height++;
			_width = std::max(_width ,(int)line.size());
			_nguards += std::count(line.begin(), line.end(), 'G'); 
			_nboxes += std::count(line.begin(), line.end(), 'x'); 
			for (const auto& pair: texture_affiche) {
				_npicts += std::count(line.begin(), line.end(), pair.first);
			}

		}
		else 
		{	
			// Put the images in a unordered map to keep track
			//  and allow it to be used as a picts
			std::string key,value;
			// We use stringstream to find the 2 first words
			// which is the letter and its texture
			std::stringstream new_line(line);
			new_line >> key >> value;
			if(key.find('#') != std::string::npos )continue;
			char* charArray = new char[value.length() + 1];
			// Copy the content of the std::string to the char array
			std::strcpy(charArray, value.c_str());
			texture_affiche[(char) key[0]] = charArray;
			std::cout << key << " " << value << "\n";
		}
	}
	// print the number of asset
	printf("nguards %d, nboxes %d, npicts %d\n", _nguards, _nboxes, _npicts);
	// Allocate memory for the labyrinth data
	_data = new char*[_width];
	for (int i = 0; i < _width; ++i) 
	{
		_data[i] = new char[_height];

	}

	file.close();
	// We re iterate though it to find other information needed
	file.open(filename);
	
	int current_line = 0;
	// Put character on _data
	while (std::getline(file, line))
	{
		if(line.empty() )continue;
		// there is a wall we put it in data
		if( line.find('|') != std::string::npos || line.find('+') != std::string::npos ||
		    line.find('-') != std::string::npos )
		{
			for(std::size_t i=0; i < line.size();++i)
			{
				if ((char)line[i] == ' ')
					_data[i][current_line] = EMPTY;
				else _data[i][current_line] = (char)line[i];
			}
			// case if the size of the line is lower than _witdh of the labyrinth
			for(std::size_t i = line.size(); i <(std::size_t)_width;++i)
			{
				_data[i][current_line] =EMPTY;
			}
			current_line++;
		}


	}
	file.close();
	// See _data if it works properly
	printMaze();
	
	std::cout<< "extract Walls\n";
	// Make the wall on the maze
	extractWalls();
	std::cout<< "finish Walls\n";
	std::cout << "extract Box\n";
	// Make the box of the maze
	extractBox();
	std::cout << "finish Box\n";
	extractTresor();
	std::cout << "Extract Guards\n";
	_guards = new Mover*[_nguards];
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
			if(_data[i][j] == 'G')
			{
				_guards[accumulator] = new Gardien((int)i * Environnement::scale,
								  (int)j * Environnement::scale,
								  this,
								  gardien_png[turn]);
				++accumulator;
				turn = (turn+1) % gardien_png.size();
				_data[i][j] = EMPTY;
			}
			// Case for chaser
			if(_data[i][j] == 'C')
			{
				_guards[0] = new Chasseur((int) i* Environnement::scale,
							  (int) j * Environnement::scale,
							   this);
				_data[i][j] = EMPTY;
			}
		}
	}
	std::cout << "Finish Extract Guards\n";
	std::cout << "Extract pict\n";
	extractPict();
	std::cout << "Finish Extract Pict \n";
	// We consider guards as empty because they move
	_nmarks = 0;
	_marks = NULL;
	std::cout << "Finish init maze\n";
}
/*
 * Destructor of the class
 **/
Labyrinthe::~Labyrinthe()
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
}
/*
 * Extract the Walls for the maze
 **/
void Labyrinthe::extractWalls()
{
	std::vector<Wall> all_walls;
	// Declare the texture for the wall
	char tmp[128];
	sprintf (tmp, "%s/%s", texture_dir, "brickwall.jpg");


	for(int i=0; i < _height;++i)
	{
		for (int j=0; j < _width;++j) 
		{
			Wall wall;
			if(_data[j][i] == '+')
			{
				if(j+1 < _width && _data[j+1][i] != EMPTY && _data[j+1][i] != '|')
				{
					int endWall = j+1;
					while (endWall < _width && _data[endWall][i] != '+') 
					{
						endWall++;
					}
					wall._x1 = j;
					wall._y1 = i;
					wall._x2 = endWall;
					wall._y2 = i;
					wall._ntex = wall_texture(tmp);
					all_walls.push_back(wall);
				}
				if(i+1 < _height && _data[j][i+1] != EMPTY && _data[j][i + 1] != '-' )
				{
					int endWall = i+1;
					while (endWall < _height && _data[j][endWall] != '+')
					{
						endWall++;
					}
					wall._x1 = j;
					wall._x2 = j;
					wall._y2 = endWall;
					wall._y1 = i;
					wall._ntex = wall_texture(tmp);
					all_walls.push_back(wall);
				}
				
			}
		}
	}
	_nwall = all_walls.size();
	_walls = new Wall[_nwall];
	std::copy(all_walls.begin(),all_walls.end(), _walls);
}
/**
 *  put the box in the labyrinth
 */
void Labyrinthe::extractBox()
{
	char tmp[128];
	sprintf (tmp, "%s/%s", texture_dir, "boite-2.jpg");
	_boxes = new Box[_nboxes];
	unsigned current = 0;
	for(int i=0; i < _height;++i)
	{
		for (int j=0; j < _width;++j) 
		{
			if(_data[j][i] == 'x')
			{
				_boxes[current]._x = j ;
				_boxes[current]._y = i ;
				_boxes[current]._ntex = wall_texture(tmp);
				current++;
			}
		}
	}

}
// Search for the tresor in data and put it in the labyrinth
void Labyrinthe::extractTresor()
{
	for(int i=0; i < _height; ++i)
	{
		for (int j=0; j < _width; ++j) 
		{
			if(_data[j][i] == 'T')
			{
				_treasor._x = j;
				_treasor._y = i;
			}
		}
	}
}
/* Make the wall to put the pict
 * x: x position of the pict
 * y: y position of the pict
 * tmp: texture of the pict
 **/
Wall Labyrinthe::makePict(int x, int y, char* tmp)
{
	Wall wall;
	wall._ntex = wall_texture(tmp);
	if( x == 0 || x == _width || _data[x-1][y] == '-'|| _data[x+1][y] == '-')
	{
		wall._x1 = x;
		wall._x2 = x + 2;
		wall._y1 = y;
		wall._y2 = y;

	}
	else{
		wall._x1 = x;
		wall._x2 = x;
		wall._y1 = y;
		wall._y2 = y + 2;
	}
	
	return wall;
}

/**
 *  Put the pict in the labyrinth
 */
void Labyrinthe::extractPict()
{
	_picts = new Wall[_npicts];
	unsigned current = 0;
	for(int i=0; i < _height;++i)
	{
		for (int j=0; j < _width;++j) 
		{
			// Look for the right picture
			for (const auto& pair : texture_affiche) {
				if(pair.first == _data[j][i])
				{
					_picts[current] = makePict(j, i, pair.second);
					current++;
				}
			}
		}
	}


}

