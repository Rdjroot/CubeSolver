#ifndef SOLVER_H
#define SOLVER_H

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
#include<memory>
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
    // 单例模式
    static std::shared_ptr<Solver> getInstance() {
            if (!instance) {
                instance = std::shared_ptr<Solver>(new Solver());
            }
            return instance;
     }

    ~Solver();

    vector<string> getSolveLatex(CubieCube originCube);

private:
    // 成员数据
    static std::shared_ptr<Solver> instance;

    CubieCube orgccb;

    // 查找最大深度
    int maxStep;

    // 最多获取结果数
    int answercount = 0;

    // 魔方初始编码
    int initCco, initCeo, initCemp2, initCcp, initCeudp, initCemp;

    // 旋转表映射
    vector<CubieCube> move;
    vector<pair<Direct, int>> phase2Move;

    // 组合数
    const int Nmove = 18;					// 共有18种转动方式(U —— U, U2, U')
    const int NTWIST = 2187;				// 角块可能方向数: 3^7，取值范围∈[0, 3^7 -1]
    const int NFLIP = 2048;					// 棱块可能方向数：2^11

    const int NCP = 40320;					// 角块可能位置数最大值：7!* 7 + ..... + 1! * 1 = 40319
    const int NEUDP = 40320;				// 上下层棱块可能位置数，公式同上，取值范围∈[0, 40319]
    const int NEMP = 24;					// 中间四棱排列最大值：3!*3 + 2!*2 + 1!*1 = 23

    const int NSLICE = 495;					// 中间棱块可能位置数组合：12 x 11 x 10 x 9 / (4!) = 495

    // 组合数表
    int C_NK[12][5];

    // 变化表
    vector<vector<int>> twistMoveTable;
    vector<vector<int>> flipMoveTable;
    vector<vector<int>> sliceMoveTable;
    vector<vector<int>> cpMoveTable;
    vector<vector<int>> eudpMoveTable;
    vector<vector<int>> empMoveTable;

    // 剪枝表
    vector<int> twistPruneTable;
    vector<int> flipPruneTable;
    vector<int> slicePruneTable;

    vector<int> CpPruneTable;
    vector<int> EUndpPruneTable;
    vector<int> EMpPruneTable;

private:
    void initCN_K();

    // 编码与解码
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

    // 初始化变换表
    void initMoveTable(function<int(CubieCube)> encode, function<CubieCube(int)> decode, char multy, vector<vector<int>>& table);
    // 初始化阶段一的剪枝表
    void initPruneTable1(vector<int>& pruneTable, vector<vector<int>>& moveTable);
    // 初始化阶段二的剪枝表
    void initPruneTable2(vector<int>& pruneTable, vector<vector<int>>& moveTable);

    void initTables();

    void searchPartOne(int twist, int flip, int slice, int curDepth, vector<int>& result, vector<int>& steps,int& finishFlag);

    void searchPartTwo(int cp, int eundp, int emp, int curDepth, vector<int>& result, vector<int>& steps, int& finishFlag);

private:
    // 构造函数
    Solver();

};

#endif // SOLVER_H
