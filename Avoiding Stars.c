#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define CONSOLE_SIZE_Y 60
#define CONSOLE_SIZE_X 60
#define Y_SPACE 15 //y�� �ؿ� ������ ����� ���� ������ ��

#define cls system("cls")
#define NO_TEST_MOD //����.������ Ŀ�� ��ǥ, ������Ʈ ��ǥ ��
#define NO_GOD_MOD //���� ����
#define START_MENU_MOD //���� �޴� ���ֱ�


//�齺�������� �ٲ�鼭 ���� ������ �þ ��, �ǵ�ġ ���� �������� �����Ǵ� ���� ����
#define ENEMY_RESET 	for (int i = 0; i < 500; i++) \
						{ \
						ENEMY[i].Print = false; \
						}





//������ ���=========================================
void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

void Cursor(int n) // Ŀ�� ���̱� & �����
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = n; //0�̸� �� ���̰� 1�̸� ����
	ConsoleCursor.dwSize = 1; // Ŀ�� ������

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

int getxy(void)
{
	CONSOLE_SCREEN_BUFFER_INFO curInfo;        // �ܼ� ���â�� ������ ��� ���ؼ� ������ ����ü

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);  //���� Ŀ���� ��ġ ������ �����ϴ� �Լ�

	printf("[%d, %d] \n", curInfo.dwCursorPosition.X, curInfo.dwCursorPosition.Y);  //����ü�� ������ ���� ���

	return 0;
}

void _Console_Size(int _col, int _lines)
{
	char setText[100];
	sprintf(setText, "mode con cols=%d lines=%d", _col, _lines);
	system(setText);
}
//====================================================




//����ü ���� & �������� ����=========================
typedef struct OBJ
{
	int x; //����� x��	
	int y; //����� y��
	bool Active; //���� ���� ����

	bool Print; //�� ��� ����
} OBJ;
//�� ��� �� ���� ĳ���� ���� �Ѿ� ���� ����� �Ǵµ�... �ϳ��θ� �ϴ°� ����



//���� ������ global�� ������ g_
int g_score; //����
int g_stage; //��������
int g_menu; //���۽� �޴� ����  1.���ӽ��� 2.���� 3.���� ����
int g_ENEMY_NUM = 0; //�⺻ �� ��
int g_ENEMY_RESET_COUNT; //�������� Ŭ���� ��, �þ�� ������ �ǵ�ġ ���� ��ҿ� �����ϴ� ���� ����
int g_Sleep_Time = 50; //���������� �ö� ���� ���� �������� ����

//���ӻ��� ���� �״� �߿����� �ʴ� ���� ������, ���� �򰥸��� g_ ����
time_t endTime, startTime;
float gap;
//�ð� ���ϱ�

time_t SwaitTime, EwaitTime; //esc�� �Ͻ� ���� �� �� �ð��� �����ð��� ���Ե��� ����. 
float pauseTime;

int Not_First = 0; //������ ó�� �ϴ°��� �Ǵ�. 0�̸� ó��, 1�̸� �װ��� �ٽ� �ϴ� ��.

OBJ ENEMY[500] = { false, };
OBJ CHARACTER = { 0, };
//====================================================





//�޴�& ��� �κ�==============================================
void Main_Menu(void);
//�޴� ����


void KeyInput(void);
// ��Ű���� �Է�(ĳ���� ��ġ) &�޴� ����


void Help(void);
//���� �� 


void Start_Menu(void);
//�ʱ� ���� �޴�


void MenuSel(void);
//�޴� ���� �Է¹ޱ�


void Ending_Menu(void);
//���� ���â
//======================================================






//���� �ٽ� �κ� �Լ���================================
void Setting(void);
// ��⺻ ����


void SpawnStar(void);
// ��ֹ�(��) ����(����)


void Char(void);
// ��ĳ���� ��ġ &�������� ���� ����


void Game_Running(void);
//���� ���� �κ�
//======================================================






int main(void)
{
	PlaySound(TEXT("BG.wav"), NULL, SND_ASYNC | SND_LOOP);

	Setting(); //�⺻ ����


#ifdef START_MENU_MOD
	if (!Not_First)
		Start_Menu(); //�ʱ� ���� ȭ��
#endif


	Main_Menu(); //�޴� ȭ��


	startTime = clock(); //�ð� ����
	Game_Running(); //���� ���� �κ�

	Ending_Menu();

	system("pause");
	return 0;
}





//�޴�& ��� �κ�==============================================
void Help(void)
{

	while (1)
	{
		cls;


		gotoxy(11, 5);
		puts("������������������������������������������������������������������������");
		gotoxy(11, 6);
		puts("��         ���۹� & ����          ��");
		gotoxy(11, 7);
		puts("������������������������������������������������������������������������\n");

		puts("\n\n * ĳ���ʹ� ����Ű ��ư���� �̵��� �����մϴ�.");
		puts("\n\n * ���� ���� ESCŰ�� ���� �Ͻ������� �����մϴ�.");
		puts("\n\n * ���������� ������ ������ ���� �����մϴ�.");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		puts("\n\n * 7�����������ʹ� ���̵��� ���� ����մϴ�. (���� �ӵ� �� ���)");
		puts("\n\n\n !����!   ���Ƿ� �ܼ�â ũ�⸦ �������� ������!");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(10, 50);
		puts("�޴��� ������ ����Ű�� �����ּ��� ...");

		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			cls;
			Main_Menu();
			break;
		}

		Sleep(100);
	}
}
//���� 


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
			case 2: // ���۹� �� ����
				Help();
				break;
			case 3: //���� ����
				cls;
				exit(0);
			case 1: //���� ����
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
//�޴� ����


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

		//�Ͻ� ����
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			cls;

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
			gotoxy(11, 5);
			printf("������������������������������������������������������������������������");
			gotoxy(11, 6);
			printf("��        ... �Ͻ� ���� ...  \t      ��");
			gotoxy(11, 7);
			printf("������������������������������������������������������������������������\n");

			gotoxy(13, 10);

			SwaitTime = clock();
			system("pause");
			EwaitTime = clock();

			pauseTime += (float)(EwaitTime - SwaitTime) / CLOCKS_PER_SEC; //�Ͻ� �������� �ð��� ���
		}
	}

	//���â���� ������ �͵�
	if (!CHARACTER.Active)
	{
		if (GetAsyncKeyState('Y') & 0x8000 || GetAsyncKeyState('y') & 0x8000)
		{
			PlaySound(TEXT("BG.wav"), NULL, SND_ASYNC | SND_LOOP);

			cls;
			Setting();

			Not_First = 1;

			main(); //���� ���۸޴�

		}


		if (GetAsyncKeyState('N') & 0x8000 || GetAsyncKeyState('n') & 0x8000)
		{
			cls;
			exit(0);
		}
	}

}
// ��Ű���� �Է�(ĳ���� ��ġ) &�޴� ����


void Start_Menu(void)
{
	int escape = 0; //���ѷ��� Ż��� ���� 

	while (1)
	{
		gotoxy(11, 5);
		printf("������������������������������������������������������������������������");
		system("color 0d");
		Sleep(500);
		gotoxy(11, 6);
		printf("��   �ڡڡ� �� ���ϱ� ���ӡڡڡ�    ��");
		system("color 0e");
		Sleep(500);
		gotoxy(11, 7);
		printf("������������������������������������������������������������������������\n");
		system("color 0c");
		Sleep(500);

		if (escape == 0)
		{
			int n = 8; //������ ��ǥ
			int time = 240;
			//���� ���Ӱ��� ������ ǥ���ϱ� ����;; �ణ�� �θ�..?����..

			//�ۼ�Ʈ �������� ���� �κ�
			for (int i = 1; i < 23; i++)
			{
				int load = ((i * 100) / 22);
				gotoxy(27, 39);
				printf("%d%%", load);
				load++;
				gotoxy(n, 40);
				n += 2;

				puts("��");
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
//�ʱ� ���� �޴�


void MenuSel(void)
{
	gotoxy(0, 0);
	puts("\t    ����Ű�� �����̽�Ű�� �����ϼ���");

	//�޴� ����
	if (g_menu == 1)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 15);
		printf("������ ����");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 20);
		printf("�����۹� �� ����");
		gotoxy(15, 25);
		printf("������ ����");
	}
	if (g_menu == 2)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 20);
		printf("�����۹� �� ����");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 25);
		printf("������ ����");
		gotoxy(15, 15);
		printf("������ ����");
	}
	if (g_menu == 3)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		gotoxy(15, 25);
		printf("������ ����");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		gotoxy(15, 20);
		printf("�����۹� �� ����");
		gotoxy(15, 15);
		printf("������ ����");
	}

#ifdef TEST_MOD
	gotoxy(0, 0);
	printf("menu : %d", g_menu);
#endif

}
//�޴� ���� �Է¹ޱ�


void Ending_Menu(void)
{
	cls;

	while (1)
	{
		KeyInput();
		gotoxy(11, 5);
		printf("������������������������������������������������������������������������");
		system("color 0d");
		KeyInput();
		Sleep(400);
		gotoxy(11, 6);
		printf("��        ... GAME OVER ...  \t      ��");
		system("color 0e");
		KeyInput();
		Sleep(400);
		gotoxy(11, 7);
		printf("������������������������������������������������������������������������\n");
		KeyInput();

		printf("\n\n\n      \t\t     [ %d ] ��������\n\n\n\n\n���� : %d\t\t   ���� ����(�ð�) : %f", g_stage, g_score, gap - pauseTime);

		printf("\n\n\n\n\n\n\n     �ٽ� �����Ͻðڽ��ϱ�?   �� �����ּ���( Y / N )");

		KeyInput();
	}
}
//���� ���â
//======================================================






//���� �ٽ� �κ� �Լ���================================
void Setting(void)
{
	system("title �� �� ���ϱ� ����! ��         -made by ȫ����  https://github.com/ZENOVERS"); //���α׷� ����

	_Console_Size(CONSOLE_SIZE_X, CONSOLE_SIZE_Y); //�ܼ� ũ�� ����

	Cursor(0); //Ŀ�� �Ⱥ��̰� ��
	srand((int)time(NULL));

	g_ENEMY_NUM = 30; //�� �ʱ�ȭ, ���� ���� �ִ� ������ ���� ����� �� �ʿ�
	g_stage = 1; //��������, 
	g_menu = 1; // �޴� ����
	g_score = 0;

	g_ENEMY_RESET_COUNT = 1; //�������� Ŭ���� ��, �þ�� ������ �ǵ�ġ ���� ��ҿ� �����ϴ� ���� ����
	endTime = 0; //���� ���� �ð� �ʱ�ȭ
	startTime = 0; //���� ���� �ð� �ʱ�ȭ
	gap = 0.0; //�� �ð� �ʱ�ȭ
	SwaitTime = 0; //�Ͻ����� ���� �ð� �ʱ�ȭ
	EwaitTime = 0; //�Ͻ����� ���� �ð� �ʱ�ȭ
	pauseTime = 0.0; //�Ͻ����� ���� �ð� ���

	ENEMY_RESET; //���� �ǵ�ġ ���� ������ �ߴ� ���� ����

	CHARACTER.Active = true; //ĳ���Ͱ� ������ false �Ҵ�
	CHARACTER.x = (int)(CONSOLE_SIZE_X / 2), CHARACTER.y = CONSOLE_SIZE_Y - Y_SPACE; //�� ĳ���� ������

	gotoxy(CHARACTER.x, CHARACTER.y);

}
// ��⺻ ����
//

void SpawnStar(void)
{
	if (CHARACTER.Active == true) //ĳ���Ͱ� ����ִٸ�
	{
		//���������� �����ϸ鼭 �� ���� ����
		if (g_score < 200)
		{
			g_score++;
			g_stage = 1;
		}
		else if (g_score < 400)
		{
			if (g_ENEMY_RESET_COUNT == 1) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}
			g_score++;
			g_stage = 2;

			g_ENEMY_NUM = 40; //���� ��
			g_Sleep_Time = 49;
		}
		else if (g_score < 800)
		{
			if (g_ENEMY_RESET_COUNT == 2) // cls�ѹ��� ����
			{
				ENEMY_RESET	 //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 2;
			g_stage = 3;

			g_ENEMY_NUM = 50; // ���� ��
			g_Sleep_Time = 48;
		}
		else if (g_score < 1200)
		{
			if (g_ENEMY_RESET_COUNT == 3) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 3;
			g_stage = 4;

			g_ENEMY_NUM = 60; // ���� ��
			g_Sleep_Time = 47;
		}
		else if (g_score < 2000)
		{
			if (g_ENEMY_RESET_COUNT == 4) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 3;
			g_stage = 5;

			g_ENEMY_NUM = 70; // ���� ��
			g_Sleep_Time = 46;
		}
		else if (g_score < 3000)
		{
			if (g_ENEMY_RESET_COUNT == 5) // cls�ѹ��� ����
			{
				ENEMY_RESET; //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
				g_ENEMY_RESET_COUNT++;
			}

			g_score += 4;
			g_stage = 6;

			g_ENEMY_NUM = 80;
			g_Sleep_Time = 45;
		}
		else if (g_score < 4500)
		{
			if (g_ENEMY_RESET_COUNT == 6) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 4;
			g_stage = 7;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 40;
		}
		else if (g_score < 7000)
		{
			if (g_ENEMY_RESET_COUNT == 7) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 5;
			g_stage = 8;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 30;
		}
		else if (g_score < 10000)
		{
			if (g_ENEMY_RESET_COUNT == 8) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
					g_ENEMY_RESET_COUNT++;
			}

			g_score += 5;
			g_stage = 9;

			g_ENEMY_NUM = 100;
			g_Sleep_Time = 20;
		}
		else if (g_score < 15000)
		{
			if (g_ENEMY_RESET_COUNT == 9) // cls�ѹ��� ����
			{
				ENEMY_RESET //�������� �ʱ�ȭ. �̰� �� �ϸ� ���� ���� �߰����� ���� ��Ÿ��
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



		for (int i = 0; i < g_ENEMY_NUM; i++) //�� ��ġ
		{
			if (!ENEMY[i].Print)
			{
				ENEMY[i].x = (rand() % CONSOLE_SIZE_X);
				ENEMY[i].y = 0;
				ENEMY[i].Print = true;

				//���� ���ڷ�(���ÿ�, �ٰ���) �������� ���� ����
				break;
			}
		}

	}

	//�� ����
	for (int i = 0; i < g_ENEMY_NUM; i++)
	{
		if (ENEMY[i].Print)
		{
			int color = (rand() % 15) + 1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);

			gotoxy(ENEMY[i].x, ENEMY[i].y);
			printf("��");

#ifdef TEST_MOD
			getxy();
#endif

			ENEMY[i].y++;

#ifndef GOD_MOD //����
			if (ENEMY[i].x == CHARACTER.x && ENEMY[i].y == CHARACTER.y) //�浹 �� ������ ����Ǳ� ����
			{
				PlaySound(TEXT("crash.wav"), NULL, SND_ASYNC);

				gotoxy(CHARACTER.x, CHARACTER.y);

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				puts("��"); //ĳ���Ͱ� �浹 �� �� ������� ����
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);


				CHARACTER.Active = false;//��

				endTime = clock(); //�ð� ���
				gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC); //�ð� ���

				Sleep(1000);

				PlaySound(TEXT("ending.wav"), NULL, SND_ASYNC | SND_LOOP);

				Ending_Menu();
			}
#endif

			if (ENEMY[i].y > CONSOLE_SIZE_Y - Y_SPACE) //���� �ܼ� â�� ����� ������� ����
			{
				ENEMY[i].Print = false;
			}

		}
	}
}
// ��ֹ�(��) ����(����)


void Char(void)
{
	if (CHARACTER.Active)
	{

		{
			gotoxy(CHARACTER.x, CHARACTER.y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			printf("��");


			gotoxy(25, 50);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), g_stage);
			printf("[ %d ] ��������\t\t\t\t", g_stage);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			printf("�� ���� : %d", g_score);

#ifdef TEST_MOD
			printf("���� �� ��: %d", g_ENEMY_NUM);
			getxy();
#endif
		}
	}
}
// ��ĳ���� ��ġ &�������� ���� ����


void Game_Running(void)
{
	while (1)
	{
		cls;
		if (CHARACTER.Active != true)
		{
			gotoxy(CHARACTER.x, CHARACTER.y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			puts("��"); //ĳ���Ͱ� �浹 �� �� ������� ����
		}


		//�� ����
		SpawnStar();

		if (CHARACTER.Active == true) //ĳ���Ͱ� �浹 �� ����
		{
			//Ű �Է�
			KeyInput();

			//ĳ���� ������
			Char();
		}

		Sleep(g_Sleep_Time);
	}

}
//���� ���� �κ�
//======================================================