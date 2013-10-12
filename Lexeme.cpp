#include "Lexeme.h"

namespace Pakon {

Lexeme::Lexeme(string token, int type, int* value) {
	this->token = token;
	this->type = type;
	this->value = value;
}

Lexeme::Lexeme() {
}

}
