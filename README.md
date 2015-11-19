# Compiler-Project

Compiler college assignment, scored 10/10.

Symbols:

letra ::= [a-z]

dígito ::= [0-9]

id ::= (letra | "_") (letra | "_" | dígito)*

oprelacional ::= <  |  >  |  <=  |  >=  |  ==  |  !=

oparitmético ::= "+"  |  "-"  |  "*"  |  "/"  |  "="

especial ::= ")"  |  "("  |  "{"  |  "}"  |  ","  |  ";"

palreservada ::= main  |  if  |  else  |  while  |  do  |  for  |  int  |  float  |  char

inteiro ::= dígito+

float ::= dígito*.dígito+

char ::= 'letra'  |  'dígito' 


BNF:

\<programa> ::= int main"("")" \<bloco>

\<bloco> ::= “{“ {\<decl_var>}* {\<comando>}* “}”

\<comando> ::= \<comando_básico> | \<iteração> | if "("\<expr_relacional>")" \<comando> {else \<comando>}?

\<comando_básico> ::= \<atribuição> | \<bloco>

\<iteração> ::= while "("\<expr_relacional>")" \<comando> | do \<comando> while "("\<expr_relacional>")"";"

\<atribuição> ::= \<id> "=" \<expr_arit> ";"

\<exr_relacional> ::= \<expr_arit> \<op_relacional> \<expr_arit>

\<expr_arit> ::= \<expr_arit> "+" \<termo>   | \<expr_arit> "-" \<termo> | \<termo>

\<termo> ::= \<termo> "*" \<fator> | \<termo> “/” \<fator> | \<fator>

\<fator> ::= “(“ \<expr_arit> “)” | \<id> | \<real> | \<inteiro> | \<char>
