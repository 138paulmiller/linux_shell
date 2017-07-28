#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define PATH_COUNT  3
static const char* PATH[PATH_COUNT] = {"./" , "/bin/",  "/usr/bin/" } ;

int execute(const char* cmd, const char* args);
char* get_input(char* prompt);
char* str_append(const char* str_a, const char* str_b);
main(int argc, char ** argv)
{
	//TODO if cmd is wget and try use fopen on arg and download
	//		files line by line. 
	// First do it in serial by repeatedly calling process_child 
	// Next have each instance of wget process_child call process_child on the next arg
	
	int quit = 1;
	char * input;
	do{
		input = get_input("\ncmd>");//get input from stdin
		//split input by space
		quit = strcmp(input, "quit");
		if(quit != 0)
		{
			char* cmd = strtok(input, " "); //get cmd,
			char* args = strtok(0, ""); //get remaining string
			execute( cmd, args);
		}
		free(input);
	}while(quit != 0);
	
	printf("\nGoodbye\n");
}
//executes cmd at path with given argument
int execute(const char* cmd, const char* args)
{
	//get arg list by tokeninzing args
	int i = 1;
	char* args_buffer=0; 
	char* argv[1024]; //max 5 args
	if(args)
	{
		args_buffer =  (char*)malloc(sizeof(char)*strlen(args));
		strcpy(args_buffer, args);
		argv[1] = strtok(args_buffer, " "); //get cmd		
		while(i<1024&& (argv[++i] = strtok(0, " ")) != 0);
		if(i<1024 && argv[i] == 0) i--;
	}
	int pid = fork(); //fork child process
	//concurrent section, if pid is zero then current 
	//running is child
	if (pid == 0) 
	{
		int j=0;
		while(j < PATH_COUNT)
		{
			argv[0] = str_append(PATH[j++], cmd);
			execvp(argv[0], argv); //if it returns the process failed
			//if control reaches heare than failed so free arg path and try again
			free(argv[0]);
		}
		exit(-1);	
	}
	else //else if current running pid is not zero, it is 		
	{
		int status;
		waitpid(pid, &status, 0);
		if(args_buffer)
			free(args_buffer);
		if(WIFEXITED(status))
		{	
			printf("Exited: %d", WEXITSTATUS(status));
		}else
			printf("%s Command Not found", cmd);
		
	}
	return 0;
}		

char *get_input( char* prompt)
{
	printf("%s", prompt);
	char *line = NULL;
	ssize_t size = 0; // let getline allocate buffer size for us
	//read from stdin default deliminates buffer with newline
	getline(&line, &size, stdin);
	//remove newline at end of input
	if(line != 0 && size > 0){
		int i = -1;
		//find first newline
		while(++i < size && line[i]!='\n');
		line[i]='\0';
	}
	return line;
}

char* str_append(const char* str_a,const char* str_b)
{
	ssize_t size = (strlen(str_a) + strlen(str_b))+1;
	char * str = (char*)malloc(sizeof(char)*size);
	strcpy(str, str_a); //copy first into str buf
	strcat(str, str_b); //concatenate second
	str[size-1] = '\0'; //terminate
	return str;
}

