#pragma once

#include <list>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <cmath>

#include "Lexeme.h"
#include "Error.h"

using namespace std;

namespace Pakon {

struct Identifier {
	int value;
	bool present;
};

class LexicalAnalyser {

public:
	LexicalAnalyser();
	~LexicalAnalyser();

public:
	string sourceText;
	bool scan();
	list<Lexeme> getNextOperator();
	void print();
	void print(list<Lexeme> someList);
	void printErrorsTable();
	map<string, Identifier> getIdTable();

private:
	set<char> operationsTable;
	set<string> keyWordsTable;
	map<string, Identifier> identifiersTable;
	list<Lexeme> errorsTable;
	list<Lexeme> lexemes;
	list<Lexeme>::iterator curLexemesPos;

// Состояния лексического анализатора
private:
	int curState;
	const static int stEmpty		= 1;
	const static int stIdentifier	= 2;
	const static int stOpSymbol		= 3;
	const static int stAssign		= 4;
	const static int stLeftShift	= 5;
	const static int stRightShift	= 6;
	const static int stConst		= 7;
	const static int stError		= 8;

private:
	bool success;
	void clearComment();
	int toOctal(int a);

	bool isSpace(char symbol);
	bool isLetter(char symbol);
	bool isNumber(char symbol);
	bool isOctalNumber(char symbol);
	bool isNotOctalNumber(char symbol);
	bool isOperationSymbol(char symbol);
	bool isKeyWord(string token);
};

}
