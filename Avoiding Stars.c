#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define CONSOLE_SIZE_Y 60
#define CONSOLE_SIZE_X 60
#define Y_SPACE 15 //y축 밑에 공간을 만들기 위해 설정한 값

#define cls system("cls")
#define NO_TEST_MOD //유지.보수용 커서 좌표, 오비젝트 좌표 등
#define NO_GOD_MOD //죽지 않음
#define START_MENU_MOD //시작 메뉴 없애기


//↓스테이지가 바뀌면서 적의 개수가 늘어날 때, 의도치 않은 공간에서 스폰되는 것을 방지
#define ENEMY_RESET 	for (int i = 0; i < 500; i++) \
						{ \
						ENEMY[i].Print = false; \
						}





//유용한 기능=========================================
void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

void Cursor(int n) // 커서 보이기 & 숨기기
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = n; //0이면 안 보이고 1이면 보임
	ConsoleCursor.dwSize = 1; // 커서 사이즈

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

int getxy(void)
{
	CONSOLE_SCREEN_BUFFER_INFO curInfo;        // 콘솔 출력창의 정보를 담기 위해서 정의한 구조체

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);  //현재 커서의 위치 정보를 저장하는 함수

	printf("[%d, %d] \n", curInfo.dwCursorPosition.X, curInfo.dwCursorPosition.Y);  //구조체의 저장한 값을 출력

	return 0;
}

void _Console_Size(int _col, int _lines)
{
	char setText[100];
	sprintf(setText, "mode con cols=%d lines=%d", _col, _lines);
	system(setText);
}
//====================================================




//구조체 선언 & 지역변수 선언=========================
typedef struct OBJ
{
	int x; //사용자 x값	
	int y; //사용자 y값
	bool Active; //게임 오버 여부

	bool Print; //적 출력 여부
} OBJ;
//↑ 사실 적 따로 캐릭터 따로 총알 따로 만들면 되는데... 하나로만 하는게 편함



//전역 변수는 global의 약자인 g_
int g_score; //점수
int g_stage; //스테이지
int g_menu; //시작시 메뉴 선택  1.게임시작 2.도움말 3.게임 종료
int g_ENEMY_NUM = 0; //기본 적 수
int g_ENEMY_RESET_COUNT; //스테이지 클리어 시, 늘어나는 적들이 의도치 않은 장소에 스폰하는 것을 방지
int g_Sleep_Time = 50; //스테이지가 올라갈 수록 적이 빨라지게 만듦

//쓰임새가 낮고 그닥 중요하지 않는 전역 변수는, 괜히 헷갈리니 g_ 생략
time_t endTime, startTime;
float gap;
//시간 구하기

time_t SwaitTime, EwaitTime; //esc로 일시 정지 시 그 시간은 생존시간에 포함되지 않음. 
float pauseTime;

int Not_First = 0; //게임이 처음 하는건지 판단. 0이면 처음, 1이면 죽고나서 다시 하는 판.

OBJ ENEMY[500] = { false, };
OBJ CHARACTER = { 0, };
//====================================================





//메뉴& 기능 부분==============================================
void Main_Menu(void);
//메뉴 고르기


void KeyInput(void);
// ↑키보드 입력(캐릭터 위치) &메뉴 선택


void Help(void);
//도움말 등 


void Start_Menu(void);
//초기 실행 메뉴


void MenuSel(void);
//메뉴 선택 입력받기


void Ending_Menu(void);
//게임 결과창
//======================================================






//게임 핵심 부분 함수들================================
void Setting(void);
// ↑기본 설정


void SpawnStar(void);
// 장애물(별) 스폰(생성)


void Char(void);
// ↑캐릭터 위치 &스테이지 레벨 관련


void Game_Running(void);
//게임 실행 부분
//======================================================






int main(void)
{
	PlaySound(TEXT("BG.wav"), NULL, SND_ASYNC | SND_LOOP);

	Setting(); //기본 값들


#ifdef START_MENU_MOD
	if (!Not_First)
		Start_Menu(); //초기 실행 화면
#endif


	Main_Menu(); //메뉴 화면


	startTime = clock(); //시간 측정
	Game_Running(); //게임 실행 부분

	Ending_Menu();

	system("pause");
	return 0;
}





//메뉴& 기능 부분==============================================
void Help(void)
{

	while (1)
	{
		cls;


		gotoxy(11, 5);
		puts("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		gotoxy(11, 6);
		puts("┃         조작법 & 도움말          ┃");
		gotoxy(11, 7);
		puts("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");

		puts("\n\n * 캐릭터는 방향키 버튼으로 이동이 가능합니다.");
		puts("\n\n * 게임 도중 ESC키를 눌러 일시정지가 가능합니다.");
		puts("\n\n * 스테이지가 증가할 때마다 적도 증가합니다.");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		puts("\n\n * 7스테이지부터는 난이도가 대폭 상승합니다. (낙하 속도 급 상승)");
		puts("\n\n\n !주의!   임의로 콘솔창 크기를 변경하지 마세요!");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(10, 50);
		puts("메뉴로 가려면 엔터키를 눌러주세요 ...");

		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			cls;
			Main_Menu();
			break;
		}

		Sleep(100);
	}
}
//도움말 


void Main_Menu(void)
{
	while (1)
	{
		MenuSel();
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			g_menu--;
			if (g_menu < 1)
				g_menu = 3;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			g_menu++;
			if (g_menu > 3)
				g_menu = 1;
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			switch (g_menu)
			{
			case 2: // 조작법 및 도움말
				Help();
				break;
			case 3: //게임 종료
				cls;
				exit(0);
			case 1: //게임 시작
				break;
			}
			break;
#ifdef TEST_MOD
			gotoxy(70, 6);
			getxy();
#endif
		}

		MenuSel();

		Sleep(100);
	}
}
//메뉴 고르기


void KeyInput(void)
{
	if (CHARACTER.Active)
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{

			CHARACTER.x--;

			if (CHARACTER.x < 0)
				CHARACTER.x = 0;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			CHARACTER.x++;

			if (CHARACTER.x > CONSOLE_SIZE_X - 2)
				CHARACTER.x = CONSOLE_SIZE_X - 2;
		}

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			CHARACTER.y--;

			if (CHARACTER.y < 0)
				CHARACTER.y = 0;

		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			CHARACTER.y++;
			if (CHARACTER.y > CONSOLE_SIZE_Y - Y_SPACE)
				CHARACTER.y = CONSOLE_SIZE_Y - Y_SPACE;
		}

		//일시 중지
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			cls;

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
			gotoxy(11, 5);
			printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
			gotoxy(11, 6);
			printf("┃        ... 일시 중지 ...  \t      ┃");
			gotoxy(11, 7);
			printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");

			gotoxy(13, 10);

			SwaitTime = clock();
			system("pause");
			EwaitTime = clock();

			pauseTime += (float)(EwaitTime - SwaitTime) / CLOCKS_PER_SEC; //일시 중지동안 시간을 계산
		}
	}

	//결과창에서 나오는 것들
	if (!CHARACTER.Active)
	{
		if (GetAsyncKeyState('Y') & 0x8000 || GetAsyncKeyState('y') & 0x8000)
		{
			PlaySound(TEXT("BG.wav"), NULL, SND_ASYNC | SND_LOOP);

			cls;
			Setting();

			Not_First = 1;

			main(); //게임 시작메뉴

		}


		if (GetAsyncKeyState('N') & 0x8000 || GetAsyncKeyState('n') & 0x8000)
		{
			cls;
			exit(0);
		}
	}

}
// ↑키보드 입력(캐릭터 위치) &메뉴 선택


void Start_Menu(void)
{
	int escape = 0; //무한루프 탈출용 변수 

	while (1)
	{
		gotoxy(11, 5);
		printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		system("color 0d");
		Sleep(500);
		gotoxy(11, 6);
		printf("┃   ★★★ 별 피하기 게임★★★    ┃");
		system("color 0e");
		Sleep(500);
		gotoxy(11, 7);
		printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
		system("color 0c");
		Sleep(500);

		if (escape == 0)
		{
			int n = 8; //게이지 좌표
			int time = 240;
			//뭔가 게임같은 느낌을 표현하기 위해;; 약간의 로망..?같은..

			//퍼센트 게이지가 차는 부분
			for (int i = 1; i < 23; i++)
			{
				int load = ((i * 100) / 22);
				gotoxy(27, 39);
				printf("%d%%", load);
				load++;
				gotoxy(n, 40);
				n += 2;

				puts("■");
				Sleep(time);
				time -= 10;

#ifdef TEST_MOD
				getxy();
#endif

			}

			escape = 1;
		}
		else
		{
			break;
		}
	}

	cls;
}
//초기 실행 메뉴


void MenuSel(void)
{
	gotoxy(0, 0);
	puts("\t    방향키와 스페이스키로 조작하세요");

	//메뉴 선택
	if (g_menu == 1)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 15);
		printf("▶게임 시작");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 20);
		printf("▶조작법 및 도움말");
		gotoxy(15, 25);
		printf("▶게임 종료");
	}
	if (g_menu == 2)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 20);
		printf("▶조작법 및 도움말");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 25);
		printf("▶게임 종료");
		gotoxy(15, 15);
		printf("▶게임 시작");
	}
	if (g_menu == 3)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 25);
		printf("▶게임 종료");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 20);
		printf("▶조작법 및 도움말");
		gotoxy(15, 15);
		printf("▶게임 시작");
	}

#ifdef TEST_MOD
	gotoxy(0, 0);
	printf("menu : %d", g_menu);
#endif

}
//메뉴 선택 입력받기


void Ending_Menu(void)
{
	cls;

	while (1)
	{
		KeyInput();
		gotoxy(11, 5);
		printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		system("color 0d");
		KeyInput();
		Sleep(400);
		gotoxy(11, 6);
		printf("┃        ... GAME OVER ...  \t      ┃");
		system("color 0e");
		KeyInput();
		Sleep(400);
		gotoxy(11, 7);
		printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
		KeyInput();

		printf("\n\n\n      \t\t     [ %d ] 스테이지\n\n\n\n\n점수 : %d\t\t   생존 점수(시간) : %f", g_stage, g_score, gap - pauseTime);

		printf("\n\n\n\n\n\n\n     다시 도전하시겠습니까?   꾹 눌러주세요( Y / N )");

		KeyInput();
	}
}
//게임 결과창
//======================================================






//게임 핵심 부분 함수들================================
void Setting(void)
{
	system("title ★ 별 피하기 게임! ★         -made by 홍지훈  https://github.com/ZENOVERS"); //프로그램 제목

	_Console_Size(CONSOLE_SIZE_X, CONSOLE_SIZE_Y); //콘솔 크기 조정

	Cursor(0); //커서 안보이게 함
	srand((int)time(NULL));

	g_ENEMY_NUM = 30; //적 초기화, 굳이 여기 있는 이유는 게임 재시작 시 필요
	g_stage = 1; //스테이지, 
	g_menu = 1; // 메뉴 선택
	g_score = 0;

	g_ENEMY_RESET_COUNT = 1; //스테이지 클리어 시, 늘어나는 적들이 의도치 않은 장소에 스폰하는 것을 방지
	endTime = 0; //게임 종료 시간 초기화
	startTime = 0; //게임 시작 시간 초기화
	gap = 0.0; //총 시간 초기화
	SwaitTime = 0; //일시정지 시작 시간 초기화
	EwaitTime = 0; //일시정지 종료 시간 초기화
	pauseTime = 0.0; //일시중지 동안 시간 계산

	ENEMY_RESET; //적이 의도치 않은 공간에 뜨는 것을 방지

	CHARACTER.Active = true; //캐릭터가 죽으면 false 할당
	CHARACTER.x = (int)(CONSOLE_SIZE_X / 2), CHARACTER.y = CONSOLE_SIZE_Y - Y_SPACE; //내 캐릭터 시작점

	gotoxy(CHARACTER.x, CHARACTER.y);

}
// ↑기본 설정
//

void SpawnStar(void)
{
	if (CHARACTER.Active == true) //캐릭터가 살아있다면
	{
		//스테이지가 증가하면서 적 수가 늘음
		if (g_score < 200)
		{
			g_score++;
			g_stage = 1;
		}
		else if (g_score < 400)
		{
			if (g_ENEMY_RESET_COUNT == 1) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}
			g_score++;
			g_stage = 2;

			g_ENEMY_NUM = 40; //적의 수
			g_Sleep_Time = 49;
		}
		else if (g_score < 800)
		{
			if (g_ENEMY_RESET_COUNT == 2) // cls한번만 실행
			{
				ENEMY_RESET	 //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 2;
			g_stage = 3;

			g_ENEMY_NUM = 50; // 적의 수
			g_Sleep_Time = 48;
		}
		else if (g_score < 1200)
		{
			if (g_ENEMY_RESET_COUNT == 3) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 3;
			g_stage = 4;

			g_ENEMY_NUM = 60; // 적의 수
			g_Sleep_Time = 47;
		}
		else if (g_score < 2000)
		{
			if (g_ENEMY_RESET_COUNT == 4) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 3;
			g_stage = 5;

			g_ENEMY_NUM = 70; // 적의 수
			g_Sleep_Time = 46;
		}
		else if (g_score < 3000)
		{
			if (g_ENEMY_RESET_COUNT == 5) // cls한번만 실행
			{
				ENEMY_RESET; //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
				g_ENEMY_RESET_COUNT++;
			}

			g_score += 4;
			g_stage = 6;

			g_ENEMY_NUM = 80;
			g_Sleep_Time = 45;
		}
		else if (g_score < 4500)
		{
			if (g_ENEMY_RESET_COUNT == 6) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 4;
			g_stage = 7;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 40;
		}
		else if (g_score < 7000)
		{
			if (g_ENEMY_RESET_COUNT == 7) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 한 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 5;
			g_stage = 8;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 30;
		}
		else if (g_score < 10000)
		{
			if (g_ENEMY_RESET_COUNT == 8) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 5;
			g_stage = 9;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 20;
		}
		else if (g_score < 15000)
		{
			if (g_ENEMY_RESET_COUNT == 9) // cls한번만 실행
			{
				ENEMY_RESET //스테이지 초기화. 이걸 안 하면 게임 도중 중간에도 별이 나타남
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 6;
			g_stage = 10;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 10;
		}
		else
		{
			g_score += 10;
			g_Sleep_Time = 5;
		}



		for (int i = 0; i < g_ENEMY_NUM; i++) //적 위치
		{
			if (!ENEMY[i].Print)
			{
				ENEMY[i].x = (rand() % CONSOLE_SIZE_X);
				ENEMY[i].y = 0;
				ENEMY[i].Print = true;

				//별이 일자로(동시에, 다같이) 떨어지는 것을 방지
				break;
			}
		}

	}

	//적 생성
	for (int i = 0; i < g_ENEMY_NUM; i++)
	{
		if (ENEMY[i].Print)
		{
			int color = (rand() % 15) + 1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);

			gotoxy(ENEMY[i].x, ENEMY[i].y);
			printf("★");

#ifdef TEST_MOD
			getxy();
#endif

			ENEMY[i].y++;

#ifndef GOD_MOD //무적
			if (ENEMY[i].x == CHARACTER.x && ENEMY[i].y == CHARACTER.y) //충돌 시 게임이 종료되기 시작
			{
				PlaySound(TEXT("crash.wav"), NULL, SND_ASYNC);

				gotoxy(CHARACTER.x, CHARACTER.y);

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				puts("▒"); //캐릭터가 충돌 시 이 모양으로 변함
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);


				CHARACTER.Active = false;//끝

				endTime = clock(); //시간 계산
				gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC); //시간 계산

				Sleep(1000);

				PlaySound(TEXT("ending.wav"), NULL, SND_ASYNC | SND_LOOP);

				Ending_Menu();
			}
#endif

			if (ENEMY[i].y > CONSOLE_SIZE_Y - Y_SPACE) //별이 콘솔 창을 벗어나면 출력하지 않음
			{
				ENEMY[i].Print = false;
			}

		}
	}
}
// 장애물(별) 스폰(생성)


void Char(void)
{
	if (CHARACTER.Active)
	{

		{
			gotoxy(CHARACTER.x, CHARACTER.y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			printf("●");


			gotoxy(25, 50);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), g_stage);
			printf("[ %d ] 스테이지\t\t\t\t", g_stage);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			printf("내 점수 : %d", g_score);

#ifdef TEST_MOD
			printf("현재 적 수: %d", g_ENEMY_NUM);
			getxy();
#endif
		}
	}
}
// ↑캐릭터 위치 &스테이지 레벨 관련


void Game_Running(void)
{
	while (1)
	{
		cls;
		if (CHARACTER.Active != true)
		{
			gotoxy(CHARACTER.x, CHARACTER.y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			puts("▒"); //캐릭터가 충돌 시 이 모양으로 변함
		}


		//별 생성
		SpawnStar();

		if (CHARACTER.Active == true) //캐릭터가 충돌 시 멈춤
		{
			//키 입력
			KeyInput();

			//캐릭터 움직임
			Char();
		}

		Sleep(g_Sleep_Time);
	}

}
//게임 실행 부분
//======================================================