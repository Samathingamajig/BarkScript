barkscript : BarkScript.cpp Lexer.cpp Parser.cpp
	.\build.bat

cleanobj :
	rm BarkScript.obj Lexer.obj Parser.obj

cleanobjcmd :
	del BarkScript.obj Lexer.obj Parser.obj
