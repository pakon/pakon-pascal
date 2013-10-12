#pragma once

#include <list>
#include <set>
#include <iostream>

#include "Lexeme.h"
#include "SyntaxTree.h"

using namespace std;

namespace Pakon {

class SyntaxAnalyser {
public:
	SyntaxAnalyser();
	~SyntaxAnalyser();
	SyntaxTreeNode internalForm;

	set<string> operationsSymbols;

	list<LexemesSequence> explode(list<Lexeme> sequece);

	bool parse(list<Lexeme> sentence);
	bool isOperator(list<Lexeme> sentence);
	bool isAssignment(list<Lexeme> sentence);
	bool isCompoundOperator(list<Lexeme> sentence);
	bool isSimpleCondition(list<Lexeme> sentence);
	bool isFullCondition(list<Lexeme> sentence);
	bool isCycle(list<Lexeme> sentence);
	bool isExpression(list<Lexeme> sentence);
	bool isOperand(list<Lexeme> sentence);

	SyntaxTreeNode makeOperator(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeAssignment(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeCompoundOperator(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeSimpleCondition(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeFullCondition(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeCycle(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeExpression(list<Lexeme> sentence, SyntaxTreeNode* parent);
	SyntaxTreeNode makeOperand(list<Lexeme> sentence, SyntaxTreeNode* parent);
};

}
