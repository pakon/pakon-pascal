#pragma once

#include <list>
#include <iostream>
#include "Lexeme.h"

using namespace std;

namespace Pakon {

class SyntaxTreeNode {
public:
	SyntaxTreeNode();
	SyntaxTreeNode(int type, SyntaxTreeNode* parent);
	~SyntaxTreeNode();
	void print(int indent);
	SyntaxTreeNode* parent;
	list<SyntaxTreeNode> children;
	int type;
	const static int tyTerminal		= 0;
	const static int tyOperator		= 1;
	const static int tyExpression	= 2;
	const static int tyOperand		= 3;
	Lexeme value;
};

class SyntaxTree {
public:
	SyntaxTree(SyntaxTreeNode* root);
	~SyntaxTree();
	SyntaxTreeNode* root;
	void print();
};

}
