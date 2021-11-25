#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<windows.h>
#include<time.h>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>
#define screen_x 65
#define screen_y 70
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
COORD bufferSize = { screen_x,screen_y };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };

using namespace std;
int life = 3;
int score = 0;
int item_time = 0;
int countDot = 0;
bool exitGame = false;
bool isESC = false;
unsigned int pastTime = 0;
unsigned int pastTimeItem = 0;
const int ScrHeight = 25;
const int ScrWidth = 24;
string NamePlayer;
int Distance;
int tempMap[ScrHeight][ScrWidth];

char map[ScrHeight][ScrWidth] = {
	"#######################",//1
	"#..........#..........#",//2
	"#@###.####.#.####.###@#",//3
	"#.###.####.#.####.###.#",//4
	"#.....................#",//5
	"#.###.#.#######.#.###.#",//6
	"#.....#.#######.#.....#",//7
	"#####.#....#....#.#####",//8
	"#####.#### # ####.#####",//9
	"#####.#         #.#####",//10
	"#####.# #     # #.#####",//11
	"W    .  #     #  .    W",//12
	"#####.# ##   ## #.#####",//13
	"#####.#         #.#####",//14
	"#####.# ####### #.#####",//15
	"#####.# ####### #.#####",//16
	"#..........#..........#",//17
	"#.###.####.#.####.###.#",//18
	"#@..#...... ......#..@#",//19
	"###.#.#.#######.#.#.###",//20
	"#.....#....#....#.....#",//21
	"#.########.#.########.#",//22
	"#.########.#.########.#",//23
	"#.....................#",//24
	"#######################"//25
};

int setConsole(int x, int y) {
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

void SetConsoleSize(int W, int H) {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, W, H, TRUE);
}

int setMode() {
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void gotoxy(int x, int y) {
	//setPosition
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setcolor(int fg, int bg) {
	//setColor
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

void hideCursor(bool visible) {
	//hide Cursor
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

class player {
public:
	int x;
	int y;
	char dir;
	int delay;
	int count_delay;

	void draw() {
		if (dir == '1') tempMap[y][x] = 1;
		else if (dir == '2') tempMap[y][x] = 2;
		else if (dir == '3') tempMap[y][x] = 3;
		else if (dir == '4' || dir == '5') tempMap[y][x] = 4;
		else tempMap[y][x] = 0;
	}

	void move(int xf, int yf) {
		dead();
		if (count_delay == delay) {
			x = xf;
			y = yf;
			count_delay = 0;
		}
		count_delay++;
		eat();
		EatItem();
	}

	bool collision(int fx, int fy) {
		if (tempMap[fy][fx] == 9) return true;
		return false;
	}

	void dead() {
		if (tempMap[y][x] == 15|| tempMap[y][x] == 16 || tempMap[y][x] == 17 || tempMap[y][x] == 18) {
			dir = '5';
			x = 11;
			y = 18;
			life--;
		}
	}

	void eat() {
		if (tempMap[y][x] == 8) { //dot
			map[y][x] = 'o';
			score += 10;
			countDot++;
		}
	}

	void EatItem() {
		if (tempMap[y][x] == 6) {
			map[y][x] = 'o';
			score += 50;
			item_time = 20;
		}
	}

};
player pacman;
class Monster {
public:
	int x;
	int y;
	int Act;
	int dir = 0;
	int ch;//character
	int delay;
	int count_delay;
	bool isEat = false;
	unsigned long pastTimeEnemy = 0;
	unsigned long delayEnemy = 0;
	unsigned long pastEnemy = 0;
	void draw_Blinky() {
		//red
		ch = 1;
		if (item_time > 0) {
			tempMap[y][x] = 25;
			isEat = true;
		}
		else if (item_time == 0) {
			tempMap[y][x] = 15;
			isEat = false;
		}
	}

	void draw_Inky() {
		//blue
		ch = 2;
		if (item_time > 0) {
			tempMap[y][x] = 26;
			isEat = true;
		}
		else if (item_time == 0) {
			tempMap[y][x] = 16;
			isEat = false;
		}
	}

	void draw_Pinky() {
		//pink
		ch = 3;
		if (item_time > 0) {
			tempMap[y][x] = 27;
			isEat = true;
		}
		else if (item_time == 0) {
			tempMap[y][x] = 17;
			isEat = false;
		}
	}

	void draw_Clyde() {
		//orange
		ch = 4;
		if (item_time > 0) {
			tempMap[y][x] = 28;
			isEat = true;
		}
		else if (item_time == 0) {
			tempMap[y][x] = 18;
			isEat = false;
		}
	}

	void move() {
		if (clock() - pastEnemy >= 500) {
			if (Act == 0 && !collision(x, y - 1)) y--;
			if (Act == 1 && !collision(x, y + 1)) y++;
			if (Act == 2 && !collision(x - 1, y)) x--;
			if (Act == 3 && !collision(x + 1, y)) x++;
			Act = rand() % 4;
			pastEnemy = clock();
		}
	}

	bool collision(int fx, int fy) {
		if (tempMap[fy][fx] == 9) return true;
		return false;
	}
};

Monster Blinky;//red
Monster Inky;//blue
Monster Pinky;//pink
Monster Clyde;//orange
void setup() {
	srand(time(NULL));
	pacman.x = 11; //start
	pacman.y = 18; //start
	pacman.dir = '5';
	pacman.delay = 4;

	Blinky.x = 11;
	Blinky.y = 10;
	Blinky.delayEnemy = 500;

	Inky.x = 9;
	Inky.y = 11;
	Inky.delayEnemy = 500;

	Pinky.x = 11;
	Pinky.y = 12;
	Pinky.delayEnemy = 500;

	Clyde.x = 13;
	Clyde.y = 11;
	Clyde.delayEnemy = 500;
}

void layout() {
	for (int i = 0; i < ScrHeight; i++) {
		for (int j = 0; j < ScrWidth; j++) {
			if (map[i][j] == '#') tempMap[i][j] = 9;//wall
			else if (map[i][j] == '.') tempMap[i][j] = 8;//dot
			else if (map[i][j] == '@') tempMap[i][j] = 6;//item
			else if (map[i][j] == ' ') tempMap[i][j] = 0;//way
			else if (map[i][j] == 'W') tempMap[i][j] = 10;//warp
			else if (map[i][j] == 'o') tempMap[i][j] = 0;//way
		}
	}
}

void display() {

	setcolor(15, 0);
	gotoxy(40, 6); cout << "HIGH SCORE : " << score;
	setcolor(6, 0);
	gotoxy(40, 8); cout << "Life : " << life;
	setcolor(9, 0);
	gotoxy(40, 10); cout << "Item Time : ";
	layout();
	pacman.draw();

	if (item_time > 0) {
		if (item_time == 0) {
			item_time = 0;
		}
		if (clock() - pastTimeItem >= 1000 && item_time != 0) {
			gotoxy(40, 10); cout << "                  ";
			setcolor(9, 0);
			gotoxy(40, 10); cout << "Item Time : " << --item_time;
			pastTimeItem = clock();
		}
	}

	Blinky.draw_Blinky();
	Inky.draw_Inky();
	Pinky.draw_Pinky();
	Clyde.draw_Clyde();
	for (int i = 0; i < ScrHeight; i++) {
		for (int j = 0; j < ScrWidth; j++) {
			setcolor(7, 0);
			gotoxy(j + 4, i + 2); //setPosition
			if (tempMap[i][j] == 9) {
				setcolor(1, 0);
				cout << char(219); //Wall
			}
			setcolor(1, 0);
			if (tempMap[i][j] == 10) cout << char(177); //warp

			setcolor(14, 0);
			if (tempMap[i][j] == 8) cout << char(249); //dot
			if (tempMap[i][j] == 6) cout << char(153); //Item

			setcolor(6, 0);
			if (tempMap[i][j] == 1) cout << char(85);//Player up
			if (tempMap[i][j] == 2) cout << char(239);//Player down
			if (tempMap[i][j] == 3) cout << char(41);//Player left
			if (tempMap[i][j] == 4) cout << char(40);//Player right

			setcolor(4, 0);
			if (tempMap[i][j] == 15) cout << char(225); //red
			setcolor(11, 0);
			if (tempMap[i][j] == 16) cout << char(232); //blue
			setcolor(12, 0);
			if (tempMap[i][j] == 17) cout << char(234); //pink
			setcolor(2, 0);
			if (tempMap[i][j] == 18) cout << char(128); //orange

			setcolor(9, 15);
			if (tempMap[i][j] == 25) cout << char(225); //can eat red
			setcolor(9, 15);
			if (tempMap[i][j] == 26) cout << char(232); //can eat blue
			setcolor(9, 15);
			if (tempMap[i][j] == 27) cout << char(234); //can eat pink
			setcolor(9, 15);
			if (tempMap[i][j] == 28) cout << char(128); //can eat orange

			setcolor(0, 0);
			if (tempMap[i][j] == 0) cout << ' '; //way
			setcolor(7, 0);
		}
	}
}
void menu();
void input() {
	if (_kbhit()) {
		switch (_getch()) {
		case KEY_UP:
			if (!pacman.collision(pacman.x, pacman.y - 1)) {
				pacman.dir = '1';
				pacman.move(pacman.x, pacman.y--);
			}
			break;
		case KEY_DOWN:
			if (!pacman.collision(pacman.x, pacman.y + 1)) {
				pacman.dir = '2';
				pacman.move(pacman.x, pacman.y++);
			}
			break;
		case KEY_LEFT:
			if (!pacman.collision(pacman.x - 1, pacman.y)) {
				pacman.dir = '3';
				pacman.move(pacman.x--, pacman.y);
			}
			break;
		case KEY_RIGHT:
			if (!pacman.collision(pacman.x + 1, pacman.y)) {
				pacman.dir = '4';
				pacman.move(pacman.x++, pacman.y);
			}
			break;
		case 'w':
			if (!pacman.collision(pacman.x, pacman.y - 1)) {
				pacman.dir = '1';
				pacman.move(pacman.x, pacman.y--);
			}
			break;
		case 's':
			if (!pacman.collision(pacman.x, pacman.y + 1)) {
				pacman.dir = '2';
				pacman.move(pacman.x, pacman.y++);
			}
			break;
		case 'a':
			if (!pacman.collision(pacman.x - 1, pacman.y)) {
				pacman.dir = '3';
				pacman.move(pacman.x--, pacman.y);
			}
			break;
		case 'd':
			if (!pacman.collision(pacman.x + 1, pacman.y)) {
				pacman.dir = '4';
				pacman.move(pacman.x++, pacman.y);
			}
			break;
		case char(27):
			isESC = true;
			break;
		}
		fflush(stdin);
	}
}

void enemyCatch(int y, int x) {
	if (tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4) {
		setup();
		life--;
	}
}

void Blinky_movement() {
	int difx_Blinky = abs(pacman.x - Blinky.x);
	int dify_Blinky = abs(pacman.y - Blinky.y);
	if (clock() - Blinky.pastTimeEnemy >= Blinky.delayEnemy)
	{
		if (difx_Blinky > dify_Blinky && difx_Blinky != dify_Blinky)
		{
			if (pacman.x > Blinky.x)
			{
				if (!Blinky.collision(Blinky.x + 1, Blinky.y))
				{
					Blinky.x++;
				}
				else if (!Blinky.collision(Blinky.x, Blinky.y - 1) && Blinky.dir == 2)
				{
					Blinky.y--;
				}
				else if (!Blinky.collision(Blinky.x, Blinky.y + 1) && Blinky.dir == 3)
				{
					Blinky.y++;
				}
				else
				{
					Blinky.dir = (rand() % 4) + 2;
				}
			}
			else if (pacman.x < Blinky.x)
			{
				if (!Blinky.collision(Blinky.x - 1, Blinky.y))
				{
					Blinky.x--;
				}
				else if (!Blinky.collision(Blinky.x, Blinky.y - 1) && Blinky.dir == 2)
				{
					Blinky.y--;
				}
				else if (!Blinky.collision(Blinky.x, Blinky.y + 1) && Blinky.dir == 3)
				{
					Blinky.y++;
				}
				else
				{
					Blinky.dir = (rand() % 4) + 2;
				}
			}
		}
		else if (difx_Blinky < dify_Blinky && difx_Blinky != dify_Blinky)
		{
			if (pacman.y > Blinky.y)
			{
				if (!Blinky.collision(Blinky.x, Blinky.y + 1))
				{
					Blinky.y++;
				}
				else if (!Blinky.collision(Blinky.x - 1, Blinky.y) && Blinky.dir == 0)
				{
					Blinky.x--;
				}
				else if (!Blinky.collision(Blinky.x + 1, Blinky.y) && Blinky.dir == 1)
				{
					Blinky.x++;
				}
				else
				{
					Blinky.dir = rand() % 2;
				}
			}
			else if (pacman.y < Blinky.y)
			{
				if (!Blinky.collision(Blinky.x, Blinky.y - 1))
				{
					Blinky.y--;
				}
				else if (!Blinky.collision(Blinky.x - 1, Blinky.y) && Blinky.dir == 0)
				{
					Blinky.x--;
				}
				else if (!Blinky.collision(Blinky.x + 1, Blinky.y) && Blinky.dir == 1)
				{
					Blinky.x++;
				}
				else
				{
					Blinky.dir = rand() % 2;
				}
			}
		}
		else
		{
			Blinky.move();
		}
		if(Blinky.isEat == false) enemyCatch(Blinky.y, Blinky.x);
		Blinky.pastTimeEnemy = clock();
	}
	//Warp
	if (tempMap[Blinky.y][Blinky.x] == 10 && Blinky.x < 11) {
		Blinky.x = 21;
		Blinky.y = 11;
	}

	if (tempMap[Blinky.y][Blinky.x] == 10 && Blinky.x > 11) {
		Blinky.x = 1;
		Blinky.y = 11;
	}
}

void Inky_movement() {
	int difx_Inky = abs(pacman.x - Inky.x);
	int dify_Inky = abs(pacman.y - Inky.y);
	if (clock() - Inky.pastTimeEnemy >= Inky.delayEnemy)
	{
		if (difx_Inky > dify_Inky && difx_Inky != dify_Inky)
		{
			if (pacman.x > Inky.x)
			{
				if (!Inky.collision(Inky.x + 1, Inky.y))
				{
					Inky.x++;
				}
				else if (!Inky.collision(Inky.x, Inky.y - 1) && Inky.dir == 2)
				{
					Inky.y--;
				}
				else if (!Inky.collision(Inky.x, Inky.y + 1) && Inky.dir == 3)
				{
					Inky.y++;
				}
				else
				{
					Inky.dir = (rand() % 4) + 2;
				}
			}
			else if (pacman.x < Inky.x)
			{
				if (!Inky.collision(Inky.x - 1, Inky.y))
				{
					Inky.x--;
				}
				else if (!Inky.collision(Inky.x, Inky.y - 1) && Inky.dir == 2)
				{
					Inky.y--;
				}
				else if (!Inky.collision(Inky.x, Inky.y + 1) && Inky.dir == 3)
				{
					Inky.y++;
				}
				else
				{
					Inky.dir = (rand() % 4) + 2;
				}
			}
		}
		else if (difx_Inky < dify_Inky && difx_Inky != dify_Inky)
		{
			if (pacman.y > Inky.y)
			{
				if (!Inky.collision(Inky.x, Inky.y + 1))
				{
					Inky.y++;
				}
				else if (!Inky.collision(Inky.x - 1, Inky.y) && Inky.dir == 0)
				{
					Inky.x--;
				}
				else if (!Inky.collision(Inky.x + 1, Inky.y) && Inky.dir == 1)
				{
					Inky.x++;
				}
				else
				{
					Inky.dir = rand() % 2;
				}
			}
			else if (pacman.y < Inky.y)
			{
				if (!Inky.collision(Inky.x, Inky.y - 1))
				{
					Inky.y--;
				}
				else if (!Inky.collision(Inky.x - 1, Inky.y) && Inky.dir == 0)
				{
					Inky.x--;
				}
				else if (!Inky.collision(Inky.x + 1, Inky.y) && Inky.dir == 1)
				{
					Inky.x++;
				}
				else
				{
					Inky.dir = rand() % 2;
				}
			}
		}
		else
		{
			Inky.move();
		}
		if (Inky.isEat == false) enemyCatch(Inky.y, Inky.x);
		Inky.pastTimeEnemy = clock();
	}
	//Warp
	if (tempMap[Inky.y][Inky.x] == 10 && Inky.x < 11) {
		Inky.x = 21;
		Inky.y = 11;
	}
	if (tempMap[Inky.y][Inky.x] == 10 && Inky.x > 11) {
		Inky.x = 1;
		Inky.y = 11;
	}
}

void Pinky_movement() {
	int difx_Pinky = abs(pacman.x - Pinky.x);
	int dify_Pinky = abs(pacman.y - Pinky.y);
	if (clock() - Pinky.pastTimeEnemy >= Pinky.delayEnemy)
	{
		if (difx_Pinky > dify_Pinky && difx_Pinky != dify_Pinky)
		{
			if (pacman.x > Pinky.x)
			{
				if (!Pinky.collision(Pinky.x + 1, Pinky.y))
				{
					Pinky.x++;
				}
				else if (!Pinky.collision(Pinky.x, Pinky.y - 1) && Pinky.dir == 2)
				{
					Pinky.y--;
				}
				else if (!Pinky.collision(Pinky.x, Pinky.y + 1) && Pinky.dir == 3)
				{
					Pinky.y++;
				}
				else
				{
					Pinky.dir = (rand() % 4) + 2;
				}
			}
			else if (pacman.x < Pinky.x)
			{
				if (!Pinky.collision(Pinky.x - 1, Pinky.y))
				{
					Pinky.x--;
				}
				else if (!Pinky.collision(Pinky.x, Pinky.y - 1) && Pinky.dir == 2)
				{
					Pinky.y--;
				}
				else if (!Pinky.collision(Pinky.x, Pinky.y + 1) && Pinky.dir == 3)
				{
					Pinky.y++;
				}
				else
				{
					Pinky.dir = (rand() % 4) + 2;
				}
			}
		}
		else if (difx_Pinky < dify_Pinky && difx_Pinky != dify_Pinky)
		{
			if (pacman.y > Pinky.y)
			{
				if (!Pinky.collision(Pinky.x, Pinky.y + 1))
				{
					Pinky.y++;
				}
				else if (!Pinky.collision(Pinky.x - 1, Pinky.y) && Pinky.dir == 0)
				{
					Pinky.x--;
				}
				else if (!Pinky.collision(Pinky.x + 1, Pinky.y) && Pinky.dir == 1)
				{
					Pinky.x++;
				}
				else
				{
					Pinky.dir = rand() % 2;
				}
			}
			else if (pacman.y < Pinky.y)
			{
				if (!Pinky.collision(Pinky.x, Pinky.y - 1))
				{
					Pinky.y--;
				}
				else if (!Pinky.collision(Pinky.x - 1, Pinky.y) && Pinky.dir == 0)
				{
					Pinky.x--;
				}
				else if (!Pinky.collision(Pinky.x + 1, Pinky.y) && Pinky.dir == 1)
				{
					Pinky.x++;
				}
				else
				{
					Pinky.dir = rand() % 2;
				}
			}
		}
		else
		{
			Pinky.move();
		}
		if (Pinky.isEat == false) enemyCatch(Pinky.y, Pinky.x);
		Pinky.pastTimeEnemy = clock();
	}
	//Warp
	if (tempMap[Pinky.y][Pinky.x] == 10 && Pinky.x < 11) {
		Pinky.x = 21;
		Pinky.y = 11;
	}
	if (tempMap[Pinky.y][Pinky.x] == 10 && Pinky.x > 11) {
		Pinky.x = 1;
		Pinky.y = 11;
	}
}

void Clyde_movement() {
	int difx_Clyde = abs(pacman.x - Clyde.x);
	int dify_Clyde = abs(pacman.y - Clyde.y);
	if (clock() - Clyde.pastTimeEnemy >= Clyde.delayEnemy)
	{
		if (difx_Clyde > dify_Clyde && difx_Clyde != dify_Clyde)
		{
			if (pacman.x > Clyde.x)
			{
				if (!Clyde.collision(Clyde.x + 1, Clyde.y))
				{
					Clyde.x++;
				}
				else if (!Clyde.collision(Clyde.x, Clyde.y - 1) && Clyde.dir == 2)
				{
					Clyde.y--;
				}
				else if (!Clyde.collision(Clyde.x, Clyde.y + 1) && Clyde.dir == 3)
				{
					Clyde.y++;
				}
				else
				{
					Clyde.dir = (rand() % 4) + 2;
				}
			}
			else if (pacman.x < Clyde.x)
			{
				if (!Clyde.collision(Clyde.x - 1, Clyde.y))
				{
					Clyde.x--;
				}
				else if (!Clyde.collision(Clyde.x, Clyde.y - 1) && Clyde.dir == 2)
				{
					Clyde.y--;
				}
				else if (!Clyde.collision(Clyde.x, Clyde.y + 1) && Clyde.dir == 3)
				{
					Clyde.y++;
				}
				else
				{
					Clyde.dir = (rand() % 4) + 2;
				}
			}
		}
		else if (difx_Clyde < dify_Clyde && difx_Clyde != dify_Clyde)
		{
			if (pacman.y > Clyde.y)
			{
				if (!Clyde.collision(Clyde.x, Clyde.y + 1))
				{
					Clyde.y++;
				}
				else if (!Clyde.collision(Clyde.x - 1, Clyde.y) && Clyde.dir == 0)
				{
					Clyde.x--;
				}
				else if (!Clyde.collision(Clyde.x + 1, Clyde.y) && Clyde.dir == 1)
				{
					Clyde.x++;
				}
				else
				{
					Clyde.dir = rand() % 2;
				}
			}
			else if (pacman.y < Clyde.y)
			{
				if (!Clyde.collision(Clyde.x, Clyde.y - 1))
				{
					Clyde.y--;
				}
				else if (!Clyde.collision(Clyde.x - 1, Clyde.y) && Clyde.dir == 0)
				{
					Clyde.x--;
				}
				else if (!Clyde.collision(Clyde.x + 1, Clyde.y) && Clyde.dir == 1)
				{
					Clyde.x++;
				}
				else
				{
					Clyde.dir = rand() % 2;
				}
			}
		}
		else
		{
			Clyde.move();
		}
		if (Clyde.isEat == false) enemyCatch(Clyde.y, Clyde.x);
		Clyde.pastTimeEnemy = clock();
	}
	//Warp
	if (tempMap[Clyde.y][Clyde.x] == 10 && Clyde.x < 11) {
		Clyde.x = 21;
		Clyde.y = 11;
	}
	if (tempMap[Clyde.y][Clyde.x] == 10 && Clyde.x > 11) {
		Clyde.x = 1;
		Clyde.y = 11;
	}
}

void movement() {
	if (tempMap[pacman.y][pacman.x] == 25) {
		Blinky.x = 11;
		Blinky.y = 10;
		score += 200;
	}

	if (tempMap[pacman.y][pacman.x] == 26) {
		Inky.x = 9;
		Inky.y = 11;
		score += 200;
	}

	if (tempMap[pacman.y][pacman.x] == 27) {
		Pinky.x = 11;
		Pinky.y = 12;
		score += 200;
	}

	if (tempMap[pacman.y][pacman.x] == 28) {
		Clyde.x = 13;
		Clyde.y = 11;
		score += 200;
	}
	//pac man Warp
	if (tempMap[pacman.y][pacman.x] == 10 && pacman.x < 11) {
		pacman.x = 21;
		pacman.y = 11;
	}
	if (tempMap[pacman.y][pacman.x] == 10 && pacman.x > 11) {
		pacman.x = 1;
		pacman.y = 11;
	}
	Blinky_movement();
	Inky_movement();
	Pinky_movement();
	Clyde_movement();
}
void Score();
void playGame();
void menu() {
	char c = '0';
	int scroll = 0;
	bool exit = false;
	system("CLS");
	while (exit == false) {
		setcolor(6, 0);
		gotoxy(22, 5);
		cout << "PAC-MAN Text-Mode 2021";

		setcolor(2, 0);
		gotoxy(30, 8);
		cout << "START";

		setcolor(3, 0);
		gotoxy(30, 10);
		cout << "SCORE";

		setcolor(4, 0);
		gotoxy(30, 12);
		cout << "EXIT ";
		while (c != char(13)) {
			setcolor(6, 0);
			gotoxy(27, 8 + scroll);
			//cout << "º";
			cout << char(206);
			gotoxy(0, 0);
			c = _getch();
			gotoxy(27, 8 + scroll);
			cout << " ";
			gotoxy(27, 8 + scroll);
			cout << " ";
			cout << "  ";
			if (c == 's') {
				scroll += 2;
				if (scroll > 4) {
					scroll = 0;
				}
			}
			if (c == 'w') {
				scroll -= 2;
				if (scroll < 0) {
					scroll = 4;
				}
			}
		}
		if (scroll == 4) {  //press Enter
			exit = true; // exit
			exitGame = true;
		}
		if (scroll == 2) {
			exit = true;
			system("CLS");
			Score();
		}
		if (scroll == 0) {
			exit = true;
			exitGame = false;
			system("CLS");
			playGame();
		}
		c = ' ';
		system("CLS");
	}
}

void GameOver() {
	hideCursor(1);
	setcolor(1, 0);
	gotoxy(10, 4); cout << "@#########################################@";
	gotoxy(10, 5); cout << "#                                         #";
	gotoxy(10, 6); cout << "#                                         #";
	gotoxy(10, 7); cout << "#               ";
	setcolor(4, 0);
	gotoxy(26, 7); cout << "GAME OVER!!";
	setcolor(1, 0);
	gotoxy(37, 7); cout << "               #";
	gotoxy(10, 8); cout << "#                                         #";
	gotoxy(10, 9); cout << "#  ";
	setcolor(6, 0);
	gotoxy(13, 9); cout << "Your Score : " << score;
	setcolor(1, 0);
	gotoxy(46, 9); cout << "      #";
	gotoxy(10, 10); cout << "#                                         #";
	gotoxy(46, 11); cout << "      #";
	gotoxy(10, 12); cout << "#                                         #";
	gotoxy(10, 13); cout << "#                                         #";
	gotoxy(10, 14); cout << "@#########################################@";
	gotoxy(10, 11); cout << "#  Enter your name : "; cin >> NamePlayer;

	hideCursor(0);
	vector<pair<int, string>> userScore;
	fstream file("score.txt", ios::in);
	int scorefile;
	string name;

	while (file >> name >> scorefile) {
		userScore.push_back(make_pair(scorefile, name));
	}

	file.close();
	userScore.push_back(make_pair(score, NamePlayer)); // push back data in vector
	for (int i = userScore.size() - 2; i >= 0; i--) {
		if (userScore[i].second == userScore[userScore.size() - 1].second && userScore[i].first == userScore[userScore.size() - 1].first) {
			userScore.erase(userScore.begin() + i);
		}
	}
	sort(userScore.begin(), userScore.end());

	file.open("score.txt", ios::out);

	for (int i = userScore.size() - 1; i >= 0; i--) {
		file << userScore[i].second << " ";
		file << userScore[i].first << endl;
	}

	file.close();
	score = 0;
	setcolor(6, 0);
	gotoxy(10, 17); cout << "       PRESS ESC TO GO BACK MENU       ";
	_getch();
	menu();
	if (_getch() == char(27)) {
		menu();
	}
}

void Score() {
	int n = 0;
	int color = 7;
	srand(time(NULL));
	vector<pair<int, string>> userScore;
	fstream file("score.txt", ios::in);
	int score;
	string name;
		while (file >> name >> score) {
			userScore.push_back(make_pair(score, name));
		}
		file.close();
		setcolor(3, 0);
		gotoxy(4, 4); cout << "                     * HIGH SCORES *" << endl;
		setcolor(6, 0);
		gotoxy(6, 6); cout << " RANK    NAME                              SCORE        " << endl;
		for (int i = 0; i < 5; i++) {
			color = rand() % 15  + 1;
			setcolor(color, 0);
			gotoxy(8, 8 + n); cout << i + 1;
			gotoxy(15, 8 + n); cout << userScore[i].second;
			gotoxy(49, 8 + n); cout << userScore[i].first;
			n += 2;
		}
		setcolor(4, 0);
		gotoxy(4, 23); cout << "            PRESS ESC TO GO BACK THE  MENU               ";
		
		if (_getch() == char(27)) {
			menu();
		}
}

void playGame() {
	setup();
	while (exitGame == false)
	{
		display();
		input();
		movement();
		if (life <= 0) {
			for (int i = 0; i < ScrHeight; i++) {
				for (int j = 0; j < ScrWidth; j++) {
					if (map[i][j] == 'o') map[i][j] = '.';
				}
			}
			life = 3;
			item_time = 0;
			exitGame = true;
			system("CLS");
			GameOver();
		}
		if (isESC == true) {
			for (int i = 0; i < ScrHeight; i++) {
				for (int j = 0; j < ScrWidth; j++) {
					if (map[i][j] == 'o') map[i][j] = '.';
				}
			}
			life = 3;
			item_time = 0;
			isESC = false;
			system("CLS");
			GameOver();
		}
		if (countDot >= 190) {
			for (int i = 0; i < ScrHeight; i++) {
				for (int j = 0; j < ScrWidth; j++) {
					if (map[i][j] == 'o') map[i][j] = '.';
				}
			}
			countDot = 0;
			setup();
		}
	}
}
int main() {
	SetConsoleTitle(L"PAC-MAN Text Mode 2021");
	setConsole(screen_x, screen_y);
	hideCursor(0);
	menu();
	return 0;
}