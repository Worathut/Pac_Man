#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include<iostream>
#define screen_x 80
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
unsigned int pastTime = 0;
unsigned int pastTimeItem = 0;
const int ScrHeight = 25;
const int ScrWidth = 24;
int diff_X, diff_Y;
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
	"#    .  #     #  .    #",//12
	"#####.# ####### #.#####",//13
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
		if (tempMap[y][x] == 8) {
			map[y][x] = ' ';
			score += 10;
		}
		if (tempMap[y][x] == 20) {
			map[y][x] = ' ';
			score += 100;
		}
	}

	void EatItem() {
		if (tempMap[y][x] == 6) {
			map[y][x] = ' ';
			score += 50;
			item_time = 10;
		}
	}

};
player pacman;
class Monster {
public:
	int x;
	int y;
	int dir;
	int ch;//character
	int delay;
	int count_delay;

	void draw_Blinky() {
		//red
		ch = 1;
		if (item_time > 0) tempMap[y][x] = 25;
		else if (item_time == 0) tempMap[y][x] = 15;
	}

	void draw_Inky() {
		//blue
		ch = 2;
		if (item_time > 0) tempMap[y][x] = 26;
		else if (item_time == 0) tempMap[y][x] = 16;
	}

	void draw_Pinky() {
		//pink
		ch = 3;
		if (item_time > 0) tempMap[y][x] = 27;
		else if (item_time == 0) tempMap[y][x] = 17;
	}

	void draw_Clyde() {
		//orange
		ch = 4;
		if (item_time > 0) tempMap[y][x] = 28;
		else if (item_time == 0) tempMap[y][x] = 18;
	}

	void move() {
		//monsDead();
		if (count_delay == delay) {
			dir = rand() % 4;
			if (dir == 0 && !collision(x, y - 1)) y--;
			if (dir == 1 && !collision(x, y + 1)) y++;
			if (dir == 2 && !collision(x - 1, y)) x--;
			if (dir == 3 && !collision(x + 1, y)) x++;
			count_delay = 0;
			/*
			if (ch == 1) {//red
				if (pacman.y < y && !collision(x, y - 1)) {
					dir = 1;
				}
				else if (pacman.y > y && !collision(x, y +1)) {
					dir = 0;
				}

				else if (pacman.x < x && !collision(x - 1, y)) {
					dir = 2;
				}

				else if (pacman.x > x && !collision(x + 1, y)) {
					dir = 3;
				}
			}

			if (ch == 2) {//blue
				if (pacman.y < 10) {
					dir = 0;
				}
				else dir = rand() % 4;
			}

			if (ch == 3) {//pink
				if (pacman.y > 10) {
					dir = 1;
				}
				else dir = rand() % 4;
			}

			if (ch == 4) {//orange
				dir = rand() % 4;
			}
			*/
		}
		count_delay++;
	}

	void monsDead() {
		if (tempMap[y][x] == 25 &&(tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4)) {
			x = 9;
			y = 11;
			//tempMap[y][x] = 15;
		}
		if (tempMap[y][x] == 26 && (tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4)) {
			x = 10;
			y = 11;
			//tempMap[y][x] = 16;
		}
		if (tempMap[y][x] == 27 && (tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4)) {
			x = 12;
			y = 11;
			//tempMap[y][x] = 17;
		}
		if (tempMap[y][x] == 28 && (tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4)) {
			x = 13;
			y = 11;
			//tempMap[y][x] = 18;
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
	pacman.delay = 5;

	Blinky.x = 9;
	Blinky.y = 11;
	Blinky.delay = 5;

	Inky.x = 10;
	Inky.y = 11;
	Inky.delay = 5;

	Pinky.x = 12;
	Pinky.y = 11;
	Pinky.delay = 5;

	Clyde.x = 13;
	Clyde.y = 11;
	Clyde.delay = 5;
}

void layout() {
	for (int i = 0; i < ScrHeight; i++) {
		for (int j = 0; j < ScrWidth; j++) {
			if (map[i][j] == '#') tempMap[i][j] = 9;//wall
			else if (map[i][j] == '.') tempMap[i][j] = 8;//dot
			else if (map[i][j] == '@') tempMap[i][j] = 6;//item
			else if (map[i][j] == ' ') tempMap[i][j] = 0;//way
		}
	}
}

void display() {

	setcolor(6, 0);
	gotoxy(40, 6); cout << "Score: " << score;
	setcolor(4, 0);
	gotoxy(40, 8); cout << "Life: " << life;

	layout();
	pacman.draw();

	if (item_time > 0) {
		if (item_time == 0) {
			item_time = 0;
		}
		if (clock() - pastTimeItem >= 1000 && item_time != 0) {
			gotoxy(40, 10); cout << "                  ";
			setcolor(5, 0);
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
			gotoxy(j + 4, i + 2);//setPosition
			if (tempMap[i][j] == 9) {
				setcolor(1, 0);
				cout << char(219);  //Wall//178
			}
			setcolor(14, 0);
			if (tempMap[i][j] == 8) cout << char(249);//dot
			if (tempMap[i][j] == 6) cout << char(153); //Item

			setcolor(6, 0);
			if (tempMap[i][j] == 1) cout << char(85);//Player up
			if (tempMap[i][j] == 2) cout << char(239);//Player down
			if (tempMap[i][j] == 3) cout << char(41);//Player left
			if (tempMap[i][j] == 4) cout << char(40);//Player right

			setcolor(4, 0);  
			if (tempMap[i][j] == 15) cout << char(225); //red
			setcolor(11, 0);
			if (tempMap[i][j] == 16) cout << char(225); //blue
			setcolor(13, 0);
			if (tempMap[i][j] == 17) cout << char(225); //pink
			setcolor(12, 0);
			if (tempMap[i][j] == 18) cout << char(225); //orange
			setcolor(9, 15);
			if (tempMap[i][j] == 25) cout << char(225); //can eat
			setcolor(9, 15);
			if (tempMap[i][j] == 26) cout << char(225); //can eat
			setcolor(9, 15);
			if (tempMap[i][j] == 27) cout << char(225); //can eat
			setcolor(9, 15);
			if (tempMap[i][j] == 28) cout << char(225); //can eat

			setcolor(0, 0);
			if (tempMap[i][j] == 0) cout << ' ';//way

			setcolor(7, 0);
		}
	}
}

void input() {
	if (_kbhit()) {
		switch (_getch()) {
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
		}
		fflush(stdin);
	}
}

void enemyCatch(int y, int x) {
	if (tempMap[y][x] == 1 || tempMap[y][x] == 2 || tempMap[y][x] == 3 || tempMap[y][x] == 4) {
		pacman.dir = '5';
		pacman.x = 11;
		pacman.y = 18;
		life--;
	}
}

void CatchMonster(int y, int x) {
	if (tempMap[y][x] == 25) {
		Blinky.x = 9;
		Blinky.y = 11;
	}
	if (tempMap[y][x] == 26) {
		Inky.x = 10;
		Inky.y = 11;
	}
	if (tempMap[y][x] == 27) {
		Pinky.x = 12;
		Pinky.y = 11;
	}
	if (tempMap[y][x] == 28) {
		Clyde.x = 13;
		Clyde.y = 11;
	}

}

void movement() {
	/*
	if (abs(pacman.x - Blinky.x) > abs(pacman.y - Blinky.y)) {
		if (pacman.x > Blinky.x && !Blinky.collision(Blinky.x + 1, Blinky.y)) Blinky.x++;
		else if (!Blinky.collision(Blinky.x - 1, Blinky.y)) Blinky.x--;
	}
	else {
		if (pacman.y > Blinky.y && !Blinky.collision(Blinky.x, Blinky.y + 1)) Blinky.y++;
		else if (!Blinky.collision(Blinky.x, Blinky.y - 1)) Blinky.y--;
	}
	enemyCatch(Blinky.y, Blinky.x);
	*/
	Blinky.move();
	enemyCatch(Blinky.y, Blinky.x);
	Inky.move();
	enemyCatch(Inky.y, Inky.x);
	Pinky.move();
	enemyCatch(Pinky.y, Pinky.x);
	Clyde.move();
	enemyCatch(Clyde.y, Clyde.x);
	CatchMonster(pacman.y, pacman.x);
}

int main() {
	//SetConsoleSize(800, 700);
	SetConsoleTitle(L"Text Mode Pac-Man 2021");
	setConsole(screen_x, screen_y);
	hideCursor(0);
	setup();
	bool play = true;
	while(life > 0 && play)
	{
		display();
		input();
		movement();
		if (score >= 2100) 
		{
			play = false;
		}
	}
	display();
	return 0;
}