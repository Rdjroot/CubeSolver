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
