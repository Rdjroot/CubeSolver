#pragma once
#ifndef CUBIE_CUBE_H
#define CUBIE_CUBE_H
#include<vector>
#include<string>
using std::vector;
using std::string;

// 角块
enum Corner { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
// 棱块
enum Edge { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };

struct corner_o
{
	Corner c;	// 角块
	int o;		// 方向  0，1，2
};

struct edge_o
{
	Edge e;
	int o;		// 方向 0，1
};

class CubieCube
{
public:
	vector<corner_o> co;
	vector<edge_o> eo;

	// 默认构造函数，所有位置和方向未定义的魔方
	CubieCube();

	// 初始状态为还原状态的魔方
	CubieCube(int x);

	// 自定义初始化构造函数
	CubieCube(const vector<corner_o>& crn, const vector<edge_o>& edg);

	void print() const;		// 打印函数，方便调试

	// 比较函数
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
