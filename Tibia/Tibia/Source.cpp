// NOTES TO SELF:
/*

MAXIMUM CONSOLE WIDTH: 79
MAXIMUM CONSOLE HEIGHT: 24

*/

#include <iostream>		// necessary for I/O
#include <string>		// necessary for strings
#include <vector>		// necessary for vectors
#include <cstdlib>
#include <stdlib.h>
#include <conio.h>
#include <fstream>		// necessary for FILE I/O
#include <Windows.h>	// necessary for set_cursor and set_color
#include <direct.h>
#include <ctime>		// necessary for accessing CPU time

using namespace std;

#define error1 "\n.:ERROR:.\nSelect one of the AVAILABLE options. "
#define SaveDirectory "C:\\Games"				// game folder
#define SaveDestiny "C:\\Games\\Data.txt"		// saves file

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27

//====================================================//

// GENERATING RANDOM NUMBERS
int random_number (int min, int max)
{
	max++;

	return ((rand() % (max-min)) + min);
}

//====================================================//

// CORES DA CONSOLA
/*
Legenda de cores:
(fundo,texto)
0 = Black       8 = Gray
1 = Blue        9 = Light Blue
2 = Green      10 = Light Green
3 = Aqua       11 = Light Aqua
4 = Red        12 = Light Red
5 = Purple     13 = Light Purple
6 = Yellow     14 = Light Yellow
7 = White      15 = Bright White
*/

// COR: Sets background and foreground color
void cor (int fundo, int texto)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, fundo*16 + texto);
}

// Control Cursor Position
void gotoxy (short x, short y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // gets handle (default console). this is the place we're gonna mess with
	COORD position = {x, y}; // sets the coordinates
	SetConsoleCursorPosition(handle, position); // places cursor in the coordinates in the default output handle (console)
}

// GETTING KEY STROKES
int * moving_player (int x, int y)
{
	int KB_code=0;
	int static temp [2];
	int * ponteiro;
	ponteiro =  & temp[0];
	
	while(KB_code != KB_ESCAPE )
	{
		if (_kbhit())
		{
			KB_code = _getch();

			switch (KB_code)
			{
			case KB_LEFT:
				temp[0] = x-1;
				temp[1] = y;
				return ponteiro;
				break;
			case KB_RIGHT:
				temp[0] = x+1;
				temp[1] = y;
				return ponteiro;
				break;
			case KB_UP:
				temp[0] = x;
				temp[1] = y-1;
				return ponteiro;
				break;
			case KB_DOWN:
				temp[0] = x;
				temp[1] = y+1;
				return ponteiro;
				break;
			case KB_ESCAPE:
				temp[0] = -1; // when 1st element of array is -1, open menu
				temp[1] = y;
				return ponteiro;
				break;
			}
		}
	}
	return ponteiro;
}

//====================================================//

// PRESS ENTER TO CONTINUE
void pressenter ()
{
	cor(0,14);
	cout << "Press <any key> to continue... ";
	int input;
	input = _getch();
}

//====================================================//

// STRUCTS

struct Heal_Spell
{
	int min_heal;
	int max_heal;
	int mana;
};

struct Attack_Spell
{
	int min_atk;
	int max_atk;
	int mana;
};

struct Weapon
{
	int min_atk;
	int max_atk;
};

struct Shield
{
	int def;
};

struct Monster
{
	string name;
	int hp;
	int max_hp;
	int min_atk;
	int max_atk;
	int min_loot;
	int max_loot;
	int exp;
};

struct Player
{
	string name;
	int level;
	int exp;
	int min_exp;
	int max_exp;
	int exp_percentage;
	int hp; // life
	int max_hp;
	int hp_percentage;
	int mp; // mana
	int max_mp;
	int mp_percentage;
	int gold;
	int weapon;
	int shield;
	int heal_spell;
	int attack_spell;
};

Player StartNewPlayer(string name)
{
	Player player;

	player.name = name;
	player.level = 1;
	player.exp = 0;
	player.min_exp = 0;
	player.max_exp = 10;
	player.exp_percentage = 0;
	player.hp = 30;
	player.max_hp = 30;
	player.hp_percentage = 100;
	player.mp = 10;
	player.max_mp = 10;
	player.mp_percentage = 100;
	player.gold = 50;
	player.weapon = 0;
	player.shield = 0;
	player.heal_spell = 0;
	player.attack_spell = 0;

	return player;
}

// ==== DRAWING LINES AND RECTANGLES ==== //

// Draws horizontal line
void draw_horizontal_line (int x1, int x2, int y, int CorFundo)
{
	cor(CorFundo, CorFundo);
	for (; x1 <= x2; x1++)
	{
		gotoxy(x1,y);
		cout << " ";
	}
}

// Draws vertical line
void draw_vertical_line (int y1, int y2, int x, int CorFundo)
{
	cor(CorFundo, CorFundo);
	for (; y1 <= y2; y1++)
	{
		gotoxy(x,y1);
		cout << " ";
	}
}

// Draws a rectangle
void draw_rectangle (int x1, int y1, int x2, int y2, int CorFundo)
{
	for (; y1 <= y2; y1++)
	{
		draw_horizontal_line(x1,x2,y1,CorFundo);
	}
}

//====================================================//

// FUNCTION DECLARATIONS

void game_interface (int x, int y, Player player);
void refresh_left_panel (Player player);
void refresh_right_panel (Player player);
void display_game_board (Player player);
Player refresh_player_variables (Player player);
void refresh_player_bars (Player player);

//====================================================//

#define encrypt_value1 3121
#define encrypt_value2 7132

// SAVE GAME
void save_game(Player player, int dialog_enable)
{
	ofstream coutfile (SaveDestiny, ios::app);

	if (coutfile.is_open()) // if it opens correctly
	{
		string imported_name;
		ifstream cinfile (SaveDestiny); // opens file that contains the saved games

		bool j = 0;
		
		// Now checking if the name already exists
		while (cinfile >> imported_name) // while the end of file is not reached
		{
			if (player.name.compare(imported_name) == 0) // if the strings are the same, overwrite data
			{
				j = 1; // enable overwrite
				break;
			}
			// if the strings are different, keep reading
		}
		// at this point: j = 0 to append to end. j = 1 to overwrite.

		// Overwriting data
		if (j == 1)
		{
			ifstream cinfile (SaveDestiny);

			// now determining the size of the vector (number of lines in txt)
			int line_numbers = 0;
			string line;
			while (getline(cinfile, line))
			{
				line_numbers++;
			}

			cinfile.close();	// closing
			ifstream cinfile2 (SaveDestiny);	// reopening to read from the beginning 

			// now creating the vector with the saves
			vector<vector<string>> temp_saves(line_numbers, vector<string>(2));
			string name2;
			string values;

			for (unsigned int x = 0; x < temp_saves.size(); x++)
			{
				cinfile2 >> name2;
				getline(cinfile2, values);
				
				temp_saves[x][0] = name2;
				temp_saves[x][1] = values;
			}

			coutfile.close(); // closing output file
			ofstream coutfile2 (SaveDestiny); // reopening in overwrite mode

			// delete all saves.txt, copying vector content to txt (except the one we want to overwrite)
			for (unsigned int x = 0; x < temp_saves.size(); x++)
			{
				if ( temp_saves[x][0].compare(player.name) != 0)
				{
					coutfile2 << temp_saves[x][0] << temp_saves[x][1] << endl;
				}
			}
			coutfile2.close(); // closing output file
		}

		// Appending new data...
		ofstream coutfile3 (SaveDestiny, ios::app); // reopening in append mode
		coutfile3 << player.name << " "
			<< (player.level * encrypt_value1) - encrypt_value2  << " "
			<< (player.exp * encrypt_value1) - encrypt_value2 << " "
			<< (player.min_exp * encrypt_value1) - encrypt_value2 << " "
			<< (player.max_exp * encrypt_value1) - encrypt_value2 << " "
			<< (player.hp * encrypt_value1) - encrypt_value2 << " "
			<< (player.max_hp * encrypt_value1) - encrypt_value2 << " "
			<< (player.mp * encrypt_value1) - encrypt_value2 << " "
			<< (player.max_mp * encrypt_value1) - encrypt_value2 << " "
			<< (player.gold * encrypt_value1) - encrypt_value2 << " "
			<< (player.weapon * encrypt_value1) - encrypt_value2 << " "
			<< (player.shield * encrypt_value1) - encrypt_value2 << " "
			<< (player.heal_spell * encrypt_value1) - encrypt_value2 << " "
			<< (player.attack_spell * encrypt_value1) - encrypt_value2 << endl;
	}
	else
	{
		_mkdir(SaveDirectory);
		ofstream write (SaveDestiny);
		save_game(player, 0);
	}

	if (dialog_enable == 1)
	{
		draw_rectangle(37,8,72,14,15);	// white limits
		draw_rectangle(39,9,70,13,9);	// blue background
		cor(9,15);
		gotoxy(50,10);
		cout << "GAME SAVED!";
		gotoxy(41,12);
		cor(9,14);
		cout << "Press any key to continue... ";
		int input;
		input = _getch();
	}
}

// LOAD GAME
Player load_game()
{
	system("cls");
	cor(0,10);

	string name;
	string imported_name;
	int level, exp, min_exp, max_exp, hp, max_hp, mp, max_mp, gold, weapon, shield, heal_spell, attack_spell;

	cout << "Please login to your character.\n";
	cout << "Name: "; cin >> name; cin.ignore();

	ifstream cinfile (SaveDestiny); // opens file that contains the saved games

	if (cinfile.is_open())
	{
		cinfile >> imported_name; // getting first element of file

		bool k = 0;	// k = 0 while the strings don't match. k = 1 when ths string was found

		while (!cinfile.eof()) // while the end of file is not reached
		{
			if (name.compare(imported_name) == 0) // if the strings are the same
			{
				k = 1;

				cinfile >> level >> exp >> min_exp >> max_exp >> hp >> max_hp >> mp >> max_mp >> gold >> weapon >> shield
					>> heal_spell >> attack_spell;

				// assigning values from text file
				Player player;

				player.name = name;
				player.level = (level + encrypt_value2) / encrypt_value1;
				player.exp = (exp + encrypt_value2) / encrypt_value1;
				player.min_exp = (min_exp + encrypt_value2);
				player.max_exp = (max_exp + encrypt_value2) / encrypt_value1;
				player.exp_percentage = (int) (exp - min_exp) * 100 / (max_exp - min_exp);
				player.hp = (hp + encrypt_value2) / encrypt_value1;
				player.max_hp = (max_hp + encrypt_value2) / encrypt_value1;
				player.hp_percentage = (int) hp * 100 / max_hp;
				player.mp = (mp + encrypt_value2) / encrypt_value1;
				player.max_mp = (max_mp + encrypt_value2) / encrypt_value1;
				player.mp_percentage = (int) mp * 100 / max_mp;
				player.gold = (gold + encrypt_value2) / encrypt_value1;
				player.weapon = (weapon + encrypt_value2) / encrypt_value1;
				player.shield = (shield + encrypt_value2) / encrypt_value1;
				player.heal_spell = (heal_spell + encrypt_value2) / encrypt_value1;
				player.attack_spell = (attack_spell + encrypt_value2) / encrypt_value1;

				cout << "\nLoading Successful!\n" << endl;
				pressenter();
				return player;
				break;
			}
			else // if the strings are different, keep reading
			{
				cinfile >> imported_name;
			}
		}

		if (k == 0)	// display error when the string wasn't found
		{
			cout << "\n..::ERROR::..\nCouldn't find your saved game. Make sure your name is correct.\n\n";
			pressenter();
		}
	}
	else
	{
		cout << "\n..:ERROR::..\nThere are no saved profiles yet." << endl;
		cout << "Please start a new game in the Main Menu." << endl;
		cout << "The game is now going to exit, please restart it.\n\n";
		pressenter();
		exit(0);
	}
	// DO NOT TOUCH THIS!!!
	return (load_game());
}

//====================================================//

// START MENU
void main_menu_UI ()
{
	system("cls");
	draw_rectangle(0,0,79,24,9);
	draw_rectangle(20,3,59,15,15);
	draw_rectangle(22,4,57,14,9);

	cor(9,14);
	gotoxy(37,5);
	cout << "TIBIA";

	cor(9,15);
	gotoxy(24,7);
	cout << "1. New Game";
	gotoxy(24,8);
	cout << "2. Load Game";

	gotoxy(24,10);
	cout << "Y. About";
	cor(9,12);
	gotoxy(24,11);
	cout << "X. Exit";

	cor(9,15);
	gotoxy(24,13);
	cout << "Select one option: ";
}

// ABOUT
void about_UI ()
{
	system("cls");
	cout << ":::::::::::::::::::::::::::::::::::" << endl;
	cout << "::                               ::" << endl;
	cout << "::             Tibia             ::" << endl;
	cout << "::                               ::" << endl;
	cout << ":::::::::::::::::::::::::::::::::::" << endl;
	cout << "::                               ::" << endl;
	cout << ":: About...                      ::" << endl;
	cout << "::                               ::" << endl;
	cout << ":: Version: 1.07                 ::" << endl;
	cout << "::                               ::" << endl;
	cout << ":: Author: Henrique Ferrolho     ::" << endl;
	cout << ":: Date: 03-03-2013              ::" << endl;
	cout << "::                               ::" << endl;
	cout << ":::::::::::::::::::::::::::::::::::" << endl;
}

//====================================================//

// OUTPUT PLAYER ON SCREEN
void output_player (int x, int y)
{
	// BLUE CENTER THING
	if (((54 <= x) && (x <= 56)) && ((11 <= y) && (y <= 12)))	// center of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191 << endl;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if (((53 <= x) && (x <= 57)) && (y == 10))		// top of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191 << endl;
		cor(3,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 53) && ((11 <= y) && (y <= 12)))		// left of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(3,0);
		cout << (char) 233 << (char) 191 << endl;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 52) && ((11 <= y) && (y <= 13)))		// far left of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(3,0);
		cout << (char) 191 << endl;
		cor(2,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 57) && ((11 <= y) && (y <= 12)))		// right of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(2,0);
		cout << (char) 191 << endl;
		gotoxy(x,y+1);
		cor(3,0);
		cout << (char) 227;
	}
	else if ((x == 58) && ((11 <= y) && (y <= 13)))		// far right of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(2,0);
		cout  << (char) 233 << (char) 191 << endl;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if (((54 <= x) && (x <= 56)) && (y == 13))		// bottom of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191 << endl;
		cor(2,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 53) && (y == 13))		// bottom left detail of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(3,0);
		cout << (char) 233 << (char) 191 << endl;
		cor(2,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 57) && (y == 13))		// bottom right detail of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(2,0);
		cout << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}

	// HOUSES
	else if (((((32 <= x) && (x <= 37)) && (y == 9)) ||		// magic shop	// default brown ground
			  (((36 <= x) && (x <= 38)) && ((10 <= y) && (y <= 11))) ||
			  (((36 <= x) && (x <= 37)) && (y == 12)) ||
			  (((32 <= x) && (x <= 37)) && (y == 13)) ||
			  ((x == 35) && (y == 10))) ||
			 ((((72 <= x) && (x <= 77)) && (y == 9)) ||		// dungeons
			  (((72 <= x) && (x <= 75)) && (y == 10)) ||
			  (((72 <= x) && (x <= 74)) && ((y == 11) || (y == 12)) ||
			  (((72 <= x) && (x <= 77)) && (y == 13)))) ||
			 ((((50 <= x) && (x <= 52)) && ((1 <= y) && (y <= 3))) ||	// black smith
			  (((58 <= x) && (x <= 60)) && ((1 <= y) && (y <= 3))) ||
			  (((53 <= x) && (x <= 57)) && (y == 3)) ||
			  (((54 <= x) && (x <= 56)) && (y == 4)) ||
			  (((54 <= x) && (x <= 55)) && (y == 5))))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191 << endl;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 39) && (y == 10))		// from here it's all about the detail when going through the door
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(3,0);
		cout << (char) 191 << endl;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 40) && (y == 10))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(3,0);
		cout << (char) 233;
		cor(2,0);
		cout << (char) 191;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 41) && (y == 10))
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(2,0);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 39) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
	}
	else if ((x == 40) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(2,0);
		cout << (char) 191;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 41) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(2,0);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((((32 <= x) && (x <= 34)) && (y == 10)) ||		// this is about the house interactive black ground
			 (((76 <= x) && (x <= 77)) && (y == 10)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
		cor(0,7);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((((x == 33) || (x == 77)) && (y == 11)) ||
			 ((x == 55) && (y ==1)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((((x == 33) || (x == 77)) && (y == 12)) ||
			 ((x == 55) && (y == 2)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 34) && (y == 11) || ((x == 56) && (y == 1)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(6,0);
		cout << (char) 191;
		cor(0,7);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 34) && (y == 12) || ((x == 56) && (y == 2)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(6,0);
		cout << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if (((x == 35) && ((y == 11) || (y == 12))) ||
			 ((x == 57) && ((y == 1) || (y == 2))))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(6,0);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 54) && (y == 1) || ((x == 76) && (y == 11)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(0,7);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 54) && (y == 2) || ((x == 76) && (y == 12)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(0,7);
		cout << (char) 233 << (char) 191;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if (((x == 53) && ((y == 1) || (y == 2))) ||
			 ((x == 75) && ((y == 11) || (y == 12))))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(0,7);
		cout << (char) 191;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 69) && (y == 10))		// from here it's all about the detail when going through the east door
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(3,0);
		cout << (char) 191 << endl;
		cor(2,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 69) && (y == 11))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(6,0);
		cout << (char) 191;
	}
	else if ((x == 70) && (y == 10))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(3,0);
		cout << (char) 233;
		cor(6,0);
		cout << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 70) && (y == 11))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(6,0);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 71) && (y == 10))
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << (char) 218;
		cor(6,0);
		cout << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 71) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if ((x == 56) && (y == 5))		// from here it's all about the detail when going through the east door
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233;
		cor(6,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}
	else if (((54 <= x) && (x <= 56)) && (y == 6))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191;
		cor(2,0);
		gotoxy(x,y+1);
		cout << (char) 227;
	}

	// REGULAR GRASS (default for the rest of the terrain)
	else
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << (char) 218 << (char) 233 << (char) 191 << endl;
		gotoxy(x,y+1);
		cout << (char) 227;
	}
}

// CLEAN LAST PLAYER OUTPUT ON SCREEN
void clean_last_player_output (int x, int y)
{
	// BLUE CENTER THING
	if (((54 <= x) && (x <= 56)) && ((11 <= y) && (y <= 12)))	// center of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << "   ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if (((53 <= x) && (x <= 57)) && (y == 10))		// top of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << "   ";
		cor(3,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 53) && ((11 <= y) && (y <= 12)))		// left of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(3,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 52) && ((11 <= y) && (y <= 13)))		// far left of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(3,0);
		cout << " ";
		cor(2,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 57) && ((11 <= y) && (y <= 12)))		// right of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(2,0);
		cout << " ";
		gotoxy(x,y+1);
		cor(3,0);
		cout << " ";
	}
	else if ((x == 58) && ((11 <= y) && (y <= 13)))		// far right of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(2,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if (((54 <= x) && (x <= 56)) && (y == 13))		// bottom of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << "   ";
		cor(2,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 53) && (y == 13))		// bottom left detail of center
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(3,0);
		cout << "  ";
		cor(2,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 57) && (y == 13))		// bottom right detail of center
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(2,0);
		cout << " ";
		gotoxy(x,y+1);
		cout << " ";
	}

	// HOUSES
	else if (((((32 <= x) && (x <= 37)) && (y == 9)) ||		// magic shop	// default brown ground
			  (((36 <= x) && (x <= 38)) && ((10 <= y) && (y <= 11))) ||
			  (((36 <= x) && (x <= 37)) && (y == 12)) ||
			  (((32 <= x) && (x <= 37)) && (y == 13)) ||
			  ((x == 35) && (y == 10))) ||
			 ((((72 <= x) && (x <= 77)) && (y == 9)) ||		// dungeons
			  (((72 <= x) && (x <= 75)) && (y == 10)) ||
			  (((72 <= x) && (x <= 74)) && ((y == 11) || (y == 12)) ||
			  (((72 <= x) && (x <= 77)) && (y == 13)))) ||
			 ((((50 <= x) && (x <= 52)) && ((1 <= y) && (y <= 3))) ||	// black smith
			  (((58 <= x) && (x <= 60)) && ((1 <= y) && (y <= 3))) ||
			  (((53 <= x) && (x <= 57)) && (y == 3)) ||
			  (((54 <= x) && (x <= 56)) && (y == 4)) ||
			  (((54 <= x) && (x <= 55)) && (y == 5))))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "   ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 39) && (y == 10))		// from here it's all about the detail when going through the door
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(3,0);
		cout << " ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 40) && (y == 10))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(3,0);
		cout << " ";
		cor(2,0);
		cout << " ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 41) && (y == 10))
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(2,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 39) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "   ";
	}
	else if ((x == 40) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(2,0);
		cout << " ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 41) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(2,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((((32 <= x) && (x <= 34)) && (y == 10)) ||		// this is about the house interactive black ground
			 (((76 <= x) && (x <= 77)) && (y == 10)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "   ";
		cor(0,7);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((((x == 33) || (x == 77)) && (y == 11)) ||
			 ((x == 55) && (y ==1)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << "   ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((((x == 33) || (x == 77)) && (y == 12)) ||
			 ((x == 55) && (y == 2)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << "   ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 34) && (y == 11) || ((x == 56) && (y == 1)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << "  ";
		cor(6,0);
		cout << " ";
		cor(0,7);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 34) && (y == 12) || ((x == 56) && (y == 2)))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << "  ";
		cor(6,0);
		cout << " ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if (((x == 35) && ((y == 11) || (y == 12))) ||
			 ((x == 57) && ((y == 1) || (y == 2))))
	{
		cor(0,7);
		gotoxy(x-1,y);
		cout << " ";
		cor(6,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 54) && (y == 1) || ((x == 76) && (y == 11)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(0,7);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 54) && (y == 2) || ((x == 76) && (y == 12)))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(0,7);
		cout << "  ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if (((x == 53) && ((y == 1) || (y == 2))) ||
			 ((x == 75) && ((y == 11) || (y == 12))))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(0,7);
		cout << " ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 69) && (y == 10))		// from here it's all about the detail when going through the east door
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(3,0);
		cout << " ";
		cor(2,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 69) && (y == 11))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(6,0);
		cout << " ";
	}
	else if ((x == 70) && (y == 10))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(3,0);
		cout << " ";
		cor(6,0);
		cout << " ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 70) && (y == 11))
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(6,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 71) && (y == 10))
	{
		cor(3,0);
		gotoxy(x-1,y);
		cout << " ";
		cor(6,0);
		cout << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 71) && (y == 11))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << " " << "  ";
		gotoxy(x,y+1);
		cout << " ";
	}
	else if ((x == 56) && (y == 5))		// from here it's all about the detail when going through the east door
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "  ";
		cor(8,0);
		cout << " ";
		cor(6,0);
		gotoxy(x,y+1);
		cout << " ";
	}
	else if (((54 <= x) && (x <= 56)) && (y == 6))
	{
		cor(6,0);
		gotoxy(x-1,y);
		cout << "   ";
		cor(2,0);
		gotoxy(x,y+1);
		cout << " ";
	}

	// REGULAR GRASS (default for the rest of the terrain)
	else
	{
		cor(2,0);
		gotoxy(x-1,y);
		cout << "   " << endl;
		gotoxy(x,y+1);
		cout << " ";
	}
}

//====================================================//

// DISPLAY A BAR, GIVEN %, color 1 and color 2
void make_percentage_bar (int percentage, int progress_color, int background_color)
{
	// a barra tem comprimento 24!

	int color1;
	int i = 1;

	color1 = percentage * 24 / 100;

	cor(progress_color,progress_color);
	while (i <= color1)
	{
		cout << " ";
		i++;
	}

	cor(background_color,background_color);
	while(i <= 24)
	{
		cout << " ";
		i++;
	}
}

//====================================================//

void exit_without_saving ()
{
	int x, y, background, texto;
	x = 39;
	y = 11;
	background = 12;
	texto = 0;

	draw_rectangle(x-2, y-1, x+33, y+3, 15);
	draw_rectangle(x, y, x+31, y+2, background);

	cor(background, texto);
	gotoxy(x+2, y+1);
	cout << "Exit without saving? (Y/N) ";
}

void leveled_up_dialog ()
{
	int x, y, background, texto;
	x = 40;
	y = 11;
	background = 10;
	texto = 0;

	draw_rectangle(x-2, y-1, x+31, y+3, 15);
	draw_rectangle(x, y, x+29, y+2, background);

	cor(background, texto);
	gotoxy(x+2, y+1);
	cout << "Congratulations! Level up! ";

	int input;
	input = _getch();
}

void success ()
{
	int x, y, background, texto, input;
	x = 49;
	y = 11;
	background = 10;
	texto = 0;

	draw_rectangle(x-2, y-1, x+13, y+3, 15);
	draw_rectangle(x, y, x+11, y+2, background);

	cor(background, texto);
	gotoxy(x+2, y+1);
	cout << "Success! ";
	
	input = _getch();
}

void already_bought ()
{
	int x, y, background, texto, input;
	x = 43;
	y = 11;
	background = 12;
	texto = 0;

	draw_rectangle(x-2, y-1, x+25, y+3, 15);
	draw_rectangle(x, y, x+23, y+2, background);

	cor(background, texto);
	gotoxy(x+2, y+1);
	cout << "Already bought this! ";
	
	input = _getch();
}

void not_enough_money ()
{
	int x, y, background, texto, input;
	x = 39;
	y = 11;
	background = 12;
	texto = 0;

	draw_rectangle(x-2, y-1, x+33, y+3, 15);
	draw_rectangle(x, y, x+31, y+2, background);

	cor(background, texto);
	gotoxy(x+2, y+1);
	cout << "You don't have enough money! ";

	input = _getch();
}

// ---------- INTERACTIVE FLOORS GAME MODE ---------- //

// BLACK SMITH
Player black_smith (Player player)
{
	string raw_user_input;
	int user_input;

	draw_rectangle(38,6,71,17,15);
	draw_rectangle(40,7,69,16,9);
	cor(9,15);
	gotoxy(42,8);
	cout << "Welcome to the Black Smith";
	gotoxy(45, 10);
	cout << "1. Weapons";
	gotoxy(45, 11);
	cout << "2. Shields";
	gotoxy(45, 13);
	cor(9,12);
	cout << "3. Cancel";
	gotoxy(42,15);
	cor(9,14);
	cout << "What do you want to buy? ";
	raw_user_input = _getch();
	user_input = atoi(raw_user_input.c_str());  // Converte string em numero

	switch (user_input)
	{
		// WEAPONS
	case 1:
		{
			int user_input2;

			int lolx, loly;
			lolx = 44;
			loly = 4;

			draw_rectangle(lolx-6,loly-2,lolx+27,loly+17,15);	// white limits
			draw_rectangle(lolx-4,loly-1,lolx+25,loly+16,9);	// blue background

			cor(9,15);
			gotoxy(lolx-2,loly);
			cout << "Weapons:";
			gotoxy(lolx+19,loly+1);
			cout << "Price";
			gotoxy(lolx, loly+2);
			cout << "a. Knife..............20";
			gotoxy(lolx, loly+3);
			cout << "b. Sword.............100";
			gotoxy(lolx, loly+4);
			cout << "c. Double Axe........300";
			gotoxy(lolx, loly+5);
			cout << "d. Spear.............600";
			gotoxy(lolx, loly+6);
			cout << "e. Giant Sword......1000";
			gotoxy(lolx, loly+7);
			cout << "f. Royal Axe........1500";
			gotoxy(lolx, loly+8);
			cout << "g. Royal Spear......2500";
			gotoxy(lolx, loly+9);
			cout << "h. Fire Sword.......3800";
			gotoxy(lolx, loly+10);
			cout << "i. Fire Axe.........5000";
			gotoxy(lolx, loly+11);
			cout << "j. Assassin Stars...9000";
			gotoxy(lolx, loly+13);
			cor(9,12);
			cout << "x. Cancel";
			gotoxy(lolx-2,loly+15);
			cor(9,14);
			cout << "Buy: ";
			user_input2 = _getch();

			switch (user_input2)
			{
				// knife
			case 'a':
				if (player.weapon >= 1)
				{
					already_bought();
				}
				else if (player.gold < 20)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 1;
					player.gold -= 20;
					success();
				}
				break;
				break;

				// sword
			case 'b':
				if (player.weapon >= 2)
				{
					already_bought();
				}
				else if (player.gold < 100)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 2;
					player.gold -= 100;
					success();
				}
				break;
				break;

				// double axe
			case 'c':
				if (player.weapon >= 3)
				{
					already_bought();
				}
				else if (player.gold < 300)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 3;
					player.gold -= 300;
					success();
				}
				break;
				break;

				// spear
			case 'd':
				if (player.weapon >= 4)
				{
					already_bought();
				}
				else if (player.gold < 600)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 4;
					player.gold -= 600;
					success();
				}
				break;
				break;

				// giant sword
			case 'e':
				if (player.weapon >= 5)
				{
					already_bought();
				}
				else if (player.gold < 1000)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 5;
					player.gold -= 1000;
					success();
				}
				break;
				break;

				// royal axe
			case 'f':
				if (player.weapon >= 6)
				{
					already_bought();
				}
				else if (player.gold < 1500)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 6;
					player.gold -= 1500;
					success();
				}
				break;
				break;

				// royal spear
			case 'g':
				if (player.weapon >= 7)
				{
					already_bought();
				}
				else if (player.gold < 2500)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 7;
					player.gold -= 2500;
					success();
				}
				break;
				break;

				// fire sword
			case 'h':
				if (player.weapon >= 8)
				{
					already_bought();
				}
				else if (player.gold < 3800)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 8;
					player.gold -= 3800;
					success();
				}
				break;
				break;

				// fire axe
			case 'i':
				if (player.weapon >= 9)
				{
					already_bought();
				}
				else if (player.gold < 5000)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 9;
					player.gold -= 5000;
					success();
				}
				break;
				break;

				// god spear
			case 'j':
				if (player.weapon >= 10)
				{
					already_bought();
				}
				else if (player.gold < 9000)
				{
					not_enough_money();
				}
				else
				{
					player.weapon = 10;
					player.gold -= 9000;
					success();
				}
				break;
				break;

				// back and strange char
			default:
				return (black_smith(player));
				break;
			}
			break;
		}

		// SHIELDS
	case 2:
		{
			int user_input3;

			int lolx, loly;
			lolx = 43;
			loly = 6;

			draw_rectangle(lolx-6,loly-2,lolx+30,loly+13,15);	// white limits
			draw_rectangle(lolx-4,loly-1,lolx+28,loly+12,9);	// blue background

			cor(9,15);
			gotoxy(lolx-2,loly);
			cout << "Shields:";
			gotoxy(lolx+21,loly+1);
			cout << "Price";
			gotoxy(lolx, loly+2);
			cout << "a. Wooden Shield......100";
			gotoxy(lolx, loly+3);
			cout << "b. Copper Shield......500";
			gotoxy(lolx, loly+4);
			cout << "c. Dwarven Shield....1500";
			gotoxy(lolx, loly+5);
			cout << "d. Guardian Shield...4000";
			gotoxy(lolx, loly+6);
			cout << "e. Demon Shield......7000";
			gotoxy(lolx, loly+7);
			cout << "f. Blessed Shield...12000";
			gotoxy(lolx, loly+9);
			cor(9,12);
			cout << "x. Cancel";
			gotoxy(lolx-2,loly+11);
			cor(9,14);
			cout << "Buy: ";
			user_input3 = _getch();

			switch (user_input3)
			{
				// wooden shield
			case 'a':
				if (player.shield >= 1)
				{
					already_bought();
				}
				else if (player.gold < 100)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 1;
					player.gold -= 100;
					success();
				}
				break;
				break;

				// copper shield
			case 'b':
				if (player.shield >= 2)
				{
					already_bought();
				}
				else if (player.gold < 500)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 2;
					player.gold -= 500;
					success();
				}
				break;
				break;

				// dwarven shield
			case 'c':
				if (player.shield >= 3)
				{
					already_bought();
				}
				else if (player.gold < 1500)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 3;
					player.gold -= 1500;
					success();
				}
				break;
				break;

				// guardian shield
			case 'd':
				if (player.shield >= 4)
				{
					already_bought();
				}
				else if (player.gold < 4000)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 4;
					player.gold -= 4000;
					success();
				}
				break;
				break;

				// demon shield
			case 'e':
				if (player.shield >= 5)
				{
					already_bought();
				}
				else if (player.gold < 7000)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 5;
					player.gold -= 7000;
					success();
				}
				break;
				break;

				// blessed shield
			case 'f':
				if (player.shield >= 6)
				{
					already_bought();
				}
				else if (player.gold < 12000)
				{
					not_enough_money();
				}
				else
				{
					player.shield = 6;
					player.gold -= 12000;
					success();
				}
				break;
				break;
				
				// back and strange char
			default:
				return (black_smith(player));
				break;
			}
			break;
		}

		// CANCEL
	case 3:
		break;

		// STRANGE CHAR
	default:
		return (black_smith(player));
	}
	
	cor(0,15);
	display_game_board(player);
	refresh_left_panel(player);
	refresh_right_panel(player);
	return player;
}

// SPELLS
Player spells (Player player)
{
	string raw_user_input;
	int user_input;

	draw_rectangle(38,6,71,17,15);
	draw_rectangle(40,7,69,16,9);
	cor(9,15);
	gotoxy(42,8);
	cout << "Welcome to the Magic Shop";
	gotoxy(45, 10);
	cout << "1. Healing Spells";
	gotoxy(45, 11);
	cout << "2. Attack Spells";
	gotoxy(45, 13);
	cor(9,12);
	cout << "3. Cancel";
	gotoxy(42,15);
	cor(9,14);
	cout << "What do you want to buy? ";
	raw_user_input = _getch();
	user_input = atoi(raw_user_input.c_str());  // Converte string em numero

	switch (user_input)
	{
		// HEALING SPELLS
	case 1:
		{
			int user_input2;

			int lolx, loly;
			lolx = 44;
			loly = 8;

			draw_rectangle(lolx-6,loly-2,lolx+27,loly+10,15);	// white limits
			draw_rectangle(lolx-4,loly-1,lolx+25,loly+9,9);	// blue background

			cor(9,15);
			gotoxy(lolx-2,loly);
			cout << "Spells:";
			gotoxy(lolx+19,loly+1);
			cout << "Price";
			gotoxy(lolx, loly+2);
			cout << " 1. Exura...........300";
			gotoxy(lolx, loly+3);
			cout << " 2. Exura Gran.....3500";
			gotoxy(lolx, loly+4);
			cout << " 3. Exura San......8000";
			gotoxy(lolx, loly+6);
			cor(9,12);
			cout << " 4. Cancel";
			gotoxy(lolx-2,loly+8);
			cor(9,14);
			cout << "Buy: ";
			raw_user_input = _getch();
			user_input2 = atoi(raw_user_input.c_str());  // Converte string em numero

			switch (user_input2)
			{
				// exura
			case 1:
				if (player.heal_spell >= 1)
				{
					already_bought();
				}
				else if (player.gold < 300)
				{
					not_enough_money();
				}
				else
				{
					player.heal_spell = 1;
					player.gold -= 300;
					success();
				}
				break;
				break;

				// exura gran
			case 2:
				if (player.heal_spell >= 2)
				{
					already_bought();
				}
				else if (player.gold < 3500)
				{
					not_enough_money();
				}
				else
				{
					player.heal_spell = 2;
					player.gold -= 3500;
					success();
				}
				break;
				break;

				// exura san
			case 3:
				if (player.heal_spell >= 3)
				{
					already_bought();
				}
				else if (player.gold < 8000)
				{
					not_enough_money();
				}
				else
				{
					player.heal_spell = 3;
					player.gold -= 8000;
					success();
				}
				break;
				break;

				// back and strange char
			default:
				return (spells(player));
				break;
			}
			break;
		}

		// ATTACK SPELLS
	case 2:
		{
			int user_input3;

			int lolx, loly;
			lolx = 44;
			loly = 8;

			draw_rectangle(lolx-6,loly-2,lolx+27,loly+10,15);	// white limits
			draw_rectangle(lolx-4,loly-1,lolx+25,loly+9,9);	// blue background

			cor(9,15);
			gotoxy(lolx-2,loly);
			cout << "Spells:";
			gotoxy(lolx+19,loly+1);
			cout << "Price";
			gotoxy(lolx, loly+2);
			cout << " 1. Exori...........700";
			gotoxy(lolx, loly+3);
			cout << " 2. Exori Gran.....4300";
			gotoxy(lolx, loly+4);
			cout << " 3. Adori Mort.....9000";
			gotoxy(lolx, loly+6);
			cor(9,12);
			cout << " 4. Cancel";
			gotoxy(lolx-2,loly+8);
			cor(9,14);
			cout << "Buy: ";
			raw_user_input = _getch();
			user_input3 = atoi(raw_user_input.c_str());  // Converte string em numero

			switch (user_input3)
			{
				// exori
			case 1:
				if (player.attack_spell >= 1)
				{
					already_bought();
				}
				else if (player.gold < 700)
				{
					not_enough_money();
				}
				else
				{
					player.attack_spell = 1;
					player.gold -= 700;
					success();
				}
				break;
				break;

				// exori gran
			case 2:
				if (player.attack_spell >= 2)
				{
					already_bought();
				}
				else if (player.gold < 4300)
				{
					not_enough_money();
				}
				else
				{
					player.attack_spell = 2;
					player.gold -= 4300;
					success();
				}
				break;
				break;

				// adori mort
			case 3:
				if (player.attack_spell >= 3)
				{
					already_bought();
				}
				else if (player.gold < 9000)
				{
					not_enough_money();
				}
				else
				{
					player.attack_spell = 3;
					player.gold -= 9000;
					success();
				}
				break;
				break;

				// back and strange char
			default:
				return (spells(player));
				break;
			}
			break;
		}

		// CANCEL
	case 3:
		break;

		// STRANGE CHAR
	default:
		return (spells(player));
	}
	
	cor(0,15);
	display_game_board(player);
	refresh_left_panel(player);
	refresh_right_panel(player);
	return player;
}

// FIGHTING AUX
Player fighting_aux (Player player, Monster monster, Weapon weapon, Shield shield, Heal_Spell heal_spell, Attack_Spell attack_spell)
{
	string raw_user_input;
	int user_input;

	bool k = false;
	while (k == false)
	{
		gotoxy(38,9);
		cout << "Choose what to do...                  ";
		gotoxy(59,9);
		raw_user_input = _getche();
		user_input = atoi(raw_user_input.c_str());  // Converte string em numero

		switch (user_input)
		{
			// ATTACKING
		case 1:
			{
				k = true;

				gotoxy(36,11);
				cout << "                                      ";	// don't mess with the spaces

				int deal, loose;

				deal = random_number(weapon.min_atk,weapon.max_atk);	// calculate attack

				monster.hp -= deal;
				gotoxy(40,5);
				cout << "Life: "; make_percentage_bar ((monster.hp * 100 / monster.max_hp),10,2);
				cor(0,15);
				gotoxy(36,13);
				cout << "You deal " << deal << " to the " << monster.name << ".";
				gotoxy(36,15);
				pressenter();
				cor(0,15);
				gotoxy(36,13);
				cout << "                                           ";	// don't mess with the spaces
				gotoxy(36,15);
				cout << "                                           ";	// don't mess with the spaces

				// IF MONSTER IS DEAD
				if (monster.hp <= 0)
				{
					int loot = random_number(monster.min_loot,monster.max_loot);

					player.gold += loot;
					player.exp += monster.exp;

					player.hp = player.max_hp;
					player.mp = player.max_mp;

					player = refresh_player_variables(player);
					refresh_left_panel (player);

					draw_rectangle(36,10,78,15,0);
					cor(0,15);

					gotoxy(36,13);
					cout << "You win! You gain " <<	loot << " gold.";
					gotoxy(36,15);
					pressenter();
					cor(0,15);
					return player;
					break;
				}
				else
				{
					loose = random_number(monster.min_atk,monster.max_atk);	// calculate attack

					loose -= shield.def; // defesa do escudo
					if (loose < 0)
					{
						loose = 0;
					}

					player.hp -= loose;

					if (player.hp <= 0)
					{
						player.hp = 0;
					}

					player = refresh_player_variables(player);
					refresh_left_panel(player);

					cor(0,15);
					gotoxy(36,13);
					cout << "You lose " << loose << " hitpoints.";
					gotoxy(36,15);
					pressenter();
					cor(0,15);
					gotoxy(36,13);
					cout << "                                           ";	// don't mess with the spaces
					gotoxy(36,15);
					cout << "                                           ";	// don't mess with the spaces

					// ALAS! BRAVE ADVNTRURER!
					if (player.hp == 0)
					{
						cor(0,15);
						gotoxy(4,17);
						cout << "Gold:          ";
						player.gold = (int) (player.gold / 2);

						draw_rectangle(31,1,78,23,15);
						gotoxy(37,10);
						cor(15,0);
						cout << "ALAS! BRAVE ADVENTURER, YOU ARE DEAD!";
						gotoxy(42,13);
						cout << "Press any key to continue... ";
						int input;
						input = _getch();

						player.hp = player.max_hp;
						player.mp = player.max_mp;

						player = refresh_player_variables (player);
						refresh_left_panel (player);

						cor(0,15);
						return player;
						break;
					}
					else
					{
						return (fighting_aux (player,monster,weapon,shield,heal_spell,attack_spell));
						break;
					}
				}
			}

			// ------------ USING SPELLS ------------ //
		case 2:
			{
				int spell_used; // controls the type of spell used
				int deal, heal, loose;

				gotoxy(36,11);
				cout << "                                      ";	// don't mess with the spaces

				cor(0,15);
				gotoxy(38,9);
				cout << "Casting a spell...                  ";
				gotoxy(40,11);
				cout << "1. Attack" << "\t" << "2. Heal";

				// waiting for user input
				bool r = false;
				while (r == false)
				{
					gotoxy(38,12);
					cout << "Choose spell type:                  ";
					gotoxy(57,12);
					raw_user_input = _getche();
					user_input = atoi(raw_user_input.c_str());  // Converte string em numero

					switch (user_input)
					{
						// attack
					case 1:
						{
							spell_used = 1;

							// if player hasn't bought any attack spell
							if (player.attack_spell == 0)
							{
								r = true;
								gotoxy(38,14);
								cout << "You haven't bought any spell!";
								gotoxy(38,15);
								pressenter();
								cor(0,15);
								break;
								break;
								break;
								break;
							}
							else
							{
								r = true;
								deal = random_number(attack_spell.min_atk,attack_spell.max_atk);
								break;
							}
						}

						// heal
					case 2:
						{
							spell_used = 2;

							// if player hasn't bought any heal spell
							if (player.heal_spell == 0)
							{
								r = true;
								gotoxy(38,14);
								cout << "You haven't bought any spell!";
								gotoxy(38,15);
								pressenter();
								cor(0,15);
								break;
								break;
								break;
								break;
							}
							else
							{
								r = true;
								heal = random_number(heal_spell.min_heal,heal_spell.max_heal);
								break;
							}
						}

						// back
					default:
						break;
					}
				}

				draw_rectangle(34,10,77,16,0);
				cor(0,15);

				// IF ATTACKING
				if ((spell_used == 1) && (player.mp >= attack_spell.mana))
				{
					if (player.attack_spell != 0)
					{
						monster.hp -= deal;
						player.mp -= attack_spell.mana;
						
						// refreshing left panel
						player = refresh_player_variables(player);
						refresh_left_panel(player);

						gotoxy(40,5);
						cout << "Life: "; make_percentage_bar ((monster.hp * 100 / monster.max_hp),10,2);
						cor(0,15);
						gotoxy(36,13);
						cout << "You deal " << deal << " to the " << monster.name << ".";
						gotoxy(36,15);
						pressenter();
						cor(0,15);
						gotoxy(36,13);
						cout << "                                           ";	// don't mess with the spaces
						gotoxy(36,15);
						cout << "                                           ";	// don't mess with the spaces

						// IF MONSTER IS DEAD
						if (monster.hp <= 0)
						{
							int loot = random_number(monster.min_loot,monster.max_loot);

							player.gold += loot;
							player.exp += monster.exp;

							player.hp = player.max_hp;
							player.mp = player.max_mp;

							player = refresh_player_variables(player);
							refresh_left_panel (player);

							draw_rectangle(36,10,78,15,0);
							cor(0,15);

							gotoxy(36,13);
							cout << "You win! You gain " <<	loot << " gold.";
							gotoxy(36,15);
							pressenter();
							cor(0,15);
							return player;
							break;
						}
						else
						{
							loose = random_number(monster.min_atk,monster.max_atk);	// calculate attack

							loose -= shield.def; // defesa do escudo
							if (loose < 0)
							{
								loose = 0;
							}

							player.hp -= loose;

							if (player.hp <= 0)
							{
								player.hp = 0;
							}

							player = refresh_player_variables(player);
							refresh_left_panel(player);

							cor(0,15);
							gotoxy(36,13);
							cout << "You lose " << loose << " hitpoints.";
							gotoxy(36,15);
							pressenter();
							cor(0,15);
							gotoxy(36,13);
							cout << "                                           ";	// don't mess with the spaces
							gotoxy(36,15);
							cout << "                                           ";	// don't mess with the spaces

							// ALAS! BRAVE ADVNTRURER!
							if (player.hp == 0)
							{
								cor(0,15);
								gotoxy(4,17);
								cout << "Gold:          ";
								player.gold = (int) (player.gold / 2);

								draw_rectangle(31,1,78,23,15);
								gotoxy(37,10);
								cor(15,0);
								cout << "ALAS! BRAVE ADVENTURER, YOU ARE DEAD!";
								gotoxy(42,13);
								cout << "Press any key to continue... ";
								int input;
								input = _getch();

								player.hp = player.max_hp;
								player.mp = player.max_mp;

								player = refresh_player_variables (player);
								refresh_left_panel (player);

								cor(0,15);
								return player;
								break;
							}
							else
							{
								return (fighting_aux (player,monster,weapon,shield,heal_spell,attack_spell));
								break;
							}
						}
					}
				}

				// IF HEALING
				else if ((spell_used == 2) && (player.mp >= heal_spell.mana))
				{
					if (player.heal_spell != 0)
					{
						// HEALING PLAYER
						player.hp += heal;
						player.mp -= heal_spell.mana;
						if (player.hp > player.max_hp)
						{
							player.hp = player.max_hp;
						}

						// refreshing left panel
						player = refresh_player_variables(player);
						refresh_left_panel(player);

						cor(0,15);
						gotoxy(36,13);
						cout << "You heal " << heal << " hitpoints.";
						gotoxy(36,15);
						pressenter();
						cor(0,15);
						gotoxy(36,13);
						cout << "                                           ";	// don't mess with the spaces
						gotoxy(36,15);
						cout << "                                           ";	// don't mess with the spaces

						loose = random_number(monster.min_atk,monster.max_atk);	// calculate attack

						loose -= shield.def; // defesa do escudo
						if (loose < 0)
						{
							loose = 0;
						}

						player.hp -= loose;

						if (player.hp <= 0)
						{
							player.hp = 0;
						}

						// refreshing left panel
						player = refresh_player_variables(player);
						refresh_left_panel(player);

						cor(0,15);
						gotoxy(36,13);
						cout << "You lose " << loose << " hitpoints.";
						gotoxy(36,15);
						pressenter();
						cor(0,15);
						gotoxy(36,13);
						cout << "                                           ";	// don't mess with the spaces
						gotoxy(36,15);
						cout << "                                           ";	// don't mess with the spaces

						// ALAS! BRAVE ADVNTRURER!
						if (player.hp == 0)
						{
							cor(0,15);
							gotoxy(4,17);
							cout << "Gold:          ";
							player.gold = (int) (player.gold / 2);

							draw_rectangle(31,1,78,23,15);
							gotoxy(37,10);
							cor(15,0);
							cout << "ALAS! BRAVE ADVENTURER, YOU ARE DEAD!";
							gotoxy(42,13);
							cout << "Press any key to continue... ";
							int input;
							input = _getch();

							player.hp = player.max_hp;
							player.mp = player.max_mp;

							player = refresh_player_variables (player);
							refresh_left_panel (player);

							cor(0,15);
							return player;
							break;
							break;
						}
					}
				}
				else
				{
					cor(0,15);
					gotoxy(36,13);
					cout << "You don't have enough mana!";
					gotoxy(36,15);
					pressenter();
					cor(0,15);
					gotoxy(36,13);
					cout << "                                           ";	// don't mess with the spaces
					gotoxy(36,15);
					cout << "                                           ";	// don't mess with the spaces
				}
				break;
			}

			// else
		default:
			{
				gotoxy(36,11);
				cor(4,15);
				cout << " Choose one of the available options! ";
				cor(0,15);
			}
		}
	}
	return player;
}

// FIGHTING FUNCTION
Player fighting (Player player, Monster monster, Weapon weapon, Shield shield, Heal_Spell heal_spell, Attack_Spell attack_spell)
{
	draw_rectangle(32,1,78,17,0);	// black background

	cor(0,15);
	gotoxy(34,2);
	cout << "You are fighting a " << monster.name << ".";

	gotoxy(40,5);
	cout << "Life: "; make_percentage_bar ((monster.hp * 100 / monster.max_hp),10,2);
	cor(0,15);

	gotoxy(40,7);
	cout << "1. Attack" << "\t" << "2. Use Spell";

	return (fighting_aux (player,monster,weapon,shield,heal_spell,attack_spell));
}

// DUNGEON
Player dungeon (Player player)
{
	// assigning correct values to weapon
	Weapon weapon;
	if (player.weapon == 0)
	{
		weapon.min_atk = 0;
		weapon.max_atk = 0;
	}
	else if (player.weapon == 1)
	{
		weapon.min_atk = 1;
		weapon.max_atk = 2;
	}
	else if (player.weapon == 2)
	{
		weapon.min_atk = 2;
		weapon.max_atk = 5;
	}
	else if (player.weapon == 3)
	{
		weapon.min_atk = 4;
		weapon.max_atk = 9;
	}
	else if (player.weapon == 4)
	{
		weapon.min_atk = 5;
		weapon.max_atk = 14;
	}
	else if (player.weapon == 5)
	{
		weapon.min_atk = 10;
		weapon.max_atk = 30;
	}
	else if (player.weapon == 6)
	{
		weapon.min_atk = 25;
		weapon.max_atk = 45;
	}
	else if (player.weapon == 7)
	{
		weapon.min_atk = 30;
		weapon.max_atk = 60;
	}
	else if (player.weapon == 8)
	{
		weapon.min_atk = 45;
		weapon.max_atk = 75;
	}
	else if (player.weapon == 9)
	{
		weapon.min_atk = 60;
		weapon.max_atk = 90;
	}
	else if (player.weapon == 10)
	{
		weapon.min_atk = 85;
		weapon.max_atk = 125;
	}

	// assigning correct values to weapon
	Shield shield;
	if (player.shield == 0)
	{
		shield.def = 0;
	}
	else if (player.shield == 1)
	{
		shield.def = 6;
	}
	else if (player.shield == 2)
	{
		shield.def = 15;
	}
	else if (player.shield == 3)
	{
		shield.def = 30;
	}
	else if (player.shield == 4)
	{
		shield.def = 50;
	}
	else if (player.shield == 5)
	{
		shield.def = 80;
	}
	else if (player.shield == 6)
	{
		shield.def = 120;
	}

	// assigning correct values to heal_spell
	Heal_Spell heal_spell;
	if (player.heal_spell == 0)
	{
		heal_spell.min_heal = 0;
		heal_spell.max_heal = 0;
		heal_spell.mana = 0;
	}
	else if (player.heal_spell == 1)
	{
		heal_spell.min_heal = 35;
		heal_spell.max_heal = 40;
		heal_spell.mana = 20;
	}
	else if (player.heal_spell == 2)
	{
		heal_spell.min_heal = 85;
		heal_spell.max_heal = 95;
		heal_spell.mana = 100;
	}
	else if (player.heal_spell == 3)
	{
		heal_spell.min_heal = 260;
		heal_spell.max_heal = 270;
		heal_spell.mana = 300;
	}

	// assigning correct values to attack_spell
	Attack_Spell attack_spell;
	if (player.attack_spell == 0)
	{
		attack_spell.min_atk = 0;
		attack_spell.max_atk = 0;
		attack_spell.mana = 0;
	}
	else if (player.attack_spell == 1)
	{
		attack_spell.min_atk = 45;
		attack_spell.max_atk = 50;
		attack_spell.mana = 50;
	}
	else if (player.attack_spell == 2)
	{
		attack_spell.min_atk = 95;
		attack_spell.max_atk = 105;
		attack_spell.mana = 150;
	}
	else if (player.attack_spell == 3)
	{
		attack_spell.min_atk = 230;
		attack_spell.max_atk = 280;
		attack_spell.mana = 400;
	}

	// CONTINUING
	draw_rectangle(32,1,78,17,0);	// black background
	draw_rectangle(48,2,62,4,4);	// title box
	
	cor(4,15);
	gotoxy(50,3);
	cout << "The Dungeon";
	
	cor(0,15);
	gotoxy(34,6);
	cout << "a. Rat" << "\t" << "f. Skeleton" << "\t" << "k. Bonelord";
	gotoxy(34,7);
	cout << "b. Bug" << "\t" << "g. Troll" << "\t" << "l. Cyclops";
	gotoxy(34,8);
	cout << "c. Wolf" << "\t" << "h. Orc" << "\t\t" << "m. Wyvern";
	gotoxy(34,9);
	cout << "d. Snake" << "\t" << "i. Minotaur" << "\t" << "n. Dragon";
	gotoxy(34,10);
	cout << "e. Spider" << "\t" << "j. Slime" << "\t" << "o. Demon";

	gotoxy(34,12);
	cout << "x. Cancel";

	gotoxy(34,15);
	cout << "Choose a monster: ";

	bool loop = true;
	while (loop == true)
	{
		char user_input;
		user_input = _getch();

		switch (user_input)
		{
			// RAT
		case 'a':
			{
				loop = false;
				Monster monster;
				monster.name = "Rat";
				monster.hp = 10;
				monster.max_hp = monster.hp;
				monster.min_atk = 1;
				monster.max_atk = 4;
				monster.min_loot = 0;
				monster.max_loot = 3;
				monster.exp = 2;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// BUG
		case 'b':
			{
				loop = false;
				Monster monster;
				monster.name = "Bug";
				monster.hp = 13;
				monster.max_hp = monster.hp;
				monster.min_atk = 2;
				monster.max_atk = 6;
				monster.min_loot = 2;
				monster.max_loot = 5;
				monster.exp = 3;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// WOLF
		case 'c':
			{
				loop = false;
				Monster monster;
				monster.name = "Wolf";
				monster.hp = 17;
				monster.max_hp = monster.hp;
				monster.min_atk = 4;
				monster.max_atk = 9;
				monster.min_loot = 3;
				monster.max_loot = 8;
				monster.exp = 5;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// SNAKE
		case 'd':
			{
				loop = false;
				Monster monster;
				monster.name = "Snake";
				monster.hp = 20;
				monster.max_hp = monster.hp;
				monster.min_atk = 6;
				monster.max_atk = 13;
				monster.min_loot = 5;
				monster.max_loot = 15;
				monster.exp = 7;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// SPIDER
		case 'e':
			{
				loop = false;
				Monster monster;
				monster.name = "Spider";
				monster.hp = 25;
				monster.max_hp = monster.hp;
				monster.min_atk = 6;
				monster.max_atk = 15;
				monster.min_loot = 5;
				monster.max_loot = 17;
				monster.exp = 10;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// SKELETON
		case 'f':
			{
				loop = false;
				Monster monster;
				monster.name = "Skeleton";
				monster.hp = 60;
				monster.max_hp = monster.hp;
				monster.min_atk = 13;
				monster.max_atk = 20;
				monster.min_loot = 10;
				monster.max_loot = 24;
				monster.exp = 20;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// TROLL
		case 'g':
			{
				loop = false;
				Monster monster;
				monster.name = "Troll";
				monster.hp = 80;
				monster.max_hp = monster.hp;
				monster.min_atk = 15;
				monster.max_atk = 22;
				monster.min_loot = 13;
				monster.max_loot = 25;
				monster.exp = 26;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// ORC
		case 'h':
			{
				loop = false;
				Monster monster;
				monster.name = "Orc";
				monster.hp = 90;
				monster.max_hp = monster.hp;
				monster.min_atk = 17;
				monster.max_atk = 28;
				monster.min_loot = 17;
				monster.max_loot = 22;
				monster.exp = 34;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// MINOTAUR
		case 'i':
			{
				loop = false;
				Monster monster;
				monster.name = "Minotaur";
				monster.hp = 100;
				monster.max_hp = monster.hp;
				monster.min_atk = 19;
				monster.max_atk = 36;
				monster.min_loot = 23;
				monster.max_loot = 45;
				monster.exp = 45;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// SLIME
		case 'j':
			{
				loop = false;
				Monster monster;
				monster.name = "Slime";
				monster.hp = 140;
				monster.max_hp = monster.hp;
				monster.min_atk = 22;
				monster.max_atk = 57;
				monster.min_loot = 26;
				monster.max_loot = 48;
				monster.exp = 50;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// BONELORD
		case 'k':
			{
				loop = false;
				Monster monster;
				monster.name = "Bonelord";
				monster.hp = 200;
				monster.max_hp = monster.hp;
				monster.min_atk = 42;
				monster.max_atk = 84;
				monster.min_loot = 30;
				monster.max_loot = 50;
				monster.exp = 70;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// CYCLOPS
		case 'l':
			{
				loop = false;
				Monster monster;
				monster.name = "Cyclops";
				monster.hp = 350;
				monster.max_hp = monster.hp;
				monster.min_atk = 50;
				monster.max_atk = 100;
				monster.min_loot = 42;
				monster.max_loot = 55;
				monster.exp = 100;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// WYVERN
		case 'm':
			{
				loop = false;
				Monster monster;
				monster.name = "Wyvern";
				monster.hp = 420;
				monster.max_hp = monster.hp;
				monster.min_atk = 70;
				monster.max_atk = 100;
				monster.min_loot = 42;
				monster.max_loot = 66;
				monster.exp = 190;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// DRAGON
		case 'n':
			{
				loop = false;
				Monster monster;
				monster.name = "Dragon";
				monster.hp = 900;
				monster.max_hp = monster.hp;
				monster.min_atk = 100;
				monster.max_atk = 200;
				monster.min_loot = 70;
				monster.max_loot = 90;
				monster.exp = 350;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// DEMON
		case 'o':
			{
				loop = false;
				Monster monster;
				monster.name = "Demon";
				monster.hp = 1200;
				monster.max_hp = monster.hp;
				monster.min_atk = 200;
				monster.max_atk = 250;
				monster.min_loot = 450;
				monster.max_loot = 550;
				monster.exp = 1500;
				return (fighting (player, monster, weapon, shield, heal_spell, attack_spell));
				break;
			}

			// cancel
		case 'x':
			{
				loop = false;
				return player;
				break;
			}

			// else
		default:
			{
				break;
			}
		}
	}
	return player;
}

//====================================================//

// REFRESHING FUNCTIONS

// OPTIONS MENU DISPLAY
void options_menu_display (int x, int y, Player player)
{
	string raw_user_input;
	int user_input;

	draw_rectangle(48,7,62,17,15);
	draw_rectangle(50,8,60,16,0);
	cor(0,15);
	gotoxy(52,9);
	cout << "OPTIONS";
	gotoxy(51,11);
	cout << "1. Resume";
	gotoxy(51,12);
	cout << "2. Save";
	gotoxy(51,13);
	cout << "3. Exit";
	gotoxy(51,15);
	cout << "Option: ";

	bool loop = true;
	while (loop == true)
	{
		raw_user_input = _getch();
		user_input = atoi(raw_user_input.c_str());  // Converte string em numero

		switch (user_input)
		{
		case 1:
			loop = false;
			refresh_right_panel(player);
			game_interface(x, y, player);
			break;

		case 2:
			loop = false;
			if (player.exp == 0)
			{
				player.exp = 5;
			}
			save_game(player,1);
			refresh_left_panel(player);
			refresh_right_panel(player);
			game_interface(x, y, player);
			break;

		case 3:
			{
				loop = false;
				exit_without_saving();

				bool k = false;
				while (k == false)
				{
					char continuar;
					continuar = _getch();

					switch (continuar)
					{
					case 'Y':
					case 'y':
						k = true;
						break;

					case 'N':
					case 'n':
						k = true;
						save_game(player,1);
						break;

					default:
						refresh_right_panel(player);
						game_interface(x, y, player);
						break;
					}
				}
			}

		default:
			break;
		}
	}
}

// REFRESH PLAYER VARIABLES
Player refresh_player_variables (Player player)
{
	// life parameters
	int life1 = 30;				// life at level 1
	int life_increase = 5;		// life increase at each level
	
	// life parameters
	int mana1 = 10;				// life at level 1
	int mana_increase = 5;		// life increase at each level
	
	// LEVEL UP DIALOG
	if (player.exp >= player.max_exp)
	{
		leveled_up_dialog();
		player.hp = player.max_hp = life1 + (player.level * life_increase);
		player.mp = player.max_mp = mana1 + (player.level * mana_increase);
	}

	// CONTINUING
	int exp = player.exp;

	if ((0 <= exp) && (exp < 10))
	{
		player.level = 1;
		player.min_exp = 0;
		player.max_exp = 10;
	}
	else if ((10 <= exp) && (exp < 150))
	{
		player.level = 1 + ((exp + 10) / 20);
		player.min_exp = 10 + (20 * ((exp-10) / 20));
		player.max_exp = 30 + (20 * ((exp-10) / 20));
	}
	else if ((150 <= exp) && (exp < 300))
	{
		player.level = 4 + (exp / 30);
		player.min_exp = (30 * (exp / 30));
		player.max_exp = 30 + (30 * (exp / 30));
	}
	else if ((300 <= exp) && (exp < 500))
	{
		player.level = 7 + ((exp-20) / 40);
		player.min_exp = 20 + (40 * ((exp-20) / 40));
		player.max_exp = 60 + (40 * ((exp-20) / 40));
	}
	else if ((500 <= exp) && (exp < 600))
	{
		player.level = 9 + (exp / 50);
		player.min_exp = (50 * (exp / 50));
		player.max_exp = 50 + (50 * (exp / 50));
	}
	else if ((600 <= exp) && (exp < 720))
	{
		player.level = 11 + (exp / 60);
		player.min_exp = (60 * (exp / 60));
		player.max_exp = 60 + (60 * (exp / 60));
	}
	else if ((720 <= exp) && (exp < 800))
	{
		player.level = 14 + (exp / 80);
		player.min_exp = (80 * (exp / 80));
		player.max_exp = 80 + (80 * (exp / 80));
	}
	else if ((800 <= exp) && (exp < 900))
	{
		player.level = 16 + (exp / 100);
		player.min_exp = (100 * (exp / 100));
		player.max_exp = 100 + (100 * (exp / 100));
	}
	else if ((900 <= exp) && (exp < 2100))
	{
		player.level = 19 + (exp / 150);
		player.min_exp = (150 * (exp / 150));
		player.max_exp = 150 + (150 * (exp / 150));
	}
	else if ((2100 <= exp) && (exp < 3000))
	{
		player.level = 26 + (exp / 300);
		player.min_exp = (300 * (exp / 300));
		player.max_exp = 300 + (300 * (exp / 300));
	}
	else if ((3000 <= exp) && (exp < 5000))
	{
		player.level = 30 + (exp / 500);
		player.min_exp = (500 * (exp / 500));
		player.max_exp = 500 + (500 * (exp / 500));
	}
	else if ((5000 <= exp) && (exp < 10000))
	{
		player.level = 35 + (exp / 1000);
		player.min_exp = (1000 * (exp / 1000));
		player.max_exp = 1000 + (1000 * (exp / 1000));
	}
	else if (10000 <= exp)
	{
		player.level = 42 + (exp / 3000);
		player.min_exp = (3000 * (exp / 3000));
		player.max_exp = 3000 + (3000 * (exp / 3000));
	}

	player.exp = exp;

	player.exp_percentage = 100 * (player.exp - player.min_exp) / (player.max_exp - player.min_exp);

	player.hp_percentage = 100 * player.hp / player.max_hp;

	player.mp_percentage = 100 * player.mp / player.max_mp;

	return player;
}

// REFRESH LEFT PANEL
void refresh_left_panel (Player player)
{
	save_game(player,0);

	// player profile
	cor(0,15);

	gotoxy(4,2);
	cout << player.name << "'s profile:";

	gotoxy(4,4);
	cout << "Level: " << player.level;
	gotoxy(4,5);
	make_percentage_bar(player.exp_percentage,12,4);
	cor(0,15);

	gotoxy(4,7);
	cout << "Experience: " << player.exp << "/" << player.max_exp;

	draw_horizontal_line(0,30,9,15);
	cor(0,15);

	gotoxy(4,11);
	cout << "                        ";
	gotoxy(4,11);
	cout << "Life: " << player.hp << "/" << player.max_hp;
	gotoxy(4,12);

	if (player.hp_percentage >= 50)
	{
		make_percentage_bar(player.hp_percentage,10,2);
	}
	else if ((20 <= player.hp_percentage) && (player.hp_percentage < 50))
	{
		make_percentage_bar(player.hp_percentage,14,6);
	}
	else if (player.hp_percentage < 20)
	{
		make_percentage_bar(player.hp_percentage,12,4);
	}

	cor(0,15);
	
	gotoxy(4,14);
	cout << "                        ";
	gotoxy(4,14);
	cout << "Mana: " << player.mp << "/" << player.max_mp;
	gotoxy(4,15);
	make_percentage_bar(player.mp_percentage,9,1);
	cor(0,15);

	gotoxy(4,17);
	cout << "Gold: " << player.gold;

	gotoxy(4,19);
	cout << "Weapon: ";
	switch (player.weapon)
	{
	case 0:
		cout << "None";
		break;
	case 1:
		cout << "Knife";
		break;
	case 2:
		cout << "Sword";
		break;
	case 3:
		cout << "Double Axe";
		break;
	case 4:
		cout << "Spear";
		break;
	case 5:
		cout << "Giant Sword";
		break;
	case 6:
		cout << "Royal Axe";
		break;
	case 7:
		cout << "Royal Spear";
		break;
	case 8:
		cout << "Fire Sword";
		break;
	case 9:
		cout << "Fire Axe";
		break;
	case 10:
		cout << "Assassin Stars";
		break;
	}

	gotoxy(4,20);
	cout << "Shield: ";
	switch (player.shield)
	{
	case 0:
		cout << "None";
		break;
	case 1:
		cout << "Wooden Shield";
		break;
	case 2:
		cout << "Copper Shield";
		break;
	case 3:
		cout << "Dwarven Shield";
		break;
	case 4:
		cout << "Guardian Shield";
		break;
	case 5:
		cout << "Demon Shield";
		break;
	case 6:
		cout << "Blessed Shield";
		break;
	}

	gotoxy(4,22);
	cout << "Press <ESC> for Options";

	cor(0,15);
}

// REFRESH RIGHT PANEL
void refresh_right_panel (Player player)
{
	// SCENERY COORDS: (32,1) - (78,23)

	draw_rectangle(32,1,78,23,2); // grass
	
	draw_rectangle(47,1,63,6,8); // weapon shop walls start

	draw_rectangle(49,1,61,4,6);	// north floor
	draw_rectangle(53,11,57,13,3);	// center floor
	draw_rectangle(70,8,78,16,6);	// east floor
	draw_rectangle(32,8,40,16,6);	// west floor

	// weapon shop
	draw_horizontal_line(47,47,6,2);	// bottom left corner grass detail
	draw_horizontal_line(52,56,5,6);	// door
	draw_horizontal_line(53,57,6,6);
	draw_vertical_line(1,4,62,3);		// east walls
	draw_vertical_line(1,5,63,3);
	draw_rectangle(47,1,48,4,3);		// west walls
	draw_rectangle(54,1,56,2,0);		// portal
	cor(8,0);
	gotoxy(58,5);
	cout << "BLACK";
	gotoxy(59,6);
	cout << "SMITH";

	// dungeon house
	draw_rectangle(32+38,7,38+40,8,8);		// north wall
	draw_vertical_line(13,15,40+30,3);		// east walls
	draw_vertical_line(12,14,39+30,3);
	draw_vertical_line(8,10,40+30,3);
	draw_vertical_line(7,9,39+30,3);
	draw_horizontal_line(32+38,40+38,16,8);	// south walls
	draw_horizontal_line(31+38,40+38,15,8);
	draw_rectangle(32+44,11,34+44,12,0);		//portal
	gotoxy(71,15);
	cor(8,0);
	cout << "DUNGEON";

	// magic shop
	draw_horizontal_line(32,40,16,8);	// south walls
	draw_horizontal_line(32,39,15,8);
	draw_vertical_line(13,15,40,3);		// east walls
	draw_vertical_line(12,14,39,3);
	draw_vertical_line(8,10,40,3);
	draw_vertical_line(7,9,39,3);
	draw_rectangle(32,7,38,8,8);		// north wall
	draw_rectangle(32,11,34,12,0);		//portal
	gotoxy(33,15);
	cor(8,0);
	cout << "SPELLS";
	
	// fences
	int fences_color = 7;
	draw_horizontal_line(40,47,6,fences_color);
	draw_horizontal_line(64,70,6,fences_color);
	draw_horizontal_line(40,40,7,fences_color);

	draw_rectangle(32,18,78,23,1); // messages board
	draw_horizontal_line(32,78,18,15);

	// MESSAGES BOARD
	cor(1,15);
	if (player.weapon == 0)
	{
		gotoxy(34,20);
		cout << "Welcome to Tibia " << player.name << "!";
		gotoxy(34,21);
		cout << "To start your adventure, go to the Black";
		gotoxy(34,22);
		cout << "Smith and buy a weapon. GO!";
	}
	else if ((player.weapon == 1) && (player.exp < 5))
	{
		gotoxy(34,20);
		cout << "Good job " << player.name << "!";
		gotoxy(34,21);
		cout << "You should save your progress. Press <Esc>";
		gotoxy(34,22);
		cout << "on your keyboard and select save.";
	}
	else if ((player.weapon == 1) && (player.exp == 5))
	{
		gotoxy(34,20);
		cout << "Great! You have just gained 5 experience";
		gotoxy(34,21);
		cout << "points! You should go to the Dungeon";
		gotoxy(34,22);
		cout << "to face your first monster: a Rat.";
		player = refresh_player_variables (player);
		refresh_left_panel(player);
	}
	else if ((player.weapon == 1) && (player.exp == 7))
	{
		gotoxy(34,20);
		cout << "Congratulations! You didn't die fighting";
		gotoxy(34,21);
		cout << "that Rat! I am impressed...";
		gotoxy(34,22);
		cout << "Now keep training. We'll talk soon.";
	}
	else if (player.level == 2)
	{
		gotoxy(34,20);
		cout << "Congratulations! You have just advanced";
		gotoxy(34,21);
		cout << "to level 2! You are on your way to";
		gotoxy(34,22);
		cout << "become a great warrior!";
	}
	else if (player.level == 3)
	{
		gotoxy(34,20);
		cout << "You have advanced to level 3!";
		gotoxy(34,21);
		cout << "If you have enough money, you should";
		gotoxy(34,22);
		cout << "buy a shield to protect you.";
	}
	else if ((player.shield == 0) && (player.gold >= 100))
	{
		gotoxy(34,20);
		cout << "You have enough money to buy a shield!";
		gotoxy(34,21);
		cout << "Go to the Black Smith and buy a Wooden";
		gotoxy(34,22);
		cout << "Shield. It will protect you in battle.";
	}
	else if ((player.shield != 0) && (player.gold >= 100) && (player.weapon == 1))
	{
		gotoxy(34,20);
		cout << "How is it going " << player.name << "?";
		gotoxy(34,21);
		cout << "Have you tried to buy a new weapon?";
		gotoxy(34,22);
		cout << "You will be able to deal more damage.";
	}
	else if ((player.heal_spell == 0) && (player.gold >= 300))
	{
		gotoxy(34,20);
		cout << "You have enough money to buy a spell!";
		gotoxy(34,21);
		cout << "Go to the Magic Shop and buy your first";
		gotoxy(34,22);
		cout << "healing spell: Exura.";
	}
	else if ((player.heal_spell == 1) && (player.level <= 30))
	{
		gotoxy(34,20);
		cout << "Great job! You have bought Exura. You";
		gotoxy(34,21);
		cout << "can now heal some HP during battles!";
		gotoxy(34,22);
		cout << "Make sure to use it wisely.";
	}
	else if ((player.attack_spell == 0) && (player.gold >= 700))
	{
		gotoxy(34,20);
		cout << "Hey! You have enough money to buy an";
		gotoxy(34,21);
		cout << "attack spell! Buy one! They are very";
		gotoxy(34,22);
		cout << "powerful.";
	}
	else if (player.level == 50)
	{
		gotoxy(34,20);
		cout << "You are so close to the final battle!";
		gotoxy(34,21);
		cout << "Keep going, you're just getting stronger";
		gotoxy(34,22);
		cout << "and stronger! Congratulations!";
	}
	else if ((player.weapon == 10) && (player.shield != 6))
	{
		gotoxy(34,20);
		cout << "Well done! You have just bought the best";
		gotoxy(34,21);
		cout << "weapon of the game! Now you will deal";
		gotoxy(34,22);
		cout << "damage to monsters like never before.";
	}
	else if ((player.weapon != 10) && (player.shield == 6))
	{
		gotoxy(34,20);
		cout << "Well done! You have just bought the best";
		gotoxy(34,21);
		cout << "shield of the game! Now you will absorb";
		gotoxy(34,22);
		cout << "damage from monsters like never before.";
	}
	else if ((player.weapon == 10) && (player.shield == 6))
	{
		gotoxy(34,20);
		cout << "Very, very well done! You now have the";
		gotoxy(34,21);
		cout << "best equipment of the entire game! Your";
		gotoxy(34,22);
		cout << "attack and defense rates are maximum!";
	}

	cor(0,15);
}

//====================================================//

// Game Interface (while playing)
void game_interface (int x, int y, Player player)
{
	int last_x, last_y;
	int new_x, new_y;

	output_player(x,y);

	int * player_position;

	for(;;)
	{
		// saving previous coordinates
		last_x = x;
		last_y = y;

		// gets new coordinates from keyboard
		player_position = moving_player(x, y);
		new_x = player_position[0];
		new_y = player_position[1];

		// assigns new coordinates
		if (((new_x >= 42 && new_x <= 67) && (new_y >= 7 && new_y <= 16)) ||	// main grass
			((new_x >= 38 && new_x <= 41) && (new_y >= 10 && new_y <= 11)) ||	// west door
			((new_x >= 33 && new_x <= 37) && (new_y >= 9 && new_y <= 13)) ||	// west floor
			((new_x >= 54 && new_x <= 56) && (new_y >= 4 && new_y <= 6)) ||	// north door
			((new_x >= 50 && new_x <= 60) && (new_y >= 1 && new_y <= 3)) ||	// north floor
			((new_x >= 68 && new_x <= 71) && (new_y >= 10 && new_y <= 11)) ||	// east door
			((new_x >= 72 && new_x <= 77) && (new_y >= 9 && new_y <= 13)))		// east floor
		{
			x = new_x;
			y = new_y;

			// INTERACTIVE FLOORS
			if ((x == 33) && (y == 11))			// SPELLS
			{
				player = spells(player);
			}
			else if ((x == 55) && (y == 1))		// BLACK SMITH
			{
				player = black_smith(player);
			}
			else if ((x == 77) && (y == 11))	// DUNGEON
			{
				player = dungeon(player);
				cor(0,15);

				save_game(player,0);
				cor(0,15);
				refresh_right_panel(player);
			}
		}
		else
		{
			x = last_x;
			y = last_y;
		}
		
		if (new_x == -1) // COMPLETE THIS!!!!
		{
			options_menu_display(x, y, player);
			break;
		}

		// cleaning last player position and outputing character to the new position
		clean_last_player_output(last_x,last_y);
		output_player(x,y);
	}
}

// GAME LIMITS DISPLAY
void display_game_board (Player player)
{
	int texto = 10;
	int limite_ecra = 15;

	system("cls");
	cor(0,15);
	
	// BOARD WHITE LIMITS
	draw_horizontal_line(0,79,0,15);		//linha de cima
	draw_horizontal_line(0,79,24,15);		// linha de baixo

	draw_vertical_line(0,24,0,15);			// linha da esquerda
	draw_vertical_line(0,24,1,15);			// negrito da linha da esquerda

	draw_vertical_line(0,24,30,15);			// linha do meio
	draw_vertical_line(0,24,31,15);			// negrito da linha do meio

	draw_vertical_line(0,24,78,15);			// linha da direita
	draw_vertical_line(0,23,79,15);			// negrito da linha da direita
}

// starting game and 1st display
void first_display_starting_game (Player player)
{
	player = refresh_player_variables(player);
	display_game_board(player);		// displaying limits
	refresh_left_panel(player);		// displaying left panel
	refresh_right_panel(player);	// displaying right panel
}

// Creating new character
string new_game ()
{
	system("cls");
	cor(0,10);

	string player;

	cout << "Welcome to Tibia adventurer! What is your name?\nMy name is ";
	cin >> player; cin.ignore();

	if (player.length() <= 13)
	{
		// Now checking if the name already exists
		string imported_name;

		ifstream cinfile (SaveDestiny); // opens file that contains the saved games

		bool j = 0;	// j = 0 while the strings don't match. j = 1 when the string was found

		if (cinfile.is_open())
		{
			cinfile >> imported_name; // getting first element of file

			while (!cinfile.eof()) // while the end of file is not reached
			{
				if (player.compare(imported_name) == 0) // if the strings are the same
				{
					j = 1;

					cout << "\n..::ERROR::..\nA person has already taken that name.\n" << endl;
					pressenter();
					return(new_game());
					break;
				}
				else // if the strings are different, keep reading
				{
					cinfile >> imported_name;
				}
			}
		}

		if (j == 0)	// player can use that name
		{
			cout << endl << "Okay " << player << ", are you ready to start? (Y/N) ";
		}

		// Continuing...
		bool k = false;
		int same_error_number = 0;
		while (k == false)
		{
			char continuar;
			cin >> continuar;

			switch (continuar)
			{
			case 'Y':
			case 'y':
				cin.get();
				k = true;
				return(player);
				break;

			case 'N':
			case 'n':
				cin.get();
				k = true;
				return (new_game ());
				break;

			default:
				if (same_error_number > 1)
				{
					cout << "\n..:: PLEASE STOP AND READ! ::..\n"
						<< "Choose only one of the numbers listed above (Y/N): ";
				}
				else
				{
					cout << error1;
					same_error_number++;
				}
			}
		}
	}
	else
	{
		cout << "\nYour name is too big! Choose one with 13 characters maximum.\n";
		pressenter();
		return (new_game ());
	}
	return 0;
}

int main ()
{
	/* initialize random seed: */
	srand((unsigned int) time(NULL)); 

	system("cls");
	cor(0,10);
	
	string selection_string;
	int selection_int;
	
	// Main Menu Display
	main_menu_UI();

	bool k = false;
	int same_error_number = 0;

	while (k == false)
	{
		gotoxy(0,0);
		gotoxy(43,13);
		cin >> selection_string;
		if ((selection_string == "x") || (selection_string == "X"))
		{
			cin.ignore();
			gotoxy(2,19);
			cor(0,12);
			cout << "\n> Quitting... ";
			cor(0,10);
			pressenter();
			k = true;
			break;
		}
		else if ((selection_string == "y") || (selection_string == "Y"))
		{
			cin.ignore();
			// About Display
			about_UI();
			cout << endl;
			pressenter();
			main();
			break;
		}
		else
		{
			selection_int = atoi(selection_string.c_str());  // Converte string em numero

			switch (selection_int)
			{
				// New Game
			case 1:
				{
					cin.get();
					cout << endl;
					k = true;
					Player player =	StartNewPlayer(new_game());
					first_display_starting_game(player);
					game_interface(55, 11, player);
					main();
					break;
				}

				// Load Game
			case 2:
				{
					cin.get();
					cout << endl;

					ifstream cinfile (SaveDestiny); // opens file that contains the saved games
					if (cinfile.is_open())
					{
						k = true;
						string temp = "temp";
						Player player =	StartNewPlayer(temp);
						player = load_game();
						first_display_starting_game(player);
						game_interface(55, 11, player);
						main();
						break;
					}
					else
					{
						cout << "..:ERROR::..\nThere are no saved profiles yet." << endl;
						cout << "Please start a new game to create a character.\n" << endl;
						pressenter();
						return(main());
					}
				}

				// Error
			default:
				if (same_error_number > 1)
				{
					gotoxy(2,16);
					cout << "\n..:: PLEASE STOP AND READ! ::..\n"
						 << "Choose only one of the numbers listed above (1-2 or Y/X). ";
				}
				else 
				{
					gotoxy(2,16);
					cout << error1;
					same_error_number++;
				}
			}
		}
	}
}