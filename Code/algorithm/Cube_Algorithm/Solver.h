#pragma once
#include<unordered_map>
#include<string>
#include<functional>
#include<vector>
#include "CubieCube.h"
#include<utility>
#include<set>
#include<chrono>
#include<iostream>
#include<cmath>;
using std::cin;
using std::endl;
using std::cout;
using std::function;
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::unordered_map;

class Solver
{
public:
	// ����ģʽ
	static Solver* getInstance() {
		if (instance == nullptr)
			instance = new Solver();
		return instance;
	}

	~Solver();

	vector<string> getSolveLatex(CubieCube originCube);

private:
	// ��Ա����
	static Solver* instance;

	CubieCube orgccb;

	// ����������
	int maxStep;

	// ����ȡ�����
	int answercount = 0;

	// ħ����ʼ����
	int initCco, initCeo, initCemp2, initCcp, initCeudp, initCemp;

	// ��ת��ӳ��
	vector<CubieCube> move;
	vector<pair<Direct, int>> phase2Move;

	// �����
	const int Nmove = 18;					// ����18��ת����ʽ(U ���� U, U2, U')
	const int NTWIST = 2187;				// �ǿ���ܷ�����: 3^7��ȡֵ��Χ��[0, 3^7 -1]
	const int NFLIP = 2048;					// �����ܷ�������2^11

	const int NCP = 40320;					// �ǿ����λ�������ֵ��7!* 7 + ..... + 1! * 1 = 40319
	const int NEUDP = 40320;				// ���²�������λ��������ʽͬ�ϣ�ȡֵ��Χ��[0, 40319]
	const int NEMP = 24;					// �м������������ֵ��3!*3 + 2!*2 + 1!*1 = 23 

	const int NSLICE = 495;					// �м�������λ������ϣ�12 x 11 x 10 x 9 / (4!) = 495

	// �������
	int C_NK[12][5];

	// �仯��
	vector<vector<int>> twistMoveTable;
	vector<vector<int>> flipMoveTable;
	vector<vector<int>> sliceMoveTable;
	vector<vector<int>> cpMoveTable;
	vector<vector<int>> eudpMoveTable;
	vector<vector<int>> empMoveTable;

	// ��֦��
	vector<int> twistPruneTable;
	vector<int> flipPruneTable;
	vector<int> slicePruneTable;

	vector<int> CpPruneTable;
	vector<int> EUndpPruneTable;
	vector<int> EMpPruneTable;

private:
	void initCN_K();

	// ���������
	int encodeCornerO(CubieCube x);
	CubieCube decodeCornerO(int x);
	int encodeEdgeO(CubieCube x);
	CubieCube decodeEdgeO(int x);
	int encodeCornerP(CubieCube x);
	CubieCube decodeCornerP(int x);
	int encodeEdgeUnDP(CubieCube x);
	CubieCube decodeEdgeUnDP(int x);
	int encodeEdgeMP(CubieCube x);
	CubieCube decodeEdgeMP(int x);
	int encodeEdgeMP2(CubieCube x);
	CubieCube decodeEdgeMP2(int x);

	// ��ʼ���任��
	void initMoveTable(function<int(CubieCube)> encode, function<CubieCube(int)> decode, char multy, vector<vector<int>>& table);
	// ��ʼ���׶�һ�ļ�֦��
	void initPruneTable1(vector<int>& pruneTable, vector<vector<int>>& moveTable);
	// ��ʼ���׶ζ��ļ�֦��
	void initPruneTable2(vector<int>& pruneTable, vector<vector<int>>& moveTable);
	
	void initTables();

	void searchPartOne(int twist, int flip, int slice, int curDepth, vector<int>& result, vector<int>& steps,int& finishFlag);

	void searchPartTwo(int cp, int eundp, int emp, int curDepth, vector<int>& result, vector<int>& steps, int& finishFlag);

private:
	// ���캯��
	Solver();

};

