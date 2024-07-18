//#include <bits/stdc++.h>
#include<iostream>
#include <string>
#include<unordered_map>
#include<chrono>
#include<sstream>
#include<fstream>
using namespace std;
typedef enum { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB } 	Corner;
typedef enum { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR } Edge;
typedef enum direc { U, D, L, R, F, B } fangxiang;

struct corner//角块
{
	Corner c;
	int  o;
};

struct edge//棱块
{
	Edge e;
	int o;
};

struct corner1//角块带色相
{
	Corner c;
	int color[3];
	int  o;
};

struct edge1//棱块带色相
{
	Edge e;
	int color[2];
	int o;
};

struct Cube//魔方状态
{
	corner co[8];
	edge   eo[12];
};

struct Cube1//魔方状态带色相
{
	corner1 co1[8];
	edge1   eo1[12];
};

Cube Move[6];
Cube initc;
int idcornero(Cube x)//角块方向编码
{
	int idco = 0;
	for (Corner c = URF; c < DRB; c = Corner(int(c) + 1))
		idco = 3 * idco + x.co[c].o;
	return idco;
}

int idedgeo(Cube x)//棱块方向编码
{
	int ideo = 0;
	for (Edge c = UR; c < BR; c = Edge(int(c) + 1))
		ideo = 2 * ideo + x.eo[c].o;
	return ideo;
}


int idcornerp(Cube x)//角块位置排列编码
{
	int idcp = 0;
	for (int i = DRB; i > URF; i--) {
		int s = 0;
		for (int j = i - 1; j >= URF; j--)
			if (x.co[j].c > x.co[i].c) s++;
		idcp = (idcp + s) * i;
	}
	return idcp;
}

int idedgep(Cube x)//上下层棱块位置排列编码
{
	int idep = 0;
	for (int i = DB; i > UR; i--) {
		int s = 0;
		for (int j = i - 1; j >= UR; j--)
			if (x.eo[j].e > x.eo[i].e) s++;
		idep = (idep + s) * i;
	}
	return idep;
}

int idedgez(Cube x)//中间层棱块位置排列编码
{
	int idez = 0;
	for (int i = BR; i > FR; i--) {
		int s = 0;
		for (int j = i - 1; j >= FR; j--)
			if (x.eo[j].e > x.eo[i].e) s++;
		idez = (idez + s) * (i - 8);
	}
	return idez;
}
int jie[10] = { 1,1,2,6,24,120,720,5040,40320,362880 };
Cube paijiao(int x)
{
	Cube a; int pan[8] = { 0,0,0,0,0,0,0,0 };
	for (int i = 7; i >= 0; i--)
	{
		int s = x / jie[i]; x = x % jie[i]; int shu = s + 1;
		for (int j = 7; j >= 0; j--)
		{
			if (pan[j] == 1) continue;
			shu--;
			if (shu == 0)
			{
				a.co[i].c = Corner(j);
				pan[j] = 1;
			}
		}

	}
	return a;
}

Cube paileng1(int x)
{
	Cube a; int pan[8] = { 0,0,0,0,0,0,0,0 };
	for (int i = 7; i >= 0; i--)
	{
		int s = x / jie[i]; x = x % jie[i]; int shu = s + 1;
		for (int j = 7; j >= 0; j--)
		{
			if (pan[j] == 1) continue;
			shu--;
			if (shu == 0)
			{
				a.eo[i].e = Edge(j);
				pan[j] = 1;
			}
		}

	}
	for (int i = 8; i <= 11; i++)
		a.eo[i].e = Edge(i);
	return a;
}

Cube paileng2(int x)
{
	Cube a; int pan[4] = { 0,0,0,0 };
	for (int i = 3; i >= 0; i--)
	{
		int s = x / jie[i]; x = x % jie[i]; int shu = s + 1;
		for (int j = 3; j >= 0; j--)
		{
			if (pan[j] == 1) continue;
			shu--;
			if (shu == 0)
			{
				a.eo[8 + i].e = Edge(8 + j);
				pan[j] = 1;
			}
		}

	}
	for (int i = 0; i <= 7; i++)
		a.eo[i].e = Edge(i);
	return a;
}

int C[12][5] =//组合数表
{
	400,0,0,0,0,
	1,1,0,0,0,
	1,2,1,0,0,
	1,3,3,1,0,
	1,4,6,4,1,
	1,5,10,10,5,
	1,6,15,20,15,
	1,7,21,35,35,
	1,8,28,56,70,
	1,9,36,84,126,
	1,10,45,120,210,
	1,11,55,165,330,
};

int idslice(Cube x)//中间层四个块的位置编码
{
	int ids = 0, y = 0;
	for (int i = BR; i >= UR; i--)
	{
		if (x.eo[i].e >= FR && x.eo[i].e <= BR)
		{
			ids += C[11 - i][y + 1];
			y++;
		}
	}
	return ids;
}

Cube slicecube(int x)//棱块组合数由编码变块层
{
	Cube a; int y = 4;
	for (int i = 0; i <= 11; i++)
		a.eo[i].e = Edge(0);
	for (int i = UR; i <= BR; i++)
	{
		if (x >= C[11 - i][y])
		{
			a.eo[i].e = Edge(12 - y); x -= C[11 - i][y];
			y--;
		}

	}//cout<<x<<endl;

	return a;
}

Cube twistcube(int x)//角块方向，坐标层次到块层次的转换
{
	Cube a; int tmp = 0;
	for (Corner c = DBL; c >= URF; c = Corner(int(c) - 1))
	{
		a.co[c].o = x % 3; 
		tmp = (x % 3 + tmp) % 3; 
		x /= 3;
	}
	a.co[DRB].o = 3 - tmp;
	return a;
}

Cube  flipcube(int x)//棱边方向，坐标层次到块层次的转换
{
	Cube a; int tmp = 0;
	for (Edge c = BL; c >= UR; c = Edge(int(c) - 1))
	{
		a.eo[c].o = x % 2; tmp = (x % 2 + tmp) % 2; x /= 2;
	}
	a.eo[BR].o = 2 - tmp;
	return a;
}

void cornerMultiply(const Cube* a, const Cube* b, Cube* ab)//角块的方向和位置
{
	for (Corner i = URF; i <= DRB; i = Corner(int(i) + 1))
	{
		ab->co[i].c = a->co[b->co[i].c].c;
		ab->co[i].o = (a->co[b->co[i].c].o + b->co[i].o) % 3;
	}
	return;
}

void edgeMultiply(const Cube* a, const Cube* b, Cube* ab)//棱块的方向和位置
{
	for (Edge i = UR; i <= BR; i = Edge(int(i) + 1))
	{
		ab->eo[i].e = a->eo[b->eo[i].e].e;
		ab->eo[i].o = (a->eo[b->eo[i].e].o + b->eo[i].o) % 2;
	}
	return;
}

void edgeMultiply1(const Cube* a, const Cube* b, Cube* ab)//棱块位置
{
	for (Edge i = UR; i <= BR; i = Edge(int(i) + 1))
	{
		ab->eo[i].e = a->eo[b->eo[i].e].e;
	}
	return;
}

Cube cubeMove(Cube cc, int move)//转动函数
{
	Cube ccret;
	cornerMultiply(&cc, &Move[move], &ccret);
	edgeMultiply(&cc, &Move[move], &ccret);
	return ccret;
}


struct caozuo
{
	direc a;
	int b;
};
caozuo cz[10] = { {U,0},{U,1},{U,2},{D,0},{D,1},{D,2},{L,1},{R,1},{F,1},{B,1} };
const int NTWIST = 2187;
const int NFLIP = 2048;
const int NSLICE = 495;
const int Nmove = 18;
const int NCP = 40320;
const int NEP1 = 40320;
const int NEP2 = 24;
int  twistMoveTable[NTWIST][Nmove];
int  flipMoveTable[NFLIP][Nmove];
int  sliceMoveTable[NSLICE][Nmove];
int  cpMoveTable[NCP][Nmove];
int  epMoveTable1[NEP1][Nmove];
int  epMoveTable2[NEP2][Nmove];

void initcpMoveTable()
{
	vector<std::chrono::duration<double>> ttt;
	static auto two = std::chrono::high_resolution_clock::now();
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NCP - 1; i++)
	{
		a = paijiao(i);
		static auto three = std::chrono::high_resolution_clock::now();
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				cornerMultiply(&a, &Move[j], &b);
				static auto four = std::chrono::high_resolution_clock::now();
				a = b;
				cpMoveTable[i][j * 3 + k] = idcornerp(b);
				static auto five = std::chrono::high_resolution_clock::now();
				const static vector<std::chrono::duration<double>> duration = { three - two,four - three,five - four };
				ttt = duration;
			}
			cornerMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}

	}

	cout << "flags :";
	for (auto d : ttt)
		cout << d.count() << " ,";
	cout << endl;
	return;
}

void initepMoveTable1()
{
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NEP1 - 1; i++)
	{
		a = paileng1(i);
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				edgeMultiply(&a, &Move[j], &b);
				a = b;
				epMoveTable1[i][j * 3 + k] = idedgep(b);
			}
			edgeMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}

	}
	return;
}

void initepMoveTable2()
{
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NEP2 - 1; i++)
	{
		a = paileng2(i);
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				edgeMultiply(&a, &Move[j], &b);
				a = b;//cout<<idedgez(a)<<endl;
				epMoveTable2[i][j * 3 + k] = idedgez(b);
			}
			edgeMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}
	}
	return;
}

void initSliceMoveTable()//初始化中间棱块位置组合转动表
{
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NSLICE - 1; i++)
	{
		a = slicecube(i);
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				edgeMultiply(&a, &Move[j], &b);
				a = b;
				sliceMoveTable[i][j * 3 + k] = idslice(b);
			}
			edgeMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}
	}
	return;
}

void initTwistMoveTable()//初始化角块方向转动表
{
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NTWIST - 1; i++)
	{
		a = twistcube(i);
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				cornerMultiply(&a, &Move[j], &b);
				a = b;
				twistMoveTable[i][j * 3 + k] = idcornero(b);
			}
			cornerMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}
	}
	return;
}

void initFlipMoveTable()//初始化棱块方向转动表
{
	Cube a, b;
	int i, j, move;
	for (int i = 0; i <= NFLIP - 1; i++)
	{
		a = flipcube(i);
		for (int j = U; j <= B; j++) {
			for (int k = 0; k < 3; k++) {
				edgeMultiply(&a, &Move[j], &b);
				a = b;
				flipMoveTable[i][j * 3 + k] = idedgeo(b);
			}
			edgeMultiply(&a, &Move[j], &b); a = b;	//cout<<idcornero(b)<<endl;
		}
	}
	return;
}


int PruneTable1[NTWIST];
int PruneTable2[NFLIP];
int PruneTable3[NSLICE];
int PruneTable4[NCP];
int PruneTable5[NEP1];
int PruneTable6[NEP2];
void initPruneTable1()//角块方向的剪枝表
{
	for (int i = 0; i <= NTWIST - 1; i++)
	{
		PruneTable1[i] = -1;
	}
	int depth = 0;
	PruneTable1[0] = 0;
	int done = 1; int index;
	while (done < NTWIST)
	{
		for (int i = 0; i < NTWIST; i++)
		{
			if (PruneTable1[i] == depth)
			{
				for (int j = 0; j <= 17; j++)
				{
					index = twistMoveTable[i][j];
					if (PruneTable1[index] == -1)
					{
						PruneTable1[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NTWIST << " , the biggest depth is: " << depth << endl;
	return;
}

void initPruneTable2()//棱块方向的剪枝表
{
	for (int i = 0; i <= NFLIP - 1; i++)
	{
		PruneTable2[i] = -1;
	}
	int depth = 0;
	PruneTable2[0] = 0;
	int done = 1; int index;
	while (done < NFLIP)
	{
		for (int i = 0; i < NFLIP; i++)
		{
			if (PruneTable2[i] == depth)
			{
				for (int j = 0; j <= 17; j++)
				{
					index = flipMoveTable[i][j];
					if (PruneTable2[index] == -1)
					{
						PruneTable2[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NFLIP << " , the biggest depth is: " << depth << endl;
	return;
}

void initPruneTable3()//棱块位置组合的剪枝表
{
	for (int i = 0; i <= NSLICE - 1; i++)
	{
		PruneTable3[i] = -1;
	}
	int depth = 0;
	PruneTable3[0] = 0;
	int done = 1; int index;
	while (done < NSLICE)
	{
		for (int i = 0; i < NSLICE; i++)
		{
			if (PruneTable3[i] == depth)
			{
				for (int j = 0; j <= 17; j++)
				{
					index = sliceMoveTable[i][j];
					if (PruneTable3[index] == -1)
					{
						PruneTable3[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NSLICE << " , the biggest depth is: " << depth << endl;
	return;
}

void initPruneTable4()//角块位置的剪枝表
{
	for (int i = 0; i <= NCP - 1; i++)
	{
		PruneTable4[i] = -1;
	}
	int depth = 0;
	PruneTable4[0] = 0;
	int done = 1; int index;
	while (done < NCP)
	{
		for (int i = 0; i < NCP; i++)
		{
			if (PruneTable4[i] == depth)
			{
				for (int j = 0; j < 10; j++)
				{
					int x = cz[j].a; int y = cz[j].b;
					index = cpMoveTable[i][x * 3 + y];
					if (PruneTable4[index] == -1)
					{
						PruneTable4[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NCP << " , the biggest depth is: " << depth << endl;
	return;
}

void initPruneTable5()//上下层棱块位置的剪枝表
{
	for (int i = 0; i <= NEP1 - 1; i++)
	{
		PruneTable5[i] = -1;
	}
	int depth = 0;
	PruneTable5[0] = 0;
	int done = 1; int index;
	while (done < NEP1)
	{
		for (int i = 0; i < NEP1; i++)
		{
			if (PruneTable5[i] == depth)
			{
				for (int j = 0; j < 10; j++)
				{
					int x = cz[j].a; int y = cz[j].b;
					index = epMoveTable1[i][x * 3 + y];
					if (PruneTable5[index] == -1)
					{
						PruneTable5[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NEP1 << " , the biggest depth is: " << depth << endl;
	return;
}

void initPruneTable6()//中层棱块位置的剪枝表
{
	for (int i = 0; i <= NEP2 - 1; i++)
	{
		PruneTable6[i] = -1;
	}
	int depth = 0;
	PruneTable6[0] = 0;
	int done = 1; int index;
	while (done < NEP2)
	{
		for (int i = 0; i < NEP2; i++)
		{
			if (PruneTable6[i] == depth)
			{
				for (int j = 0; j < 10; j++)
				{
					int x = cz[j].a; int y = cz[j].b;
					index = epMoveTable2[i][x * 3 + y];
					if (PruneTable6[index] == -1)
					{
						PruneTable6[index] = depth + 1;
						done++;
					}
				}
			}

		}
		depth++;//cout<<done<<endl;
	}
	cout << "cur table size is: " << NEP2 << " , the biggest depth is: " << depth << endl;
	return;
}

int tranint(char x)
{
	if (x == 'y') return 0;
	if (x == 'w') return 1;
	if (x == 'b') return 2;
	if (x == 'g') return 3;
	if (x == 'r') return 4;
	if (x == 'o') return 5;
}

int jiaokuai(int x, int y, int z)//角块的识别
{
	int a, b, c;
	a = 0; b = 3; c = 4; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 0;
	a = 0; b = 2; c = 4; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 1;
	a = 0; b = 2; c = 5; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 2;
	a = 0; b = 3; c = 5; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 3;
	a = 1; b = 3; c = 4; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 4;
	a = 1; b = 2; c = 4; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 5;
	a = 1; b = 2; c = 5; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 6;
	a = 1; b = 3; c = 5; if ((x == a && y == b && z == c) || (x == a && z == b && y == c) || (y == a && x == b && z == c) || (y == a && z == b && x == c) || (z == a && x == b && y == c) || (z == a && y == b && x == c)) return 7;
}

int lengkuai(int x, int y)
{
	int a, b;
	a = 0; b = 3; if ((x == a && y == b) || (x == b && y == a)) return 0;
	a = 0; b = 4; if ((x == a && y == b) || (x == b && y == a)) return 1;
	a = 0; b = 2; if ((x == a && y == b) || (x == b && y == a)) return 2;
	a = 0; b = 5; if ((x == a && y == b) || (x == b && y == a)) return 3;
	a = 1; b = 3; if ((x == a && y == b) || (x == b && y == a)) return 4;
	a = 1; b = 4; if ((x == a && y == b) || (x == b && y == a)) return 5;
	a = 1; b = 2; if ((x == a && y == b) || (x == b && y == a)) return 6;
	a = 1; b = 5; if ((x == a && y == b) || (x == b && y == a)) return 7;
	a = 4; b = 3; if ((x == a && y == b) || (x == b && y == a)) return 8;
	a = 4; b = 2; if ((x == a && y == b) || (x == b && y == a)) return 9;
	a = 5; b = 2; if ((x == a && y == b) || (x == b && y == a)) return 10;
	a = 5; b = 3; if ((x == a && y == b) || (x == b && y == a)) return 11;
}

int zhongji(int kuai, int wei, int c0, int c1)
{
	if (kuai == 0 && wei == 0)
	{
		if (c0 <= 1) return 0;
		if (c1 <= 1) return 1;
	}
	if (kuai == 0 && wei == 1)
	{
		if (c0 <= 1) return 1;
		if (c1 <= 1) return 0;
	}
	if (kuai == 1 && wei == 0)
	{
		if (c0 >= 2 && c0 <= 3) return 1;
		if (c1 >= 2 && c1 <= 3) return 0;
	}
	if (kuai == 1 && wei == 1)
	{
		if (c0 >= 2 && c0 <= 3) return 0;
		if (c1 >= 2 && c1 <= 3) return 1;
	}
}

Cube getcube1()//通过色相获取魔方初始状态
{
	Cube1 x; Cube y; int tmp[10]; char amp[10];
	for (int i = 1; i <= 9; i++)//上面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[2].color[0] = tmp[1];
	x.eo1[3].color[0] = tmp[2];
	x.co1[3].color[0] = tmp[3];
	x.eo1[2].color[0] = tmp[4];
	x.eo1[0].color[0] = tmp[6];
	x.co1[1].color[0] = tmp[7];
	x.eo1[1].color[0] = tmp[8];
	x.co1[0].color[0] = tmp[9];
	for (int i = 1; i <= 9; i++)//下面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[5].color[0] = tmp[1];
	x.eo1[5].color[0] = tmp[2];
	x.co1[4].color[0] = tmp[3];
	x.eo1[6].color[0] = tmp[4];
	x.eo1[4].color[0] = tmp[6];
	x.co1[6].color[0] = tmp[7];
	x.eo1[7].color[0] = tmp[8];
	x.co1[7].color[0] = tmp[9];

	for (int i = 1; i <= 9; i++)//左面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[2].color[1] = tmp[1];
	x.eo1[2].color[1] = tmp[2];
	x.co1[1].color[2] = tmp[3];
	x.eo1[10].color[0] = tmp[4];
	x.eo1[9].color[0] = tmp[6];
	x.co1[6].color[2] = tmp[7];
	x.eo1[6].color[1] = tmp[8];
	x.co1[5].color[1] = tmp[9];
	
	
	for (int i = 1; i <= 9; i++)//右面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[0].color[1] = tmp[1];
	x.eo1[0].color[1] = tmp[2];
	x.co1[3].color[2] = tmp[3];
	x.eo1[8].color[0] = tmp[4];
	x.eo1[11].color[0] = tmp[6];
	x.co1[4].color[2] = tmp[7];
	x.eo1[4].color[1] = tmp[8];
	x.co1[7].color[1] = tmp[9];

	for (int i = 1; i <= 9; i++)//前面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[1].color[1] = tmp[1];
	x.eo1[1].color[1] = tmp[2];
	x.co1[0].color[2] = tmp[3];
	x.eo1[9].color[1] = tmp[4];
	x.eo1[8].color[1] = tmp[6];
	x.co1[5].color[2] = tmp[7];
	x.eo1[5].color[1] = tmp[8];
	x.co1[4].color[1] = tmp[9];
	
	
	
	
	for (int i = 1; i <= 9; i++)//后面
	{
		cin >> amp[i]; tmp[i] = tranint(amp[i]);
	}
	x.co1[3].color[1] = tmp[1];
	x.eo1[3].color[1] = tmp[2];
	x.co1[2].color[2] = tmp[3];
	x.eo1[11].color[1] = tmp[4];
	x.eo1[10].color[1] = tmp[6];
	x.co1[7].color[2] = tmp[7];
	x.eo1[7].color[1] = tmp[8];
	x.co1[6].color[1] = tmp[9];

	for (int i = 0; i <= 7; i++)
	{
		y.co[i].c = Corner(jiaokuai(x.co1[i].color[0], x.co1[i].color[1], x.co1[i].color[2]));
		for (int j = 0; j <= 2; j++)
		{
			if (x.co1[i].color[j] <= 1)
			{
				y.co[i].o = j; break;
			}
		}
	}

	for (int i = 0; i <= 11; i++)
	{
		y.eo[i].e = Edge(lengkuai(x.eo1[i].color[0], x.eo1[i].color[1]));
		y.eo[i].o = zhongji(y.eo[i].e / 8, i / 8, x.eo1[i].color[0], x.eo1[i].color[1]);
	}
	return y;
}


Cube getcube2()//通过打乱公式获取魔方初始状态
{
	Cube ans;
	for (int i = 0; i <= 7; i++)
	{
		ans.co[i].c = Corner(i); ans.co[i].o = 0;
	}
	for (int i = 0; i <= 11; i++)
	{
		ans.eo[i].e = Edge(i); ans.eo[i].o = 0;
	}

	string a;
	getline(cin, a); getline(cin, a); a = a + " ";
	int len = a.length();
	int hou = -1, qian = 0;
	for (int i = 0; i <= len - 1; i++)
	{
		if (a.at(i) == ' ')
		{
			qian = i;
			int len = qian - hou - 1;
			string x = a.substr(hou + 1, len);
			hou = qian;
			int ci = 0;
			if (x.length() == 1) ci = 1;
			else if (x.substr(1) == "'") ci = 3;
			else if (x.at(1) == '2') ci = 2;
			int lei = 0;
			char tmp = x.at(0);
			switch (tmp)
			{
			case 'U':lei = 0; break;
			case 'D':lei = 1; break;//U1 U1 D1 D D L1
			case 'L':lei = 2; break;
			case 'R':lei = 3; break;
			case 'F':lei = 4; break;
			case 'B':lei = 5; break;
			}
			for (int i = 1; i <= ci; i++)
				ans = cubeMove(ans, lei);

		}

	}

	return ans;

}
struct zhuan
{
	int xulie[10000000];
	int len = 0;
}ans;
int flag = 0; int xu = 0; int minn = 25;
string trans[18] = { "U","U2","U'","D","D2","D'","L","L2","L'",
				  "R","R2","R'","F","F2","F'","B","B2","B'" };

int initccp, initcep1, initcep2;
int pan[10000];
int tryOne_twice = 0, tryTwo_twice = 0;
vector<string> saveSteps;
void search2(int CP, int EP1, int EP2, int cnt, int togo2)//阶段二的搜索
{
	std::ostringstream oss;
	if (flag) return;
	tryTwo_twice++;
	oss << "search2( " << CP << ", " << EP1 << ", " << EP2 << ", " << togo2 << ");";
	if (togo2 == 0)
	{
		if (CP == 0 && EP1 == 0 && EP2 == 0)
		{
			cout << "tryOne_twice: " << tryOne_twice << endl;
			cout << "tryTwo_twice: " << tryTwo_twice << endl;
			xu++; cout << xu << ":" << ans.len << " ";
			for (int i = 1; i <= ans.len; i++)
				cout << trans[ans.xulie[i]] << " ";
			cout << endl; minn = ans.len;
			for (auto s : saveSteps)
			{
				cout << s << endl;
			}
			flag = 1;
			pan[cnt]--;
		}
		return;
	}
	else
	{
		int cp, ep1, ep2;
		for (int i = 0; i <= 9; i++)
		{
			if (flag) return;
			int x = cz[i].a; int y = cz[i].b; int j = 3 * x + y;
			int move = ans.xulie[ans.len];
			if (move / 3 == x)  continue;
			if (move / 3 - x == 1 && (move / 3) % 2 == 1) continue;
			cp = cpMoveTable[CP][j];
			ep1 = epMoveTable1[EP1][j];
			ep2 = epMoveTable2[EP2][j];
			int dist2 = max(max(PruneTable4[cp], PruneTable5[ep1]), PruneTable6[ep2]);
			if (dist2 > togo2 - 1) continue;
			string str = oss.str();
			ans.len++;
			ans.xulie[ans.len] = x * 3 + y;
			saveSteps.push_back(str);
			if (togo2 == 1)
			{
				int stop = 1;
			}
			search2(cp, ep1, ep2, cnt, togo2 - 1);
			saveSteps.pop_back();
			ans.len--;
		}
	}
	return;

}

void search1(int twist, int flip, int slice, int togo1)//阶段一的搜索
{
	std::ostringstream oss;
	oss << "search1( " << twist << ", " << flip << ", " << slice << ", " << togo1 << ");";
	if (togo1 == 0)
	{
		if (twist == 0 && flip == 0 && slice == 0)
		{
			int x = initccp; Cube a = initc, b;
			for (int i = 1; i <= ans.len; i++)
			{
				x = cpMoveTable[x][ans.xulie[i]];
				for (int j = 0; j <= ans.xulie[i] % 3; j++)
				{
					edgeMultiply1(&a, &Move[ans.xulie[i] / 3], &b); a = b;
				}
			}
			int y = idedgep(b), z = idedgez(b);
			//cout<<x<<" "<<y<<" "<<z<<endl;
			int move = max(max(PruneTable4[x], PruneTable5[y]), PruneTable6[z]);
			for (int i = move; i <= minn - ans.len; i++)
			{
				if (flag)	break;
				if (pan[i + ans.len] <= 0) continue;
				search2(x, y, z, i + ans.len, i);
			}
			flag = 0;
		}
		return;
	}
	else
	{
		int flip1, twist1, slice1;
		for (int i = 0; i <= 17; i++)
		{
			int x = ans.xulie[ans.len];
			if (x / 3 == i / 3)  continue;
			if (x / 3 - i / 3 == 1 && (x / 3) % 2 == 1) continue;
			twist1 = twistMoveTable[twist][i];
			flip1 = flipMoveTable[flip][i];
			slice1 = sliceMoveTable[slice][i];
			int dist1 = max(max(PruneTable1[twist1], PruneTable2[flip1]), PruneTable3[slice1]);
			if (dist1 > togo1 - 1) continue;
			string result = oss.str();
			ans.len++;
			ans.xulie[ans.len] = i;
			saveSteps.push_back(result);
			search1(twist1, flip1, slice1, togo1 - 1);
			saveSteps.pop_back();
			ans.len--;
		}
	}
	return;
}
typedef enum { g, w, o, r, y, b } color;

int main()
{
	//{URF,UFL,ULB,UBR,DFR,DLF,DBL,DRB}
	//{UR,UF,UL,UB,DR,DF,DL,DB,FR,FL,BL,BR}
	Move[U] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{UBR,0},{URF,0},{UFL,0},{ULB,0},{DFR,0},{DLF,0},{DBL,0},{DRB,0}},
		  {{UB,0},{UR,0},{UF,0},{UL,0},{DR,0},{DF,0},{DL,0},{DB,0},{FR,0},{FL,0},{BL,0},{BR,0}}
	};
	Move[D] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{URF,0},{UFL,0},{ULB,0},{UBR,0},{DLF,0},{DBL,0},{DRB,0},{DFR,0}},
		  {{UR,0},{UF,0},{UL,0},{UB,0},{DF,0},{DL,0},{DB,0},{DR,0},{FR,0},{FL,0},{BL,0},{BR,0}}
	};
	Move[L] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{URF,0},{ULB,1},{DBL,2},{UBR,0},{DFR,0},{UFL,2},{DLF,1},{DRB,0}},
		  {{UR,0},{UF,0},{BL,0},{UB,0},{DR,0},{DF,0},{FL,0},{DB,0},{FR,0},{UL,0},{DL,0},{BR,0}}
	};
	Move[R] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{DFR,2},{UFL,0},{ULB,0},{URF,1},{DRB,1},{DLF,0},{DBL,0},{UBR,2}},
		  {{FR,0},{UF,0},{UL,0},{UB,0},{BR,0},{DF,0},{DL,0},{DB,0},{DR,0},{FL,0},{BL,0},{UR,0}}
	};
	Move[F] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{UFL,1},{DLF,2},{ULB,0},{UBR,0},{URF,2},{DFR,1},{DBL,0},{DRB,0}},
		  {{UR,0},{FL,1},{UL,0},{UB,0},{DR,0},{FR,1},{DL,0},{DB,0},{UF,1},{DF,1},{BL,0},{BR,0}}
	};
	Move[B] = { //URF槽的元素被UFL槽的元素替代，且顺时针扭转，方向加1
		{{URF,0},{UFL,0},{UBR,1},{DRB,2},{DFR,0},{DLF,0},{ULB,2},{DBL,1}},
		  {{UR,0},{UF,0},{UL,0},{BR,1},{DR,0},{DF,0},{DL,0},{BL,1},{FR,0},{FL,0},{UB,1},{DB,1}}
	};
	initepMoveTable2();
	initTwistMoveTable();
	
	initFlipMoveTable();
	
	initSliceMoveTable();
	unordered_map<int, string> stepMap = { {0,"U"},{1,"U2"},{2,"U'"},
	{3,"D"},{4,"D2"},{5,"D'"},
	{6,"L"},{7,"L2"},{8,"L'"},
	{9,"R"},{10,"R2"},{11,"R'"},
	{12,"F"},{13,"F2"},{14,"F'"},
	{15,"B"},{16,"B2"},{17,"B'"},
	};


	initcpMoveTable();
	initepMoveTable1();
	initepMoveTable2();
	initPruneTable1();
	initPruneTable4();
	initPruneTable2();
	initPruneTable5();
	initPruneTable6();
	initPruneTable3();

	for (int i = 0; i <= 999; i++) pan[i] = 3;
	int n;

	cout << "欢迎来到简化版kociemba algorithm解魔方程序" << endl;
	cout << "请选择表述魔方的初始状态的方式" << endl;
	cout << "方式一:输入魔方六个面的颜色" << endl;
	cout << "方式二:输入标准打乱公式" << endl;
	cout << "选择方式一请输入 1，选择方式二请输入 2" << endl;
	cin >> n;
	if (n == 1)
	{
		cout << "请输入魔方六个面的色块颜色，标准魔方配色（黄，白，蓝，绿，红，橙）分别用 y,w,b,g,r,o 表示" << endl;
		cout << "请输入以空格隔开的54个色块颜色对应的字母，以黄色面为顶面，红色面为前面" << endl;
		cout << "请按上，下，左，右，前，后面的顺序输入，每个面的颜色均以左上角开始，从左到右，从上到下" << endl;
		cout << "关于左上角色块的确定，请固定前面和上面，上面的左上角远离前面，四个侧面的左上角块与上面相邻，下面的左上角块与前面相邻" << endl;
		initc = getcube1();
		ans.xulie[0] = 7890;
		cout << "开始求解" << endl;
		int twist = idcornero(initc);
		int flip = idedgeo(initc);
		int slice = idslice(initc);
		initccp = idcornerp(initc);
		initcep1 = idedgep(initc);
		initcep2 = idedgez(initc);
		int x = max(max(PruneTable1[twist], PruneTable2[flip]), PruneTable3[slice]);
		for (int i = x; i <= minn; i++)
		{
			search1(twist, flip, slice, i);
		}
	}
	if (n == 2)
	{
		cout << "请将所有打乱公式输入到一行，示例 U(顶层顺时针),U'(右上角的'为英文的)(顶层逆时针),U2(顶层180°)UDLRFB" << endl;
		initc = getcube2();
		ans.xulie[0] = 7890;


		cout << "开始求解" << endl;
		int twist = idcornero(initc);
		int flip = idedgeo(initc);
		int slice = idslice(initc);
		initccp = idcornerp(initc);
		initcep1 = idedgep(initc);
		initcep2 = idedgez(initc);
		int x = max(max(PruneTable1[twist], PruneTable2[flip]), PruneTable3[slice]);
		for (int i = x; i <= minn; i++)
		{
			search1(twist, flip, slice, i);
		}
	}
	return 0;
}
