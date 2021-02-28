# BarkScript

## What is BarkScript?

BarkScript is an attempt at making a programming language. As of 2021-02-23, all that the language has is:

1. Variable declarations and reassignments
2. Numbers, Booleans, and Null primitives
3. Addition +, Subtraction -, Multiplication \*, Division /, Exponentiation \*\*, Floored Division //, Unary Plus/Positive +, Unary Minus/Negative -
4. "Global Constant Variables" like `true` (Boolean), `false` (Boolean), `Infinity` (Number), `NaN` (Number), `null` (Null)

Documentation has been started but will not be released for a while.

## How it works:

The language is currently split into 3 stages:

1. **The Lexer (a.k.a. Tokenizer)**, which reads through a string of input and products a list (std::vector) of Token's
2. **The Parser**, which reads through the list of Token's from the Lexer and generates an Abstract Syntax Tree of Nodes based on recursive rules (a human readable version of this can be found in [syntax.txt](https://github.com/Samathingamajig/BarkScript/blob/main/syntax.txt) (a guide for how to understand syntax.txt will be made))
3. **The Interpreter**, which travels down the Abstract Syntax Tree and calls the functions defined in each Node's class/struct

## What are the goals:

Refer to the [todo section](https://github.com/Samathingamajig/BarkScript/projects/1) for live goals & progress towards them

More long term goals (in no particular order) include:

- Conversion to some kind of bytecode
- NPM package that can compile to said bytecode
- Compile to some kind of native executable
- Have HTTP traffic available
- A large standard library, split up, of course
- Options for strict typing
- Compilation to WASM (would require strict typing)
- Graphics

## Contributions

Contributions are welcome, ESPECIALLY if I'm doing something wrong, just please remember to keep the code style and file structure consistent (if you add files, remember to modify the project files [1](https://github.com/Samathingamajig/BarkScript/blob/main/BarkScript.vcxproj) [2](https://github.com/Samathingamajig/BarkScript/blob/main/BarkScript.vcxproj.filters))

## License

All code in this repository is under the MIT license, except code in the [/vendor](https://github.com/Samathingamajig/BarkScript/tree/main/vendor) folder. [strings_with_arrows.h](https://github.com/Samathingamajig/BarkScript/blob/main/vendor/strings_with_arrows/strings_with_arrows.h) is a translation from Python3 [here](https://github.com/davidcallanan/py-myopl-code/blob/master/ep14/strings_with_arrows.py) by [David Callanan](https://github.com/davidcallanan).
