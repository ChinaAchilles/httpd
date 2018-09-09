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
	char buf[1024]={0};
	urldecode(arg,buf);
	cout<<arg<<endl;																	////////////////////////////////////////////

//	string name="张三";	
//	string tel="10086";
//	string addr="陕西西安";
//	string school="西安工业大学";

	strtok(buf,"&=");
	string name=strtok(NULL,"&=");
	strtok(NULL,"&=");
	string tel=strtok(NULL,"&=");
	strtok(NULL,"&=");
	string addr=strtok(NULL,"&=");
	strtok(NULL,"&=");
	string school=strtok(NULL,"&=");

	MYSQL *myfd=initMysql();
	connectMysql(myfd);
	insertMysql(myfd,name,tel,addr,school);
	connectClose(myfd);
	return 0;
}
