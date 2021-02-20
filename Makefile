windowsvs : build BarkScript.cpp lexer/Lexer.cpp parser/Parser.cpp object/Object.cpp interpreter/Interpreter.cpp symboltable/SymbolTable.cpp
	.\build.bat

linuxgpp : build BarkScript.cpp lexer/Lexer.cpp parser/Parser.cpp object/Object.cpp interpreter/Interpreter.cpp symboltable/SymbolTable.cpp
	g++ -o ./build/BarkScript -O2 -Wall BarkScript.cpp lexer/Lexer.cpp parser/Parser.cpp object/Object.cpp interpreter/Interpreter.cpp symboltable/SymbolTable.cpp

cleanobj :
	rm *.obj

cleanobjcmd :
	del *.obj

build :
	mkdir build
