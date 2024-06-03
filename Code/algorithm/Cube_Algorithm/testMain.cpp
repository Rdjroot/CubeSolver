#include "CubieCube.h"
#include "Solver.h"
#include<sstream>
#include<iostream>

int main()
{
	Solver* s = Solver::getInstance();
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
	auto start = std::chrono::high_resolution_clock::now();
	
	CubieCube originCube(latex);

	vector<string> answer = s->getSolveLatex(originCube);
	auto end = std::chrono::high_resolution_clock::now();

	for (auto ans : answer)
		cout << ans << " ";
	cout << endl;
	std::chrono::duration<double> duration = end - start;
	cout << "Using time : " << duration.count() << endl;
	return 0;
}