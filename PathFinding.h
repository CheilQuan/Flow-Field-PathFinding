#ifndef _PATHFINDING_H_
#define _PATHFINDING_H_

#include <gl/glut.h>  
#include <iostream>  
#include <windows.h>
#include <string.h>  
#include <sstream> 
#include <set>
#include <time.h>
#include "DebugFunc.h"

using namespace std;  

#pragma comment(lib,"glut32.lib") 

const float WORLD_WIDTH		= 200.0;	//���細�ڵĿ��;
const float WORLD_HEIGHT	= 160.0;	//���細�ڵĸ߶�;
const float WINDOW_WIDTH	= 800.0;	//���ڿ��;
const float WINDOW_HEIGHT	= 500.0;	//���ڸ߶�;
const float GRID_SIZE		= 4.0;		//����Ĵ�С;
const float MOVE_STEP		= 1.8;		//������ƶ�����;
const int MAX_CHAR          = 128;		//�ַ�����󳤶�;
const int MOVE_OBJECT_NUM   = 200;		//�ƶ�Ŀ������;
const int INFI				= 0xFFFFFFF;//����ֵ;

#define GLUT_DISABLE_ATEXIT_HACK 
#define CALC_MOVE_SPEED(_obj, _objIdx)	\
		((_obj).s * MOVE_STEP / gMap[(_objIdx).x][(_objIdx).y].c)     //�����ƶ�Ŀ��ĳ�ʼ�ٶȺ�����λ�õĴ���ֵ�����ƶ�Ŀ����ٶ�;

typedef enum EDirection	//����ö��;
{
	ED_U	= 3,				//��;����ֵ�ɸýڵ��븸�ڵ��������꣨x,y����x*30+y*3����õ�
	ED_D	= -3,				//��;
	ED_L	= -30,				//��;
	ED_R	= 30,				//��;
	ED_UL	= -27,				//����;
	ED_UR	= 33,				//����;
	ED_DL	= -33,				//����;
	ED_DR	= 27,				//����;
	ED_NULL	= 10,				//��;
}EDirection;
typedef enum EGridType
{
	EGT_NORMAL,			//��ͨ����;
	EGT_OPEN,			//open�б��е�����;
	EGT_CLOSE,			//close�б��е�����;
	EGT_OBSTOCLE,		//�ϰ���;
	EGT_DESTINATION,	//�յ�;
}EGridType;

typedef struct SGridInfo 
{
	int		c;				//�þ��������Ĵ���;
	int		pl;				//�Ӹ�����㵽�յ��·������;
	int		d;				//����������������;
	int		t;				//��������;
	SGridInfo()
	{
		c	= 0;
		pl	= INFI;
		d	= ED_NULL;
		t   = EGT_NORMAL;
	}
}SGridInfo;
typedef struct SPoint
{
	GLfloat		x;
	GLfloat		y;
	SPoint(GLfloat ax = 0.0f, GLfloat ay = 0.0f)
	{
		x = ax;
		y = ay;
	}
}SPoint;
typedef struct SCoordinate
{
	int		x;
	int		y;
	SCoordinate(int ax = 0, int ay = 0)
	{
		x = ax;
		y = ay;
	}
	const SCoordinate &operator=(const SCoordinate &d)
	{
		x = d.x;
		y = d.y;
		return *this;
	}
}SCoordinate; 
typedef struct SColorRGB
{
	GLfloat		r;
	GLfloat		g;
	GLfloat		b;
	SColorRGB(GLfloat ar, GLfloat ag, GLfloat ab)
	{
		r = ar;
		g = ag;
		b = ab;
	}
}SColorRGB;
typedef struct SOpenGridInfo
{
	SCoordinate c;
	int			pl;			//·������;

	SOpenGridInfo(const SCoordinate &ac, int l = 0)
	{
		c  = ac;
		pl = l;
	}
	bool operator<(const SOpenGridInfo &o) const
	{
		return pl < o.pl;
	}
}SOpenGridInfo;
typedef struct SMoveObject
{
	SPoint	p;		//����;
	float	s;		//�ƶ��ٶ�;
	SMoveObject(const SPoint ap=SPoint(), float as = 20.0f)
	{
		p = ap;
		s = as;
	}
}SMoveObject;

string Num2String(int i);  
SPoint Index2World(const SCoordinate &idx);
SPoint Pixel2World(const SPoint &pixel);
SCoordinate World2Index(const SPoint &p);

void Initial();  
void PathFindDisplay();
void ReshapeWin(int w, int h);
void MouseClick(int button, int state, int x, int y);
void MouseMove(int x, int y);
void TimeerFunc(int value);
void InitMoveObject();
/***********************��ͼ����**********************/
void DrawMap(int hGridNum, int vGridNum);
void DrawObstacle(int hGridNum, int vGridNum);
void DrawFlowField(int hGridNum, int vGridNum);
void DrawMoveObject(int hGridNum, int vGridNum);
void DrawDestination(const SPoint &dIdx);
/***********************����ͼ��**********************/
void DrawString(string strn); 
void DrawQuads(const SPoint &ldp, const SPoint &urp, const SColorRGB &c,int mode); //���£������������꣬�Լ����Ʊ��߻����;
void DrawLineSurroundQuads(const SCoordinate &idx);   //������Χ�ɵľ���;
void DrawPoint(const SPoint &p, GLint size, const SColorRGB &c);
void DrawArraw(const SPoint &sp, const SPoint & ep);

/***********************Ѱ·�㷨**********************/
void CalcFlowField(const SCoordinate &d, int hGridNum, int vGridNum); //�����յ�;
void ChangeObjectPosition();				//���������ı��ƶ�Ŀ��λ��;
#endif