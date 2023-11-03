
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <fstream> // Add this line to include the fstream header
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "command.h"

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

// for chdir()

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Check if the command is "cd"
	if (strcmp(_simpleCommands[0]->_arguments[0], "cd") == 0) {
		// If no argument is provided, change to home directory
		if (_simpleCommands[0]->_numberOfArguments == 1) {
			chdir(getenv("HOME"));
		}
		// Otherwise, change to the specified directory
		else {
			chdir(_simpleCommands[0]->_arguments[1]);
		}
	}
	// Otherwise, execute the command normally
	else {
		pid_t pid = fork();
		if (pid == 0) {
			// Child process
			// Setup i/o redirection
			if (_inputFile) {
				int fd = open(_inputFile, O_RDONLY);
				dup2(fd, 0);
				close(fd);
			}
		if (_outFile) {
    int fd;
    if (_append) {
        fd = open(_outFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
    } else {
        fd = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    dup2(fd, 1);
    close(fd);
}
			if (_errFile) {
				int fd = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(fd, 2);
				close(fd);
			}
			// Execute the command
			execvp(_simpleCommands[0]->_arguments[0], _simpleCommands[0]->_arguments);
			perror("execvp");
			exit(1);
		}
		else if (pid > 0) {
			// Parent process
			if (!_background) {
				waitpid(pid, NULL, 0);
			}
			else {
				// Print the background process ID
				printf("Background process ID: %d\n", pid);
			}
		}
		else {
			perror("fork");
			exit(1);
		}
	}

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	//printf("myshell>");
	//print working directory
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("%s", cwd);
	printf(">");


	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

void handle_SIGINT(int signal) {
	printf("\n");
	Command::_currentCommand.prompt();
	fflush(stdout);
}
void handle_SIGCHLD(int sig){
	std::fstream fs;
	fs.open("log.txt", std::fstream::app);
	// fs child terminated message to log.txt with signal number
	fs << "Child terminated with signal number: " << sig << "\n";
	fs.close();
}

int 
main()
{   
	signal(SIGINT, handle_SIGINT);
	signal(SIGCHLD, handle_SIGCHLD);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

