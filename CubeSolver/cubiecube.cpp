#include "cubiecube.h"
// 未定义魔方
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

// 还原状态魔方
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

// 花括号构造法
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

// 根据打乱公式获取魔方状态
CubieCube::CubieCube(const vector<std::string> &latex)
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

// 根据六面颜色构造魔方
CubieCube::CubieCube(unordered_map<string, vector<string>> ruckCube)
{
    CcColor cc(ruckCube);
    for (Corner i = URF; i <= DRB; i = Corner(i + 1))
    {
        this->co[i].c = cc.getCorner(cc.co1[i].color[0], cc.co1[i].color[1], cc.co1[i].color[2]);
        // 获取方向
        for (int j = 0; j <= 2; j++)
        {
            if (cc.co1[i].color[j] == 'Y' || cc.co1[i].color[j] == 'W')
            {
                this->co[i].o = j;
                break;
            }
        }
    }
    for (Edge i = UR; i <= BR; i = Edge(i + 1))
    {
        this->eo[i].e = cc.getEdge(cc.eo1[i].color[0], cc.eo1[i].color[1]);
        int pos = i / 8;
        // 获取方向
        if (pos == 0)
        {
            if (cc.eo1[i].color[0] == 'Y' || cc.eo1[i].color[0] == 'W' || cc.eo1[i].color[1] == 'Y' || cc.eo1[i].color[1] == 'W')
            {
                this->eo[i].o = (cc.eo1[i].color[0] == 'Y' || cc.eo1[i].color[0] == 'W') ? 0 : 1;
            }
            else
            {
                if (cc.eo1[i].color[0] == 'R' || cc.eo1[i].color[0] == 'O')
                {
                    this->eo[i].o = 1;
                }
                else
                {
                    this->eo[i].o = 0;
                }
            }
        }
        else {
            if (cc.eo1[i].color[0] == 'Y' || cc.eo1[i].color[0] == 'W' || cc.eo1[i].color[1] == 'Y' || cc.eo1[i].color[1] == 'W')
            {
                this->eo[i].o = (cc.eo1[i].color[0] == 'Y' || cc.eo1[i].color[0] == 'W') ? 1 : 0;
            }
            else {
                if (cc.eo1[i].color[0] == 'R' || cc.eo1[i].color[0] == 'O')
                {
                    this->eo[i].o = 0;
                }
                else
                {
                    this->eo[i].o = 1;
                }
            }
        }

    }
}

bool CubieCube::operator==(const CubieCube &other)
{
    for(int i = URF; i <= DRB; i++)
    {
        if(this->co[i].c != other.co[i].c || this->co[i].o != other.co[i].o )
            return false;
    }
    for(int i = UR; i <= BR; i++)
    {
        if(this->eo[i].e != other.eo[i].e || this->eo[i].o != other.eo[i].o )
            return false;
    }
    return true;
}

// 检查魔方是否合法
bool CubieCube::checkValid()
{
    int crcount = 0;
    for (int i = 0; i < 8; i++)
        crcount += this->co[i].o;
    if (crcount % 3 != 0)
        return false;

    int egcount = 0;
    for (int j = 0; j < 12; j++)
        egcount += this->eo[j].o;
    if (egcount % 2 != 0)
        return false;

    return true;
}

CcColor::CcColor(unordered_map<string, vector<string>>& ruckCube)
{
    vector<string> yellowface = ruckCube["yellow"];
    this->co1[ULB].color[0] = colorUpper[yellowface[0]];
    this->eo1[UB].color[0] = colorUpper[yellowface[1]];
    this->co1[UBR].color[0] = colorUpper[yellowface[2]];
    this->eo1[UL].color[0] = colorUpper[yellowface[3]];
    this->eo1[UR].color[0] = colorUpper[yellowface[5]];
    this->co1[UFL].color[0] = colorUpper[yellowface[6]];
    this->eo1[UF].color[0] = colorUpper[yellowface[7]];
    this->co1[URF].color[0] = colorUpper[yellowface[8]];

    // 下
    vector<string> whiteface = ruckCube["white"];
    this->co1[DLF].color[0] = colorUpper[whiteface[0]];
    this->eo1[DF].color[0] = colorUpper[whiteface[1]];
    this->co1[DFR].color[0] = colorUpper[whiteface[2]];
    this->eo1[DL].color[0] = colorUpper[whiteface[3]];
    this->eo1[DR].color[0] = colorUpper[whiteface[5]];
    this->co1[DBL].color[0] = colorUpper[whiteface[6]];
    this->eo1[DB].color[0] = colorUpper[whiteface[7]];
    this->co1[DRB].color[0] = colorUpper[whiteface[8]];

    // 前
    vector<string> blueface = ruckCube["blue"];
    this->co1[UFL].color[1] = colorUpper[blueface[0]];
    this->eo1[UF].color[1] = colorUpper[blueface[1]];
    this->co1[URF].color[2] = colorUpper[blueface[2]];
    this->eo1[FL].color[1] = colorUpper[blueface[3]];
    this->eo1[FR].color[1] = colorUpper[blueface[5]];
    this->co1[DLF].color[2] = colorUpper[blueface[6]];
    this->eo1[DF].color[1] = colorUpper[blueface[7]];
    this->co1[DFR].color[1] = colorUpper[blueface[8]];

    // 后
    vector<string> greenface = ruckCube["green"];
    this->co1[UBR].color[1] = colorUpper[greenface[0]];
    this->eo1[UB].color[1] = colorUpper[greenface[1]];
    this->co1[ULB].color[2] = colorUpper[greenface[2]];
    this->eo1[BR].color[1] = colorUpper[greenface[3]];
    this->eo1[BL].color[1] = colorUpper[greenface[5]];
    this->co1[DRB].color[2] = colorUpper[greenface[6]];
    this->eo1[DB].color[1] = colorUpper[greenface[7]];
    this->co1[DBL].color[1] = colorUpper[greenface[8]];

    // 右
    vector<string> redface = ruckCube["red"];
    this->co1[URF].color[1] = colorUpper[redface[0]];
    this->eo1[UR].color[1] = colorUpper[redface[1]];
    this->co1[UBR].color[2] = colorUpper[redface[2]];
    this->eo1[FR].color[0] = colorUpper[redface[3]];
    this->eo1[BR].color[0] = colorUpper[redface[5]];
    this->co1[DFR].color[2] = colorUpper[redface[6]];
    this->eo1[DR].color[1] = colorUpper[redface[7]];
    this->co1[DRB].color[1] = colorUpper[redface[8]];

    // 左
    vector<string> orangeface = ruckCube["orange"];
    this->co1[ULB].color[1] = colorUpper[orangeface[0]];
    this->eo1[UL].color[1] = colorUpper[orangeface[1]];
    this->co1[UFL].color[2] = colorUpper[orangeface[2]];
    this->eo1[BL].color[0] = colorUpper[orangeface[3]];
    this->eo1[FL].color[0] = colorUpper[orangeface[5]];
    this->co1[DBL].color[2] = colorUpper[orangeface[6]];
    this->eo1[DL].color[1] = colorUpper[orangeface[7]];
    this->co1[DLF].color[1] = colorUpper[orangeface[8]];
}

// 获取角块位置
Corner CcColor::getCorner(char a, char b, char c)
{

    Corner tt;
    string temp = "";
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    std::sort(temp.begin(), temp.end());
    tt = clToCrn[temp];
    return tt;

}

Edge CcColor::getEdge(char a, char b)
{

    Edge tt;
    string temp = "";
    temp.push_back(a);
    temp.push_back(b);
    std::sort(temp.begin(), temp.end());
    tt = clToEdg[temp];
    return tt;

}

//  旋转公式
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
    // 正面转动
    rotate[F] = {
        {{UFL,1},{DLF,2},{ULB,0},{UBR,0},{URF,2},{DFR,1},{DBL,0},{DRB,0}},	// 角块
        {{UR, 0},{FL,1},{UL,0},{UB,0},{DR,0},{FR,1},{DL,0},{DB,0},{UF,1},{DF,1},{BL,0},{BR,0}}	// 棱块
    };
    rotate[B] = {
        {{URF,0},{UFL,0},{UBR,1},{DRB,2},{DFR,0},{DLF,0},{ULB,2},{DBL,1}},
        {{UR,0},{UF,0},{UL,0},{BR,1},{DR,0},{DF,0},{DL,0},{BL,1},{FR,0},{FL,0},{UB,1},{DB,1}}
    };

    return rotate;
}

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex)
{
    CubieCube newCube;
    cornerMultiply(&origin, &mvLatex, &newCube);		// 角块变换
    edgeMultiply(&origin, &mvLatex, &newCube);			// 棱块变换
    return newCube;
}

void cornerMultiply(const CubieCube *org, const CubieCube *way, CubieCube *nC)
{
    // 旋转方式
    for (int crn = URF; crn <= DRB; crn++)
    {
        // 角块值变换
        nC->co[crn].c = org->co[way->co[crn].c].c;
        // 方向变换
        nC->co[crn].o = (org->co[way->co[crn].c].o + way->co[crn].o) % 3;
    }
}

void edgeMultiply(const CubieCube *org, const CubieCube *way, CubieCube *nC)
{
    for (int i = UR; i <= BR; i++)
    {
        nC->eo[i].e = org->eo[way->eo[i].e].e;
        nC->eo[i].o = (org->eo[way->eo[i].e].o + way->eo[i].o) % 2;
    }
}
