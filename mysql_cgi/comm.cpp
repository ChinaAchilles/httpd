#include "comm.h"
using namespace std;
MYSQL* initMysql()
{
		MYSQL* myfd=mysql_init(NULL);
		return myfd;
}
int  connectMysql(MYSQL *myfd)
{
		int ret=0;
		if(mysql_real_connect(myfd,"127.0.0.1","root","123321","contact",3306,NULL,0)){
				cout<<"connect success!"<<endl;
		}
		else{
				cerr<<"connect failed"<<endl;
				ret=-1;
		}
		return ret;
}

int  insertMysql(MYSQL *myfd,const string &name,const string &tel,const string &addr,const string &school)
{
		//		printf("%s\n",name.c_str());
		//		printf("%s\n",tel.c_str());
		//		printf("%s\n",addr.c_str());
		//		printf("%s\n",school.c_str());
		string sql="INSERT INTO peopInfo (name,tel,addr,school) VALUES (\"";
		sql+=name;
		sql+="\",\"";
		sql+=tel;
		sql+="\",\"";
		sql+=addr;
		sql+="\",\"";
		sql+=school;
		sql+="\")";
		cout<<sql<<endl;
		return mysql_query(myfd,sql.c_str());
}
void connectClose(MYSQL *myfd)
{
		mysql_close(myfd);
}

void selectMysql(MYSQL *myfd)
{
		string sql="SELECT * FROM peopInfo";
		mysql_query(myfd,sql.c_str());

		MYSQL_RES *result=mysql_store_result(myfd);
		int rows=mysql_num_rows(result);
		int cols=mysql_num_fields(result);
		int i=0;
		cout<<"<meta charset=\"utf-8\">"<<endl;
		cout<<"<table border=\"4\">"<<endl;
		for(;i<rows;i++){
				cout<<"<tr>"<<endl;
				int j=0;
				MYSQL_ROW row=mysql_fetch_row(result);
				for(;j<cols;j++){
						cout<<"<td>"<<row[j]<<"</td>";
				}
				cout<<"</tr>"<<endl;
		}
		cout<<"</table>"<<endl;
}
char* urldecode(char* encd,char* decd) 
{ 
		int j,i; 
		char *cd =(char*) encd; 
		char p[2]; 
		int num;
		j=0;

		for( i = 0; i < strlen(cd); i++  )
		{
				memset( p, 0x00, 2  );
				if( cd[i] != '%'  )
				{
						decd[j++] = cd[i];
						continue;
				}

				p[0] = cd[++i];
				p[1] = cd[++i];

				p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0 );
				p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0 );
				decd[j++] = (unsigned char)(p[0] * 16 + p[1]);

		}
		decd[j] = '\0';

		return decd;
}
