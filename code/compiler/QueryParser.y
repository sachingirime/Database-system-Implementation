%{
	#include <cstdio>
	#include <cstring>
	#include <cstdlib>
	#include <iostream>
	#include "ParseTree.h" 

	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" void yyerror(char* s);
  
	// these data structures hold the result of the parsing
	struct FuncOperator* finalFunction; // the aggregate function
	struct TableList* tables; // the list of tables in the query
	struct AndList* predicate; // the predicate in WHERE
	struct NameList* groupingAtts; // grouping attributes
	struct NameList* attsToSelect; // the attributes in SELECT
	int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query 

	// new for CREATE INDEX
	char* createIndexName;
	char* createIndexTable;
	char* createIndexAttribute;
%}


// stores all of the types returned by production rules: DO NOT REMOVE!!!
%union {
 	struct FuncOperand* myOperand;
	struct FuncOperator* myOperator; 
	struct TableList* myTables;
	struct ComparisonOp* myComparison;
	struct Operand* myBoolOperand;
	struct AndList* myAndList;
	struct NameList* myNames;
	char* actualChars;
	char whichOne;
}


%token <actualChars> YY_NAME
%token <actualChars> YY_FLOAT
%token <actualChars> YY_INTEGER
%token <actualChars> YY_STRING
%token SELECT
%token GROUP 
%token DISTINCT
%token BY
%token FROM
%token WHERE
%token SUM
%token AND
%token CREATE
%token INDEX
%token TABLE
%token ON

%type <myAndList> AndList
%type <myOperand> SimpleExp
%type <myOperator> CompoundExp
%type <whichOne> Op 
%type <myComparison> BoolComp
%type <myComparison> Condition
%type <myTables> Tables
%type <myBoolOperand> Literal
%type <myNames> Atts

%start SQL


//******************************************************************************
/* This is the PRODUCTION RULES section which defines how to "understand" the 
 * input language and what action to take for each "statment".
 */
//******************************************************************************

%%

SQL: SELECT SelectAtts FROM Tables WHERE AndList
{
	tables = $4;
	predicate = $6;	
	groupingAtts = NULL;
}

| SELECT SelectAtts FROM Tables WHERE AndList GROUP BY Atts
{
	tables = $4;
	predicate = $6;	
	groupingAtts = $9;
};

| CREATE INDEX YY_NAME TABLE YY_NAME ON YY_NAME
  {
	createIndexName = $3;
	createIndexTable = $5;
	createIndexAttribute = $7;
  };

SelectAtts: Function ',' Atts 
{
	attsToSelect = $3;
	distinctAtts = 0;
}

| Function
{
	attsToSelect = NULL;
}

| Atts 
{
	distinctAtts = 0;
	finalFunction = NULL;
	attsToSelect = $1;
}

| DISTINCT Atts
{
	distinctAtts = 1;
	finalFunction = NULL;
	attsToSelect = $2;
	finalFunction = NULL;
};


Function: SUM '(' CompoundExp ')'
{
	finalFunction = $3;
};


Atts: YY_NAME
{
	$$ = (struct NameList*) malloc (sizeof (struct NameList));
	$$->name = $1;
	$$->next = NULL;
} 

| Atts ',' YY_NAME
{
	$$ = (struct NameList*) malloc (sizeof (struct NameList));
	$$->name = $3;
	$$->next = $1;
};


Tables: YY_NAME 
{
	$$ = (struct TableList*) malloc (sizeof (struct TableList));
	$$->tableName = $1;
	$$->next = NULL;
}

| Tables ',' YY_NAME
{
	$$ = (struct TableList*) malloc (sizeof (struct TableList));
	$$->tableName = $3;
	$$->next = $1;
};


CompoundExp: SimpleExp Op CompoundExp
{
	$$ = (struct FuncOperator*) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = (struct FuncOperator*) malloc (sizeof (struct FuncOperator));
	$$->leftOperator->leftOperator = NULL;
	$$->leftOperator->leftOperand = $1;
	$$->leftOperator->right = NULL;
	$$->leftOperand = NULL;
	$$->right = $3;
	$$->code = $2;	
}

| '(' CompoundExp ')' Op CompoundExp
{
	$$ = (struct FuncOperator*) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = $2;
	$$->leftOperand = NULL;
	$$->right = $5;
	$$->code = $4;
}

| '(' CompoundExp ')'
{
	$$ = $2;
}

| SimpleExp
{
	$$ = (struct FuncOperator*) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = NULL;
	$$->leftOperand = $1;
	$$->right = NULL;	
}

| '-' CompoundExp
{
	$$ = (struct FuncOperator*) malloc (sizeof (struct FuncOperator));	
	$$->leftOperator = $2;
	$$->leftOperand = NULL;
	$$->right = NULL;	
	$$->code = '-';
};


Op: '-'
{
	$$ = '-';
}

| '+'
{
	$$ = '+';
}

| '*'
{
	$$ = '*';
}

| '/'
{
	$$ = '/';
};


AndList: Condition AND AndList
{
        // we have to pre-pend the OrList to the AndList
        // first we allocate space for this node
        $$ = (struct AndList*) malloc (sizeof (struct AndList));

        // hang the OrList off of the left
        $$->left = $1;

        // hang the AndList off of the right
        $$->rightAnd = $3;
}

| Condition
{
        // return the OrList
        $$ = (struct AndList*) malloc (sizeof (struct AndList));
        $$->left = $1;
        $$->rightAnd = NULL;
};


Condition: Literal BoolComp Literal
{
        // in this case we have a simple literal/variable comparison
        $$ = $2;
        $$->left = $1;
        $$->right = $3;
};


BoolComp: '<'
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp*) malloc (sizeof (struct ComparisonOp));
        $$->code = LESS_THAN;
}

| '>'
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp*) malloc (sizeof (struct ComparisonOp));
        $$->code = GREATER_THAN;
}

| '='
{
        // construct and send up the comparison
        $$ = (struct ComparisonOp*) malloc (sizeof (struct ComparisonOp));
        $$->code = EQUALS;
};

Literal: YY_STRING
{
        // construct and send up the operand containing the string
        $$ = (struct Operand*) malloc (sizeof (struct Operand));
        $$->code = STRING;
        $$->value = $1;
}

| YY_FLOAT
{
        // construct and send up the operand containing the FP number
        $$ = (struct Operand*) malloc (sizeof (struct Operand));
        $$->code = FLOAT;
        $$->value = $1;
}

| YY_INTEGER
{
        // construct and send up the operand containing the integer
        $$ = (struct Operand*) malloc (sizeof (struct Operand));
        $$->code = INTEGER;
        $$->value = $1;
}

| YY_NAME
{
        // construct and send up the operand containing the name
        $$ = (struct Operand*) malloc (sizeof (struct Operand));
        $$->code = NAME;
        $$->value = $1;
};


SimpleExp: YY_FLOAT
{
        // construct and send up the operand containing the FP number
        $$ = (struct FuncOperand*) malloc (sizeof (struct FuncOperand));
        $$->code = FLOAT;
        $$->value = $1;
} 

| YY_INTEGER
{
        // construct and send up the operand containing the integer
        $$ = (struct FuncOperand*) malloc (sizeof (struct FuncOperand));
        $$->code = INTEGER;
        $$->value = $1;
} 

| YY_NAME
{
        // construct and send up the operand containing the name
        $$ = (struct FuncOperand*) malloc (sizeof (struct FuncOperand));
        $$->code = NAME;
        $$->value = $1;
};

%%
