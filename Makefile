windowsvs : BarkScript.cpp Lexer.cpp Parser.cpp Object.cpp Interpreter.cpp
	.\build.bat

linuxgpp : BarkScript.cpp Lexer.cpp Parser.cpp Object.cpp Interpreter.cpp
	g++ -o ./build/BarkScript -O2 -Wall BarkScript.cpp Lexer.cpp Parser.cpp Object.cpp Interpreter.cpp

cleanobj :
	rm BarkScript.obj Lexer.obj Parser.obj Object.obj Interpreter.obj

cleanobjcmd :
	del BarkScript.obj Lexer.obj Parser.obj Object.obj Interpreter.obj
