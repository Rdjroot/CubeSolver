#include "Solver.h"

Solver* Solver::instance = nullptr;

Solver::Solver()
{
	// �������
	this->maxStep = 25;

	this->move = initMove();

	// �������
	initCN_K();

	this->twistMoveTable = vector<vector<int>>(NTWIST, vector<int>(Nmove));		// all�ǿ�ԭʼ���� --18��ת��-->  ���'s�������
	this->flipMoveTable = vector<vector<int>>(NFLIP, vector<int>(Nmove));		// ��鷽��任��
	this->sliceMoveTable = vector<vector<int>>(NSLICE, vector<int>(Nmove));		// �в����������б任��

	this->cpMoveTable = vector<vector<int>>(NCP, vector<int>(Nmove));			// �ǿ�λ�ñ任��
	this->eudpMoveTable = vector<vector<int>>(NEUDP, vector<int>(Nmove));		// ���²����任��
	this->empMoveTable = vector<vector<int>>(NEMP, vector<int>(Nmove));			// �в����任��

	// ������֦��
	this->twistPruneTable = vector<int>(NTWIST, -1);
	this->flipPruneTable = vector<int>(NFLIP, -1);
	this->slicePruneTable = vector<int>(NSLICE, -1);		// �в����λ����ϼ�֦��

	this->CpPruneTable = vector<int>(NCP, -1);
	this->EUndpPruneTable = vector<int>(NEUDP, -1);
	this->EMpPruneTable = vector<int>(NEMP, -1);		// �в����λ�ü�֦��

	// �׶�2���������
	this->phase2Move = { {U,0},{U,1},{U,2},{D,0},{D,1},{D,2},{L,1},{R,1},{F,1},{B,1} };

	initTables();
}

Solver::~Solver()
{
	if (instance != nullptr)
		delete instance;
}

void Solver::initCN_K()
{

	const vector<vector<int>> temp = {
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
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			this->C_NK[i][j] = temp[i][j];
		}
	}
}

/** �ǿ鷽�����
* �ǿ�ķ��������֣����Ի���Ϊ3
* 3^(6-c)*o1 + .... + 3^(6-c)*o7 (o �� [0,6])
* ��Ϊ���߸��ǿ�ȷ�������Ժ����һ��Ҳȷ����
*/
int Solver::encodeCornerO(CubieCube x)
{
	short int idx_co = 0;
	for (Corner c = URF; c < DRB; c = Corner(int(c) + 1))
	{
		idx_co = 3 * idx_co + x.co[c].o;
	}
	return idx_co;
}

// �ǿ鷽�����  ����������תΪħ���Ľǿ�ṹ����
CubieCube Solver::decodeCornerO(int x)
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
int Solver::encodeEdgeO(CubieCube x)
{
	int idx_eo = 0;
	for (Edge e = UR; e < BR; e = Edge(int(e) + 1))
	{
		idx_eo = 2 * idx_eo + x.eo[e].o;
	}
	return idx_eo;
}

// ��鷽�����
CubieCube Solver::decodeEdgeO(int x)
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
int Solver::encodeCornerP(CubieCube x)
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

// �ǿ�λ�ý���
CubieCube Solver::decodeCornerP(int x)
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
int Solver::encodeEdgeUnDP(CubieCube x)
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
CubieCube Solver::decodeEdgeUnDP(int x)
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
int Solver::encodeEdgeMP(CubieCube x)
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
CubieCube Solver::decodeEdgeMP(int x)
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

// �в����λ�����������
int Solver::encodeEdgeMP2(CubieCube x)
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
CubieCube Solver::decodeEdgeMP2(int x)
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

/**	��ʼ���任����
* m�� ħ��ת��ʵ��
* encode�� ���뷽ʽ��װ
* de code�� ���뷽ʽ��װ
* multy:   'c' ��ת���� 'e' ��ת��
* table:  ��ʼ���ı�
*/
void Solver::initMoveTable(function<int(CubieCube)> encode, function<CubieCube(int)> decode, char multy, vector<vector<int>>& table)
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

void Solver::initPruneTable1(vector<int>& pruneTable, vector<vector<int>>& moveTable)
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

void Solver::initPruneTable2(vector<int>& pruneTable, vector<vector<int>>& moveTable)
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

// ��ʼ�����б�����ʼ��һ��
void Solver::initTables()
{
	auto one = std::chrono::high_resolution_clock::now();
	// ת����
	char c = 'c';
	char e = 'e';

	//// ��װ����Ϊfunction������Ϊ���
	function<int(CubieCube)> enCO = [this](CubieCube x) {return this->encodeCornerO(x); };
	function<CubieCube(int)> deCO = [this](int x) { return this->decodeCornerO(x); };
	function<int(CubieCube)> enEO = [this](CubieCube x) {return this->encodeEdgeO(x); };
	function<CubieCube(int)> deEO = [this](int x) { return this->decodeEdgeO(x); };
	function<int(CubieCube)> enEMP2 = [this](CubieCube x) {return this->encodeEdgeMP2(x); };
	function<CubieCube(int)> deEMP2 = [this](int x) { return this->decodeEdgeMP2(x); };
	auto two = std::chrono::high_resolution_clock::now();

	initMoveTable(enCO, deCO, c, twistMoveTable);			// �ǿ鷽��ת����
	initMoveTable(enEO, deEO, e, flipMoveTable);			// ��鷽��ת����
	initMoveTable(enEMP2, deEMP2, e, sliceMoveTable);		// �в������ϱ任��

	auto three = std::chrono::high_resolution_clock::now();
	initPruneTable1(twistPruneTable, twistMoveTable);
	initPruneTable1(flipPruneTable, flipMoveTable);
	initPruneTable1(slicePruneTable, sliceMoveTable);

	auto four = std::chrono::high_resolution_clock::now();

	function<int(CubieCube)> enCP = [this](CubieCube x) {return this->encodeCornerP(x); };
	function<CubieCube(int)> deCP = [this](int x) { return this->decodeCornerP(x); };
	function<int(CubieCube)> enEUnDP = [this](CubieCube x) {return this->encodeEdgeUnDP(x); };
	function<CubieCube(int)> deEUnDP = [this](int x) { return this->decodeEdgeUnDP(x); };
	function<int(CubieCube)> enEMP = [this](CubieCube x) {return this->encodeEdgeMP(x); };
	function<CubieCube(int)> deEMP = [this](int x) { return this->decodeEdgeMP(x); };
	auto five = std::chrono::high_resolution_clock::now();

	initMoveTable(enCP, deCP, c, cpMoveTable);				// �ǿ�λ��ת����
	initMoveTable(enEUnDP, deEUnDP, e, eudpMoveTable);		// ���²����λ�ñ仯��
	initMoveTable(enEMP, deEMP, e, empMoveTable);			// �в����λ�ñ仯��

	auto six = std::chrono::high_resolution_clock::now();

	initPruneTable2(CpPruneTable, cpMoveTable);
	initPruneTable2(EUndpPruneTable, eudpMoveTable);
	initPruneTable2(EMpPruneTable, empMoveTable);
	auto seven = std::chrono::high_resolution_clock::now();

	vector<std::chrono::duration<double>> duration = { two - one, three - two, four - three, five - four, six - five,  seven - six };
	cout << "all running time:  ";
	for (auto dt : duration)
		cout << dt.count() << ", ";
	cout << endl;
}

void Solver::searchPartTwo(int cp, int eundp, int emp, int curDepth, vector<int>& result, vector<int>& steps, int& finishFlag)
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
				this->answercount++;
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
			searchPartTwo(cpNew, eundpNew, empNew, curDepth - 1,result,steps,finishFlag);
			steps.pop_back();
		}
	}
}

// �׶�1�� ���ͽǿ鷽��ֵ�ܺ�Ϊ0���в���鴦���в�
// cornerO, edgeO, edgeMp2
// ����DFS�㷨
void Solver::searchPartOne(int twist, int flip, int slice, int curDepth, vector<int>& result, vector<int>& steps, int& finishFlag)
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
			CubieCube a = this->orgccb, b;
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
				searchPartTwo(ccp, ceudp, cemp, i, result, steps, finishFlag);
				if (this->answercount < 3)
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
			steps.push_back(mv);
			searchPartOne(twistNew, flipNew, sliceNew, curDepth - 1, result, steps, finishFlag);
			steps.pop_back();		// ����
		}

	}
}


// ��ȡ��ԭ��ʽ
vector<string> Solver::getSolveLatex(CubieCube originCube)
{
	this->answercount = 3;
	this->orgccb = originCube;
	initCco = encodeCornerO(originCube);
	initCeo = encodeEdgeO(originCube);
	initCemp2 = encodeEdgeMP2(originCube);

	initCcp = encodeCornerP(originCube);
	initCeudp = encodeEdgeUnDP(originCube);
	initCemp = encodeEdgeMP(originCube);

	int dist = std::max(twistPruneTable[initCco], std::max(flipPruneTable[initCeo], slicePruneTable[initCemp2]));
	int finishFlag = 0;
	vector<int> result;
	vector<int> steps;

	for (int i = dist; i <= maxStep; i++)
	{
		searchPartOne(initCco, initCeo, initCemp2, i, result, steps, finishFlag);
		if (finishFlag)
			break;
	}

	vector<string> restore;
	if (!result.empty())
	{
		// int -> string��ת��
		for (auto step : result)
		{
			restore.push_back(stepMap[step]);
		}
	}

	this->answercount = 0;
	return restore;
}
