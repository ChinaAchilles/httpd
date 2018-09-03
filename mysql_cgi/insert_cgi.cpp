#include "comm.h"
using namespace std;

int main()

{
	char arg[1024];
	if(getenv("METHOD")){
		if(strcmp("GET",getenv("METHOD"))==0){
			strcpy(arg,getenv("QUERY_STRING"));
		}
		else{
			int content_length=atoi(getenv("CONTENT_LENGTH"));
			int i=0;
			char c;
			for(;i<content_length;i++){
				read(0,&c,1);
				arg[i]=c;
			}
			arg[i]='\0';
		}
	}
	cout<<arg<<endl;

//	string name="张三";	
//	string school="北大";
//	string hobby="敲代码";
//	name=zhangsan&school=xigongyuan&hobby=eat
	
	strtok(arg,"&=");
	string name=strtok(NULL,"&=");
	strtok(NULL,"&=");
	string school=strtok(NULL,"&=");
	strtok(NULL,"&=");
	string hobby=strtok(NULL,"&=");

	MYSQL *myfd=initMysql();
	connectMysql(myfd);
	insertMysql(myfd,name,school,hobby);
	connectClose(myfd);
	return 0;
}
