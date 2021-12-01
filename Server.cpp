//Nathan Farrar
#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <list>
#include <mysql++/mysql++.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    // Connect to database with: database, server, userID, password
    //string username, password;
    mysqlpp::Connection myDB("cse278", "localhost", "cse278", "S3rul3z");

    return 0;
}