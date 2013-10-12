#include <iostream>
#include <fstream>

#include "LexicalAnalyser.h"
#include "SyntaxAnalyser.h"
#include "Interpreter.h"

using namespace Pakon;
using namespace std;

int main(int argc, char* argv[]) {
	if (argc == 2) {

		LexicalAnalyser* la = new LexicalAnalyser();
		map<string, Identifier> idTable;

		ifstream inFile;
		inFile.open(argv[1]);
		if (inFile.is_open()) {
			char c;
			while(inFile.get(c)) {
				la->sourceText.push_back(c);
			}
		}
		else {
			cout << "Can't read file: " << argv[1] << endl;
			cin.get();
			exit(0);
		}

		list<Lexeme> op;
		if (la->scan()) {
			la->print();
			cout << "Scaned source text successful!" << endl;
			cin.get();

			SyntaxAnalyser* sa = new SyntaxAnalyser();
			idTable = la->getIdTable();
			Interpreter* interp = new Interpreter(idTable);

			op = la->getNextOperator();
			while (!op.empty()) {
				la->print(op);
				if (sa->parse(op)) {
					SyntaxTree st(&sa->internalForm);
					st.print();
					if (interp->interpret(&sa->internalForm)) {
						interp->printIdentifiers();
					}
					else {
						cout << "Has interpret errors!" << endl;
						cin.get();
					}
				}
				else {
					cout << "Has syntax errors!" << endl;
					cin.get();
				}
				cin.get();
				op = la->getNextOperator();
			}
			cout << "Done!" << endl;
			cin.get();
		}
		else {
			cout << "Has lexical errors!" << endl;
			la->printErrorsTable();
			cin.get();
		}
	}
	else {
		cout << "No file loaded!" << endl;
		cin.get();
	}
	return 0;
}
