#pragma once

#include <string>

using namespace std;


namespace Pakon {

class Error {
public:
	int type;
	Lexeme token;
};

}
