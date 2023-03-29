# Parserlib

A c++17 recursive-descent parser library that can parse left-recursive grammars.

## Table Of Contents
[Introduction](#Introduction)

[Using the Library](#LibraryUsage)

[Writing a Grammar](#GrammarConstruction)

## <a id="Introduction"></a>Introduction

Parserlib allows writing of recursive-descent parsers in c++ using the language's operators in order to imitate <a src="https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form">Extended Backus-Naur Form (EBNF)</a> syntax.

The library can handle left recursion.

Here is a Calculator grammar example:

```cpp
extern Rule<> add;

const auto val = +terminalRange('0', '9');

const auto num = val
               | terminal('(') >> add >> terminal(')');

Rule<> mul = mul >> terminal('*') >> num
           | mul >> terminal('/') >> num
           | num;

Rule<> add = add >> terminal('+') >> mul
           | add >> terminal('-') >> mul
           | mul;
```

The above grammar is a direct translation of the following left-recursive EBNF grammar:

```
add = add + mul
    | add - mul
    | mul
    
mul = mul * num
    | mul / num
    | num
    
num = val
    | ( add )
    
val = (0..9)+
```

## <a name="LibraryUsage"></a>Using the Library

The library is available as headers only, since every class is templated.

In order to use it, you have to have the path to its root include folder in your project's include folder list.

Then, you have to either include the root header, like this:

```cpp
#include "parserlib.hpp"
```

Or use the various headers in the subfolder 'parserlib'.

All the code is included in the namespace `parserlib`:

```cpp
using namespace parserlib;
```

## <a id="GrammarConstruction"></a>Writing a Grammar

A grammar can be written as a series of parsing expressions, formed by operators and by functions that create parser objects.

### Terminals

The most basic parser is the `TerminalParser`, which is used to parse a terminal. In order to write a terminal expression, the following code must be written:

```cpp
terminal('x')
terminal("abc")
```

*The terminals in this library are by default of type `char`, but they can be customized to be anything.*

Other types of terminal parsers are:

```cpp
terminalRange('a', 'z') //parses all values between 'a' and 'z'.
terminalSet('+', '-') //parses '+' or '-'.
```

### Sequences

Terminals can be combined in sequences using the `operator >>`:

```cpp
const auto ab = terminal('a') >> terminal('b');
const auto abc = ab >> terminal('c');
```

In order to parse a sequence successfully, all members of that sequence shall parse successfully.

### Branches

Expressions can have branches:

```cpp
const auto this_or_that = terminal("this")
                        | terminal("that");
```

Branches are followed in top-to-bottom fashion.
If a branch fails to parse, then the next branch is selected, until a branch is found or no more branches exist to follow.

### Loops
