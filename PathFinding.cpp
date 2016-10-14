#include "PathFinding.h"

SGridInfo **gMap			= NULL;		//��ͼ;

float gCurWinWidth			= WINDOW_WIDTH;
float gCurWinHeight			= WINDOW_HEIGHT;
int gClickDownX				= -1;		//�ϴ���갴�µ�λ��;��������, -1��ʾ��������¼�;
int gClickDownY				= -1; 
SCoordinate gDest(0, 0);				//�յ�λ��;
SMoveObject gObjectPosition[MOVE_OBJECT_NUM];//�ƶ�Ŀ���λ��;

void SetObstocle(int x, int y)
{
	if (x < 0 || y < 0 || x >= WORLD_WIDTH/GRID_SIZE || y >= WORLD_HEIGHT/GRID_SIZE)
	{
		return;
	}
	gMap[x][y].c = INFI;
	gMap[x][y].d = ED_NULL;
	gMap[x][y].t = EGT_OBSTOCLE;
}
void SetDestination(const SCoordinate &d)
{
	if (d.x < 0 || d.y < 0 || d.x >= WORLD_WIDTH/GRID_SIZE || d.y >= WORLD_HEIGHT/GRID_SIZE)
	{
		return;
	}
	gMap[gDest.x][gDest.y].t	= EGT_NORMAL;
	gMap[gDest.x][gDest.y].pl	= 0; 
	gMap[d.x][d.y].d			= ED_NULL;
	gMap[d.x][d.y].t			= EGT_DESTINATION;
	gDest = d;
}
void RecoverGridType()
{
	for (int x = 0; x < WORLD_WIDTH/GRID_SIZE; ++x)
	{
		for (int y = 0; y < WORLD_HEIGHT/GRID_SIZE; ++y)
		{
			if (EGT_DESTINATION != gMap[x][y].t && EGT_OBSTOCLE != gMap[x][y].t)
			{
				gMap[x][y].pl = INFI;
				gMap[x][y].t  = EGT_NORMAL;
			}
		}
	}
	gMap[gDest.x][gDest.y].pl = 0;
}

string Num2String(int i){  
	stringstream ss;  
	ss << i;  
	return ss.str();  
}  
void DrawString(const string &strn) {  
	static int isFirstCall = 1;  
	static GLuint lists;  
	const char *str = strn.c_str();  
	if (isFirstCall) { // ����ǵ�һ�ε��ã�ִ�г�ʼ��;  
		// Ϊÿһ��ASCII�ַ�����һ����ʾ�б�;  
		isFirstCall = 0;  

		// ����MAX_CHAR����������ʾ�б���; 
		lists = glGenLists(MAX_CHAR);  

		// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���;  
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);  
	}  
	// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�;  
	for (; *str != '\0'; ++str)  
		glCallList(lists + *str);  
}  
SPoint Index2World(const SCoordinate &idx)
{
	float x = idx.x * GRID_SIZE + GRID_SIZE / 2;
	float y = idx.y * GRID_SIZE + GRID_SIZE / 2;
	x = min(x, WORLD_WIDTH);
	x = max(x, 0);
	y = min(y, WORLD_HEIGHT);
	y = max(y, 0);
	
	return SPoint(x, y);
}
SPoint Pixel2World(const SPoint &pixel)
{
	float x = pixel.x / gCurWinWidth * WORLD_WIDTH;
	float y = pixel.y / gCurWinHeight * WORLD_HEIGHT;
	x = min(x, WORLD_WIDTH);
	x = max(x, 0);
	y = min(y, WORLD_HEIGHT);
	y = max(y, 0);
	y = abs(y - WORLD_HEIGHT);

	return SPoint(x, y);
}
SCoordinate World2Index(const SPoint &p)
{
	int x = p.x / GRID_SIZE;
	int y = p.y / GRID_SIZE;
	x = min(x, WORLD_WIDTH/GRID_SIZE - 1);
	x = max(x, 0);
	y = min(y, WORLD_HEIGHT/GRID_SIZE - 1);
	y = max(y, 0);

	return SCoordinate(x, y);
}

void InitMap(int hGridNum, int vGridNum)
{
	srand((unsigned)time(NULL));
	if (NULL == gMap)
	{
		gMap = new SGridInfo*[hGridNum];
		for (int x = 0; x < hGridNum; ++x)
		{	
			gMap[x] = new SGridInfo[vGridNum];
			for (int y = 0; y < vGridNum; ++y)
			{
				gMap[x][y].d = ED_D;
				//gMap[x][y].c = rand() % 10 + 15;	//�Ǿ��ȵ�ͼ��ÿ���ڵ�Ĵ��۷�Χ��15~25��;
				gMap[x][y].c = 20;				//���ȵ�ͼ;
			}
		}

		// �����ϰ���;
		for (int x = hGridNum/5; x <= hGridNum*4/5; ++x)
		{
			SetObstocle(x, vGridNum/2);
		}
		for (int y = vGridNum/4; y <= vGridNum*3/4; ++y)
		{
			SetObstocle(hGridNum/2, y);
		}

		SetDestination(gDest);
		CalcFlowField(gDest, hGridNum, vGridNum);
		InitMoveObject();
	}
}
void InitMoveObject()
{
	for (int i = 0; i < MOVE_OBJECT_NUM; ++i)
	{
		gObjectPosition[i].p.x = rand() % (int)WORLD_WIDTH;
		gObjectPosition[i].p.y = rand() % (int)WORLD_HEIGHT;
		gObjectPosition[i].s   = rand() % 10 + 5;
	}
}
void ReleaseMap(int hGridNum)
{
	if (NULL != gMap)
	{
		for (int i = 0; i < hGridNum; ++i)
		{
			delete []gMap[i];
		}
		delete []gMap;
		gMap = NULL;
	}
}
void PathFindDisplay()
{
	int hGridNum = WORLD_WIDTH/GRID_SIZE;		//ˮƽ�ʹ�ֱ����������;
	int vGridNum = WORLD_HEIGHT/GRID_SIZE;

	InitMap(hGridNum, vGridNum);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	DrawMap(hGridNum, vGridNum);
	DrawObstacle(hGridNum, vGridNum);
	DrawDestination(Index2World(gDest));
	DrawFlowField(hGridNum, vGridNum);
	DrawMoveObject(hGridNum, vGridNum);
	glFlush();
	glutSwapBuffers();
}

void DrawMap(int hGridNum, int vGridNum)
{
	//������ɫ;  
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.5f, 0.9f, 0.89f); 
#pragma region verticesArray
/*#define VERTEX_ARRAY 0
#if VERTEX_ARRAY
	glEnableClientState(GL_VERTEX_ARRAY);
	static GLfloat *fVertices = NULL;
	int verCnt = 4 * (hGridNum + vGridNum + 2);
	static bool firstCall = true;
	if (firstCall)
	{
		GLfloat xCoor = 0.0f;
		fVertices = new GLfloat[verCnt];
		for (int x = 0; x <= 4*hGridNum; x += 4)
		{
			fVertices[x] = xCoor;
			fVertices[x + 1] = 0.0f;
			fVertices[x + 2] = xCoor;
			fVertices[x + 3] = WORLD_HEIGHT;

			xCoor += GRID_SIZE;
		}
		firstCall = false;
	}
	glVertexPointer(2, GL_FLOAT, 0, fVertices);
#endif
*/
#pragma endregion  
	// ���ƻ�������;
	GLfloat lineWidth = 0.5f;  
	GLfloat xCoor = 0.0f;
	GLfloat yCoor = 0.0f; 
	glLineWidth(lineWidth);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
		for (int x = 0; x < hGridNum; ++x)
		{
			for (int y = 0; y < vGridNum; ++y)
			{
				glVertex2f(xCoor, yCoor);
				glVertex2f(xCoor + GRID_SIZE, yCoor);
				glVertex2f(xCoor + GRID_SIZE, yCoor + GRID_SIZE);
				glVertex2f(xCoor, yCoor + GRID_SIZE);
				yCoor += GRID_SIZE;
			}
			xCoor += GRID_SIZE;
			yCoor = 0.0f;
		}	
	glEnd();
/*
	glBegin(GL_LINES);  
#if VERTEX_ARRAY
	 	for (int i = 0; i <= 2 * hGridNum; ++i)
	 	{
	 		glArrayElement(i);
	 	}
#else
  		for (int x = 0; x <= hGridNum; ++x)
 		{
 			glVertex2f(xCoor, 0.0f);  
 			glVertex2f(xCoor, WORLD_HEIGHT);  
 			xCoor += GRID_SIZE;
 		}
 		for (int y = 0; y <= vGridNum; ++y)
 		{
 			glVertex2f(0.0f, yCoor);  
 			glVertex2f(WORLD_WIDTH, yCoor);
 			yCoor += GRID_SIZE;
 		}

	glEnd(); 
#endif*/
}
void DrawObstacle(int hGridNum, int vGridNum)
{
	for (int x = 0; x < hGridNum; ++x)
	{
		for (int y = 0; y < vGridNum; ++y)
		{
			if (gMap[x][y].c == INFI)
			{
				DrawLineSurroundQuads(SCoordinate(x, y));
			}
		}
	}
}
void DrawDestination(const SPoint &dIdx)
{
	DrawPoint(dIdx, 7, SColorRGB(0.0f, 0.0f, 1.0f));
}
void DrawFlowField(int hGridNum, int vGridNum)
{
	for (int x = 0; x < hGridNum; ++x)
	{
		for (int y = 0; y < vGridNum; ++y)
		{
			SPoint sp = Index2World(SCoordinate(x, y));
			switch(gMap[x][y].d)
			{
			case ED_U:
				DrawArraw(sp, SPoint(sp.x, sp.y + GRID_SIZE/2));
				break;
			case ED_D:
				DrawArraw(sp, SPoint(sp.x, sp.y - GRID_SIZE/2));
				break;
			case ED_L:
				DrawArraw(sp, SPoint(sp.x - GRID_SIZE/2, sp.y));
				break;
			case ED_R:
				DrawArraw(sp, SPoint(sp.x + GRID_SIZE/2, sp.y));
				break;
			case ED_UL:
				DrawArraw(sp, SPoint(sp.x - GRID_SIZE/2, sp.y + GRID_SIZE/2));
				break;
			case ED_UR:
				DrawArraw(sp, SPoint(sp.x + GRID_SIZE/2, sp.y + GRID_SIZE/2));
				break;
			case ED_DL:
				DrawArraw(sp, SPoint(sp.x - GRID_SIZE/2, sp.y - GRID_SIZE/2));
				break;
			case ED_DR:
				DrawArraw(sp, SPoint(sp.x + GRID_SIZE/2, sp.y - GRID_SIZE/2));
				break;
			default:
				break;
			}
		}
	}
}
void DrawMoveObject(int hGridNum, int vGridNum)
{
	for (int i = 0; i < MOVE_OBJECT_NUM; ++i)
	{
		DrawPoint(gObjectPosition[i].p, 4, SColorRGB(0.0f, 1.0f, 1.0f));
	}
}

void DrawQuads(const SPoint &ldp, const SPoint &urp, const SColorRGB &c, int mode)
{
	glColor3f(c.r, c.g, c.b);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glBegin(GL_QUADS);
		glVertex2f(ldp.x, ldp.y);
		glVertex2f(ldp.x, urp.y);
		glVertex2f(urp.x, urp.y);
		glVertex2f(urp.x, ldp.y);
	glEnd();
}
void DrawLineSurroundQuads(const SCoordinate &idx)
{
	if (idx.x < 0 || idx.y < 0)
	{
		return;
	}
	int x = idx.x * GRID_SIZE;
	int y = idx.y * GRID_SIZE;
	
	DrawQuads(SPoint(x, y), SPoint(x + GRID_SIZE, y + GRID_SIZE),
			  SColorRGB(1.0f, 0.0f, 0.0f), GL_FILL);

	DrawQuads(SPoint(x, y), SPoint(x + GRID_SIZE, y + GRID_SIZE),
			  SColorRGB(0.0f, 0.0f, 0.0f), GL_LINE);
}
void DrawPoint(const SPoint &p, GLint size, const SColorRGB &c)
{
	glColor3f(c.r, c.g, c.b);
	glPointSize(size);
	glBegin(GL_POINTS);
		glVertex2f(p.x, p.y);
	glEnd();
}
void DrawArraw(const SPoint &sp, const SPoint &ep)  //������������;
{
	DrawPoint(sp, 2, SColorRGB(0.5f, 0.1f, 0.3f));

	glColor3f(0.5f, 0.5f, 0.5f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glVertex2f(sp.x, sp.y);
		glVertex2f(ep.x, ep.y);
	glEnd();
}

void TimeerFunc(int value)
{
	ChangeObjectPosition();
	PathFindDisplay();
	glutTimerFunc(40, TimeerFunc, 1);
}
void MouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		SCoordinate   ci;			//���λ�õ�map����;
		static SPoint cp;			//�����ʱλ�õ����ص�λ��;
		switch (state)
		{
		case GLUT_DOWN:
#if MYDEBUG
			ci = World2Index(Pixel2World(SPoint(x, y)));    //�����������;			
#endif
			gClickDownX = cp.x = x; 
			gClickDownY = cp.y = y;
			PRINT("Mouse clicked down point (%d,%d):(%d,%d)\n",x, y, ci.x, ci.y);
			break;

		case GLUT_UP:
			ci = World2Index(Pixel2World(SPoint(x, y)));    //���������;
			PRINT("Mouse clicked up point (%d,%d):(%d,%d)\n",x, y, ci.x, ci.y); 
			if (abs(x - cp.x) < GRID_SIZE*gCurWinWidth/WORLD_WIDTH*1.0/2.0 &&
				abs(y - cp.y) < GRID_SIZE*gCurWinHeight/WORLD_HEIGHT*1.0/2.0
				)				//��������ק��ƫ����<1/2��������Ϊ����Ŀ���;
			{
				if (gMap[ci.x][ci.y].c != INFI)
				{
					//����յ�;
					glClear(GL_COLOR_BUFFER_BIT);
					SetDestination(ci);
					CalcFlowField(ci, WORLD_WIDTH/GRID_SIZE, WORLD_HEIGHT/GRID_SIZE);
					PathFindDisplay();				//������������;
					glFlush();
				}
			}
			gClickDownY = gClickDownX = cp.x = cp.y = -1;
			break;

		default:
			break;
		}
	}
}
void MouseMove(int x, int y)
{
	if (-1 == gClickDownY && -1 == gClickDownX)
	{
		return;
	}
	
	SCoordinate dIdx = World2Index(Pixel2World(SPoint(x, y)));  //��ǰ���λ�ö�Ӧ��map����;
	bool drawObs = false;
	if (abs(x - gClickDownX) >= GRID_SIZE*gCurWinWidth/WORLD_WIDTH*1.0/2.0)  //x���򻬶�;
	{
		int derction = (x - gClickDownX)/abs(x - gClickDownX);
		gClickDownX = x + derction*GRID_SIZE*gCurWinWidth/WORLD_WIDTH*1.0/2.0;
		gClickDownX = max(gClickDownX, 0);
		gClickDownX = min(gClickDownX, gCurWinWidth);
		drawObs = true;
	}
	if (abs(y - gClickDownY) >= GRID_SIZE*gCurWinHeight/WORLD_HEIGHT*1.0/2.0) //y���򻬶�;
	{
		int direction = (y - gClickDownY)/abs(y - gClickDownY);
		gClickDownY = y + direction*GRID_SIZE*gCurWinHeight/WORLD_HEIGHT*1.0/2.0;
		gClickDownY = max(gClickDownY, 0);
		gClickDownY = min(gClickDownY, gCurWinHeight);
		drawObs = true;				
 	}
	if (drawObs)
	{	
		if (EGT_CLOSE == gMap[dIdx.x][dIdx.y].t)
		{
			SetObstocle(dIdx.x, dIdx.y);
			CalcFlowField(gDest, WORLD_WIDTH/GRID_SIZE, WORLD_HEIGHT/GRID_SIZE);
			PathFindDisplay();
			glFlush();
		}
	}
}

void Initial()  
{  
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);				//������ɫ;  
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();									//�ص�ԭ��;						
	gluOrtho2D(0.0, WORLD_WIDTH, 0.0, WORLD_HEIGHT);	//ͶӰ���ü�����С������;  
}  
void ReshapeWin(int w, int h)
{
	GLfloat aspectRatio=(GLfloat)w/(GLfloat)h;
	if(w <= h)
	{
		gluOrtho2D(0.0, WORLD_WIDTH, 0.0, WORLD_HEIGHT/aspectRatio);
	}
	else
	{
		gluOrtho2D(0.0, WORLD_WIDTH*aspectRatio, 0.0, WORLD_HEIGHT);
	}
	if (w != gCurWinWidth)
	{
		gCurWinWidth  = w;
		gCurWinWidth = max(1, gCurWinWidth);
	}
	if (h != gCurWinHeight)
	{
		gCurWinHeight = h;
		gCurWinHeight = max(1, gCurWinHeight);
	}
}
SPoint JumpToSuitablePos(const SCoordinate &curIdx)		//���ݸõ���Χ���ϰ�������������ƶ�Ŀ�����Ծ��λ��;
{
	return SPoint(rand() % 6 - 3, rand() % 6 - 3);		//ʵ�ʹ����д��������;
}
void ChangeObjectPosition()
{
	if (NULL == gMap)
	{
		return;
	}
	JumpToSuitablePos(SCoordinate(1,1));
	for (int i = 0; i < MOVE_OBJECT_NUM; ++i)
	{
		SCoordinate op = World2Index(gObjectPosition[i].p);
		float		moveSpeed = CALC_MOVE_SPEED(gObjectPosition[i], op);
		switch (gMap[op.x][op.y].d)
		{
		case ED_U:
			gObjectPosition[i].p.y += moveSpeed;
			break;
		case ED_D:
			gObjectPosition[i].p.y -= moveSpeed;
			break;
		case ED_L:
			gObjectPosition[i].p.x -= moveSpeed;
			break;
		case ED_R:
			gObjectPosition[i].p.x += moveSpeed;
			break;
		case ED_UL:
			gObjectPosition[i].p.x -= moveSpeed;
			gObjectPosition[i].p.y += moveSpeed;
			break;
		case ED_UR:
			gObjectPosition[i].p.x += moveSpeed;
			gObjectPosition[i].p.y += moveSpeed;
			break;
		case ED_DL:
			gObjectPosition[i].p.x -= moveSpeed;
			gObjectPosition[i].p.y -= moveSpeed;
			break;
		case ED_DR:
			gObjectPosition[i].p.x += moveSpeed;
			gObjectPosition[i].p.y -= moveSpeed;
			break;
		default:			//������ĳ���޷���ĵ㣬���ڸõ���Χ����;
			{
				SPoint offset = JumpToSuitablePos(op);
				gObjectPosition[i].p.x += offset.x;
				gObjectPosition[i].p.y += offset.y;
				break;

			}
		}
		gObjectPosition[i].p.x = max(1, gObjectPosition[i].p.x);
		gObjectPosition[i].p.x = min(WORLD_WIDTH, gObjectPosition[i].p.x);
		gObjectPosition[i].p.y = max(1, gObjectPosition[i].p.y);
		gObjectPosition[i].p.y = min(WORLD_HEIGHT, gObjectPosition[i].p.y);
	}
}

/*********��������**********/
bool IsCorner(const SCoordinate &p, const SCoordinate &s)   //�Ƿ����ϰ���սǴ��������ֹб�ԽǴ�Խ;
{
	int x = p.x - s.x;
	int y = p.y - s.y;
	
	if (x != 0 && y != 0)  //б�Խǹ�ϵ;
	{
		if (EGT_OBSTOCLE == gMap[s.x + x][s.y].t)
		{
			return true;
		}
		if (EGT_OBSTOCLE == gMap[s.x][s.y + y].t)
		{
			return true;
		}
		else 
			return false;
	}
	return false;
}
int CalcCost(const SCoordinate &p, const SCoordinate &s)   //·�����ۺ�������;
{
	int dirCost = 10 * sqrt((float)(abs(p.x-s.x)+abs(p.y-s.y)));		//����ͷ�;б�Խǳͷ�ϵ������ˮƽ��ֱ����;
	return gMap[p.x][p.y].pl + gMap[s.x][s.y].c + dirCost; 
}
int ParentDirection(const SCoordinate &p, const SCoordinate &s)  //ͨ�����ڵ���Լ���������㷽��;
{	
	int x = p.x - s.x;
	int y = p.y - s.y;

	return (30*x + 3*y);
}
void UpdateOpenList(multiset<SOpenGridInfo> &openList, const SCoordinate &cneterIdx)
{
	int sx, sy, ex, ey;
	sx = max(cneterIdx.x - 1, 0);
	sy = max(cneterIdx.y - 1, 0);
	ex = min(cneterIdx.x + 1, WORLD_WIDTH/GRID_SIZE - 1);
	ey = min(cneterIdx.y + 1, WORLD_HEIGHT/GRID_SIZE - 1);
	for (int x = sx; x <= ex; ++x)
	{
		for (int y = sy; y <= ey; ++y)
		{
			SGridInfo &curGrid = gMap[x][y];
			if (EGT_NORMAL == curGrid.t)    //��ͨ�ڵ�;
			{
				if (IsCorner(cneterIdx, SCoordinate(x, y)))		//�ǹսǵ�б�Խǵ㣬������;
				{
					continue;
				}
				int cost = CalcCost(cneterIdx, SCoordinate(x, y));
				openList.insert(SOpenGridInfo(SCoordinate(x, y), cost));
				curGrid.t  = EGT_OPEN;
				curGrid.pl = cost; 
				curGrid.d  = ParentDirection(cneterIdx, SCoordinate(x, y));
			}
			else if (EGT_CLOSE == curGrid.t)
			{
				int cost = CalcCost(cneterIdx, SCoordinate(x, y));
				if (cost < curGrid.pl)
				{
					curGrid.pl = cost;
					curGrid.d  = ParentDirection(cneterIdx, SCoordinate(x, y));
				}
			}
		}
	}
}
void CalcFlowField(const SCoordinate &d, int hGridNum, int vGridNum)
{
 	RecoverGridType();
	multiset<SOpenGridInfo> openList;				//������ؼ��ϣ���������;
	openList.insert(SOpenGridInfo(d, gMap[d.x][d.y].c));
	SCoordinate		curIdx = d;						//��ǰѡ�е�����·���ڵ�;
	while (!openList.empty()) 
	{
		openList.erase(openList.begin());			//��ǰ�����closeList;
		gMap[curIdx.x][curIdx.y].t = EGT_CLOSE;  

		UpdateOpenList(openList, curIdx);			//���µ�ǰ����Χ�ĵ㵽OpenList;
		
		if (openList.empty())
		{
			break;
		}
		curIdx = openList.begin()->c;				//ѡ��ǰopenlist�����ŵ�·����;
	}
	gMap[gDest.x][gDest.y].t = EGT_DESTINATION;		//�ָ��յ��״̬;
}
