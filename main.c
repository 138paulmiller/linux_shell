#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
const char* USR_BIN_PATH = "/usr/bin/";
const char* BIN_PATH = "/bin/";
void process_parent();
void process_child(const char* cmd, const char* args);
int execute(const char*path, const char* cmd, const char* args);
char* get_input(const char* prompt);
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
		input = get_input("cmd>");
		//split input by space
		char* cmd = strtok(input, " "); //get cmd,
		char* args = strtok(0, ""); //get remaining string
		quit = strcmp(input, "quit");
		if(quit != 0)
		{
			int pid = fork(); //fork child process
			//concurrent section, if pid is zero then current 
			//running is child
			if (pid == 0) 
			{
				process_child(cmd, args);
			}
			else //else if current running pid is not zero, it is 
				//paretn process
			{
				process_parent();
			}
		}
	}while(quit);
	
	printf("\nGoodbye\n");
}
void process_parent()
{
	if(wait(0)>-1)
		printf("Done\n");
}

void process_child(const char* cmd, const char* args)
{
	//-1 indicates failure to execute
	//try to execute from bin_path
	if(execute(BIN_PATH, cmd, args) == -1)
	//try to execute from user/bin
		if(execute(USR_BIN_PATH, cmd, args) == -1)
			printf("Command not found"); 

}
//executes cmd at path with given argument
int execute(const char*path, const char* cmd, const char* arg)
{
	char * full_path = str_append(path, cmd); 
	return execlp(full_path, cmd, arg, (char*)0);
}

char *get_input(const char* prompt)
{
	printf("\n%s", prompt);
	char *line = NULL;
	ssize_t size = 0; // let getline allocate buffer size for us
	//read from stdin default deliminates buffer with newline
	getline(&line, &size, stdin);
	//remove newline at end of input
	int i = -1;
	//find first newline
	while(++i < size && line[i]!='\n');
	line[i]='\0';
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

