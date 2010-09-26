#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <termios.h>
using namespace std;

struct tile
{
	int type;
	bool locked;
	bool connected;
};

int sizeX;
int sizeY;

int currentX = 0;
int currentY = 0;

int centreX;
int centreY;

vector<vector<tile> > maze;

string chooseTile(int type);
void setColor(int color);
void setBackColor(int color);
void setCursor(int X, int Y);
void rotateClockwise(int n);
bool findEmptyCell();
void wrapCoords();
void clearConnected();
void setConnected(int X, int Y);
void drawMap();
void drawTile(int X,int Y);
template<class T> T fromString(const string& s);

int main(int argc, char *argv[])
{
	vector<string> args(argv, argv + argc);
	if(args.size() < 3)
	{
		cout << "usage: " + string(args.at(0)) + " x-size y-size" << endl;
		cout << "Move cursor:\t\tArrow keys" << endl;
		cout << "Rotate clockwise:\t. or Space" << endl;
		cout << "Rotate anticlockwise:\t," << endl;
		cout << "Lock tile:\t\tl or s" << endl;
		cout << "Quit:\t\t\tq" << endl;
		return 1;
	}
	sizeX = fromString<int>(args.at(1));
	sizeY = fromString<int>(args.at(2));

	centreX = sizeX/2;
	centreY = sizeY/2;

	for(int i=0; i<sizeX; ++i)
	{
		maze.push_back(vector<tile>(sizeY));
	}

	termios before, after;
	tcgetattr (STDIN_FILENO, &before); // fill 'before' with current termios values
	after = before; // make a copy to be modified
	after.c_lflag &= (~ICANON); // Disable canonical mode, including line buffering
	after.c_lflag &= (~ECHO); // Don't echo characters on the screen (optional)
	tcsetattr (STDIN_FILENO, TCSANOW, &after); // Set the modified flags

	srand(time(NULL));

	///////////
	// Map generation
	///////////
	//Set centre tile to be source:
	//maze.at(centreX).at(centreY) = 15;
	//Do a series of random walks (without looping) to generate a tree:
	while(findEmptyCell())
	{
		while(1)
		{
			int blocked = 0x00;
			int connected = 0x00;
			bool chosen = false;
			if(currentX > 0 && !maze.at(currentX-1).at(currentY).locked)
			{
				if(maze.at(currentX-1).at(currentY).type) connected |= 0x01;
			}
			else
			{
				blocked |= 0x01;
			}
			if(currentY < sizeY-1 && !maze.at(currentX).at(currentY+1).locked)
			{
				if(maze.at(currentX).at(currentY+1).type) connected |= 0x02;
			}
			else
			{
				blocked |= 0x02;
			}
			if(currentX < sizeX-1 && !maze.at(currentX+1).at(currentY).locked)
			{
				if(maze.at(currentX+1).at(currentY).type) connected |= 0x04;
			}
			else
			{
				blocked |= 0x04;
			}
			if(currentY > 0 && !maze.at(currentX).at(currentY-1).locked)
			{
				if(maze.at(currentX).at(currentY-1).type) connected |= 0x08;
			}
			else
			{
				blocked |= 0x08;
			}
			if(connected)
			{
				while(!chosen)
				{
					switch(rand()%4)
					{
						case 0:
							if(connected & 0x01)
							{
								maze.at(currentX).at(currentY).type |= 0x01;
								--currentX;
								maze.at(currentX).at(currentY).type |= 0x04;
								chosen = true;
							}
							break;
						case 1:
							if(connected & 0x02)
							{
								maze.at(currentX).at(currentY).type |= 0x02;
								++currentY;
								maze.at(currentX).at(currentY).type |= 0x08;
								chosen = true;
							}
							break;
						case 2:
							if(connected & 0x04)
							{
								maze.at(currentX).at(currentY).type |= 0x04;
								++currentX;
								maze.at(currentX).at(currentY).type |= 0x01;
								chosen = true;
							}
							break;
						case 3:
							if(connected & 0x08)
							{
								maze.at(currentX).at(currentY).type |= 0x08;
								--currentY;
								maze.at(currentX).at(currentY).type |= 0x02;
								chosen = true;
							}
							break;
					}
				}
				break;
			}
			else if(blocked == 0x0F)
			{
				break;
			}
			else
			{
				while(!chosen)
				{
					switch(rand()%4)
					{
						case 0:
							if(!(blocked & 0x01))
							{
								maze.at(currentX).at(currentY).locked = true;
								maze.at(currentX).at(currentY).type |= 1;
								--currentX;
								maze.at(currentX).at(currentY).type |= 4;
								chosen = true;
							}
							break;
						case 1:
							if(!(blocked & 0x02))
							{
								maze.at(currentX).at(currentY).locked = true;
								maze.at(currentX).at(currentY).type |= 2;
								++currentY;
								maze.at(currentX).at(currentY).type |= 8;
								chosen = true;
							}
							break;
						case 2:
							if(!(blocked & 0x04))
							{
								maze.at(currentX).at(currentY).locked = true;
								maze.at(currentX).at(currentY).type |= 4;
								++currentX;
								maze.at(currentX).at(currentY).type |= 1;
								chosen = true;
							}
							break;
						case 3:
							if(!(blocked & 0x08))
							{
								maze.at(currentX).at(currentY).locked = true;
								maze.at(currentX).at(currentY).type |= 8;
								--currentY;
								maze.at(currentX).at(currentY).type |= 2;
								chosen = true;
							}
							break;
					}
				}
			}
		}
		for(int i=0; i<maze.size(); ++i)
		{
			for(int j=0; j<maze.at(i).size(); ++j)
			{
				maze.at(i).at(j).locked = false;
			}
		}
	}
	//randomize it all:
	for(int i=0; i<maze.size(); ++i)
	{
		for(int j=0; j<maze.at(i).size(); ++j)
		{
			currentX = i;
			currentY = j;
			rotateClockwise(rand()%4);
		}
	}

	cout << "\033[2J";
	drawMap();
	cout << "\033[s";

	char ch;
	while(1)
	{
		if(ch=cin.get())
		{
			if(ch == 'q')
			{
				tcsetattr (STDIN_FILENO, TCSANOW, &before);
				return 0;
			}
			else if(ch == ' ' || ch == '.')
			{
				if(!maze.at(currentX).at(currentY).locked)
				{
					rotateClockwise(1);
					drawMap();
				}
			}
			else if(ch == ',')
			{
				if(!maze.at(currentX).at(currentY).locked)
				{
					rotateClockwise(3);
					drawMap();
				}
			}
			else if(ch == 'l' || ch == 's')
			{
				maze.at(currentX).at(currentY).locked = !maze.at(currentX).at(currentY).locked;
				drawMap();
			}
			else if(ch == 27)
			{
				ch=cin.get();
				if(ch == '[')
				{
					//we have a special character
					ch=cin.get();
					if(ch == 'A')
					{
						//cout << "Up";
						int oldX = currentX;
						currentX = (--currentX%sizeX + sizeX)%sizeX;
						drawTile(oldX, currentY);
						drawTile(currentX, currentY);
						cout << "\033[u";
					}
					else if(ch == 'B')
					{
						//cout << "Down";
						int oldX = currentX;
						currentX = (++currentX%sizeX + sizeX)%sizeX;
						drawTile(oldX, currentY);
						drawTile(currentX, currentY);
						cout << "\033[u";
					}
					else if(ch == 'C')
					{
						//cout << "Right";
						int oldY = currentY;
						currentY = (++currentY%sizeY + sizeY)%sizeY;
						drawTile(currentX, oldY);
						drawTile(currentX, currentY);
						cout << "\033[u";
					}
					else if(ch == 'D')
					{
						//cout << "Left";
						int oldY = currentY;
						currentY = (--currentY%sizeY + sizeY)%sizeY;
						drawTile(currentX, oldY);
						drawTile(currentX, currentY);
						cout << "\033[u";
					}
					else
					{
					}
				}
				else
				{
					//cout << "ESC" << ch;
				}
			}
			else
			{
				//cout << ch;
			}
		}
		
	}
}

string chooseTile(int type)
{
	switch(type)
	{
		case 0x00:
			return ".";
		case 0x01:
			return "╨";
		case 0x02:
			return "╞";
		case 0x03:
			return "╚";
		case 0x04:
			return "╥";
		case 0x05:
			return "║";
		case 0x06:
			return "╔";
		case 0x07:
			return "╠";
		case 0x08:
			return "╡";
		case 0x09:
			return "╝";
		case 0x0A:
			return "═";
		case 0x0B:
			return "╩";
		case 0x0C:
			return "╗";
		case 0x0D:
			return "╣";
		case 0x0E:
			return "╦";
		case 0x0F:
			return "╬";
		default:
			return "*";
	}
}

void setBackColor(int color)
{
	if(color == -1)
	{
		cout << "\033[0m";
	}
	else if(color >= 0 && color <8)
	{
		cout << "\033[4" << color << "m";
	}
}

void setColor(int color)
{
	if(color == -1)
	{
		cout << "\033[0m";
	}
	else if(color >= 0 && color <8)
	{
		cout << "\033[3" << color << "m";
	}
}

void setCursor(int X, int Y)
{
	cout << "\033[" << X+1 << ";" << Y+1 << "f";
}

void rotateClockwise(int n)
{
	int current = maze.at(currentX).at(currentY).type;
	current &= 0x0F;
	current = (current << n) | (current >> (4 - n));
	current &= 0x0F;
	maze.at(currentX).at(currentY).type &= 0xF0;
	maze.at(currentX).at(currentY).type |= current;
}

bool findEmptyCell()
{
	for(int i=0; i<maze.size(); ++i)
	{
		for(int j=0; j<maze.at(i).size(); ++j)
		{
			if(maze.at(i).at(j).type == 0)
			{
				currentX = i;
				currentY = j;
				return true;
			}
		}
	}
	return false;
}

void wrapCoords()
{
	currentX = (currentX%sizeX + sizeX)%sizeX;
	currentY = (currentY%sizeY + sizeY)%sizeY;
}

void clearConnected()
{
	for(int i=0; i<maze.size(); ++i)
	{
		for(int j=0; j<maze.at(i).size(); ++j)
		{
			maze.at(i).at(j).connected = false;
		}
	}
}

void setConnected(int X, int Y)
{
	if(!maze.at(X).at(Y).connected)
	{
		maze.at(X).at(Y).connected = true;
		if(X > 0 && maze.at(X).at(Y).type & 0x01 && maze.at(X-1).at(Y).type & 0x04)
		{
			setConnected(X-1,Y);
		}
		if(Y <sizeY-1 && maze.at(X).at(Y).type & 0x02 && maze.at(X).at(Y+1).type & 0x08)
		{
			setConnected(X,Y+1);
		}
		if(X < sizeX-1 && maze.at(X).at(Y).type & 0x04 && maze.at(X+1).at(Y).type & 0x01)
		{
			setConnected(X+1,Y);
		}
		if(Y > 0 && maze.at(X).at(Y).type & 0x08 && maze.at(X).at(Y-1).type & 0x02)
		{
			setConnected(X,Y-1);
		}
	}
}

void drawMap()
{
	clearConnected();
	setConnected(centreX, centreY);
	//clear screen:
	int connected = 0;
	for(int i=0; i<maze.size(); ++i)
	{
		for(int j=0; j<maze.at(i).size(); ++j)
		{
			drawTile(i,j);
			if(maze.at(i).at(j).connected)
			{
				++connected;
			}
		}
		cout << endl;
	}
	cout << connected << "/" << sizeX*sizeY << endl;
	if(connected == sizeX*sizeY)
	{
		cout << "Congratulations!";
	}
	else
	{
		cout << "\033[K";
	}
	cout << endl;
}

void drawTile(int X,int Y)
{
	setCursor(X,Y);
	if(X == centreX && Y == centreY)
	{
		//set centre to purple
		setBackColor(5);
	}
	if(maze.at(X).at(Y).locked)
	{
		//set locked to blue
		setBackColor(4);
	}
	if(maze.at(X).at(Y).connected)
	{
		//set connected green
		setColor(2);
	}
	if(X == currentX && Y == currentY)
	{
		//set cursor to red
		setBackColor(1);
	}
	cout << chooseTile(maze.at(X).at(Y).type);
	setColor(-1);
}

//converts from a string to an arbitray numerical format
template<class T> T fromString(const string& s)
{
	istringstream stream(s);
	T t;
	stream >> t;
	return t;
}
