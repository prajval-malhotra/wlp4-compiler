# WLP4 Compiler

This is a compiler for the WLP4 language done as a part of the course CS 241 at the University of Waterloo. It includes an assembler for the MIPS assembly language, a scanner, a parser, semantic analyzer, and code generator. More details about WLP4 can be found [here](https://student.cs.uwaterloo.ca/~cs241/wlp4/).

## Scanner

### Usage

```
cd root/scanner
./scanner < [input] > [output] 
./scanner < main.wlp4 > main.wlp4i
```

This program is a scanner (lexical analyzer) for the WLP4 programming language. It reads in a WLP4 program from standard input and identifies the tokens, in the order they appear in the input. For each token, the scanner computes the kind of the token and the lexeme (the string of characters making up the token), and prints it to standard output, one line per token. If the input cannot be scanned or parsed, then we return an appropriate error message. 

## Parser

### Usage

```
cd root/parser
./parser < tokens.txt > main.wlp4i
```

This program is a parser that takes in a grammar and generates the appropriate derivation. If the input sequence is in the language, we output the derivation for the sequence using the reversed rightmost derivation format. Else, a message of “Error at k”, where k is the erroneous, non-recognized part of the input.

## Semantic Analysis and Code Generation

This program checks syntax, type checking, redeclarations, invalid types for parameters passed to functions, invalid operations between different types, etc. Provided the program is semantically correct, we run the code generator, which generates MIPS instructions for valid programs conforming to all semantical rules. 

### Usage

```
cd root/generator
./generator < main.wlp4i > main.asm
```

This program takes as input a .wlp4i file and, if it conforms to the context-sensitive syntax of WLP4, produces as output a MIPS .asm file that may be assembled with the assembler.

## Assembler

### Usage

```
root/assembler
./asm < main.asm > mips.out
```

This program creates an assembler for the MIPS assembly language. It converts ASCII text commands into MIPS machine language that can be executed by the MIPS emulators.
