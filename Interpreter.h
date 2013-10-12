#pragma once

#include <map>
#include <list>
#include <string>
#include <algorithm>

#include "LexicalAnalyser.h"
#include "SyntaxTree.h"

using namespace std;

namespace Pakon {

class Interpreter {
public:
	Interpreter(map<string, Identifier> idTable);
	~Interpreter();
	map<string, int> operationsMap;
	map<string, Identifier> identifiersMap;
	bool interpret(SyntaxTreeNode* op);
	int* calculate(SyntaxTreeNode* expr);
	int* getValue(SyntaxTreeNode operand);
	void printIdentifiers();

private:
	const static int exNot		= 1;
	const static int exAnd		= 2;
	const static int exOr		= 3;
	const static int exSum		= 4;
	const static int exDif		= 5;
	const static int exMul		= 6;
	const static int exDiv		= 7;
	const static int exLeft		= 8;
	const static int exRight	= 9;
	const static int exEq		= 10;
	const static int exLess		= 11;
	const static int exLarge	= 12;
};

}
