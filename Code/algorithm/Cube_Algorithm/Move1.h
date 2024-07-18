#pragma once
#ifndef MOVE_H
#define MOVE_H
#include<vector>
#include "CubieCube.h"

using std::vector;

class CubieCube;


vector<CubieCube> initMove();

CubieCube cubeMove(CubieCube origin, CubieCube mvLatex);

void cornerMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

void edgeMultiply(const CubieCube* org, const CubieCube* way, CubieCube* nC);

#endif 