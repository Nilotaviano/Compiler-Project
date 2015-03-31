#include "Parser.h"

#include <stdio.h>
#include <iostream>


Parser::Parser(char *file_name)
	:scanner(fopen(file_name, "r")),
	over_(false)
{
}


Parser::~Parser()
{
}

void Parser::Begin()
{
	if (Program()) {
		cout << "Ok!\n";
	}
	else {
		cout << "Erro(s) encontrado(s)!\n";
	}
}

void Parser::ReportSyntaxError(string error)
{
	cout << "ERRO de sintaxe na linha " + to_string(scanner.GetCurrentLine()) + " coluna " + to_string(scanner.GetCurrentColumn()) + ", ultimo token lido '" + current_token_->ToString() + "': " + error + "\n";
}

void Parser::ReportLexycalError(string error)
{

}

bool Parser::Program()
{
	current_token_ = scanner.GetNextToken();

	//Error
	if (current_token_ == nullptr) {
		cout << scanner.GetError() << "\n";
		return false;
	}
	else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "int") {
		tokens_.push_back(current_token_);
		current_token_ = scanner.GetNextToken();

		if (current_token_ == nullptr) {
			cout << scanner.GetError() << "\n";
			return false;
		}
		else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD && current_token_->get_lexeme() == "main") {
			tokens_.push_back(current_token_);
			current_token_ = scanner.GetNextToken();

			if (current_token_ == nullptr) {
				cout << scanner.GetError() << "\n";
				return false;
			}
			else if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
				tokens_.push_back(current_token_);
				current_token_ = scanner.GetNextToken();

				if (current_token_ == nullptr) {
					cout << scanner.GetError() << "\n";
					return false;
				}
				else if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
					tokens_.push_back(current_token_);
					current_token_ = scanner.GetNextToken();

					if (current_token_ == nullptr) {
						cout << scanner.GetError() << "\n";
						return false;
					}
					else if (Block()) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					ReportSyntaxError("Esperado o simbolo '('");
					return false;
				}
			}
			else {
				ReportSyntaxError("Esperado o simbolo '('");
				return false;
			}
		}
		else {
			ReportSyntaxError("Esperada a palavra-chave 'main'");
			return false;
		}
	}
	else {
		ReportSyntaxError("Esperada a palavra-chave 'int'");
		return false;
	}
}

bool Parser::Block()
{
	if (current_token_->get_token_class() == TokenClassEnum::L_BRACE) {
		tokens_.push_back(current_token_);
		current_token_ = scanner.GetNextToken();

		if (current_token_ == nullptr) {
			cout << scanner.GetError() << "\n";
			return false;
		}
		else {

			if (Command()) {

				if (current_token_->get_token_class() == TokenClassEnum::R_BRACE) {
					return true;
				}
				else {
					ReportSyntaxError("Esperado o simbolo '}'");
					return false;
				}
			}
			else {
				return false;
			}
		}
	}
	else {
		ReportSyntaxError("Esperado o simbolo '{'");
		return false;
	}
}

bool Parser::Command()
{
	return false;
}

bool Parser::BasicCommand()
{
	return false;
}

bool Parser::Iteration()
{
	return false;
}

bool Parser::Assignment()
{
	return false;
}

bool Parser::RelationalOperation()
{
	return false;
}

bool Parser::ArithmeticOperation()
{
	return false;
}

bool Parser::Term()
{
	return false;
}

bool Parser::Factor()
{
	return false;
}
