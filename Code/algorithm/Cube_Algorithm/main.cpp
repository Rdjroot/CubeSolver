#include<iostream>
#include "CubieCube.h"		// ħ���ṹ ��ת��
//#include "Move1.h"			// ����ת��
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

/** �ǿ鷽�����
* �ǿ�ķ��������֣����Ի���Ϊ3
* 3^(6-c)*o1 + .... + 3^(6-c)*o7 (o �� [0,6])
* ��Ϊ���߸��ǿ�ȷ�������Ժ����һ��Ҳȷ����
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

// �ǿ鷽�����  ����������תΪħ���Ľǿ�ṹ����
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

// ��鷽����룬�ͽǿ���뷽ʽһ��
int encodeEdgeO(CubieCube x)
{
	int idx_eo = 0;
	for (Edge e = UR; e < BR; e = Edge(int(e) + 1))
	{
		idx_eo = 2 * idx_eo + x.eo[e].o;
	}
	return idx_eo;
}

// ��鷽�����
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

/** �ǿ�λ�ñ���
* ����չ��ʽ����
* ��ĩβ��ǰ������ͳ��ǰ���ж��ٽǿ���ڵ�ǰ�ǿ�(��λǰ����������)
* sum(ͳ���� * (λ����)!)
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

// �ǿ�λ�ý���
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

// ����&�ײ����λ�ñ��룬���뷽ʽͬ�ǿ�λ��
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

// ���²����λ�ý���
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

// �м�����λ�����б��룬���뷽ʽͬ�ǿ�λ��
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

// �в����λ�ý���
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

//�������
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

// �в����λ�����������
int encodeEdgeMP2(CubieCube x)
{
	int idx_slice = 0, y = 0;
	for (int i = BR; i >= UR; i--)
	{
		// �ж��Ƿ�Ϊ�м����
		if (x.eo[i].e >= FR && x.eo[i].e <= BR)
		{
			idx_slice += C_NK[11 - i][y + 1];
			y++;
		}
	}
	return idx_slice;
}

// �в����λ�����������
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

// ����ת����
const int Nmove = 18;					// ����18��ת����ʽ(U ���� U, U2, U')
const int NTWIST = 2187;				// �ǿ���ܷ�����: 3^7��ȡֵ��Χ��[0, 3^7 -1]
const int NFLIP = 2048;					// �����ܷ�������2^11

const int NCP = 40320;					// �ǿ����λ�������ֵ��7!* 7 + ..... + 1! * 1 = 40319
const int NEUDP = 40320;				// ���²�������λ��������ʽͬ�ϣ�ȡֵ��Χ��[0, 40319]
const int NEMP = 24;					// �м������������ֵ��3!*3 + 2!*2 + 1!*1 = 23 

const int NSLICE = 495;					// �м�������λ������ϣ�12 x 11 x 10 x 9 / (4!) = 495

vector<vector<int>> twistMoveTable(NTWIST, vector<int>(Nmove));		// all�ǿ�ԭʼ���� --18��ת��-->  ���'s�������
vector<vector<int>> flipMoveTable(NFLIP, vector<int>(Nmove));		// ��鷽��任��
vector<vector<int>> sliceMoveTable(NSLICE, vector<int>(Nmove));		// �в����������б任��

vector<vector<int>> cpMoveTable(NCP, vector<int>(Nmove));			// �ǿ�λ�ñ任��
vector<vector<int>> eudpMoveTable(NEUDP, vector<int>(Nmove));		// ���²����任��
vector<vector<int>> empMoveTable(NEMP, vector<int>(Nmove));			// �в����任��

/**	��ʼ���任����
* m�� ħ��ת��ʵ��
* encode�� ���뷽ʽ��װ
* de code�� ���뷽ʽ��װ
* multy:   'c' ��ת���� 'e' ��ת��
* table:  ��ʼ���ı�
*/
void initMoveTable(function<int(CubieCube)> encode, function<CubieCube(int)> decode, char multy, vector<vector<int>>& table)
{
	// ���������
	int n = table.size();

	// ת�������ĺ�������
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
		// ���ͱ��� -> ħ���Ŀ�ṹ
		a = decode(i);
		// ���ֻ���ת��
		for (int j = U; j <= B; j++)
		{
			// ����ת�������ַ���U��U2��U'
			// �ֱ��Ӧ˳ʱ��90�㣬180�㣬270��
			for (int k = 0; k < 3; k++)
			{
				// ��ÿ����ת���ģ�ʹ洢��a��
				mFunc(&a, &move[j], &b);
				a = b;
				table[i][j * 3 + k] = encode(a);
			}
			// ��תһ�Σ�����ԭ״̬����Ϊ������360��
			mFunc(&a, &move[j], &b);
			a = b;
		}
	}
}

// ������֦��
vector<int> twistPruneTable(NTWIST, -1);
vector<int> flipPruneTable(NFLIP, -1);
vector<int> slicePruneTable(NSLICE, -1);		// �в����λ����ϼ�֦��

vector<int> CpPruneTable(NCP, -1);
vector<int> EUndpPruneTable(NEUDP, -1);
vector<int> EMpPruneTable(NEMP, -1);		// �в����λ�ü�֦��
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
			// �����ǰ״̬�ѿɴ�
			if (pruneTable[i] == depth)
			{
				for (int j = 0; j < 18; j++)
				{
					// ת�����״̬��
					index = moveTable[i][j];
					if (index > sz) {
						cout << "error: " << index << ", " << i << ", " << j << endl;
					}
					// ������״̬��δ�ִ��
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
			// �����ǰ״̬�ѿɴ�
			if (pruneTable[i] == depth)
			{
				for (int j = 0; j < 10; j++)
				{
					// m��ת����ʽ�� t��ת��������0��90�㣬1��180�㣬2��270�㣨����ʱ��90�㣩
					int d = phase2Move[j].first;
					int t = phase2Move[j].second;
					// ת�����״̬��
					index = moveTable[i][d * 3 + t];
					// ������״̬��δ�ִ��
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

// ��ʼ�����б�
void initTables()
{
	auto one = std::chrono::high_resolution_clock::now();
	// ת����
	char c = 'c';
	char e = 'e';

	//// ��װ����Ϊfunction������Ϊ���
	function<int(CubieCube)> enCO = encodeCornerO;
	function<CubieCube(int)> deCO = decodeCornerO;
	function<int(CubieCube)> enEO = encodeEdgeO;
	function<CubieCube(int)> deEO = decodeEdgeO;
	function<int(CubieCube)> enEMP2 = encodeEdgeMP2;
	function<CubieCube(int)> deEMP2 = decodeEdgeMP2;
	auto two = std::chrono::high_resolution_clock::now();

	initMoveTable(enCO, deCO, c, twistMoveTable);			// �ǿ鷽��ת����
	initMoveTable(enEO, deEO, e, flipMoveTable);			// ��鷽��ת����
	initMoveTable(enEMP2, deEMP2, e, sliceMoveTable);		// �в������ϱ任��

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

	initMoveTable(enCP, deCP, c, cpMoveTable);				// �ǿ�λ��ת����
	initMoveTable(enEUnDP, deEUnDP, e, eudpMoveTable);		// ���²����λ�ñ仯��
	initMoveTable(enEMP, deEMP, e, empMoveTable);			// �в����λ�ñ仯��

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
	if (curDepth == 0)		// �������þ�
	{
		if (cp == 0 && eundp == 0 && emp == 0)
		{
			finishFlag = 1;
			// �������νⷨ������ȡ��̵���һ��
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
			// L���治����L', L2���Դ�����·������Ե�LR �� RLҲ���о����ϵ
			if (!steps.empty())
			{
				int tail = steps.back();
				if (d == tail / 3 || (tail / 3 - d == 1 && (tail / 3) % 2 == 1))
					continue;
			}

			// ����ת������
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

// �׶�1�� ���ͽǿ鷽��ֵ�ܺ�Ϊ0���в���鴦���в�
// cornerO, edgeO, edgeMp2
// ����DFS�㷨
void search1(int twist, int flip, int slice, int curDepth)
{
	if (finishFlag)
		return;
	if (curDepth == 0)
	{
		// �׶�һ������ɣ��Ѵ��״̬
		if (twist == 0 && flip == 0 && slice == 0)
		{
			// ��ȡ�µĽ�λ�ã�������λ�ã�����λ��
			int ccp = initCcp;
			CubieCube a = originCube, b;
			for (auto s : steps)
			{
				ccp = cpMoveTable[ccp][s];
				int d = s / 3;			// ����
				int t = s % 3;			// ����
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
			// ��ǰ��ԭ�������̾���
			distNew = std::max(slicePruneTable[sliceNew], std::max(flipPruneTable[flipNew], twistPruneTable[twistNew]));

			// ������Ҫ������������Ĳ�������֦
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
			steps.pop_back();		// ����
		}

	}
}

int main()
{
	initTables();					// ��ʼ�����б�
	cout << "init table finish" << endl;
	string input;
	vector<string> latex;
	cout << "Please input: ";
	std::getline(std::cin, input);		// ��һ������洢��input��

	std::istringstream iss(input);		// ����istring��������input������ȥ		
	string str;
	while (iss >> str)					// istring����ͨ���ո���в�֣����ݸ�ֵ��str		
	{
		latex.push_back(str);
	}

	// ��ʼ��ħ��
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
