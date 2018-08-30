#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h> 
#include <signal.h>

#define MAX 1024

static void usage(const char *proc)
{
	printf("Usage:%s port\n",proc);
}

static void show_404(int sock)
{
	char line[MAX];
	sprintf(line,"HTTP/1.0 404 Not Fount\r\n");
	send(sock,line,strlen(line),0);
	sprintf(line,"Content-Type: text/html\r\n");
	send(sock,line,strlen(line),0);

	struct stat st;
	stat("wwwroot/404.html",&st);
	int fd=open("wwwroot/404.html",O_RDONLY);
	sendfile(sock,fd,NULL,st.st_size);
	close(fd);
}
static int getLine(int sock,char line[],int num)
{
	char c = 'x';
	int i = 0;
	while(c != '\n' && i < num - 1){
		ssize_t s = recv(sock,&c,1,0);
		if(s > 0){
			if(c == '\r'){
				recv(sock,&c,1,MSG_PEEK);
				if(c == '\n'){
					recv(sock,&c,1,0);
				}else{
					c = '\n';
				}
			}


			line[i++] = c;
		}else{
			break;
		}
	}
	line[i] = 0;
	return i;
}

void clearHeader(int sock)
{
	char line[MAX];
	do{
		getLine(sock,line,MAX);
	}while(strcmp(line,"\n") != 0);
}

void echoError(int sock,int status_code)
{
	switch(status_code){
		case 404:
			show_404(sock);
			break;
		case 500:
			break;
		case 400:
			break;
		case 403:
			break;
		default:
			break;
	}
}

int echo_www(int sock,char *path,int size)
{
	clearHeader(sock);
	int fd = open(path,O_RDONLY);
	if(fd < 0)
	{
		return 404;
	}
	char *stuff = path + strlen(path)-1;
	while(*stuff != '.'&& stuff != path){
		stuff--;
	}

	char line[MAX];
	sprintf(line,"HTTP/1.0 200 OK\r\n");
	send(sock,line,strlen(line),0);
	if(strcmp(stuff,".html") == 0){
		sprintf(line,"Content-Type: text/html\r\n");
	}
	else if(strcmp(stuff,".css") == 0){
		sprintf(line,"Content-Type: text/css\r\n");
	}
	else if(strcmp(stuff,".js") == 0){
		sprintf(line,"Content-Type: application/x-javascript\r\n");
	}
	send(sock,line,strlen(line),0);

	sprintf(line,"Content-Length: %d\r\n",size);
	send(sock,line,strlen(line),0);
	sprintf(line,"\r\n");
	send(sock,line,strlen(line),0);

	sendfile(sock,fd,NULL,size);

	close(fd);
}

int exe_cgi(int sock,char *method,char *path,char* query_string)
{
	//
	char line[MAX];
	char method_env[MAX/16];
	char query_string_env[MAX];
	char content_length_env[MAX/16];

	int content_length = -1;
	if(strcasecmp(method,"GET") == 0){
		clearHeader(sock);
	}
	else{//POST
		do{
			getLine(sock,line,MAX);

			if(strncasecmp(line,"Content-Length: ",16) == 0){
				content_length = atoi(line+16);
			}
		}while(strcmp(line,"\n") != 0);
		if(content_length == -1){
			return 404;
		}
	}

	int input[2];
	int output[2];
	pipe(input);
	pipe(output);

	pid_t id = fork();
	if(id < 0){
		return 404;
	}
	else if(id == 0){//child
		close(input[1]);
		close(output[0]);

		dup2(input[0],0);
		dup2(output[1],1);

		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);
		if(strcasecmp(method,"GET") == 0){
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			putenv(query_string_env);
		}
		else{
			sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
		
			putenv(content_length_env);
		}

		//exec(path);
		execl(path,path,NULL);
		close(input[0]);
		close(output[1]);
		exit(0);
	}
	else{//parent
		close(input[0]);
		close(output[1]);

		int i = 0;
		char c;
		if(strcasecmp(method,"POST") ==0){
			for(;i < content_length;i++){
				recv(sock,&c,1,0);
				write(input[1],&c,1);
			}
		}
		sprintf(line,"HTTP/1.0 200 OK\r\n");
		send(sock,line,strlen(line),0);
		sprintf(line,"Content-Type: text/html\r\n");
		send(sock,line,strlen(line),0);
		sprintf(line,"\r\n");
		send(sock,line,strlen(line),0);

		while(read(output[0],&c,1) > 0){
			send(sock,&c,1,0);
		}

		waitpid(id,NULL,0);

		close(input[1]);
		close(output[0]);
	}
	return 200;
}

void* handlerRequest(void *arg)
{
	int sock = (int)arg;
	char line[MAX];
	char method[MAX/16];
	char url[MAX];
	
	int i,j;
	char path[MAX];
	char *query_string = NULL;
	int status_code = 200;
	int cgi = 0;

	getLine(sock,line,MAX);

	printf("line:%s\n",line);

	i=0;
	j=0;
	while(i < sizeof(method)- 1 && j < sizeof(line) && !isspace(line[j])){
		method[i] = line[j];
		i++,j++;
	}
	method[i] = 0;
	if(strcasecmp(method,"GET") == 0){
	}
	else if(strcasecmp(method,"POST")==0){
		cgi = 1;
	}
	else{
		status_code = 404;
		clearHeader(sock);
		goto end;
	}

	while(j < sizeof(line) && isspace(line[j])){
		j++;
	}






	i = 0;
	while(i < sizeof(url) -1 && j <sizeof(line) && !isspace(line[j])){
		url[i] = line[j];
		i++,j++;
	}
	url[i] = 0;



	//method,url,cgi
	if(strcasecmp(method,"GET") ==0){
		query_string = url;
		while(*query_string != '\0'){
			if(*query_string == '?'){
				cgi = 1;
				*query_string = '\0';
				query_string++;
				break;
			}
			query_string++;
		}
	}

	sprintf(path,"wwwroot%s",url);
	if(path[strlen(path)-1] == '/'){
		strcat(path,"index.html");
	}


	printf("method: %s\n",method);
	printf("path: %s\n",path);
	printf("cgi: %d\n",cgi);
	printf("query_string: %s\n",query_string);

	struct stat st;
	if(stat(path,&st) < 0){
		status_code = 404;
		clearHeader(sock);
		goto end;
	}
	else{
		if(S_ISDIR(st.st_mode)){
			strcat(path,"/index.html");
		}
		else if(st.st_mode&(S_IXUSR|S_IXGRP|S_IXOTH)){
			cgi = 1;
		}
		else{
			//do nothing;
		}
		if(cgi){
			status_code = exe_cgi(sock,method,path,query_string);
		}
		else{
			status_code =echo_www(sock,path,st.st_size);
		}
	}
end:
	if(status_code != 200){
		echoError(sock,status_code);
	}

	close(sock);
}

int startup(int port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		exit(2);
	}

	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htons(INADDR_ANY);
	local.sin_port = htons(port);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0){
		perror("bind");
		exit(3);
	}
	if(listen(sock,10) < 0){
		perror("listen");
		exit(4);
	}
	return sock;
}

int main(int argc,char *argv[])
{
	if(argc != 2){
		usage(argv[0]);
		return 1;
	}
	signal(SIGPIPE,SIG_IGN);
	int listen_sock = startup(atoi(argv[1]));

	for(;;){
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int sock = accept(listen_sock,(struct sockaddr*)&client,&len);
		if(sock < 0){
			perror("accept");
			continue;
		}

		printf("get a new connect...!!\n");
		pthread_t tid;
		pthread_create(&tid,NULL,handlerRequest,(void*)sock);
		pthread_detach(tid);
	}
	return 0;

}
