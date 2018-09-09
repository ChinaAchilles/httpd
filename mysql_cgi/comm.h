#pragma once
#include <mysql.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
using namespace std;
MYSQL *initMysql();
int connectMysql(MYSQL *myfd);
int insertMysql(MYSQL *myfd,const string &name,const string &tel,const string &addr,const string &school);
void selectMysql(MYSQL *myfd);
void deleteMysql();
//void modifyMysql();
void connectClose(MYSQL *myfd);

char* urldecode(char* encd,char* decd); 







































































