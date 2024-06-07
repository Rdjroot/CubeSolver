#include "CubieCube.h"
#include "Solver.h"
#include<sstream>
#include<algorithm>
#include<iostream>

int main()
{
	Solver* s = Solver::getInstance();
	cout << "init table finish" << endl;
	//string input;
	//vector<string> latex;
	//cout << "Please input: ";
	//std::getline(std::cin, input);		// 将一行输入存储到input中

	//std::istringstream iss(input);		// 创建istring流，并将input拷贝过去		
	//string str;
	//while (iss >> str)					// istring流将通过空格进行拆分，传递赋值给str		
	//{
	//	latex.push_back(str);
	//}
	unordered_map < string, vector<string>> ruckCube = { { "yellow", { "white", "white", "yellow", "green", "yellow", "orange", "blue", "green", "green", }},
{ "orange", { "blue", "white", "yellow", "red", "orange", "blue", "green", "red", "yellow", }},
{ "red", { "yellow", "yellow", "blue", "green", "red", "red", "white", "orange", "blue", }},
{ "green", { "red", "blue", "red", "green", "green", "white", "white", "white", "white", }},
{ "white", { "red", "red", "red", "blue", "white", "blue", "orange", "orange", "orange", }},
{ "blue", { "orange", "orange", "orange", "yellow", "blue", "yellow", "green", "yellow", "green", }} };
	auto start = std::chrono::high_resolution_clock::now();
	//

	CubieCube originCube(ruckCube);
	originCube.print();

	vector<string> answer = s->getSolveLatex(originCube);
	auto end = std::chrono::high_resolution_clock::now();

	for (auto ans : answer)
		cout << ans << " ";
	cout << endl;
	std::chrono::duration<double> duration = end - start;
	cout << "Using time : " << duration.count() << endl;
	return 0;


	//unordered_map<char, char> ftoc = { {'U','Y'},{'D','W'},{'F','B'},{'R','R'},{'B','G'},{'L','O'}};

	//unordered_map<Corner, string> cornerMap = {
	//	{ URF, "URF" }, { UFL, "UFL" }, { ULB, "ULB" }, { UBR, "UBR" },
	//	{ DFR, "DFR" }, { DLF, "DLF" }, { DBL, "DBL" }, { DRB, "DRB" }
	//};

	//unordered_map<Edge, string> edgeMap = {
	//	{ UR, "UR" }, { UF, "UF" }, { UL, "UL" }, { UB, "UB" },
	//	{ DR, "DR" }, { DF, "DF" }, { DL, "DL" }, { DB, "DB" },
	//	{ FR, "FR" }, { FL, "FL" }, { BL, "BL" }, { BR, "BR" }
	//};

	//for (int i = URF; i <= DRB; i++)
	//{
	//	string cb = cornerMap[Corner(i)];
	//	string temp = "";
	//	for (auto c : cb)
	//	{
	//		temp += ftoc[c];
	//	}
	//	std::sort(temp.begin(), temp.end());
	//	cout << " { \"" << temp << "\", " << cb << "}, ";
	//}
	//cout << endl;
	//for (int i = UR; i <= BR; i++)
	//{
	//	string eb = edgeMap[Edge(i)];
	//	string temp = "";
	//	for (auto e : eb)
	//	{
	//		temp += ftoc[e];
	//	}
	//	std::sort(temp.begin(), temp.end());
	//	cout << " { \"" << temp << "\", " << eb << "}, ";
	//}
	//cout << endl;




}
