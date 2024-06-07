#ifndef CUBIECUBE_H
#define CUBIECUBE_H
#include<vector>
#include<unordered_map>
#include<string>
#include<algorithm>
using std::unordered_map;
using std::vector;
using std::string;

// 角块
enum Corner{ URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB } ;
// 棱块
enum Edge { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };
// 转动方向
enum Direct { U, D, L, R, F, B };

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


static unordered_map<string, char> colorUpper = { {"yellow",'Y'},
    {"white",'W'},
    {"blue",'B'},
    {"red",'R'},
    {"green",'G'},
    {"orange",'O'}
};

static unordered_map<string, Corner> clToCrn = { { "BRY", URF},  { "BOY", UFL},  { "GOY", ULB},  { "GRY", UBR},  { "BRW", DFR},  { "BOW", DLF},  { "GOW", DBL},  { "GRW", DRB} };

static unordered_map<string, Edge> clToEdg = { { "RY", UR},  { "BY", UF},  { "OY", UL},  { "GY", UB},  { "RW", DR},  { "BW", DF},  { "OW", DL},  { "GW", DB},  { "BR", FR},  { "BO", FL},  { "GO", BL},  { "GR", BR} };

class CcColor {
    struct cn_c
    {
        Corner c;
        int o;
        char color[3];
    };
    struct eg_c
    {
        Edge e;
        int o;
        char color[2];
    };
public:
    cn_c co1[8];
    eg_c eo1[12];

    CcColor(unordered_map<string, vector<string>>& ruckCube);

    // 获取角块位置
    Corner getCorner(char a, char b, char c);

    // 获取棱块位置
    Edge getEdge(char a, char b);
};

class CubieCube
{
public:
    corner_o co[8];
    edge_o eo[12];

    // 默认构造函数，所有位置和方向未定义的魔方
    CubieCube();

    // 初始状态为还原状态的魔方
    CubieCube(int x);

    // 自定义初始化构造函数，为了MOVE模型
    CubieCube(const std::initializer_list<corner_o> crn, const std::initializer_list<edge_o> edg);

    // 根据打乱公式构造魔方
    CubieCube(const vector<string>& latex);

    // 根据六面颜色构造魔方
    CubieCube(unordered_map<string, vector<string>> ruckCube);

    bool operator==(const CubieCube &other);

    // 检查当前魔方是否合法
    bool checkValid();
};

vector<CubieCube> initMove();

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex);

void cornerMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

void edgeMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);
#endif // CUBIECUBE_H
