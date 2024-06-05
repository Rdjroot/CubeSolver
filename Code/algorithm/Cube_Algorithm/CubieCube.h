#pragma once
#ifndef CUBIE_CUBE_H
#define CUBIE_CUBE_H
#include<vector>
#include<unordered_map>
#include<string>
using std::unordered_map;
using std::vector;
using std::string;

// �ǿ�
enum Corner{ URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB } ;
// ���
enum Edge { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };
// ת������
enum Direct { U, D, L, R, F, B };

struct corner_o
{
	Corner c;	// �ǿ�
	int o;		// ����  0��1��2
};

struct edge_o
{
	Edge e;
	int o;		// ���� 0��1
};

static unordered_map<string, int> moveMap = {
	{"U",0},{"U2",1},{"U'",2},
	{"D",3},{"D2",4},{"D'",5},
	{"L",6},{"L2",7},{"L'",8},
	{"R",9},{"R2",10},{"R'",11},
	{"F",12},{"F2",13},{"F'",14},
	{"B",15},{"B2",16},{"B'",17},
};

static unordered_map<int, string> stepMap = { {0,"U"},{1,"U2"},{2,"U'"},
	{3,"D"},{4,"D2"},{5,"D'"},
	{6,"L"},{7,"L2"},{8,"L'"},
	{9,"R"},{10,"R2"},{11,"R'"},
	{12,"F"},{13,"F2"},{14,"F'"},
	{15,"B"},{16,"B2"},{17,"B'"},
};

class CubieCube
{
public:
	corner_o co[8];
	edge_o eo[12];

	// Ĭ�Ϲ��캯��������λ�úͷ���δ�����ħ��
	CubieCube();

	// ��ʼ״̬Ϊ��ԭ״̬��ħ��
	CubieCube(int x);

	// �Զ����ʼ�����캯����Ϊ��MOVEģ��
	CubieCube(const std::initializer_list<corner_o> crn, const std::initializer_list<edge_o> edg);

	CubieCube(const vector<string>& latex);

	void print() const;		// ��ӡ�������������

	// �ȽϺ���
	bool equalCorner(const CubieCube& other) const;
	bool equalCornerO(const CubieCube& other) const;
	bool equalCornerP(const CubieCube& other) const;

	bool equalEdge(const CubieCube& other) const;
	bool equalEdgeO(const CubieCube& other) const;
	bool equalEdgeP(const CubieCube& other) const;

	bool equal(const CubieCube& other) const;

private:
	static string cornerToString(Corner c);
	static string edgeToString(Edge e);

};

vector<CubieCube> initMove();

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex);

void cornerMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

void edgeMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);
#endif