grammar pdfl;

// Rules
pgm: expr_list EOF;

expr_list: expr SEMI expr;
expr: set_expr |
      rel_expr |
      fxn_expr |
      IDENT |
      DIGIT |
      ;     // eps
set_expr: IDENT LPAREN tuple_list RPAREN EQ LBRACE constr list RBRACE COLON stmt_list;
tuple_list: IDENT | IDENT COMMA tuple_list;
constr_list: constraint | constraint CARET constr_list;
stmt_list: stmt | stmt COMMA stmt_list
rel_expr: LBRACE constr_list RBRACE STAR IDENT;
fxn_expr: IDENT EQ IDENT LPAREN expr_list RPAREN;


environment: ID LPAREN RPAREN EQ INVERSE ID LPAREN RPAREN;
//         { Environment* env = new Environment();
//           env->setInverse(*$1,*$6);
//           delete $1;
//           delete $6;
//           $$ = env;
//         }

omega_set: in_symbolic conjunct_list;
// {
//    std::list<Conjunction*>::iterator it=$2->begin();
//    std::list<Conjunction*>::iterator end=$2->end();
//    Set* set = new Set((*it)->arity());
//    for (; it!=end; ++it) {
//       set->addConjunction(*it);
//    }
//    $$ = set;
//    delete $2;
//    delete $1;
// }

isl_set: in_symbolic LBRACE semi_conjunct SEMI semi_conjunct_list RBRACE;
// {
//    Set* set = new Set($3->arity());
//    set->addConjunction($3);
//
//    std::list<Conjunction*>::iterator it=$5->begin();
//    std::list<Conjunction*>::iterator end=$5->end();
//    for (; it!=end; ++it) {
//       set->addConjunction(*it);
//    }
//    $$ = set;
//    delete $5;
//    delete $1;
// }

omega_relation: in_symbolic rel_conjunct_list;
// {
//    std::list<Conjunction*>::iterator it=$2->begin();
//    std::list<Conjunction*>::iterator end=$2->end();
//    int inarity = (*it)->inarity();
//    int outarity = (*it)->arity()-inarity;
//    Relation* relation = new Relation(inarity,outarity);
//    for (; it!=end; ++it) {
//       relation->addConjunction(*it);
//    }
//    $$ = relation;
//    delete $2;
//    delete $1;
// }

isl_relation: in_symbolic LBRACE semi_rel_conjunct SEMI semi_rel_conjunct_list RBRACE;
// {
//    int inarity = $3->inarity();
//    int outarity = $3->arity()-inarity;
//    Relation* relation = new Relation(inarity,outarity);
//    relation->addConjunction($3);
//
//    std::list<Conjunction*>::iterator it=$5->begin();
//    std::list<Conjunction*>::iterator end=$5->end();
//    for (; it!=end; ++it) {
//       relation->addConjunction(*it);
//    }
//    $$ = relation;
//    delete $5;
//    delete $1;
// }

   conjunct_list: single_conjunct
//         {
//            std::list<Conjunction*>* conlist=
//               new std::list<Conjunction*>();
//            conlist->push_back($1);
//            $$ = conlist;
//         }

      | conjunct_list UNION single_conjunct;
//         {
//            $1->push_back($3);
//            $$=$1;
//         }

   single_conjunct: LBRACE tuple_decl_with_brackets optional_constraints RBRACE;
//         {
//            Conjunction* conj = new Conjunction($2->size());
//            conj->setTupleDecl( *($2) );
//            delete $2;
//
//            std::list<Exp*>::iterator it2 = $3->begin();
//            std::list<Exp*>::iterator end2 = $3->end();
//            for (; it2 != end2; ++it2) {
//               if ((*it2)->isEquality())
//                  { conj->addEquality(*it2); }
//               else if ((*it2)->isInequality())
//                  { conj->addInequality(*it2); }
//            }
//
//            conj->substituteTupleDecl();
//            $$ = conj;
//            delete $3;
//         }

   rel_conjunct_list: rel_single_conjunct
//         {
//            std::list<Conjunction*>* conlist=
//               new std::list<Conjunction*>();
//            conlist->push_back($1);
//            $$ = conlist;
//         }

      | rel_conjunct_list UNION rel_single_conjunct;
//         {
//            $1->push_back($3);
//            $$=$1;
//         }

   rel_single_conjunct: LBRACE tuple_decl_with_brackets ARROW tuple_decl_with_brackets optional_constraints RBRACE;
//         {
//            int arity = $2->size() + $4->size(), inarity = $2->size();
//            Conjunction* conj = new Conjunction(arity, inarity);
//            conj->setTupleDecl( *($2), *($4) );
//            delete $2;
//            delete $4;
//
//            std::list<Exp*>::iterator it2 = $5->begin();
//            std::list<Exp*>::iterator end2 = $5->end();
//            for (; it2 != end2; ++it2) {
//               if ((*it2)->isEquality())
//                  { conj->addEquality(*it2); }
//               else if ((*it2)->isInequality())
//                  { conj->addInequality(*it2); }
//            }
//
//            conj->substituteTupleDecl();
//            $$ = conj;
//
//            delete $5;
//         }

   semi_conjunct_list: semi_conjunct SEMI semi_conjunct_list
//         {
//            $3->push_back($1);
//            $$ = $3;
//         }

      | semi_conjunct;
//         {
//            std::list<Conjunction*>* conlist =
//               new std::list<Conjunction*>();
//            conlist->push_back($1);
//            $$ = conlist;
//         }

   semi_conjunct: tuple_decl_with_brackets optional_constraints;
//         {
//            Conjunction* conj = new Conjunction($1->size());
//            conj->setTupleDecl(*($1));
//            delete $1;
//
//            std::list<Exp*>::iterator it2 = $2->begin();
//            std::list<Exp*>::iterator end2 = $2->end();
//            for (; it2 != end2; ++it2) {
//               if ((*it2)->isEquality())
//                  { conj->addEquality(*it2); }
//               else if ((*it2)->isInequality())
//                  { conj->addInequality(*it2); }
//            }
//
//            conj->substituteTupleDecl();
//            $$ = conj;
//            delete $2;
//
//         }

   semi_rel_conjunct_list: semi_rel_conjunct SEMI semi_rel_conjunct_list
//         {
//            $3->push_back($1);
//            $$ = $3;
//         }

      | semi_rel_conjunct;
//         {
//            std::list<Conjunction*>* conlist =
//               new std::list<Conjunction*>();
//            conlist->push_back($1);
//            $$ = conlist;
//         }

   semi_rel_conjunct: tuple_decl_with_brackets ARROW tuple_decl_with_brackets optional_constraints
//         {
//            int arity = $1->size() + $3->size(), inarity = $3->size();
//            Conjunction* conj = new Conjunction(arity, inarity);
//            conj->setTupleDecl(*($1),*($3));
//            delete $1;
//            delete $3;
//
//            std::list<Exp*>::iterator it2 = $4->begin();
//            std::list<Exp*>::iterator end2 = $4->end();
//            for (; it2 != end2; ++it2) {
//               if ((*it2)->isEquality())
//                  { conj->addEquality(*it2); }
//               else if ((*it2)->isInequality())
//                  { conj->addInequality(*it2); }
//            }
//
//            conj->substituteTupleDecl();
//            $$ = conj;
//            delete $4;
//
//         }
      | tuple_decl_with_brackets ARROW; // error;
//      {
//            delete $1;
//            $$ =  new Conjunction(0,0);
//            yyclearin;
//      }

   in_symbolic: LBRACKET symbol_list RBRACKET ARROW
//         {$$ = $2;}

      | ; //epsilon
//         {$$ = new std::list<std::string>();}

   symbol_list: ID
//         {
//            std::list<std::string>* symbols = new std::list<std::string>();
//            symbols->push_back(*$1);
//            $$ = symbols;
//            delete $1;
//         }

      | ID COMMA symbol_list;
//         {
//            $3->push_back(*$1);
//            $$ = $3;
//            delete $1;
//         }

   tuple_decl_with_brackets: LBRACKET RBRACKET
//         {
//            $$ = new TupleDecl();
//         }

      | /* epsilon */
//         {
//            $$ = new TupleDecl();
//         }

      | LBRACKET tuple_decl RBRACKET
//         { $$= $2; }

      | LBRACKET tuple_decl; // error;
//         {
//            delete $2;
//            $$ = new TupleDecl(0);
//            yyclearin;
//         }

   tuple_decl: tuple_elem
//         {
//            $$ = $1;
//         }
      | tuple_decl COMMA tuple_elem;
//         {
//            $$ = $1;
//            $$->concat(*$3);
//            delete $3;
//         }

   tuple_elem: variable_id
//         { $$ = new TupleDecl(1);
//           $$->setTupleElem(0, *$1);
//           delete $1;
//         }
      | INT;
//         { $$ = new TupleDecl(1);
//           $$->setTupleElem(0, $1);
//         }

   variable_id: ID;
//         {$$ = $1;}

  optional_constraints: COLON constraint_list
//         {$$ = $2;}
      | COLON constraint_list // error
//         {
//            for (std::list<Exp*>::iterator it=$2->begin(); it!=$2->end(); ++it)
//            { delete *it; }
//
//            delete $2;
//            $$ = new std::list<Exp*>();
//            yyclearin;
//         }
      | COLON //epsilon
//         {$$ = new std::list<Exp*>();}
      | ; //epsilon
//         {$$ = new std::list<Exp*>();}

   constraint_list: constraint
//         {
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->push_back($1);
//            $$ = clist;
//         }
      | constraint_list AND constraint
//         {
//            $1->push_back($3);
//            $$ = $1;
//         }
      | constraint_list WAND constraint
//         {
//            $1->push_back($3);
//            $$ = $1;
//         }
     | constraint_range
//         {
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->splice(clist->end(), *($1) );
//            $$ = clist;
//         }
      | constraint_list AND constraint_range
//         {
//            $1->splice ($1->end(), *($3) );
//            $$ = $1;
//         }
      | constraint_list WAND constraint_range;
//         {
//            $1->splice ($1->end(), *($3) );
//            $$ = $1;
//         }

   constraint: constraint_eq
//         {$$ = $1;}
      | constraint_gt
//         {$$ = $1;}
      | constraint_gte
//         {$$ = $1;}
      | constraint_lt
//         {$$ = $1;}
      | constraint_lte;
//         {$$ = $1;}

   constraint_eq: expression EQ expression;
//         {
//            $1->setEquality();
//            $3->multiplyBy(-1);
//            $1->addExp($3);
//            $$ = $1;
//         }

   constraint_lt: expression LT expression;
//         {
//            $3->setInequality();
//            $1->addTerm(new Term(1));
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//            $$ = $3;
//         }

   constraint_lte: expression LTE expression;
//         {
//            $3->setInequality();
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//            $$ = $3;
//         }

   constraint_gt: expression GT expression;
//         {
//            $1->setInequality();
//            $3->addTerm(new Term(1));
//            $3->multiplyBy(-1);
//            $1->addExp($3);
//            $$ = $1;
//         }

   constraint_gte: expression GTE expression;
//         {
//            $1->setInequality();
//            $3->multiplyBy(-1);
//            $1->addExp($3);
//            $$ = $1;
//         }

   constraint_range: constraint_range_lte_lt
//         {$$ = $1;}
      | constraint_range_lt_lte
//         {$$ = $1;}
      | constraint_range_lte_lte
//         {$$ = $1;}
      | constraint_range_lt_lt;
//         {$$ = $1;}

   constraint_range_lte_lt: expression LTE expression LT expression;
//         {
//            Exp* tExp = $3->clone();
//
//            $3->setInequality();
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//
//            $5->setInequality();
//            tExp->addTerm(new Term(1));
//            tExp->multiplyBy(-1);
//            $5->addExp(tExp);
//
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->push_back($3);
//            clist->push_back($5);
//            $$ = clist;
//         }

   constraint_range_lt_lte: expression LT expression LTE expression;
//         {
//            Exp* tExp = $3->clone();
//
//            $3->setInequality();
//            $1->addTerm(new Term(1));
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//
//            $5->setInequality();
//            tExp->multiplyBy(-1);
//            $5->addExp(tExp);
//
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->push_back($3);
//            clist->push_back($5);
//            $$ = clist;
//         }

   constraint_range_lte_lte: expression LTE expression LTE expression;
//         {
//            Exp* tExp = $3->clone();
//
//            $3->setInequality();
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//
//            $5->setInequality();
//            tExp->multiplyBy(-1);
//            $5->addExp(tExp);
//
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->push_back($3);
//            clist->push_back($5);
//            $$ = clist;
//         }

   constraint_range_lt_lt: expression LT expression LT expression;
//         {
//            Exp* tExp = $3->clone();
//
//            $3->setInequality();
//            $1->addTerm(new Term(1));
//            $1->multiplyBy(-1);
//            $3->addExp($1);
//
//            $5->setInequality();
//            tExp->addTerm(new Term(1));
//            tExp->multiplyBy(-1);
//            $5->addExp(tExp);
//
//            std::list<Exp*>* clist = new std::list<Exp*>();
//            clist->push_back($3);
//            clist->push_back($5);
//            $$ = clist;
//         }

   expression: expression_int
//         { $$ = $1; }
      | expression_simple
//         { $$ = $1; }
      | expression_unop
//         { $$ = $1; }
//      | expression_binop
//         { $$ = $1; }
      | expression PLUS expression
      | expression DASH expression
      | expression_int_mult
//         { $$ = $1; }
      | expression_tuple;
//         { $$ = $1; }

   expression_tuple: LPAREN expression_list RPAREN;
//         {
//            // If there is only one expression then just pass
//            // that up, otherwise create a TupleExpTerm and put that
//            // in an Exp.
//            if ($2->size() == 1) {
//                $$ = $2->front();
//            } else {
//                TupleExpTerm* tuple = new TupleExpTerm($2->size());
//                unsigned int count = 0;
//                for (std::list<Exp*>::iterator i=$2->begin(); i != $2->end();
//                        ++i) {
//                    tuple->setExpElem(count++,*i);
//                }
//                Exp* exp = new Exp();
//                exp -> addTerm(tuple);
//                $$ = exp;
//            }
//            delete $2;
//         }

   expression_int: INT;
//         {
//            Exp* exp = new Exp();
//            exp->addTerm(new Term($1));
//            $$ = exp;
//         }

    expression_unop: DASH expression UMINUS;
//   expression_unop: DASH expression %prec UMINUS;
//         {
//            $2->multiplyBy(-1);
//            $$ = $2;
//         }

//   expression_binop: expression PLUS expression
//         {
//            $1->addExp($3);
//            $$ = $1;
//         }
//      | expression DASH expression;
//         {
//            $3->multiplyBy(-1);
//            $1->addExp($3);
//            $$ = $1;
//         }

   expression_int_mult: INT expression_simple
//         {
//            $2->multiplyBy($1);
//            $$ = $2;
//         }
      | INT STAR expression_simple
//         {
//            $3->multiplyBy($1);
//            $$ = $3;
//         }
      | expression_simple STAR INT;
//         {
//            $1->multiplyBy($3);
//            $$ = $1;
//         }

   expression_simple: expression_id
//         { $$ = $1; }
      | expression_func
//         { $$ = $1; }
      | expression_tuple;
//         { $$ = $1; }

   expression_id: ID;
//         {
//            Exp* exp = new Exp();
//            exp->addTerm(new VarTerm(*$1));
//            $$ = exp;
//            delete $1;
//         }

/*
   expression_paren: LPAREN expression RPAREN
         { $$=$2; }
*/

   expression_func: variable_id LPAREN expression_list RPAREN
//         {
//            UFCallTerm* uf_call = new UFCallTerm(*$1, $3->size());
//            unsigned int count = 0;
//            for (std::list<Exp*>::iterator i=$3->begin(); i != $3->end(); ++i) {
//                uf_call->setParamExp(count++,*i);
//            }
//            Exp* exp = new Exp();
//            exp -> addTerm(uf_call);
//            $$ = exp;
//            delete $1;
//            delete $3;
//         }
      | variable_id LPAREN expression_list RPAREN LBRACKET INT RBRACKET
//         {
//            UFCallTerm* uf_call = new UFCallTerm(*$1, $3->size(), $6);
//            unsigned int count = 0;
//            for (std::list<Exp*>::iterator i=$3->begin(); i != $3->end(); ++i) {
//                uf_call->setParamExp(count++,*i);
//            }
//            Exp* exp = new Exp();
//            exp -> addTerm(uf_call);
//            $$ = exp;
//            delete $3;
//            delete $1;
//         }
      | variable_id LPAREN expression_list; // error;
//         {
//            for (std::list<Exp*>::iterator it=$3->begin(); it!=$3->end(); ++it)
//            { delete *it; }
//            $$ = new Exp();
//            delete $1;
//            delete $3;
//            yyclearin;
//         }

   expression_list: expression_list COMMA expression
//         {
//            $1->push_back($3);
//            $$ = $1;
//         }
      | expression;
//         {
//            std::list<Exp*>* exp_list = new std::list<Exp*>();
//            exp_list->push_back($1);
//            $$ = exp_list;
//         }

// Tokens

//{IDENT} { yylval.sval = new std::string(yytext);
//          return ID;}
//
//{DIGIT} { yylval.ival = atoi(yytext);
//          return INT;}
//
//{INVALID_IDENT} { //yylval.sval = new std::string(yytext);
//          return INVALID_ID;}

DIGIT:    [0-9]+;
/*IDENT    [a-zA-Z][a-zA-Z0-9'_]**/
IDENT: [a-zA-Z'_]?[a-zA-Z'$]+[a-zA-Z0-9'$_]*;
/*catches invalid identifiers*/
INVALID_IDENT : [_$]*[a-zA-Z'_]?[a-zA-Z'$]+[a-zA-Z0-9'$_]*;
PLUS : '+';
DASH : '-';
STAR : '*';
LBRACE : '{';
RBRACE : '}';
LBRACKET : '[';
RBRACKET : ']';
LPAREN : '(';
RPAREN : ')';
COMMA : ',';
COLON : ':';
SEMI : ';';
EQ : '=';
LT : '<';
GT : '>';
LTE : '<=';
GTE : '>=';
ARROW : '->';
AND : '&&';
UNION : [uU] [nN] [iI] [oO] [nN];
INVERSE : [iI] [nN] [vV] [eE] [rR] [sS] [eE];
EXISTS : [eE] [xX] [iI] [sS] [tT] [sS];
OR : [oO][nN];
WAND : [aA][nN][dD];                            /*produces the word and*/
WS : [ \t\n\r] + -> skip;
ID: IDENT;
INT: DIGIT;
UMINUS: DASH;
