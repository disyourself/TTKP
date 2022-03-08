/*
	������ܿ�����־
	1. ������Ŀ
	2. �����ز�
	3.������Ϸ����
		ʵ�ʿ�������
		�Գ�ѧ�ߣ����û����濪ʼ
		
		ѡ��ͼ�ο����������
		������ܣ����ڡ�easyx��ͼ�ο�ģ�
		1) ������Ϸ����
		2) ʵ����Ϸ����
			a.������Ϸ����
			b.ѭ������������ʵ��
		3��ʵ����Ϸ����
			a.���ر�����Դ
			b.��Ⱦ(����֪ʶ:���꣩
			��������:����ͼƬpng���ֺ�ɫ
	4.ʵ����ұ���
	5.ʵ����ҵ���Ծ
		��ʱ������ʱ�Ῠ��
	6.ʵ�����С�ڹ�
	7.�����ϰ���ṹ����������
		�ڹ�ת��̫����.
	8.ʹ���ϰ���ṹ����ʼ��
	9.��װ�����ϰ������ʾ
	10.ʵ����ҵ��¶�
	11.ʵ�֡����ӡ�obstacle
	12.ʵ����ײ���

C����>>C++>>�㷨>>���ݿ�>>windows(win32+mfc)>>QT>>linux������

*/

#include <stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>		//�ɱ�����
#include"tools.h"

using namespace std;	//���������ռ�

#include<mmsystem.h>
#include "main.h"
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 100

//����ͼƬ
IMAGE imagBgs[3];
int bgX[3];		//����ͼƬx����
int bgSpeed[3] = { 1,2,4 };

//Ӣ��ͼƬ
IMAGE imgHeros[12];
int heroX;		//��ҵ�x����
int heroY;		//��ҵ�y����
int	heroIndex;	//��ҵı���ͼƬ֡���

bool heroJump;	//��ʾ���������Ծ
int jumpHeightMax;	//��Ծ��ߵ�
int heroJumpOff;	//��Ծƫ����offset
int update;			//��ʾ�Ƿ���Ҫ����ˢ�»���

IMAGE imgHeroDown[2];
bool heroDown;		//��ʾ����Ƿ��¶�

int heroBlood;		//Ӣ��Ѫ��

typedef enum ty{
	tortoise, //�ڹ� 0
	lion,	//ʨ�� 1
	hook1,	//���� 2
	hook2,
	hook3,
	hook4,
	OBSTACLE_TYPE_COUNT //������ 6
}obstacle_type;

int lastObstacleIndex;	//��һ���ϰ�������


vector<vector<IMAGE>>obstacleImgs;	//����ϰ���ĸ���ͼƬ IMAGE obstacleImgs{][];

typedef struct obstacle {
	int type;  //�ϰ�������� tortoise С�ڹ� 0, lion ʨ�� 1
	int imgIndex;	//��ǰ��ʾ��ͼƬ���
	int x, y;	//�ϰ��������
	int speed;
	int power; //ɱ����
	bool exist;
	bool hited; //��ײ
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];	//�ṹ�� type imgindex x y.......




//��Ϸ��ʼ��
void init() {
	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT,EW_SHOWCONSOLE);	

	//��ʾ��ʼ����
	loadimage(0,"res/over.png");
	system("pause");
	
	//���ر�����Դ
	char name[64];

	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png",i + 1);			//"res/bg001.png" "res/bg002.png"
		loadimage(&imagBgs[i], name);
		bgX[i] = 0;
	}

	//����hero����ͼƬ
	for (int i = 0; i < 12; i++) {			
		sprintf(name, "res/hero%d.png",i+1);
		loadimage(&imgHeros[i], name);
	}
	
	//������ҵĳ�ʼλ��
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumpOff = -6;
	
	//��ʼ��Ѫ��
	heroBlood = 100;

	update = true;

	//����С�ڹ��ز�
	/*loadimage(&imgTortoise, "res/t1.png");
	TortoiseExist = false;
	TortoiseY = 345 - imgTortoise.getheight() + 5;*/
	
	//���ظ����ϰ���
	IMAGE imgTortoise;				
	vector<IMAGE>imgTortoiseArray;
	for (int i = 0; i < 7; i++) {
		sprintf(name, "res/t%d.png", i + 1);
		loadimage(&imgTortoise, name);
		imgTortoiseArray.push_back(imgTortoise);
	}
	obstacleImgs.push_back(imgTortoiseArray);

	IMAGE imgLion;
	vector<IMAGE>imgLionArray;
	for (int i = 0; i < 6; i++){
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);
	
	//����4������
	IMAGE imgHook;
	for (int i = 0; i < 4; i++) {
		vector<IMAGE>imgHookArray;
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgHook, name,63,260,true);
		imgHookArray.push_back(imgHook);
		obstacleImgs.push_back(imgHookArray);
	}



	//��ʼ���ϰ����
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}

	//�����¶��ز�
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;

	//Ԥ������Ч
	preLoadSound("res/hit.mp3");

	//���ر�������
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);

	int lastObstacleIndex = -1;	

}

void creatObstacle() {
	int i;
	for (i = 0; i <OBSTACLE_COUNT; i++){
		if (!obstacles[i].exist) {
			break;
		}
	}
	
	if (i>=OBSTACLE_COUNT){
		return;
	}

	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imgIndex = 0;
	//obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_COUNT);

	obstacles[i].type = rand() % 3;

	//ʨ�Ӻ����Ӳ��ܹ���
	if (lastObstacleIndex >= hook1 && lastObstacleIndex <= hook4 && obstacles[lastObstacleIndex].x > (WIN_WIDTH - 500)); {
		obstacles[i].type = tortoise; 
	}
	lastObstacleIndex = i;

	//������Ӷ��� ��һ����������
	if (obstacles[i].type == hook1||obstacles[i].type==hook2) {
		obstacles[i].type = rand() % 5;
	}



	//�ϰ���λ��
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = 345 + 5 - obstacleImgs[obstacles[i].type][0].getheight();
	

	//�ϰ����ƶ��ٶ�
	if (obstacles[i].type == tortoise) {
		obstacles[i].speed = 4;
		obstacles[i].power = 5;		//���޸�
	}
	else if (obstacles[i].type == lion) {
		obstacles[i].speed = 8;
		obstacles[i].power = 20;
	}
	else if (obstacles[i].type>=hook1 && obstacles[i].type <= hook4) {
		obstacles[i].speed = 2;
		obstacles[i].power =20;
		obstacles[i].y = 0;
	}

	//�Ż��������ϰ�����Ӻ�ʨ�Ӷ�·
}

void checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].hited==false && obstacles[i].exist) {
			int a1x, a1y, a2x, a2y;
			int off = 30;
			if (!heroDown) {
				a1x = heroX + off;
				a1y = heroY + off;
				a2x = heroX + imgHeros[heroIndex].getwidth() - off;
				a2y = heroY + imgHeros[heroIndex].getheight();
			}
			else {
				a1x = heroX + off;
				a1y = 345-imgHeroDown[heroIndex].getheight();
				a2x = heroX + imgHeroDown[heroIndex].getwidth() - off;
				a2y = 345;
			}
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y + img.getheight() - 10;

			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) {
				
				heroBlood = heroBlood - obstacles[i].power;
				printf("Ѫ��ʣ��%d\n", heroBlood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
				//update = false;
			}

		}
	}

}

void fly() {
	for (int i = 0; i < 3; i++) {
		bgX[i] -= bgSpeed[i];

		if (bgX[i] < -WIN_WIDTH) {
			bgX[i] = 0;
		}
	}

	//ʵ����Ծ
	if (heroJump) {
		if (heroY < jumpHeightMax) {
			heroJumpOff = 4;
		}
		heroY += heroJumpOff;

		if (heroY > 345 - imgHeros[0].getheight()) {
			heroJump = false;
			heroJumpOff = -4;
		}
	}//�¶�
	else if (heroDown) {
		static int count = 0;			//�ӳ��¶�֡
		int delay[2] = { 8,30 };
		count++;
		if (count >= delay[heroIndex]){
			count = 0;
			heroIndex++;
			if (heroIndex >= 2) {
				heroIndex = 0;
				heroDown = false;
			}
		}
	}
	else {
		heroIndex = (heroIndex + 1) % 12;
	}

	//����С�ڹ�
	static int frameCount = 0;
	static int enemyFre = 100;	//���˳���Ƶ��
	frameCount++;

	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre = 100 + rand() % 100;		//50-249֡����һ��
		creatObstacle();
	}

	/*if (TortoiseExist) {
		TortoiseX -= bgSpeed[2];
		if (TortoiseX < -imgTortoise.getwidth()) {
			TortoiseExist = false;
		}
	}*/

	//���������ϰ��������
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			obstacles[i].x -= obstacles[i].speed + bgSpeed[0];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][0].getwidth() * 2) {
				obstacles[i].exist = false;
			}

			int len = (int)obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}

	//��Һ��ϰ���ġ���ײ��⡱����
	checkHit();
}

//��Ⱦ����Ϸ������
void updateBg() {
	putimagePNG2(bgX[0], 0, &imagBgs[0]);
	putimagePNG2(bgX[1], 119, &imagBgs[1]);
	putimagePNG2(bgX[2], 330, &imagBgs[2]);
}

void jump() {
	heroJump = true;
	update = true;

}

void down() {
	update = true;
	heroDown = true;
	heroIndex = 0;

}

void keyEvent() {
	char ch;
	if (_kbhit()){		//����а������£�kbhit�������� true
		ch = _getch();
		if (ch == ' ') {
			jump();
		}
		else if (ch == 'c') {
			down();
		}
	}
}

void updateEnemy() {
	
	//��ȾС�ڹ�
	/*if (TortoiseExist) {
		putimagePNG2(TortoiseX, TortoiseY, WIN_WIDTH, &imgTortoise);
	}*/

	//��Ⱦ�ϰ���
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH, &obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}


}

void updateHero() {
	if (!heroDown) {
		putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
	}
	else {
		int y = imgHeroDown[0].getheight()/2;
		putimagePNG2(heroX, heroY +  y, &imgHeroDown[heroIndex]);
	}
}

void updateBloodBar() {
	drawBloodBar(10, 10, 200, 10,2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}


void checkOver() {
	if (heroBlood <=0) {
		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		
		//��֮ͣ�󣬳�ֵ�����ֱ����һ��
		heroBlood = 100;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}

int main(void){
	init();

	int timer = 0;

	while (1) {
		keyEvent();
		timer += getDelay();	

		if (timer > 15) {		//ˢ���ٶ�
			timer = 0;
			update = true;
		}
		 
		if (update) {
			update = false;
			BeginBatchDraw();		//��ӡ���һ�����

			updateBg();
			//putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
			updateHero();
			updateEnemy();
			updateBloodBar();
			checkOver();


			FlushBatchDraw();

			fly();
		}
		//Sleep(30);		//����30ms
	}
	
	system("pause");

	return 0;
}
