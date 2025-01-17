#include "include/apue.h"
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
static void	sig_int(int);		/* our signal-catching function */



void tokenize(char* buf, char** cmd, char** target, char* redirection);
int main(void)
{
	char*	buf= malloc(sizeof(char)*MAXLINE);
	char*	cmd= malloc(sizeof(char)*MAXLINE);
	char*	target= malloc(sizeof(char)*MAXLINE);
	char redirection;
	pid_t	pid;
	int		status;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error");

	printf("%% ");	/* print prompt (printf requires %% to print %) */
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /* replace newline with null */
		tokenize(buf, &cmd, &target, &redirection);
		if ((pid = fork()) < 0) {
			err_sys("fork error");
		} else if (pid == 0) {		// child 1	
			int fd;
			if (redirection == '>'){
				//close(1);
				fd = open(target, O_CREAT|O_TRUNC |O_WRONLY, S_IWUSR | S_IRUSR);
				dup2(fd, 1);
			}
			else if (redirection == '|'){
				int pipe_fds[2];	
				pipe(pipe_fds);
				if ((pid =fork())<0){
					err_sys("error fork 2");
				}
				if (pid == 0){ //child 2
					dup2(pipe_fds[0],0);
					close(pipe_fds[1]);
					execlp(target, target, (char*)0);
					err_ret("couldn't execute (target of pipe): %s", target);
					exit(127);
				}
				//parent 2 (child 1)
				dup2(pipe_fds[1],1);
				close(pipe_fds[0]);
				
				
			}
			execlp(cmd, cmd, (char *)0);
			err_ret("couldn't execute: %s", cmd);
			exit(127);
		}

		/* parent 1 */
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error");
		printf("%% ");
	}
	exit(0);
}

void
sig_int(int signo)
{
	printf("interrupt\n%% ");
}

int remove_char(char** str, int index){
	if (index <0 || index > strlen(*str) - 1)
		return -1;
	char* copy = malloc(sizeof(char)*(strlen(*str)+1));
	char* subst1 = malloc(sizeof(char)*strlen(copy));
	char* subst2 = malloc(sizeof(char)*strlen(copy));
	
	strcpy(copy, *str);
	strncpy(subst1, copy, index);
	subst1[index]='\0';
	strncpy(subst2, copy + index +1, strlen(copy)-index-1);
	subst2[strlen(copy)-index +1]='\0';
	strcat(subst1,subst2);
	subst1[strlen(copy)-1]='\0';
	//free(*str);
	//*str=subst1;
	strcpy(*str,subst1);	
	strcpy(copy,"");	
	strcpy(subst1,"");	
	strcpy(subst2,"");
	free(copy);
	free(subst1);
	free(subst2);

	return 0;
}
void tokenize(char* buf, char** cmd, char** target, char* redirection){
	
	for (int i=0; buf[i]!='\0';i++){
		if (buf[i] == ' '){
			remove_char(&buf, i);
			i--;			
		}
	}
	*redirection = '\0';
	char* redirections=">|";
	for (int i =0; i<2; i++){
		char redir=redirections[i];
		if (strchr(buf, redir)){
			*redirection = redir;
			*cmd = strtok(buf, redirections);
			*target = strtok(NULL,redirections);
			return;
		}
		
	}
	*cmd=buf;
	return;
	
	//*cmd = strtok(buf, " ");
	//*redirection = strtok(NULL, "|>");



				//*target = strtok(NULL, " ")
		//*target = strtok(NULL, " ")
	//*target = strtok(NULL, " ")
//*target = strtok(NULL, " ")
}

