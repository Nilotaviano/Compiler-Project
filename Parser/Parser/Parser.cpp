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

bool Parser::LexycalErrorOccurred()
{
	if (current_token_ == nullptr) {
		cout << scanner.GetError() << "\n";
		return true;
	}
	else {
		return false;
	}
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
			//TODO VariableDeclaration
			//TODO n commands
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

bool Parser::VariableDeclaration()
{
	if (Type()) {
		tokens_.push_back(current_token_);
		current_token_ = scanner.GetNextToken();

		if (!LexycalErrorOccurred()) {
			if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER) {
				tokens_.push_back(current_token_);
				current_token_ = scanner.GetNextToken();

				if (!LexycalErrorOccurred()) {
					while (current_token_->get_token_class() == TokenClassEnum::COMMA) {
						tokens_.push_back(current_token_);
						current_token_ = scanner.GetNextToken();

						if (!LexycalErrorOccurred()) {

							if (current_token_->get_token_class() != IDENTIFIER) {
								ReportSyntaxError("Esperado um idenficador");
								return false;
							}
							else {
								tokens_.push_back(current_token_);
								current_token_ = scanner.GetNextToken();

								if (LexycalErrorOccurred()) {
									return false;
								}
							}
						}
						else {
							return false;
						}
					}//while comma

					if (current_token_->get_token_class() == TokenClassEnum::SEMICOLON) {
						return true;
					}
					else {
						ReportSyntaxError("Esperado o simbolo ';'");
						return false;
					}
				}
				else {
					return false;
				}
			}
			else {
				ReportSyntaxError("Esperado ao menos um identificador");
				return false;
			}
		}
		else {
			return false;
		}
	}
	return false;
}

bool Parser::Type()
{
	if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD) {
		if (current_token_->get_lexeme() == "int" || current_token_->get_lexeme() == "float" || current_token_->get_lexeme() == "char") {
			return true;
		}
	}
	return false;
}

bool Parser::Command()
{
	if (current_token_->get_token_class() == TokenClassEnum::IDENTIFIER || current_token_->get_token_class() == TokenClassEnum::L_BRACE) {
		return BasicCommand();
	}
	else if (current_token_->get_token_class() == TokenClassEnum::RESERVED_WORD) {

		if (current_token_->get_lexeme() == "while" || current_token_->get_lexeme() == "do") {
			return Iteration();
		}
		else if (current_token_->get_lexeme() == "if") {
			tokens_.push_back(current_token_);
			current_token_ = scanner.GetNextToken();

			if (!LexycalErrorOccurred()) {
				if (current_token_->get_token_class() == TokenClassEnum::L_PAREN) {
					tokens_.push_back(current_token_);
					current_token_ = scanner.GetNextToken();

					if (!LexycalErrorOccurred()) {
						if (RelationalExpression()) {
							if (current_token_->get_token_class() == TokenClassEnum::R_PAREN) {
								tokens_.push_back(current_token_);
								current_token_ = scanner.GetNextToken();

								if (!LexycalErrorOccurred()) {
									if (Command()) {
										tokens_.push_back(current_token_);
										current_token_ = scanner.GetNextToken();

										if (!LexycalErrorOccurred()) {
											if (current_token_->get_lexeme() == "else") {
												tokens_.push_back(current_token_);
												current_token_ = scanner.GetNextToken();

												if (!LexycalErrorOccurred()) {
													return Command();
												}
												else {
													return false;
												}
											}
											else {
												return true;
											}
										}
										else {
											return false;
										}
									}
									else {
										ReportSyntaxError("Esperado ao menos um comando");
										return false;
									}
								}
								else {
									return false;
								}
							}
							else {
								ReportSyntaxError("Esperado o simbolo ')'");
								return false;
							}
						}
						else {
							ReportSyntaxError("Esperada uma expressão relacional");
							return false;
						}
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
				return false;
			}
		}
	}
	else {
		ReportSyntaxError("Esperada um comando básico, uma iteração ou um 'if'");
		return false;
	}
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

bool Parser::RelationalExpression()
{
	return false;
}

bool Parser::ArithmeticExpression()
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
