#include "CubieCube.h"
#include<iostream>
#include<unordered_map>
using std::cout;
using std::cin;
using std::endl;
using std::unordered_map;

// δ����ħ��
CubieCube::CubieCube()
{
	for (int c = URF; c <= DRB; c++)
	{
		this->co[c] = { URF, 0 };
	}
	for (int e = UR; e <= BR; e++)
	{
		this->eo[e] = { UR,0 };
	}
}

// ��ԭ״̬ħ��
CubieCube::CubieCube(int x)
{
	for (int c = URF; c <= DRB; c++)
	{
		this->co[c] = { Corner(c), 0 };
	}
	for (int e = UR; e <= BR; e++)
	{
		this->eo[e] = { Edge(e),0 };
	}
}

// �����Ź��취
CubieCube::CubieCube(const std::initializer_list<corner_o> crn, const std::initializer_list<edge_o> edg)
{
	int i = 0;
	for (auto c : crn)
	{
		this->co[i++] = c;
	}
	int j = 0;
	for (auto e : edg)
	{
		this->eo[j++] = e;
	}
}

CubieCube::CubieCube(const vector<string>& latex)
{
	vector<CubieCube> move = initMove();
	CubieCube born(0);
	for (auto lt : latex)
	{
		int t = moveMap[lt];
		int d = t / 3;
		for (int i = 0; i <= t % 3; i++)
		{
			born = cubeMove(born, move[d]);
		}
	}

	for (int c = URF; c <= DRB; c++)
	{
		this->co[c] = born.co[c];
	}

	for (int e = UR; e <= BR; e++)
	{
		this->eo[e] = born.eo[e];
	}
}

vector<CubieCube> initMove()
{
	vector<CubieCube> rotate(6);
	rotate[U] = {
			{{UBR,0},{URF,0},{UFL,0},{ULB,0},{DFR,0},{DLF,0},{DBL,0},{DRB,0}},
			{{UB,0},{UR,0},{UF,0},{UL,0},{DR,0},{DF,0},{DL,0},{DB,0},{FR,0},{FL,0},{BL,0},{BR,0}}
	};
	rotate[D] = {
		{{URF,0},{UFL,0},{ULB,0},{UBR,0},{DLF,0},{DBL,0},{DRB,0},{DFR,0}},
		{{UR,0},{UF,0},{UL,0},{UB,0},{DF,0},{DL,0},{DB,0},{DR,0},{FR,0},{FL,0},{BL,0},{BR,0}}
	};
	rotate[L] = {
		{{URF,0},{ULB,1},{DBL,2},{UBR,0},{DFR,0},{UFL,2},{DLF,1},{DRB,0}},
		{{UR,0},{UF,0},{BL,0},{UB,0},{DR,0},{DF,0},{FL,0},{DB,0},{FR,0},{UL,0},{DL,0},{BR,0}}
	};
	rotate[R] = {
		{{DFR,2},{UFL,0},{ULB,0},{URF,1},{DRB,1},{DLF,0},{DBL,0},{UBR,2}},
		{{FR,0},{UF,0},{UL,0},{UB,0},{BR,0},{DF,0},{DL,0},{DB,0},{DR,0},{FL,0},{BL,0},{UR,0}}
	};
	// ����ת��
	rotate[F] = {
		{{UFL,1},{DLF,2},{ULB,0},{UBR,0},{URF,2},{DFR,1},{DBL,0},{DRB,0}},	// �ǿ�
		{{UR, 0},{FL,1},{UL,0},{UB,0},{DR,0},{FR,1},{DL,0},{DB,0},{UF,1},{DF,1},{BL,0},{BR,0}}	// ���
	};
	rotate[B] = {
		{{URF,0},{UFL,0},{UBR,1},{DRB,2},{DFR,0},{DLF,0},{ULB,2},{DBL,1}},
		{{UR,0},{UF,0},{UL,0},{BR,1},{DR,0},{DF,0},{DL,0},{BL,1},{FR,0},{FL,0},{UB,1},{DB,1}}
	};

	return rotate;
}

void cornerMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC)
{
	// ��ת��ʽ
	for (int crn = URF; crn <= DRB; crn++)
	{
		// �ǿ�ֵ�任
		nC->co[crn].c = org->co[way->co[crn].c].c;
		// ����任
		nC->co[crn].o = (org->co[way->co[crn].c].o + way->co[crn].o) % 3;
	}
}

void edgeMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC)
{
	for (int i = UR; i <= BR; i++)
	{
		nC->eo[i].e = org->eo[way->eo[i].e].e;
		nC->eo[i].o = (org->eo[way->eo[i].e].o + way->eo[i].o) % 2;
	}
}

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex)
{
	CubieCube newCube;
	cornerMultiply(&origin, &mvLatex, &newCube);		// �ǿ�任
	edgeMultiply(&origin, &mvLatex, &newCube);			// ���任
	return newCube;
}


// ��ӡcube
void CubieCube::print() const
{
	cout << "Corners: {";
	for (const auto& c : co) {
		cout << "{" << cornerToString(c.c) << "," << c.o << "},";
	}
	cout << "}\nEdges: {";
	for (const auto& e : eo) {
		cout << "{" << edgeToString(e.e) << "," << e.o << "},";
	}
	cout << "}\n";
}

// �ж�����ħ���Ľǿ�ṹ�Ƿ�һ��
bool CubieCube::equalCorner(const CubieCube& other) const
{
	bool p = equalCornerP(other);
	bool o = equalCornerO(other);
	if (p&&o)
		return true;

	return false;
}

// �Ƚ�����ħ���Ľǿ鷽��ֵ�Ƿ�һ��
bool CubieCube::equalCornerO(const CubieCube& other) const
{
	for (int i = 0; i < 8; i++)
	{
		int cur = this->co[i].o;
		int cmp = other.co[i].o;
		if (cur != cmp)
		{
			cout << "Corner Orientation Equal:" << std::boolalpha << false << endl;
			cout << "Compare Failed! Failed index is: " << i << endl;
			return false;
		}
	}
	return true;
}

// �Ƚ�����ħ���Ľǿ�λ��ֵ�Ƿ�һ��
bool CubieCube::equalCornerP(const CubieCube& other) const
{
	for (int i = 0; i < 8; i++)
	{
		Corner cur = this->co[i].c;
		Corner cmp = other.co[i].c;
		if (cur != cmp)
		{
			cout << "Corner Pos Equal:" << std::boolalpha << false << endl;
			cout << "Compare Failed! Failed index is: " << i << endl;
			cout << "this Corner pos: " << cornerToString(cur);
			cout << "; compare Corner pos: " << cornerToString(cmp) << endl;;
			return false;
		}
	}
	return true;
}

// �ж�����ħ�������ṹ�Ƿ�һ��
bool CubieCube::equalEdge(const CubieCube& other) const
{
	int p = equalEdgeP(other);
	int o = equalEdgeO(other);
	if (p && o)
		return true;

	return false;
}

// �Ƚ�����ħ������鷽��ֵ�Ƿ�һ��
bool CubieCube::equalEdgeO(const CubieCube& other) const
{
	for (int i = 0; i < 12; i++)
	{
		int cur = this->eo[i].o;
		int cmp = other.eo[i].o;
		if (cur != cmp)
		{
			cout << "Edge Orientation Equal:" << std::boolalpha << false << endl;
			cout << "Compare Failed! Failed index is: " << i << endl;
			return false;
		}
	}
	return true;
}

// �Ƚ�����ħ�������λ��ֵ�Ƿ�һ��
bool CubieCube::equalEdgeP(const CubieCube& other) const
{
	for (int i = 0; i < 12; i++)
	{
		Edge cur = this->eo[i].e;
		Edge cmp = other.eo[i].e;
		if (cur != cmp)
		{
			cout << "Edge Pos Equal:" << std::boolalpha << false << endl;
			cout << "Compare Failed! Failed index is: " << i << endl;
			cout << "this Edge pos: " << edgeToString(cur);
			cout << "; compare Edge pos: " << edgeToString(cmp) << endl;;
			return false;
		}
	}
	return true;
}

// �Ƚ�����ħ���Ƿ���ȫһ��
bool CubieCube::equal(const CubieCube& other) const
{
	if (equalEdge(other) && equalCorner(other))
		return true;

	return false;
}

// ö��ֵתstring
string CubieCube::cornerToString(Corner c)
{
	static unordered_map<Corner, string> cornerMap = {
		{ URF, "URF" }, { UFL, "UFL" }, { ULB, "ULB" }, { UBR, "UBR" },
		{ DFR, "DFR" }, { DLF, "DLF" }, { DBL, "DBL" }, { DRB, "DRB" }
	};
	return cornerMap[c];
}

string CubieCube::edgeToString(Edge e) {
	static unordered_map<Edge, string> edgeMap = {
		{ UR, "UR" }, { UF, "UF" }, { UL, "UL" }, { UB, "UB" },
		{ DR, "DR" }, { DF, "DF" }, { DL, "DL" }, { DB, "DB" },
		{ FR, "FR" }, { FL, "FL" }, { BL, "BL" }, { BR, "BR" }
	};
	return edgeMap[e];
}


