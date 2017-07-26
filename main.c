#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
const char* USR_BIN_PATH = "/usr/bin/";
const char* BIN_PATH = "/bin/";
const char* DOWNLOAD = "dl"; //download url_file 

void download_parallel(const char* url_file);
void process(const char* cmd, const char* args);
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
	char * input="";
	while(quit != 0){
		//parent read input
		input = get_input("\ncmd>");//get input from stdin
			
		//split input by space
		quit = strcmp(input, "quit");
		if(quit != 0)
		{
			char* cmd = strtok(input, " "); //get cmd,
			char* args = strtok(0, ""); //get remaining string
			if(fork()==0)
				process(cmd, args);
			else
				wait(0);
		}
	}
	
	printf("\nGoodbye\n");
}

void download_parallel(const char* url_file)
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
		char* line = (char*)(malloc(sizeof(char)*file_size));
		int pid;
		int child_threads=0;
		while(fgets(line, file_size, file))
		{
			child_threads++;
			//each line is an arg to wget
			if(line[strlen(line)-1]=='\n')
			line[strlen(line)-1]='\0'; //remove newline
			//printf("GET %s %d", line, strlen(line));
			pid = fork(); //for a child
			if(pid < 0)
			{
				printf("ERROR Forking process");
				exit(-1);
			}
			else if (pid == 0) 
				process("wget", line);
			//do not wait for child, continue forking for ewach download
		}
		fclose (file);
		//parent process to finish early wait for other
		//wait for all child threads to finish
		while(child_threads>0)
		{
			child_threads--;
			printf("PID: %d Waiting", pid);
			wait(0);
		}
	}
}

void process(const char* cmd, const char* args)
{
	//-1 indicates failure to execute
	//try to execute from bin_path
	if(execute(BIN_PATH, cmd, args) == -1)
	//try to execute from user/bin
		if(execute(USR_BIN_PATH, cmd, args) == -1)
			if(strcmp(cmd, DOWNLOAD)==0)
				download_parallel(args);
			else
				printf("Command not found"); 
	exit(1);
}
//executes cmd at path with given argument
int execute(const char*path, const char* cmd, const char* args)
{
	char * full_path = str_append(path, cmd); 
	//get arg list by tokeninzing args
	int i = 0;
	char* arg_list[6]; //max 5 args
	char* args_buffer =  (char*)malloc(sizeof(char)*strlen(args));
	strcpy(args_buffer, args);
	arg_list[i++] = strtok(args_buffer, " "); //get cmd,
	while(i<6 && (arg_list[i++] = strtok(0, " ")) != 0);
		
	int status;
	switch(i)
	{
		case 0:
		status =  execlp(full_path, cmd, arg_list[0], (char*)0);
		break;
		case 1:
		status =  execlp(full_path, cmd, arg_list[0], arg_list[1], (char*)0);
		break;
		case 2:
		status =  execlp(full_path, cmd, arg_list[0], arg_list[1], arg_list[2], (char*)0);
		break;
		case 3:
		status =  execlp(full_path, cmd, arg_list[0], arg_list[1], arg_list[2], arg_list[3], (char*)0);
		break;
		case 4:
		status =  execlp(full_path, cmd, arg_list[0], arg_list[1], arg_list[2], arg_list[3], arg_list[4],(char*)0);
		break;
		case 5:
		status =  execlp(full_path, cmd, arg_list[0], arg_list[1], arg_list[2], arg_list[3], arg_list[4], arg_list[5], (char*)0);
		break;
	}
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

