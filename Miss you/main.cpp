#include<stdio.h>
#include<graphics.h>//之前要先安装EasyX,不然导入这个包会报错
#include<conio.h>
#include<math.h>
#include<time.h>//随机数
#include<mmsystem.h>//多媒体设备接口的头文件
#pragma comment(lib,"winmm.lib")//Media control interface
//这条指令和你在工程中添加"winmm.lib"的功能一样，告诉编译器要导入winmm库

#define NUM 13 //在整个屏幕内,最多出现13个烟雾弹
void Welcome();//进入程序运行界面
void init(int i);//初始化每个小烟花的属性值:比如x,y坐标值,是否发射等
void Load();//加载图片
void Choose(DWORD &t1);//选择要发送的烟花弹
void shoot();//发射烟花弹
void Show(DWORD*);		// 绽放烟花
void Style(DWORD& st1);
void write(DWORD&);
void clear(DWORD*);
struct Jet
{
	int x, y;//烟花弹的坐标
	int hx, hy;//最高点的坐标
	int height;//高:y-hy
	bool isshot;//是否发射
	DWORD t1, t2, dt;//DWORD是无符号的,相当于unsigned long ,
	//用t1,t2,dt控制速度
	IMAGE Img[2];//存放图片的数组
	byte n : 1;//下标	位段:/:指一个位来存放n
	//位段:(bit-field)是以位为单位来定义结构体(或联合体)中的成员变量所占的空间

}jet[NUM];//定义结构体数组
struct Fire
{
	int r;//当前爆炸半径
	int max_r;//爆炸中心距离边缘最大半径
	int x, y;//爆炸中心在窗口的坐标
	int cen_x,cen_y;//爆炸中心相对图片左上角的坐标
	int width;//图片的宽度
	int height;//图片的高度
	int xy[240][240];//储存图片像素点
	bool isshow;//是否显示
	bool isdraw;//是否画出
	DWORD t1, t2, dt;//绽放速度
}fire[NUM];
int main()
{
	initgraph(1000, 600);//初始化一个窗口
	srand((unsigned int)time(NULL));		//为了得到不一样的随机数
	
	Welcome();
	Load();
	DWORD t1 = timeGetTime();			// 筛选烟花计时
	DWORD st1 = timeGetTime();			// 播放花样计时
	DWORD tm = timeGetTime();			//字体播放计时
	DWORD tn;
	DWORD TP;
	DWORD TC;

	DWORD* pMem = GetImageBuffer();		// 获取窗口显存指针,对每个像素点进行操作
	
	for (int i = 0; i < NUM; i++)
	{
		init(i);//初始化每个小烟花的相关数据

	}
	
	Load();								// 将烟花图片信息加载进相应结构中
	IMAGE img;
	loadimage(&img, "./file/情侣图片.jpg", 150, 266);
	BeginBatchDraw();					// 开始批量绘图
	while (!_kbhit())
	{
		Sleep(10);
		clear(pMem);
		TC = timeGetTime();
		if (TC - t1 > 0)
		{
			
			putimage(850, 334, &img);
		}
		Choose(t1);
		
		shoot();
		Show(pMem);
		Style(st1);
		FlushBatchDraw();	// 显示前面的所有绘图操作
	}
	getchar();

	return 0;
}
void Welcome()//程序运行一开始进入的界面
{
	//设置文本(文字)颜色
	mciSendString("open ./file/小曲儿-小幸运.mp3 alias music ", 0, 0, 0);//alias取别名
	mciSendString("play music repeat", 0, 0, 0);
	mciSendString("open ./file/录音1.mp3 alias music1 ", 0, 0, 0);//alias取别名
	mciSendString("play music1", 0, 0, 0);
	settextcolor(YELLOW);
	for (int i = 0; i < 50; i++)
	{
		int x = 200 + int(180 * sin(2 * 3.1415926*i / 60));
		//500指圆心x坐标,180指半径
		int y = 100 + int(180 * cos(2 * 3.1415926*i / 60));
		//300指圆心x坐标,180指半径
		cleardevice();
		settextstyle(i, 0, "华文彩云");//中间为0时它的宽为自适应

		outtextxy(x, y-80, "其实我的心里一直都是你");//在指定的位置输出文本
		outtextxy(x+100, y+100, "我想对你说:");
		Sleep(25);
	}
	Sleep(2000);
	mciSendString("open ./file/录音2.mp3 alias music2 ", 0, 0, 0);//alias取别名
	mciSendString("play music2", 0, 0, 0);
	cleardevice();
	settextstyle(25, 0, "楷体");
	outtextxy(300, 150, "原来你是我最想留住的幸运");
	Sleep(3000);
	outtextxy(300, 200, "原来我们和爱情曾经靠的那么近");
	Sleep(3000);
	outtextxy(300, 250, "那为我对抗世界的决定");
	Sleep(3000);
	outtextxy(300, 300, "那陪我淋的雨");
	Sleep(2000);
	outtextxy(300, 350, "一幕幕都是你");
	Sleep(2000);
	outtextxy(300, 400, "一层不染的真心.");
	Sleep(1000);
	outtextxy(550, 450, "---小幸运");

}
//加载图片
void Load()
{
/****储存烟花的像素点图片****/

	IMAGE fImg, gImg;
	loadimage(&fImg,"./file/flower.jpg",3120,240);
	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&fImg);
		getimage(&gImg, i * 240, 0, 240, 240);
		SetWorkingImage(&gImg);
		for (int a = 0; a < 240; a++)
		{
			for (int b = 0; b < 240; b++)
			{
				fire[i].xy[a][b] = getpixel(a, b);//获取点的颜色
			}
		}

	}
	IMAGE jetImg;//存放图片	
	loadimage(&jetImg,"./file/shoot.jpg",200,50);//加载图片,将图片保存到jetImage对象里面去
	//[2]拆分图片
	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&jetImg);//设置当前的设备,当前设备设为jetImg
		int n = rand() % 5;
		getimage(&jet[i].Img[0],n*20,0,20,50);//得到的是暗色图片
		//getimage()从当前设备获取图片
		//getimage()参数1指将得到的拆分出的图片存放的位置
		//参数2,3指得到的图片在原图片左上角的位置
		//参数4,5指的是新的到的图片的宽和高		下面类推
		getimage(&jet[i].Img[1], (n+5) * 20, 0, 20, 50);//得到的是明亮图片
	}
	SetWorkingImage(NULL);//重将当前设备设置为窗口

}//设置烟花小图片
//初始化,为程序做一些提前的准备
void init(int i)
{
	// 分别为：烟花中心到图片边缘的最远距离、烟花中心到图片左上角的距离 (x、y) 两个分量
	int r[13] = { 120,120,155,123,130,147,138,138,130,135,140,132,155 };
	int x[13] = { 120,120,110,117,110,93,102,102,110,105,100,108,110 };
	int y[13] = { 120,120,85,118,120,103,105,110,110,120,120,104,85 };
	//从这里可以看出中心坐标相对图片相对居中
/**** 初始化烟花 *****/
	fire[i].x = 0;//烟花中心的坐标
	fire[i].y = 0;
	fire[i].width = 240;
	fire[i].height = 240;
	fire[i].max_r = r[i];//最大半径
	fire[i].cen_x = x[i];//爆炸中心相对图片左上角坐标
	fire[i].cen_y = y[i];
	fire[i].isshow = false;
	fire[i].isdraw = false;
	fire[i].dt = 5;//绽放时间间隔
	fire[i].t1 = timeGetTime();
	fire[i].r = 0;//从零开始绽放
/**** 初始化烟花弹 *****/
	jet[i].x = 0;
	jet[i].y = 0;
	jet[i].hx = 0;
	jet[i].hy = 0;
	jet[i].height = 0;
	jet[i].t1 = GetTickCount();//获取系统时间
	jet[i].dt = rand() % 5;//发射速度时间间隔
	jet[i].n = 0;
	jet[i].isshot = false;
}//烟花释放效果图
void Choose(DWORD &t1)//选择烟花弹发射
{
	DWORD t2 = timeGetTime();//获取系统时间;
	if (t2 - t1 > 100)		//每100ms产生一个烟花弹
	{
		
		int n = rand() % 20;//因为产生的随机数不同,所以只能产生唯一的烟花弹
		if (n < 13 && jet[n].isshot == false&&fire[n].isshow==false)
		{
			/***重置烟花弹,预备发射***/
			jet[n].x = rand() % 1000;
			jet[n].y = rand() % 50 + 550;
			jet[n].hx = jet[n].x;
			jet[n].hy = rand() % 300;
			jet[n].height = jet[n].y - jet[n].hy;
			jet[n].isshot = true;
			putimage(jet[n].x, jet[n].y, &jet[n].Img[jet[n].n], SRCINVERT);
			/**** 播放每个烟花弹的声音 *****/
			char c1[50], c2[30], c3[30];
			sprintf_s(c1, "open ./file/shoot.mp3 alias s%d", n);
			sprintf_s(c2, "play s%d", n);
			sprintf_s(c3, "close n%d", n);

			mciSendString(c3, 0, 0, 0);
			mciSendString(c1, 0, 0, 0);
			mciSendString(c2, 0, 0, 0);
		}
		t1 = t2;
	}
}
void shoot()
{
	/**** 烟花弹的上升 *****/	
	for (int i = 0; i < NUM; i++)
	{
		jet[i].t2 = timeGetTime();
		if (jet[i].t2 - jet[i].t1 >= jet[i].dt&&jet[i].isshot==true)
		{
			putimage(jet[i].x, jet[i].y, &jet[i].Img[jet[i].n],SRCINVERT);//SRCINVERT保证图片是透明的
			if (jet[i].y >= jet[i].hy)
			{
				jet[i].n++;
				jet[i].y -= 5;
			}
			putimage(jet[i].x, jet[i].y, &jet[i].Img[jet[i].n], SRCINVERT);
			/**** 上升到高度的 3 / 4，减速 *****/
			if ((jet[i].y - jet[i].hy) * 4 < jet[i].height)
				jet[i].dt = rand() % 4 + 10;
			/**** 上升到最大高度 *****/
			if (jet[i].y <= jet[i].hy)
			{
				// 播放爆炸声
				char c1[50], c2[30], c3[30];
				sprintf_s(c1, "open ./file/bomb.wav alias n%d", i);
				sprintf_s(c2, "play n%d", i);
				sprintf_s(c3, "close s%d", i);

				mciSendString(c3, 0, 0, 0);
				mciSendString(c1, 0, 0, 0);
				mciSendString(c2, 0, 0, 0);
				putimage(jet[i].x, jet[i].y, &jet[i].Img[jet[i].n], SRCINVERT);	// 擦掉烟花弹
				fire[i].x = jet[i].hx + 10;											// 在烟花弹中间爆炸
				fire[i].y = jet[i].hy;												// 在最高点绽放
				fire[i].isshow = true;					// 开始绽放
				jet[i].isshot = false;					// 停止发射
			}
				jet[i].t1 = jet[i].t2;
			//产生一个烟花弹发送一个,产生一个烟花弹发送一个
		}
	}
}
void Show(DWORD* pMem)
{
	//烟花各阶段绽放时间间隔,制作变速绽放效果
	int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };
	for (int i = 0; i < NUM; i++)
	{
		fire[i].t2 = timeGetTime();
		// 增加爆炸半径，绽放烟花，增加时间间隔做变速效果
		if (fire[i].t2 - fire[i].t1 > fire[i].dt && fire[i].isshow == true)
		{

			if (fire[i].r < fire[i].max_r)
			{
				fire[i].r++;
				fire[i].dt = drt[fire[i].r / 10];
				fire[i].isdraw = true;
			}
			if (fire[i].r >= fire[i].max_r - 1)
			{
				fire[i].isdraw = false;
				init(i);//重新初始化,这样就可以循环发送烟雾弹
			}
			fire[i].t1 = fire[i].t2;
			// 如果该号炮花可爆炸，根据当前爆炸半径画烟花，颜色值接近黑色的不输出。
			if (fire[i].isdraw)
			{//参考:
				for (double a = 0; a <= 6.28; a += 0.01)
				{
					int x1 = (int)(fire[i].cen_x + fire[i].r * cos(a));				// 相对于图片左上角的坐标
					int y1 = (int)(fire[i].cen_y - fire[i].r * sin(a));

					if (x1 > 0 && x1 < fire[i].width && y1 > 0 && y1 < fire[i].height)	// 只输出图片内的像素点
					{	//使得形成烟花效果
						int b = fire[i].xy[x1][y1] & 0xff;
						int g = (fire[i].xy[x1][y1] >> 8) & 0xff;
						int r = (fire[i].xy[x1][y1] >> 16);

						// 烟花像素点在窗口上的坐标
						int xx = (int)(fire[i].x + fire[i].r * cos(a));
						int yy = (int)(fire[i].y - fire[i].r * sin(a));

						// 较暗的像素点不输出、防止越界
						if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1000 && yy > 0 && yy < 600)
							pMem[yy * 1000 + xx] = BGR(fire[i].xy[x1][y1]);	// 显存操作绘制烟花
					}
				}
				fire[i].isdraw = false;
			}
		}
	}
}
/****显示花样****/
void Style(DWORD& st1)
{//参考:
	
	DWORD st2 = timeGetTime();//通过控制时间来显示花样烟花时间
	if (st2 - st1 >15000)		// 一首歌的时间
	{
		// 心形坐标
		double x[13] = {45.4,45.7,34.0, 56.7,25.9,65.5,34.6,56.5,40.9,50.6,38.4,52.8, 64.8 };
		double y[13] = {12.5, 35.9,5.0,5.0,13.8,13.8,28.4,28.4, 6.6,6.6,31.6,31.6,10.7 };
		for (int i = 0; i < NUM; i++)
		{
			/**** 规律分布烟花弹 ***/
			jet[i].x = (int)(x[i] * 10);
			jet[i].y = (int)((y[i] + 75) * 10);
			jet[i].hx = (int)(jet[i].x);
			jet[i].hy = (int)(y[i] * 10);
			jet[i].height = (int)(jet[i].y - jet[i].hy);
			jet[i].isshot = true;
			jet[i].dt = 7;
			putimage(jet[i].x, jet[i].y, &jet[i].Img[jet[i].n], SRCINVERT);	// 显示烟花弹

																			/**** 设置烟花参数 ***/
			fire[i].x = (int)(jet[i].x + 10);
			fire[i].y =(int)( jet[i].hy);
			fire[i].isshow = false;
			fire[i].r = 0;

			/**** 播放发射声音 ***/
			char c1[50], c2[30], c3[30];
			sprintf_s(c1, "open ./file/shoot.mp3 alias s%d", i);
			sprintf_s(c2, "play s%d", i);
			sprintf_s(c3, "close n%d", i);

			mciSendString(c3, 0, 0, 0);
			mciSendString(c1, 0, 0, 0);
			mciSendString(c2, 0, 0, 0);
		}
		st1 = st2;
	}
}
void clear(DWORD* pMem)
{
	 pMem = GetImageBuffer();		// 获取窗口显存指针,对每个像素点进行操作
	// 随机选择 4000 个像素点擦除
	for (int clr = 0; clr < 1000; clr++)
	{
		for (int j = 0; j < 2; j++)
		{
			int px1 = rand() % 1000;
			int py1 = rand() % 600;

			if (py1 < 599)				// 防止越界
				pMem[py1 * 1000 + px1] = pMem[py1 * 1000 + px1 + 1] = BLACK;	// 对显存赋值擦出像素点
		}
	}//对像素点进行擦除
}
