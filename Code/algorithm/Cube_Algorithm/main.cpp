#include<iostream>
#include "CubieCube.h"		// 魔方结构 与转动
//#include "Move1.h"			// 方向转动
#include<algorithm>
#include<set>
#include<functional>
#include<chrono>
#include<cmath>
#include<algorithm>
#include<utility>
#include<unordered_map>
#include<sstream>
#include<thread>
#include<fstream>

using std::pair;
using std::string;
using std::to_string;
using std::endl;
using std::cout;
using std::function;
using std::unordered_map;
using std::fstream;

unordered_map<int, string> stepMap = { {0,"U"},{1,"U2"},{2,"U'"},
	{3,"D"},{4,"D2"},{5,"D'"},
	{6,"L"},{7,"L2"},{8,"L'"},
	{9,"R"},{10,"R2"},{11,"R'"},
	{12,"F"},{13,"F2"},{14,"F'"},
	{15,"B"},{16,"B2"},{17,"B'"},
};

unordered_map<string, int> moveMap = {
	{"U",0},{"U2",1},{"U'",2},
	{"D",3},{"D2",4},{"D'",5},
	{"L",6},{"L2",7},{"L'",8},
	{"R",9},{"R2",10},{"R'",11},
	{"F",12},{"F2",13},{"F'",14},
	{"B",15},{"B2",16},{"B'",17},
};

vector<CubieCube> move = initMove();

/** 角块方向编码
* 角块的方向有三种，所以基底为3
* 3^(6-c)*o1 + .... + 3^(6-c)*o7 (o ∈ [0,6])
* 因为当七个角块确定方向以后，最后一个也确定了
*/
int encodeCornerO(CubieCube x)
{
	short int idx_co = 0;
	for (Corner c = URF; c < DRB; c = Corner(int(c) + 1))
	{
		idx_co = 3 * idx_co + x.co[c].o;
	}
	return idx_co;
}

// 角块方向解码  将整数编码转为魔方的角块结构方向
CubieCube decodeCornerO(int x)
{
	CubieCube ccb;
	int count = 0;
	for (Corner cn = DBL; cn >= URF; cn = Corner(int(cn) - 1))
	{
		ccb.co[cn].o = x % 3;
		count += ccb.co[cn].o;
		x = (x - ccb.co[cn].o) / 3;
	}
	count = count % 3;
	if (count > 0)
	{
		ccb.co[DRB].o = 3 - count;
	}
	else
	{
		ccb.co[DRB].o = 0;
	}
	return ccb;
}

// 棱块方向编码，和角块编码方式一致
int encodeEdgeO(CubieCube x)
{
	int idx_eo = 0;
	for (Edge e = UR; e < BR; e = Edge(int(e) + 1))
	{
		idx_eo = 2 * idx_eo + x.eo[e].o;
	}
	return idx_eo;
}

// 棱块方向解码
CubieCube decodeEdgeO(int x)
{
	CubieCube ccb;
	int count = 0;
	for (Edge eg = BL; eg >= UR; eg = Edge(int(eg) - 1))
	{
		ccb.eo[eg].o = x % 2;
		count += ccb.eo[eg].o;
		x = (x - ccb.eo[eg].o) / 2;
	}
	count = count % 2;
	if (count > 0)
	{
		ccb.eo[BR].o = 2 - count;
	}
	else
	{
		ccb.eo[BR].o = 0;
	}
	return ccb;
}

/** 角块位置编码
* 康托展开式编码
* 从末尾往前遍历，统计前面有多少角块大于当前角块(首位前无数不计算)
* sum(统计数 * (位置数)!)
*/
int encodeCornerP(CubieCube x)
{
	int idx_cp = 0;
	for (int i = DRB; i > URF; i--) {
		int s = 0;
		for (int j = i - 1; j >= URF; j--) {
			if (x.co[j].c > x.co[i].c)
				s++;
		}
		idx_cp = (idx_cp + s) * i;
	}
	return idx_cp;
}

int factorial[10] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };

// 角块位置解码
CubieCube decodeCornerP(int x)
{
	std::set<Corner> corners = { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
	CubieCube ans;
	int kangtuo[8] = { 0,0,0,0,0,0,0,0 };
	int pos = 1;
	int i = 2;
	for (int i = 2; i <= 7; i++)
	{
		int p = x % i;
		kangtuo[pos++] = p;
		x = (x - p) / i;
	}
	kangtuo[pos] = x;
	for (int j = 7; j >= 0; j--)
	{
		int q = kangtuo[j];
		auto it = corners.rbegin();
		std::advance(it, q);
		ans.co[j].c = *it;
		corners.erase(*it);
	}
	return ans;
}

// 顶层&底层棱块位置编码，编码方式同角块位置
int encodeEdgeUnDP(CubieCube x)
{
	int idx_eudp = 0;
	for (int i = DB; i > UR; i--)
	{
		int temp = 0;
		for (int j = i - 1; j >= UR; j--) {
			if (x.eo[j].e > x.eo[i].e)
				temp++;
		}
		idx_eudp = (idx_eudp + temp) * i;
	}
	return idx_eudp;
}

// 上下层棱块位置解码
CubieCube decodeEdgeUnDP(int x)
{
	std::set<Edge> edges = { UR, UF, UL, UB, DR, DF, DL, DB };
	CubieCube ans;
	int kangtuo[8] = { 0,0,0,0,0,0,0,0 };
	int pos = 1;
	for (int i = 2; i <= 7; i++)
	{
		int p = x % i;
		kangtuo[pos++] = p;
		x = (x - p) / i;
	}
	kangtuo[pos] = x;
	for (int j = 7; j >= 0; j--)
	{
		int q = kangtuo[j];
		auto it = edges.rbegin();
		std::advance(it, q);
		ans.eo[j].e = *it;
		edges.erase(*it);
	}
	for (int i = FR; i <= BR; i++)
	{
		ans.eo[i].e = Edge(i);
	}
	return ans;
}

// 中间层棱块位置排列编码，编码方式同角块位置
int encodeEdgeMP(CubieCube x)
{
	int idx_emp = 0;
	for (int i = BR; i > FR; i--)
	{
		int temp = 0;
		for (int j = i - 1; j >= FR; j--)
		{
			if (x.eo[j].e > x.eo[i].e)
				temp++;
		}
		idx_emp = (idx_emp + temp) * (i - 8);
	}
	return idx_emp;
}

// 中层棱块位置解码
CubieCube decodeEdgeMP(int x)
{
	std::set<Edge> edges = { FR, FL, BL, BR };
	CubieCube ans;
	int kangtuo[4] = { 0,0,0,0 };
	int pos = 1;
	for (int i = 2; i <= 3; i++)
	{
		int p = x % i;
		kangtuo[pos++] = p;
		x = (x - p) / i;
	}
	kangtuo[pos] = x;
	for (int j = 3; j >= 0; j--)
	{
		int q = kangtuo[j];
		auto it = edges.rbegin();
		std::advance(it, q);
		ans.eo[j + 8].e = *it;
		edges.erase(*it);
	}
	for (int i = UR; i <= DB; i++)
	{
		ans.eo[i].e = Edge(i);
	}
	return ans;
}

//组合数表
static int C_NK[12][5] =
{
	{400,0,0,0,0},
	{1,1,0,0,0},
	{1,2,1,0,0},
	{1,3,3,1,0},
	{1,4,6,4,1},
	{1,5,10,10,5},
	{1,6,15,20,15},
	{1,7,21,35,35},
	{1,8,28,56,70},
	{1,9,36,84,126},
	{1,10,45,120,210},
	{1,11,55,165,330}
};

// 中层棱块位置组合数编码
int encodeEdgeMP2(CubieCube x)
{
	int idx_slice = 0, y = 0;
	for (int i = BR; i >= UR; i--)
	{
		// 判断是否为中间棱块
		if (x.eo[i].e >= FR && x.eo[i].e <= BR)
		{
			idx_slice += C_NK[11 - i][y + 1];
			y++;
		}
	}
	return idx_slice;
}

// 中层棱块位置组合数解码
CubieCube decodeEdgeMP2(int x)
{
	CubieCube a;
	int y = 4;
	for (int i = UR; i <= BR; i++)
	{
		if (x >= C_NK[11 - i][y])
		{
			a.eo[i].e = Edge(12 - y);
			x -= C_NK[11 - i][y--];
		}
	}
	return a;
}

// 建立转动表
const int Nmove = 18;					// 共有18种转动方式(U —— U, U2, U')
const int NTWIST = 2187;				// 角块可能方向数: 3^7，取值范围∈[0, 3^7 -1]
const int NFLIP = 2048;					// 棱块可能方向数：2^11

const int NCP = 40320;					// 角块可能位置数最大值：7!* 7 + ..... + 1! * 1 = 40319
const int NEUDP = 40320;				// 上下层棱块可能位置数，公式同上，取值范围∈[0, 40319]
const int NEMP = 24;					// 中间四棱排列最大值：3!*3 + 2!*2 + 1!*1 = 23 

const int NSLICE = 495;					// 中间棱块可能位置数组合：12 x 11 x 10 x 9 / (4!) = 495

vector<vector<int>> twistMoveTable(NTWIST, vector<int>(Nmove));		// all角块原始方向 --18种转动-->  结果's方向编码
vector<vector<int>> flipMoveTable(NFLIP, vector<int>(Nmove));		// 棱块方向变换表
vector<vector<int>> sliceMoveTable(NSLICE, vector<int>(Nmove));		// 中层棱块组合排列变换表

vector<vector<int>> cpMoveTable(NCP, vector<int>(Nmove));			// 角块位置变换表
vector<vector<int>> eudpMoveTable(NEUDP, vector<int>(Nmove));		// 上下层棱块变换表
vector<vector<int>> empMoveTable(NEMP, vector<int>(Nmove));			// 中层棱块变换表

/**	初始化变换表方法
* m： 魔方转动实例
* encode： 编码方式封装
* de code： 解码方式封装
* multy:   'c' 角转动， 'e' 棱转动
* table:  初始化的表
*/
void initMoveTable(function<int(CubieCube)> encode, function<CubieCube(int)> decode, char multy, vector<vector<int>>& table)
{
	// 编码可能性
	int n = table.size();

	// 转动函数的函数别名
	auto mFunc = [multy](CubieCube* org, CubieCube* way, CubieCube* nC) {
		if (multy == 'c')
		{
			cornerMultiply(org, way, nC);
		}
		else if (multy == 'e')
		{
			edgeMultiply(org, way, nC);
		}
		};

	CubieCube a, b;
	for (int i = 0; i < n; i++)
	{
		// 整型编码 -> 魔方的块结构
		a = decode(i);
		// 六种基本转动
		for (int j = U; j <= B; j++)
		{
			// 基本转动的三种方向，U、U2、U'
			// 分别对应顺时针90°，180°，270°
			for (int k = 0; k < 3; k++)
			{
				// 将每次旋转后的模型存储到a中
				mFunc(&a, &move[j], &b);
				a = b;
				table[i][j * 3 + k] = encode(a);
			}
			// 再转一次，返回原状态，因为这里是360度
			mFunc(&a, &move[j], &b);
			a = b;
		}
	}
}

// 建立剪枝表
vector<int> twistPruneTable(NTWIST, -1);
vector<int> flipPruneTable(NFLIP, -1);
vector<int> slicePruneTable(NSLICE, -1);		// 中层棱块位置组合剪枝表

vector<int> CpPruneTable(NCP, -1);
vector<int> EUndpPruneTable(NEUDP, -1);
vector<int> EMpPruneTable(NEMP, -1);		// 中层棱块位置剪枝表
vector<pair<Direct, int>> phase2Move = { {U,0},{U,1},{U,2},{D,0},{D,1},{D,2},{L,1},{R,1},{F,1},{B,1} };

void initPruneTable1(vector<int>& pruneTable, vector<vector<int>>& moveTable)
{
	int depth = 0;
	pruneTable[0] = 0;
	int done = 1;
	int index;
	int sz = pruneTable.size();
	while (done < sz)
	{
		for (int i = 0; i < sz; i++)
		{
			// 如果当前状态已可达
			if (pruneTable[i] == depth)
			{
				for (int j = 0; j < 18; j++)
				{
					// 转动后的状态码
					index = moveTable[i][j];
					if (index > sz) {
						cout << "error: " << index << ", " << i << ", " << j << endl;
					}
					// 如果这个状态还未抵达过
					if (pruneTable[index] == -1)
					{
						pruneTable[index] = depth + 1;
						done++;
					}
				}
			}
		}
		depth++;
	}
}

void initPruneTable2(vector<int>& pruneTable, vector<vector<int>>& moveTable)
{
	int depth = 0;
	pruneTable[0] = 0;
	int done = 1;
	int index;
	int sz = pruneTable.size();
	while (done < sz)
	{
		for (int i = 0; i < sz; i++)
		{
			// 如果当前状态已可达
			if (pruneTable[i] == depth)
			{
				for (int j = 0; j < 10; j++)
				{
					// m是转动方式， t是转动次数，0表90°，1表180°，2表270°（即逆时针90°）
					int d = phase2Move[j].first;
					int t = phase2Move[j].second;
					// 转动后的状态码
					index = moveTable[i][d * 3 + t];
					// 如果这个状态还未抵达过
					if (pruneTable[index] == -1)
					{
						pruneTable[index] = depth + 1;
						done++;
					}
				}
			}
		}
		depth++;
	}
}

// 初始化所有表
void initTables()
{
	auto one = std::chrono::high_resolution_clock::now();
	// 转动表
	char c = 'c';
	char e = 'e';

	//// 封装函数为function变量作为入参
	function<int(CubieCube)> enCO = encodeCornerO;
	function<CubieCube(int)> deCO = decodeCornerO;
	function<int(CubieCube)> enEO = encodeEdgeO;
	function<CubieCube(int)> deEO = decodeEdgeO;
	function<int(CubieCube)> enEMP2 = encodeEdgeMP2;
	function<CubieCube(int)> deEMP2 = decodeEdgeMP2;
	auto two = std::chrono::high_resolution_clock::now();

	initMoveTable(enCO, deCO, c, twistMoveTable);			// 角块方向转动表
	initMoveTable(enEO, deEO, e, flipMoveTable);			// 棱块方向转动表
	initMoveTable(enEMP2, deEMP2, e, sliceMoveTable);		// 中层棱块组合变换表

	auto three = std::chrono::high_resolution_clock::now();
	initPruneTable1(twistPruneTable, twistMoveTable);
	initPruneTable1(flipPruneTable, flipMoveTable);
	initPruneTable1(slicePruneTable, sliceMoveTable);

	auto four = std::chrono::high_resolution_clock::now();

	function<int(CubieCube)> enCP = encodeCornerP;
	function<CubieCube(int)> deCP = decodeCornerP;
	function<int(CubieCube)> enEUnDP = encodeEdgeUnDP;
	function<CubieCube(int)> deEUnDP = decodeEdgeUnDP;
	function<int(CubieCube)> enEMP = encodeEdgeMP;
	function<CubieCube(int)> deEMP = decodeEdgeMP;
	auto five = std::chrono::high_resolution_clock::now();

	initMoveTable(enCP, deCP, c, cpMoveTable);				// 角块位置转动表
	initMoveTable(enEUnDP, deEUnDP, e, eudpMoveTable);		// 上下层棱块位置变化表
	initMoveTable(enEMP, deEMP, e, empMoveTable);			// 中层棱块位置变化表

	auto six = std::chrono::high_resolution_clock::now();

	initPruneTable2(CpPruneTable, cpMoveTable);
	initPruneTable2(EUndpPruneTable, eudpMoveTable);
	initPruneTable2(EMpPruneTable, empMoveTable);
	auto seven = std::chrono::high_resolution_clock::now();


	//vector<std::chrono::duration<double>> duration = {two-one, three - two, four - three, five - four, six - five,  seven - six };
	//cout << "all running time:  ";
	//for (auto dt : duration)
	//	cout << dt.count() << ", ";
	//cout << endl;
}

vector<int> steps;
vector<int> result;
const int maxStep = 25;
int finishFlag = 0;
int initCco, initCcp, initCeo, initCeudp, initCemp, initCemp2;
CubieCube originCube(0);

std::set<int> sr1 = { 1989, 477, 483, 78, 56, 1487,137 };
std::set<int> sr2 = { 33491, 4019, 3676, 36930,21804,22261,22252,30896,38048,37952,37957,11934,28374 };
vector<string> saveSteps;
int answercount = 0;
void search2(int cp, int eundp, int emp, int curDepth)
{
	if (finishFlag)
		return;
	if (curDepth == 0)		// 步数已用尽
	{
		if (cp == 0 && eundp == 0 && emp == 0)
		{
			finishFlag = 1;
			// 查找三次解法，并获取最短的那一个
			if (result.empty() || result.size() >= steps.size())
			{
				answercount++;
				result = steps;
			}
			return;
		}
	}
	else
	{
		int cpNew, eundpNew, empNew;
		for (auto pmv : phase2Move)
		{
			if (finishFlag)
				return;
			int d = pmv.first;
			int t = pmv.second;
			// L后面不能是L', L2，以此缩短路径；相对的LR 与 RL也具有镜像关系
			if (!steps.empty())
			{
				int tail = steps.back();
				if (d == tail / 3 || (tail / 3 - d == 1 && (tail / 3) % 2 == 1))
					continue;
			}

			// 进行转动操作
			int mv = 3 * d + t;
			cpNew = cpMoveTable[cp][mv];
			eundpNew = eudpMoveTable[eundp][mv];
			empNew = empMoveTable[emp][mv];

			int dist2 = std::max(CpPruneTable[cpNew], std::max(EUndpPruneTable[eundpNew], EMpPruneTable[empNew]));

			if (dist2 > curDepth - 1)
				continue;

			steps.push_back(mv);
			search2(cpNew, eundpNew, empNew, curDepth - 1);
			steps.pop_back();
		}
	}
}

// 阶段1： 棱块和角块方向值总和为0；中层棱块处于中层
// cornerO, edgeO, edgeMp2
// 采用DFS算法
void search1(int twist, int flip, int slice, int curDepth)
{
	if (finishFlag)
		return;
	if (curDepth == 0)
	{
		// 阶段一任务完成，已达成状态
		if (twist == 0 && flip == 0 && slice == 0)
		{
			// 获取新的角位置，上下棱位置，中棱位置
			int ccp = initCcp;
			CubieCube a = originCube, b;
			for (auto s : steps)
			{
				ccp = cpMoveTable[ccp][s];
				int d = s / 3;			// 方向
				int t = s % 3;			// 次数
				for (int j = 0; j <= t; j++)
				{
					edgeMultiply(&a, &move[d], &b);
					a = b;
				}
			}
			int cemp = encodeEdgeMP(a);
			int ceudp = encodeEdgeUnDP(a);

			int distForSec = std::max(CpPruneTable[ccp], std::max(EUndpPruneTable[ceudp], EMpPruneTable[cemp]));

			int sz = steps.size();
			for (int i = distForSec; i <= (maxStep - sz); i++)
			{
				if (finishFlag)
					break;
				search2(ccp, ceudp, cemp, i);
				if (answercount < 3)
					finishFlag = 0;
			}
		}
		return;
	}
	else
	{
		int flipNew, twistNew, sliceNew, distNew;
		for (int mv = 0; mv < 18; mv++)
		{
			if (!steps.empty())
			{
				int x = steps.back();
				if (x / 3 == mv / 3 || (x / 3 - mv / 3 == 1 && (x / 3) % 2 == 1))
					continue;
			}
			twistNew = twistMoveTable[twist][mv];
			flipNew = flipMoveTable[flip][mv];
			sliceNew = sliceMoveTable[slice][mv];
			// 当前还原的最大最短距离
			distNew = std::max(slicePruneTable[sliceNew], std::max(flipPruneTable[flipNew], twistPruneTable[twistNew]));

			// 最少需要步数大于允许的步数，剪枝
			if (distNew > curDepth - 1)
			{
				continue;
			}
			if (sr1.find(twist) != sr1.end())
			{
				int stop = 0;
			}
			steps.push_back(mv);
			search1(twistNew, flipNew, sliceNew, curDepth - 1);
			steps.pop_back();		// 回溯
		}

	}
}

int main()
{
	initTables();					// 初始化所有表
	cout << "init table finish" << endl;
	string input;
	vector<string> latex;
	cout << "Please input: ";
	std::getline(std::cin, input);		// 将一行输入存储到input中

	std::istringstream iss(input);		// 创建istring流，并将input拷贝过去		
	string str;
	while (iss >> str)					// istring流将通过空格进行拆分，传递赋值给str		
	{
		latex.push_back(str);
	}

	// 初始化魔方
	auto start = std::chrono::high_resolution_clock::now();
	for (auto lt : latex)
	{
		int t = moveMap[lt];
		int d = t / 3;
		for (int i = 0; i <= t % 3; i++)
		{
			originCube = cubeMove(originCube, move[d]);
		}
	}
	initCco = encodeCornerO(originCube);
	initCeo = encodeEdgeO(originCube);
	initCemp2 = encodeEdgeMP2(originCube);

	initCcp = encodeCornerP(originCube);
	initCeudp = encodeEdgeUnDP(originCube);
	initCemp = encodeEdgeMP(originCube);

	int dist = std::max(twistPruneTable[initCco], std::max(flipPruneTable[initCeo], slicePruneTable[initCemp2]));
	for (int i = dist; i <= maxStep; i++)
	{
		search1(initCco, initCeo, initCemp2, i);
		if (finishFlag)
			break;
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;

	for (auto step : result)
	{
		string s = stepMap[step];
		cout << s << ", ";
	}
	cout << endl;
	cout << "Using time : " << duration.count() << endl;
	return 0;
}
