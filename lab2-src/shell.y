
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT  LESS NEWLINE GREATGREAT PIPE BACKGROUND GREATGREATBACK EXIT

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <string>
#include <unistd.h> 
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "command.h"
#include <regex.h>
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command:
    simple_command
    | EXIT {
        printf("   Yacc: Exit\n");
		printf("   see you next soon ^_^\n");
        exit(0);
    }
    ;
simple_command:	
	command_and_args iomodifier_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	
	;

iomodifier_opt:

	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	
	}
	|	LESS WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	} 
	| 	GREATGREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
	}  
        | GREAT WORD iomodifier_opt  {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|	LESS WORD iomodifier_opt {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	} 
	| 	GREATGREAT WORD iomodifier_opt  {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	} | PIPE commands {
		//handle pipe 
		Command::_currentCommand._pipe = 1;
	                      } 
        
	|   BACKGROUND {
               printf("Yacc: insert background (&)\n");
               Command::_currentCommand._background= 1;
                  } 
    | 	
	GREATGREATBACK WORD {
		Command::_currentCommand._append = 1;
		Command::_currentCommand._errFile = $2;
		 Command::_currentCommand._background= 1;
	
	}
	|
	;

	                           
	                           
inp_opt: WORD {
               printf("   Yacc: insert  \"%s\"\n", $1);
		Command::_currentCommand._inputFile = $1;
              }	
              |
              ;                           
	

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
