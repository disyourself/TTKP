/*
	天天酷跑开发日志
	1. 创建项目
	2. 导入素材
	3.开发游戏界面
		实际开发流程
		对初学者，从用户界面开始
		
		选择图形库或其他引擎
		天天酷跑，基于“easyx”图形库的；
		1) 创建游戏窗口
		2) 实现游戏背景
			a.三重游戏背景
			b.循环滚动背景的实现
		3）实现游戏背景
			a.加载背景资源
			b.渲染(背景知识:坐标）
			遇到问题:背景图片png出现黑色
	4.实现玩家奔跑
	5.实现玩家的跳跃
		有时候起跳时会卡顿
	6.实现随机小乌龟
	7.创建障碍物结构体数据类型
		乌龟转的太快了.
	8.使用障碍物结构体后初始化
	9.封装后多个障碍物的显示
	10.实现玩家的下蹲
	11.实现“柱子”obstacle
	12.实现碰撞检测

C语言>>C++>>算法>>数据库>>windows(win32+mfc)>>QT>>linux服务器

*/

#include <stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>		//可变数组
#include"tools.h"

using namespace std;	//声明命名空间

#include<mmsystem.h>
#include "main.h"
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 100

//背景图片
IMAGE imagBgs[3];
int bgX[3];		//背景图片x坐标
int bgSpeed[3] = { 1,2,4 };

//英雄图片
IMAGE imgHeros[12];
int heroX;		//玩家的x坐标
int heroY;		//玩家的y坐标
int	heroIndex;	//玩家的奔跑图片帧序号

bool heroJump;	//表示玩家正在跳跃
int jumpHeightMax;	//跳跃最高点
int heroJumpOff;	//跳跃偏移量offset
int update;			//表示是否需要马上刷新画面

IMAGE imgHeroDown[2];
bool heroDown;		//表示玩家是否下蹲

int heroBlood;		//英雄血量

typedef enum ty{
	tortoise, //乌龟 0
	lion,	//狮子 1
	hook1,	//柱子 2
	hook2,
	hook3,
	hook4,
	OBSTACLE_TYPE_COUNT //种类数 6
}obstacle_type;

int lastObstacleIndex;	//上一个障碍物的序号


vector<vector<IMAGE>>obstacleImgs;	//存放障碍物的各个图片 IMAGE obstacleImgs{][];

typedef struct obstacle {
	int type;  //障碍物的类型 tortoise 小乌龟 0, lion 狮子 1
	int imgIndex;	//当前显示的图片序号
	int x, y;	//障碍物的坐标
	int speed;
	int power; //杀伤力
	bool exist;
	bool hited; //碰撞
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];	//结构体 type imgindex x y.......




//游戏初始化
void init() {
	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT,EW_SHOWCONSOLE);	

	//显示初始画面
	loadimage(0,"res/over.png");
	system("pause");
	
	//加载背景资源
	char name[64];

	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png",i + 1);			//"res/bg001.png" "res/bg002.png"
		loadimage(&imagBgs[i], name);
		bgX[i] = 0;
	}

	//加载hero奔跑图片
	for (int i = 0; i < 12; i++) {			
		sprintf(name, "res/hero%d.png",i+1);
		loadimage(&imgHeros[i], name);
	}
	
	//设置玩家的初始位置
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumpOff = -6;
	
	//初始化血量
	heroBlood = 100;

	update = true;

	//加载小乌龟素材
	/*loadimage(&imgTortoise, "res/t1.png");
	TortoiseExist = false;
	TortoiseY = 345 - imgTortoise.getheight() + 5;*/
	
	//加载各种障碍物
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
	
	//加载4个柱子
	IMAGE imgHook;
	for (int i = 0; i < 4; i++) {
		vector<IMAGE>imgHookArray;
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgHook, name,63,260,true);
		imgHookArray.push_back(imgHook);
		obstacleImgs.push_back(imgHookArray);
	}



	//初始化障碍物池
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}

	//加载下蹲素材
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;

	//预加载音效
	preLoadSound("res/hit.mp3");

	//加载背景音乐
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

	//狮子和柱子不能共存
	if (lastObstacleIndex >= hook1 && lastObstacleIndex <= hook4 && obstacles[lastObstacleIndex].x > (WIN_WIDTH - 500)); {
		obstacles[i].type = tortoise; 
	}
	lastObstacleIndex = i;

	//如果柱子多了 下一个不是柱子
	if (obstacles[i].type == hook1||obstacles[i].type==hook2) {
		obstacles[i].type = rand() % 5;
	}



	//障碍物位置
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = 345 + 5 - obstacleImgs[obstacles[i].type][0].getheight();
	

	//障碍物移动速度
	if (obstacles[i].type == tortoise) {
		obstacles[i].speed = 4;
		obstacles[i].power = 5;		//可修改
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

	//优化“死亡障碍物”柱子和狮子堵路
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
				printf("血量剩余%d\n", heroBlood);
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

	//实现跳跃
	if (heroJump) {
		if (heroY < jumpHeightMax) {
			heroJumpOff = 4;
		}
		heroY += heroJumpOff;

		if (heroY > 345 - imgHeros[0].getheight()) {
			heroJump = false;
			heroJumpOff = -4;
		}
	}//下蹲
	else if (heroDown) {
		static int count = 0;			//延迟下蹲帧
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

	//创建小乌龟
	static int frameCount = 0;
	static int enemyFre = 100;	//敌人出现频率
	frameCount++;

	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre = 100 + rand() % 100;		//50-249帧出现一次
		creatObstacle();
	}

	/*if (TortoiseExist) {
		TortoiseX -= bgSpeed[2];
		if (TortoiseX < -imgTortoise.getwidth()) {
			TortoiseExist = false;
		}
	}*/

	//更新所有障碍物的坐标
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

	//玩家和障碍物的“碰撞检测”处理
	checkHit();
}

//渲染“游戏背景”
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
	if (_kbhit()){		//如果有按键按下，kbhit（）返回 true
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
	
	//渲染小乌龟
	/*if (TortoiseExist) {
		putimagePNG2(TortoiseX, TortoiseY, WIN_WIDTH, &imgTortoise);
	}*/

	//渲染障碍物
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
		
		//暂停之后，充值复活，或直接下一局
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

		if (timer > 15) {		//刷新速度
			timer = 0;
			update = true;
		}
		 
		if (update) {
			update = false;
			BeginBatchDraw();		//打印完后一次输出

			updateBg();
			//putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
			updateHero();
			updateEnemy();
			updateBloodBar();
			checkOver();


			FlushBatchDraw();

			fly();
		}
		//Sleep(30);		//休眠30ms
	}
	
	system("pause");

	return 0;
}
