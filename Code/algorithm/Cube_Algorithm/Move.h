#pragma once
#ifndef MOVE_H
#define MOVE_H
#include "CubieCube.h"

// 转动方向
enum Direct { U, D, L, R, F, B };

vector<CubieCube> initMove();

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex);

void cornerMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

void edgeMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

#endif 