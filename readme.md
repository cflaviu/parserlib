# Parserlib #

Parserlib is a PEG parser library with the following features:

- it is written in C++17.
- it utilizes C++ operators to make the grammar readable like EBNF.
- it separates concerns: parsing events are created outside of the grammar declaration.
- it supports left recursion.
- it is very versatile and can support any type of symbol or input stream.

## Version ##

The current version is 0.0.2.

The old version is kept, for now, in the subdirectory *old*.

## Example ##

Here is an example of a calculator grammar:

	extern Rule<> expr;
	
	Rule<> num = +range('0', '9');
	
	Rule<> val = '(' >> expr >> ')'
	           | num;
	
	Rule<> mul = mul >> '*' >> val
	           | mul >> '/' >> val
	           | val;
	
	Rule<> add = add >> '+' >> mul
	           | add >> '-' >> mul
	           | mul;
	
	Rule<> expr = add;

## To do... ## 

1. support for ASTs.
2. support for multiple error reporting.
2. unit tests.
2. port examples to new version.
3. write new and better examples (well, maybe :-)).
