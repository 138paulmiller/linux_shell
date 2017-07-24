#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
const char* USR_BIN_PATH = "/usr/bin/";
const char* BIN_PATH = "/bin/";
const char* DOWNLOAD = "dl"; //download url_file 

void download_serial(const char* url_file);
void process_fork(char* cmd, char*args); //forks current process
void process_parent(const char* cmd, const char* args);
void process_child(const char* cmd, const char* args);
int execute(const char*path, const char* cmd, const char* args);
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
		input = get_input("cmd>");//get input from stdin
		//split input by space
		quit = strcmp(input, "quit");
		if(quit != 0)
		{
			char* cmd = strtok(input, " "); //get cmd,
			char* args = strtok(0, ""); //get remaining string
			process_fork(cmd, args);
		}
	}while(quit != 0);
	
	printf("\nGoodbye\n");
}

void download_serial(const char* url_file)
{
	FILE *file = fopen(url_file,"r");
	if(file == 0)
	{
		printf("Could not open %s", url_file);
	}
	else
	{
		fseek(file, 0, SEEK_END); //move to end
		int file_size = ftell(file); //get pos num
		fseek(file, 0, SEEK_SET); //go to beg of file
		char* status;
		char* line = (char*)(malloc(sizeof(char)*file_size));
		while(fgets(line, file_size, file))
		{
			//each line is an arg to wget
			line[strlen(line)-1]='\0'; //remove newline
			//printf("GET %s %d", line, strlen(line));
			process_fork("wget", line);
		}
		fclose (file);
	}
}
void process_fork(char* cmd, char*args)
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
		process_parent(cmd, args);
	}
}

void process_parent(const char* cmd, const char* args)
{
	int status = wait(0);
	if(status>-1)
		printf("\n%s %s: Exited: %d\n", cmd, args, status);
}

void process_child(const char* cmd, const char* args)
{
	//-1 indicates failure to execute
	//try to execute from bin_path
	if(execute(BIN_PATH, cmd, args) == -1)
	//try to execute from user/bin
		if(execute(USR_BIN_PATH, cmd, args) == -1)
			if(strcmp(cmd, DOWNLOAD)==0)
				download_serial(args);
			else
				printf("Command not found"); 
	exit(1);
}
//executes cmd at path with given argument
int execute(const char*path, const char* cmd, const char* arg)
{
	char * full_path = str_append(path, cmd); 
	int status =  execlp(full_path, cmd, arg, (char*)0);
	return status;
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

