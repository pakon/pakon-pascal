#pragma once

#include <string>
#include <list>

#include "Lexeme.h"

using namespace std;


namespace Pakon {

class Lexeme {
public:
	Lexeme();
	Lexeme(string token, int type, int* value = NULL);
	string token;
	int type;
	int* value;
public:
	const static int tyIdentifier		= 1;
	const static int tyKeyWord			= 2;
	const static int tyOperationSymbol	= 3;
	const static int tyConst			= 4;
	const static int tyUnknown			= 5;
};

typedef list<Lexeme> LexemesSequence;

}
