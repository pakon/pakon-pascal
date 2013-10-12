#include "Interpreter.h"

namespace Pakon {

Interpreter::Interpreter(map<string, Identifier> idTable) {
	this->identifiersMap = idTable;
	this->operationsMap.insert(std::make_pair("not", this->exNot));
	this->operationsMap.insert(std::make_pair("and", this->exAnd));
	this->operationsMap.insert(std::make_pair("or", this->exOr));
	this->operationsMap.insert(std::make_pair("+", this->exSum));
	this->operationsMap.insert(std::make_pair("-", this->exDif));
	this->operationsMap.insert(std::make_pair("*", this->exMul));
	this->operationsMap.insert(std::make_pair("/", this->exDiv));
	this->operationsMap.insert(std::make_pair("<<", this->exLeft));
	this->operationsMap.insert(std::make_pair(">>", this->exRight));
	this->operationsMap.insert(std::make_pair("=", this->exEq));
	this->operationsMap.insert(std::make_pair("<", this->exLess));
	this->operationsMap.insert(std::make_pair(">", this->exLarge));
}

int* Interpreter::calculate(SyntaxTreeNode* expr) {
	list<SyntaxTreeNode>::iterator it = ++expr->children.begin();
	int* operand1;
	int* operand2;
	int* result = new int;
	if (1 == expr->children.size()) {
		it = expr->children.begin()->children.begin();
		if (it->value.type == Lexeme::tyConst) {
			return it->value.value;
		}
		else {
			map<string, Identifier>::mapped_type id = this->identifiersMap[it->value.token];
			if (!id.present) {
				return NULL;
			}
			else {
				int* res = new int;
				*res = id.value;
				return res;
			}
		}
	}
	else if (this->operationsMap.find(it->value.token) != this->operationsMap.end()) {
		operand1 = this->getValue(*expr->children.begin());
		operand2 = this->getValue(*expr->children.rbegin());
		if (operand1 != NULL && operand2 != NULL) {
			int opCode = this->operationsMap[it->value.token];
			switch (opCode) {
				case this->exAnd : {
					*result = (*operand1 && *operand2);
				} break;
				case this->exOr : {
					*result = (*operand1 || *operand2);
				} break;
				case this->exSum : {
					*result = (*operand1 + *operand2);
				} break;
				case this->exDif : {
					*result = (*operand1 - *operand2);
				} break;
				case this->exMul : {
					*result = (*operand1 * *operand2);
				} break;
				case this->exDiv : {
					if (*operand2 != 0) {
						*result = (*operand1 / *operand2);
					}
					else {
						result = NULL;
					}
				} break;
				case this->exLeft : {
					*result = (*operand1 << *operand2);
				} break;
				case this->exRight : {
					*result = (*operand1 >> *operand2);
				} break;
				case this->exEq : {
					*result = (*operand1 == *operand2);
				} break;
				case this->exLess : {
					*result = (*operand1 < *operand2);
				} break;
				case this->exLarge : {
					*result = (*operand1 > *operand2);
				} break;
			}
			return result;
		}
		else {
			return NULL;
		}
	}
	else {
		operand1 = this->getValue(*expr->children.rbegin());
		if (operand1 != NULL) {
			*result = !(*operand1);
			return result;
		}
		else {
			return NULL;
		}
	}
}

int* Interpreter::getValue(SyntaxTreeNode operand) {
	if (1 == operand.children.size()) {
		list<SyntaxTreeNode>::iterator it = operand.children.begin();
		if (it->value.type == Lexeme::tyConst) {
			return operand.children.begin()->value.value;
		}
		else {
			Identifier id = this->identifiersMap[it->value.token];
			if (id.present) {
				int* res = new int;
				*res = id.value;
				return res;
			}
			else {
				return NULL;
			}
		}
	}
	else {
		list<SyntaxTreeNode>::iterator it = ++operand.children.begin();
		SyntaxTreeNode expr = *it;
		return this->calculate(&expr);
	}
}

bool Interpreter::interpret(SyntaxTreeNode* op) {
	list<SyntaxTreeNode>::iterator it = op->children.begin();
	if (it->value.token == "repeat") {
		++it;
		SyntaxTreeNode operat = *it;
		++it;
		++it;
		int* exprRes = new int;
		SyntaxTreeNode expr = *it;
		do {
			if (!this->interpret(&operat)) {
				return false;
			}
			exprRes = this->calculate(&expr);
			if (exprRes == NULL) {
				return false;
			}
		}
		while (!(*exprRes));
		return true;
	}
	else if (it->value.token == "begin") {
		SyntaxTreeNode operat;
		++it;
		while (it != --op->children.end()) {
			operat = *it;
			if (!this->interpret(&operat)) {
				return false;
			}
			++it;
		}
		return true;
	}
	else if (it->value.token == "if") {
		SyntaxTreeNode expr;
		SyntaxTreeNode op1;
		SyntaxTreeNode op2;
		++it;
		expr = *it;
		++it;
		++it;
		op1 = *it;
		++it;
		if (it->value.token == "else") {
			int* exprRes = this->calculate(&expr);
			if (exprRes == NULL) {
				return false;
			}
			else {
				if (*exprRes) {
					if (!this->interpret(&op1)) {
						return false;
					}
					else {
						return true;
					}
				}
				else {
					++it;
					op2 = *it;
					if (!this->interpret(&op2)) {
						return false;
					}
					else {
						return true;
					}
				}
			}
		}
		else {
			int* exprRes = this->calculate(&expr);
			if (exprRes == NULL) {
				return false;
			}
			else if (*exprRes) {
				if (!this->interpret(&op1)) {
					return false;
				}
			}
		}
	}
	else {
		string idStr = it->value.token;
		++it;
		++it;
		SyntaxTreeNode expr;
		expr = *it;
		int* exprRes = this->calculate(&expr);
		if (exprRes == NULL) {
			return false;
		}
		else {
			//this->printIdentifiers();
			Identifier id;
			id.present = true;
			id.value = *exprRes;
			this->identifiersMap[idStr] = id;
			return true;
		}
	}
}

void Interpreter::printIdentifiers() {
	map<string, Identifier>::iterator it;
	cout << "\nidentifier\tvalue" << endl;
	for (it = this->identifiersMap.begin(); it != this->identifiersMap.end(); ++it) {
		cout << it->first << "\t\t" << it->second.value << endl;
	}
}

}
