#include "SyntaxTree.h"

namespace Pakon {

SyntaxTreeNode::SyntaxTreeNode() {
}

SyntaxTreeNode::~SyntaxTreeNode() {
}

SyntaxTreeNode::SyntaxTreeNode(int type, SyntaxTreeNode* parent) {
	this->type = type;
	this->parent = parent;
}

void SyntaxTreeNode::print(int indent) {
	for (int i = 0; i < indent; ++i) {
		cout << "   ";
	}
	list<SyntaxTreeNode>::iterator it;
	switch (this->type) {
		case this->tyOperator : {
			cout << "operator" << endl;
			for (it = this->children.begin(); it != this->children.end(); ++it) {
				it->print(indent+1);
			}
		} break;
		case this->tyExpression : {
			cout << "expression" << endl;
			for (it = this->children.begin(); it != this->children.end(); ++it) {
				it->print(indent+1);
			}
		} break;
		case this->tyOperand : {
			cout << "operand" << endl;
			for (it = this->children.begin(); it != this->children.end(); ++it) {
				it->print(indent+1);
			}
		} break;
		case this->tyTerminal : {
			cout << this->value.token << endl;
		} break;
	}
}



SyntaxTree::SyntaxTree(SyntaxTreeNode* root) {
	this->root = root;
}

SyntaxTree::~SyntaxTree() {
}

void SyntaxTree::print() {
	root->print(0);
}

}
