#pragma once
#ifndef CUBIE_CUBE_H
#define CUBIE_CUBE_H
#include<vector>
#include<string>
using std::vector;
using std::string;

// �ǿ�
enum Corner { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
// ���
enum Edge { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };

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

class CubieCube
{
public:
	vector<corner_o> co;
	vector<edge_o> eo;

	// Ĭ�Ϲ��캯��������λ�úͷ���δ�����ħ��
	CubieCube();

	// ��ʼ״̬Ϊ��ԭ״̬��ħ��
	CubieCube(int x);

	// �Զ����ʼ�����캯��
	CubieCube(const vector<corner_o>& crn, const vector<edge_o>& edg);

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


#endif
