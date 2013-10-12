#include "SyntaxAnalyser.h"

namespace Pakon {

SyntaxAnalyser::SyntaxAnalyser() {
	this->operationsSymbols.insert("and");
	this->operationsSymbols.insert("or");
	this->operationsSymbols.insert("xor");
	this->operationsSymbols.insert("+");
	this->operationsSymbols.insert("-");
	this->operationsSymbols.insert("*");
	this->operationsSymbols.insert("/");
	this->operationsSymbols.insert(">>");
	this->operationsSymbols.insert("<<");
	this->operationsSymbols.insert("=");
	this->operationsSymbols.insert("<");
	this->operationsSymbols.insert(">");
}

SyntaxAnalyser::~SyntaxAnalyser() {
}

list<LexemesSequence> SyntaxAnalyser::explode(list<Lexeme> sequece) {
	list<LexemesSequence> result;
	list<Lexeme> op;
	list<Lexeme>::iterator it, last;
	for (it = sequece.begin(); it != sequece.end(); ++it) {
		op.clear();
		if (it->token == "repeat") {
			op.push_back(*it);
			++it;
			int counter = 0;
			while (it != sequece.end() && (it->token != "until" || counter != 0)) {
				if (it->token == "repeat") {
					++counter;
				}
				if (it->token == "until") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			if (it == sequece.end()) {
				break;
			}
			while (it != sequece.end() && it->token != ";") {
				op.push_back(*it);
				++it;
			}
			if (it == sequece.end()) {
				break;
			}
			op.push_back(*it);
			//++it;
			result.push_back(op);
		}
		else if (it->token == "begin") {
			op.push_back(*it);
			int counter = 0;
			++it;
			while (it != sequece.end() && (it->token != "end" || counter != 0)) {
				if (it->token == "begin") {
					++counter;
				}
				if (it->token == "end") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			if (it == sequece.end()) {
				break;
			}
			op.push_back(*it);
			++it;
			op.push_back(*it);
			//++it;
			result.push_back(op);
		}
		else if (it->token == "if") {
			op.push_back(*it);
			int counter = 0;
			while (it != sequece.end() && it->token != "then") {
				++it;
				op.push_back(*it);
			}
			if (it == sequece.end()) {
				break;
			}
			++it;
			counter = 0;
			while (it != sequece.end() && (it->token != "fi" || counter != 0)) {
				if (it->token == "fi") {
					++counter;
				}
				if (it->token == "then") {
					--counter;
				}
				op.push_back(*it);
				++it;
			}
			if (it == sequece.end()) {
				break;
			}
			op.push_back(*it);
			++it;
			op.push_back(*it);
			result.push_back(op);
		}
		else {
			while (it != sequece.end() && it->token != ";") {
				op.push_back(*it);
				++it;
			}
			if (it == sequece.end()) {
				break;
			}
			op.push_back(*it);
			result.push_back(op);
		}
	}
	return result;
}

bool SyntaxAnalyser::isOperator(list<Lexeme> sentence) {
	if (this->isCompoundOperator(sentence) || this->isAssignment(sentence) ||
		this->isCycle(sentence) || this->isSimpleCondition(sentence) ||
		this->isFullCondition(sentence))
	{
		return true;
	}
	else {
		return false;
	}
}

bool SyntaxAnalyser::isAssignment(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	if (!it->tyConst == Lexeme::tyIdentifier) {
		return false;
	}
	++it;
	if (it->token != ":=") {
		return false;
	}
	++it;
	list<Lexeme> seq(it, --sentence.end());
	if (!this->isExpression(seq)) {
		return false;
	}
	return true;
}

bool SyntaxAnalyser::isCompoundOperator(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme> subSequence;
	if (it->token == "begin") {
		++it;
		int counter = 0;
		while (it != sentence.end() && (counter != 0 || it->token != "end")) {
			if (it->token == "begin") {
				++counter;
			}
			if (it->token == "end") {
				--counter;
			}
			subSequence.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		list<LexemesSequence> opList = this->explode(subSequence);
		list<LexemesSequence>::iterator lsIt;
		for (lsIt = opList.begin(); lsIt != opList.end(); ++lsIt) {
			if (!this->isOperator(*lsIt)) {
				return false;
			}
		}
		if (it->token == "end") {
			++it;
			if (it->token == ";") {
				++it;
				if (it == sentence.end()) {
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool SyntaxAnalyser::isSimpleCondition(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme> subExpression;
	list<Lexeme> subOp;
	if (it->token == "if") {
		++it;
		while (it != sentence.end() && it->token != "then") {
			subExpression.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isExpression(subExpression)) {
			return false;
		}
		++it;
		while (it != sentence.end() && it->token != "fi") {
			subOp.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isOperator(subOp)) {
			return false;
		}
		++it;
		if (it->token == ";") {
			++it;
			if (it == sentence.end()) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
bool SyntaxAnalyser::isFullCondition(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme> subExpression;
	list<Lexeme> subOp;
	if (it->token == "if") {
		++it;
		while (it != sentence.end() && it->token != "then") {
			subExpression.push_back(*it);
			++it;
		}
		if (!this->isExpression(subExpression)) {
			return false;
		}
		++it;
		int counter = 0;
		while (it != sentence.end() && (it->token != "else" || counter != 0)) {
			if (it->token == "if") {
				++counter;
			}
			if (it->token == "fi") {
				--counter;
			}
			subOp.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isOperator(subOp)) {
			return false;
		}
		++it;
		counter = 0;
		subOp.clear();
		while (it != sentence.end() && (it->token != "fi" || counter != 0)) {
			if (it->token == "if") {
				++counter;
			}
			if (it->token == "fi") {
				--counter;
			}
			subOp.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isOperator(subOp)) {
			return false;
		}
		++it;
		if (it->token == ";") {
			++it;
			if (it == sentence.end()) {
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
	else {
		return false;
	}
}

bool SyntaxAnalyser::isCycle(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme> subExpression;
	list<Lexeme> subOp;
	if (it->token == "repeat") {
		++it;
		int counter = 0;
		while (it != sentence.end() && (it->token != "until" || counter != 0)) {
			if (it->token == "repeat") {
				++counter;
			}
			if (it->token == "until") {
				--counter;
			}
			subOp.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isOperator(subOp)) {
			return false;
		}
		++it;
		while (it != sentence.end() && it->token != ";") {
			subExpression.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isExpression(subExpression)) {
			return false;
		}
		++it;
		if (it == sentence.end()) {
			return true;
		}
		else
			return false;
	}
	else {
		return false;
	}
}

bool SyntaxAnalyser::isOperand(list<Lexeme> sentence) {
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme> subExpr;
	if (1 == sentence.size()) {
		if (it->type == Lexeme::tyIdentifier || it->type == Lexeme::tyConst) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (it->token == "(") {
		int counter = 0;
		++it;
		while (it != sentence.end() && (it->token != ")" || counter != 0)) {
			if (it->token == "(") {
				++counter;
			}
			if (it->token == ")") {
				--counter;
			}
			subExpr.push_back(*it);
			++it;
		}
		if (it == sentence.end()) {
			return false;
		}
		if (!this->isExpression(subExpr)) {
			return false;
		}
		return true;
	}
	else {
	return false;}
}

bool SyntaxAnalyser::isExpression(list<Lexeme> sentence) {
	list<Lexeme>::iterator it;
	list<Lexeme> subExpr;
	bool result = true;
	for (it = sentence.begin(); it != sentence.end(); ++it) {
		if (1 == sentence.size()) {
			if (it->type != Lexeme::tyIdentifier && it->type != Lexeme::tyConst) {
				result = false;
				break;
			}
		}
		else if (it->token == "(") {
			int counter = 0;
			++it;
			while (it != sentence.end() && (counter != 0 || it->token != ")")) {
				if (it->token == "(") {
					++counter;
				}
				if (it->token == ")") {
					--counter;
				}
				subExpr.push_back(*it);
				++it;
			}
			if (it == sentence.end()) {
				result = false;
				break;
			}
			if (!this->isExpression(subExpr)) {
				result = false;
				break;
			}
			++it;
			if (this->operationsSymbols.find(it->token) == this->operationsSymbols.end()) {
				result = false;
				break;
			}
			subExpr.assign(++it, sentence.end());
			if (!this->isOperand(subExpr)) {
				result = false;
				break;
			}
			else {
				result = true;
				break;
			}
		}
		else if (it->token == "not") {
			list<Lexeme> subExpr(++it, sentence.end());
			if (!this->isOperand(subExpr)) {
				result = false;
				break;
			}
		}
		else {
			if (it->type != Lexeme::tyIdentifier && it->type != Lexeme::tyConst) {
				result = false;
				break;
			}
			++it;
			if (this->operationsSymbols.find(it->token) == this->operationsSymbols.end()) {
				result = false;
				break;
			}
			subExpr.clear();
			subExpr.assign(++it, sentence.end());
			if (!this->isOperand(subExpr)) {
				result = false;
				break;
			}
			else {
				result = true;
				break;
			}
		}
	}
	return result;
}

bool SyntaxAnalyser::parse(list<Lexeme> sentence) {
	if (this->isOperator(sentence)) {
		this->internalForm = this->makeOperator(sentence, NULL);
		return true;
	}
	else {
		return false;
	}
}

SyntaxTreeNode SyntaxAnalyser::makeOperator(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	list<Lexeme>::iterator it = sentence.begin();
	if (it->token == "repeat") {
		return this->makeCycle(sentence, parent);
	}
	else if (it->token == "begin") {
		return this->makeCompoundOperator(sentence, parent);
	}
	else if (it->token == "if") {
		++it;
		int counter = 0;
		while (it != sentence.end() && (it->token != "else" || counter != 0)) {
			if (it->token == "if") {
				++counter;
			}
			if (it->token == "fi") {
				--counter;
			}
			++it;
		}
		if (it == sentence.end()) {
			return this->makeSimpleCondition(sentence, parent);
		}
		else {
			return this->makeFullCondition(sentence, parent);
		}
	}
	else {
		return this->makeAssignment(sentence, parent);
	}
}

SyntaxTreeNode SyntaxAnalyser::makeExpression(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	list<Lexeme> subOp;
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme>::iterator last = sentence.end();
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyExpression, parent);
	if (1 == sentence.size()) {
		subOp.push_back(*it);
		op->children.push_back(this->makeOperand(subOp, op));
	}
	else if (it->token == "(") {
		subOp.push_back(*it);
		++it;
		int counter = 0;
		while (it->token != ")" || counter != 0) {
			if (it->token == "(") {
				++counter;
			}
			if (it->token == ")") {
				--counter;
			}
			subOp.push_back(*it);
			++it;
		}
		subOp.push_back(*it);
		op->children.push_back(this->makeOperand(subOp, op)); // поддерево первого операнда
		++it;
		SyntaxTreeNode terminalNode(SyntaxTreeNode::tyTerminal, op);
		terminalNode.value = *it;
		op->children.push_back(terminalNode); // лексема операции
		subOp.clear();
		subOp.assign(++it, last);
		op->children.push_back(this->makeOperand(subOp, op)); // поддерево второго операнда
	}
	else if (it->token == "not") {
		subOp.push_back(*it);
		SyntaxTreeNode terminalNode(SyntaxTreeNode::tyTerminal, op);
		terminalNode.value = *it;
		op->children.push_back(terminalNode); // лексема not
		subOp.assign(++it, last);
		op->children.push_back(this->makeOperand(subOp, op)); // поддерево операнда
	}
	else {
		subOp.push_back(*it);
		op->children.push_back(this->makeOperand(subOp, op));
		++it;
		SyntaxTreeNode terminalNode(SyntaxTreeNode::tyTerminal, op);
		terminalNode.value = *it;
		op->children.push_back(terminalNode); // лексема операции
		subOp.clear();
		subOp.assign(++it, last);
		op->children.push_back(this->makeOperand(subOp, op)); // поддерево второго операнда
	}
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeOperand(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	list<Lexeme> subExpr;
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme>::iterator last = sentence.end();
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperand, parent);
	if (it->token == "(") {
		SyntaxTreeNode terminal1(SyntaxTreeNode::tyTerminal, op);
		terminal1.value = *it;
		op->children.push_back(terminal1); // лексема (
		subExpr.assign(++it, --last);
		op->children.push_back(this->makeExpression(subExpr, op)); // поддерево выражения
		SyntaxTreeNode terminal2(SyntaxTreeNode::tyTerminal, op);
		terminal2.value = *last;
		op->children.push_back(terminal2); // лексема )
	}
	else {
		SyntaxTreeNode terminal(SyntaxTreeNode::tyTerminal, op);
		terminal.value = *it;
		op->children.push_back(terminal);
	}
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeAssignment(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperator, parent);
	list<Lexeme>::iterator it = sentence.begin();
	SyntaxTreeNode child(SyntaxTreeNode::tyTerminal, op);
	child.value = *it;
	op->children.push_back(child); // лексема идентификатора
	++it;
	child.value = *it;
	op->children.push_back(child); // лексема :=
	list<Lexeme> subExpression(++it, --sentence.end());
	op->children.push_back(this->makeExpression(subExpression, op)); // поддерево выражения
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeSimpleCondition(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperator, parent);
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme>::iterator last = sentence.end();
	SyntaxTreeNode child(SyntaxTreeNode::tyTerminal, op);
	child.value = *it;
	op->children.push_back(child); // лексема if
	++it;
	list<Lexeme> subExpression;
	while (it->token != "then") {
		subExpression.push_back(*it);
		++it;
	}
	op->children.push_back(this->makeExpression(subExpression, op)); // поддерево выражения
	child.value = *it;
	op->children.push_back(child); // лексема then
	--last;
	list<Lexeme> subOp(++it, --last);
	op->children.push_back(this->makeOperator(subOp, op)); // поддерево оператора
	++last;
	child.value = *last;
	op->children.push_back(child); // лексема fi
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeFullCondition(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperator, parent);
	list<Lexeme>::iterator it = sentence.begin();
	SyntaxTreeNode child(SyntaxTreeNode::tyTerminal, op);
	child.value = *it;
	op->children.push_back(child); // лексема if
	++it;
	list<Lexeme> subExpression;
	while (it->token != "then") {
		subExpression.push_back(*it);
		++it;
	}
	op->children.push_back(this->makeExpression(subExpression, op)); // поддерево выражения
	child.value = *it;
	op->children.push_back(child); // лексема then
	list<Lexeme> subOp;
	int counter = 0;
	++it;
	while (it->token != "else" || counter != 0) {
		if (it->token == "if") {
			++counter;
		}
		if (it->token == "fi") {
			--counter;
		}
		subOp.push_back(*it);
		++it;
	}
	op->children.push_back(this->makeOperator(subOp, op)); // поддерево оператора
	child.value = *it;
	op->children.push_back(child); // лексема else
	++it;
	subOp.clear();
	while (it->token != "fi") {
		subOp.push_back(*it);
		++it;
	}
	op->children.push_back(this->makeOperator(subOp, op)); // поддерево оператора
	child.value = *it;
	op->children.push_back(child); // лексема fi
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeCycle(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperator, parent);
	list<Lexeme> subOpSentence;
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme>::iterator last = --sentence.end();
	SyntaxTreeNode child(SyntaxTreeNode::tyTerminal, op);
	child.value = *it;
	op->children.push_back(child); // лексема repeat
	++it;
	int counter = 0;
	while (it->token != "until" || counter != 0) {
		if (it->token == "repeat") {
			++counter;
		}
		if (it->token == "until") {
			--counter;
		}
		subOpSentence.push_back(*it);
		++it;
	}
	op->children.push_back(this->makeOperator(subOpSentence, op)); // поддерево опреатора
	child.value = *it;
	op->children.push_back(child); // лексема until
	list<Lexeme> subExpression(++it, last);
	op->children.push_back(this->makeExpression(subExpression, op)); // поддерево выражения
	return *op;
}

SyntaxTreeNode SyntaxAnalyser::makeCompoundOperator(list<Lexeme> sentence, SyntaxTreeNode* parent) {
	SyntaxTreeNode* op = new SyntaxTreeNode(SyntaxTreeNode::tyOperator, parent);
	list<Lexeme> subOpSentence;
	list<Lexeme>::iterator it = sentence.begin();
	list<Lexeme>::iterator last = --sentence.end();
	SyntaxTreeNode child(SyntaxTreeNode::tyTerminal, op);
	child.value = *it;
	op->children.push_back(child); // лексема begin
	subOpSentence.assign(++it, --last);
	list<LexemesSequence> ls = this->explode(subOpSentence);
	list<LexemesSequence>::iterator subOp;
	for (subOp = ls.begin(); subOp != ls.end(); ++subOp) {
		op->children.push_back(this->makeOperator(*subOp, op)); // поддерево оператора
	}
	child.value = *last;
	op->children.push_back(child); // лексема end
	return *op;
}

}
