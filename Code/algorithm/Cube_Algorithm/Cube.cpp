#include "CubieCube.h"
#include<unordered_map>
#include<iostream>
using std::cout;
using std::cin;
using std::endl;
using std::unordered_map;

CubieCube::CubieCube() :co(8), eo(12)
{}

CubieCube::CubieCube(int x)
{
	for (int c = URF; c <= DRB; c++)
	{
		this->co.push_back({ Corner(c), 0 });
	}
	for (int e = UR; e <= BR; e++)
	{
		this->eo.push_back({ Edge(e),0 });
	}
}

CubieCube::CubieCube(const vector<corner_o>& crn, const vector<edge_o>& edg)
	:co(crn), eo(edg)
{}

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
	if (p && o)
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


