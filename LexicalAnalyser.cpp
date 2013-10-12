#include "LexicalAnalyser.h"

namespace Pakon {

LexicalAnalyser::LexicalAnalyser() {
	this->success = true;

	this->operationsTable.insert(';');
	this->operationsTable.insert('+');
	this->operationsTable.insert('-');
	this->operationsTable.insert('*');
	this->operationsTable.insert('/');
	this->operationsTable.insert('(');
	this->operationsTable.insert(')');
	this->operationsTable.insert('.');
	this->operationsTable.insert('=');

	this->keyWordsTable.insert("program");
	this->keyWordsTable.insert("if");
	this->keyWordsTable.insert("then");
	this->keyWordsTable.insert("else");
	this->keyWordsTable.insert("fi");
	this->keyWordsTable.insert("repeat");
	this->keyWordsTable.insert("until");
	this->keyWordsTable.insert("begin");
	this->keyWordsTable.insert("end");
	this->keyWordsTable.insert("and");
	this->keyWordsTable.insert("or");
	this->keyWordsTable.insert("not");

	this->curState = this->stEmpty;
}

bool LexicalAnalyser::scan() {
	this->clearComment();
	string token;
	string::const_iterator nextSymbol = this->sourceText.begin();
	while (nextSymbol != this->sourceText.end()) {
		switch (this->curState) {
			case this->stEmpty: {
				if (!this->isSpace(*nextSymbol)) {
					token.push_back(*nextSymbol);
				}
				if (this->isLetter(*nextSymbol)) {
					this->curState = this->stIdentifier;
				}
				else if (this->isOctalNumber(*nextSymbol)) {
					this->curState = this->stConst;
				}
				else if (this->isNotOctalNumber(*nextSymbol)) {
					this->curState = this->stError;
				}
				else if (*nextSymbol == ':') {
					this->curState = this->stAssign;
				}
				else if (*nextSymbol == '<') {
					this->curState = this->stLeftShift;
				}
				else if (*nextSymbol == '>') {
					this->curState = this->stRightShift;
				}
				else if (this->isOperationSymbol(*nextSymbol)) {
					this->curState = this->stOpSymbol;
					--nextSymbol;
				}
				++nextSymbol;
			} break;
			case this->stIdentifier: {
				if (this->isSpace(*nextSymbol)) {
					if (this->isKeyWord(token)) {
						Lexeme lexeme(token, Lexeme::tyKeyWord);
						this->lexemes.push_back(lexeme);
					}
					else {
						Lexeme lexeme(token, Lexeme::tyIdentifier);
						this->lexemes.push_back(lexeme);
						Identifier id;
						id.present = false;
						this->identifiersTable[token] = id;
					}
					token.clear();
					this->curState = this->stEmpty;
				}
				else if (this->isLetter(*nextSymbol) || this->isNumber(*nextSymbol)) {
					token.push_back(*nextSymbol);
					this->curState = this->stIdentifier;
				}
				else {
					if (this->isKeyWord(token)) {
						Lexeme lexeme(token, Lexeme::tyKeyWord);
						this->lexemes.push_back(lexeme);
					}
					else {
						Lexeme lexeme(token, Lexeme::tyIdentifier);
						this->lexemes.push_back(lexeme);
						Identifier id;
						id.present = false;
						this->identifiersTable[token] = id;
					}
					token.clear();
					token.push_back(*nextSymbol);
					if (*nextSymbol == ':') {
						this->curState = this->stAssign;
					}
					else if (*nextSymbol == '<') {
						this->curState = this->stLeftShift;
					}
					else if (*nextSymbol == '>') {
						this->curState = this->stRightShift;
					}
					else if (this->isOperationSymbol(*nextSymbol)) {
							this->curState = this->stOpSymbol;
							--nextSymbol;
					}
				}
				++nextSymbol;
			} break;
			case this->stOpSymbol: {
				Lexeme lexeme(token, Lexeme::tyOperationSymbol);
				this->lexemes.push_back(lexeme);
				token.clear();
				this->curState = this->stEmpty;
				++nextSymbol;
			} break;
			case this->stAssign: {
				token.push_back(*nextSymbol);
				if (*nextSymbol == '=') {
					this->curState = this->stOpSymbol;
				}
				else {
					this->curState = this->stError;
				}
				++nextSymbol;
			} break;
			case this->stLeftShift: {
				if (*nextSymbol == '<') {
					token.push_back(*nextSymbol);
					this->curState = this->stOpSymbol;
				}
				else {
					this->curState = this->stError;
					Lexeme lexeme(token, Lexeme::tyOperationSymbol);
					this->lexemes.push_back(lexeme);
					token.clear();
					this->curState = this->stEmpty;
				}
				++nextSymbol;
			} break;
			case this->stRightShift: {
				if (*nextSymbol == '>') {
					token.push_back(*nextSymbol);
					this->curState = this->stOpSymbol;
				}
				else {
					this->curState = this->stError;
					Lexeme lexeme(token, Lexeme::tyOperationSymbol);
					this->lexemes.push_back(lexeme);
					token.clear();
					this->curState = this->stEmpty;
				}
				++nextSymbol;
			} break;
			case this->stConst: {
				if (this->isSpace(*nextSymbol)) {
					Lexeme lexeme(token, Lexeme::tyConst);
					int* value = new int;
					string val = "0";
					val.append(token);
					*value = this->toOctal(atoi(val.c_str()));
					lexeme.value = value;
					this->lexemes.push_back(lexeme);
					token.clear();
					this->curState = this->stEmpty;
				}
				else if (this->isOctalNumber(*nextSymbol)) {
					token.push_back(*nextSymbol);
				}
				else if (this->isLetter(*nextSymbol) || this->isNumber(*nextSymbol)) {
					token.push_back(*nextSymbol);
					this->curState = this->stError;
				}
				else {
					Lexeme lexeme(token, Lexeme::tyConst);
					int* value = new int;
					string val = "0";
					val.append(token);
					*value = this->toOctal(atoi(val.c_str()));
//					*value = atoi(token.c_str());
					lexeme.value = value;
					this->lexemes.push_back(lexeme);
					token.clear();
					token.push_back(*nextSymbol);
					if (*nextSymbol == ':') {
						this->curState = this->stAssign;
					}
					else if (*nextSymbol == '<') {
						this->curState = this->stLeftShift;
					}
					else if (*nextSymbol == '>') {
						this->curState = this->stRightShift;
					}
					else if (this->isOperationSymbol(*nextSymbol)) {
						Lexeme lexeme(token, Lexeme::tyOperationSymbol);
						int* value = new int;
						string val = "0";
						val.append(token);
						*value = this->toOctal(atoi(val.c_str()));
//						*value = atoi(token.c_str());
						lexeme.value = value;
						this->lexemes.push_back(lexeme);
						token.clear();
						this->curState = this->stEmpty;
					}
				}
				++nextSymbol;
			} break;
			case this->stError: {
				this->success = false;
				Lexeme lexeme(token, Lexeme::tyUnknown);
				this->lexemes.push_back(lexeme);
				this->errorsTable.push_back(lexeme);
				token.clear();
				this->curState = this->stEmpty;
			} break;
		}
	}
	this->curLexemesPos = this->lexemes.begin();
	return this->success;
}

bool LexicalAnalyser::isSpace(char symbol) {
	if (symbol == ' ' || symbol == '\n' || symbol == '\t') {
		return true;
	}
	else {
		return false;
	}
}

bool LexicalAnalyser::isLetter(char symbol) {
	if ((symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z')) {
		return true;
	}
	else {
		return false;
	}
}

bool LexicalAnalyser::isNumber(char symbol) {
	if (symbol >= '0' && symbol <= '9') {
		return true;
	}
	else {
		return false;
	}
}

bool LexicalAnalyser::isOctalNumber(char symbol) {
	if (symbol >= '0' && symbol <= '7') {
		return true;
	}
	else {
		return false;
	}
}

bool LexicalAnalyser::isNotOctalNumber(char symbol) {
	if (symbol == '8' || symbol == '9') {
		return true;
	}
	else {
		return false;
	}
}

bool LexicalAnalyser::isOperationSymbol(char symbol) {
	if (this->operationsTable.find(symbol) == this->operationsTable.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool LexicalAnalyser::isKeyWord(string token) {
	if (this->keyWordsTable.find(token) == this->keyWordsTable.end()) {
		return false;
	}
	else {
		return true;
	}
}

void LexicalAnalyser::print() {
	list<Lexeme>::iterator it;
	for (it = this->lexemes.begin(); it != this->lexemes.end(); ++it) {
		cout << it->token << "\t";
		switch (it->type) {
			case Lexeme::tyIdentifier : {
				cout << "identifier";
			} break;
			case Lexeme::tyKeyWord : {
				cout << "keyword";
			} break;
			case Lexeme::tyOperationSymbol : {
				cout << "operation symbol";
			} break;
			case Lexeme::tyConst : {
				cout << "const";
			} break;
		}
		cout << endl;
	}
}

void LexicalAnalyser::print(list<Lexeme> someList) {
	list<Lexeme>::iterator it;
	for (it = someList.begin(); it != someList.end(); ++it) {
		cout << it->token << " ";
	}
	cout << endl;
}

void LexicalAnalyser::printErrorsTable() {
	list<Lexeme>::iterator it;
	for (it = this->errorsTable.begin(); it != this->errorsTable.end(); ++it) {
		cout << it->token << endl;
	}
}

list<Lexeme> LexicalAnalyser::getNextOperator() {
	list<Lexeme> op;
	list<Lexeme>::iterator it, last;
	if (this->curLexemesPos == this->lexemes.begin()) {
		++this->curLexemesPos;
	}
	last = --this->lexemes.end();
	--last;
	for (it = this->curLexemesPos; it != last; ++it) {
		if (it->token == "repeat") {
			op.push_back(*it);
			++it;
			int counter = 0;
			while (it->token != "until" || counter != 0) {
				if (it->token == "repeat") {
					++counter;
				}
				if (it->token == "until") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			while (it->token != ";") {
				op.push_back(*it);
				++it;
			}
			op.push_back(*it);
			++it;
			this->curLexemesPos = it;
			return op;
		}
		else if (it->token == "begin") {
			op.push_back(*it);
			int counter = 0;
			++it;
			while (it->token != "end" || counter != 0) {
				if (it->token == "begin") {
					++counter;
				}
				if (it->token == "end") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			op.push_back(*it);
			++it;
			op.push_back(*it);
			++it;
			this->curLexemesPos = it;
			return op;
		}
		else if (it->token == "if") {
			op.push_back(*it);
			while (it->token != "then") {
				++it;
				op.push_back(*it);
			}
			++it;
			int counter = 0;
			while (it->token != "fi" || counter != 0) {
				if (it->token == "if") {
					++counter;
				}
				if (it->token == "fi") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			op.push_back(*it);
			++it;
			op.push_back(*it);
			++it;
			this->curLexemesPos = it;
			return op;
		}
		else {
			op.push_back(*it);
			while (it->token != ";") {
				++it;
				op.push_back(*it);
			}
			++it;
			this->curLexemesPos = it;
			return op;
		}
	}
	return op;
}

void LexicalAnalyser::clearComment() {
	string::size_type it1, it2;
	string::size_type pos = 0;
	it1 = this->sourceText.find("(*");
	if (it1 != string::npos) {
		while (pos != string::npos) {
			it2 = this->sourceText.find("*)");
			this->sourceText.erase(it1, it2-it1+2);
			it1 = this->sourceText.find("(*");
			pos = it1;
		}
	}
}

map<string, Identifier> LexicalAnalyser::getIdTable() {
	return this->identifiersTable;
}

int LexicalAnalyser::toOctal(int a) {
	int i = 0;
	int res = 0;
	while (a != 0) {
		int m = a % 10;
		res += m*pow(8, i);
		++i;
		a = a / 10;
	}
	return res;
}

}
