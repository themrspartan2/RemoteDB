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

#define MYPORT 8080
#define BUFFER_SIZE 1024
#define IP "127.0.0.1"

using namespace std;

int id;
struct sockaddr_in serverAddr;
socklen_t length;
list<int> activeConn;

mysqlpp::StoreQueryResult sendQuery(string data)
{
    mysqlpp::Connection myDB("cse278", "localhost", "cse278", "S3rul3z");
    mysqlpp::Query query = myDB.query();
    query << data;
    query.parse();
    return query.store();
}

void getConnection()
{
    while (1)
    {
        int newConn = accept(id, (struct sockaddr *)&serverAddr, &length);
        activeConn.push_back(newConn);
        cout << "New Connection: Client #";
        printf("%d\n", newConn);
    }
}

void getData()
{
    //server checks for message every 2 seconds
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    while (1)
    {
        //For every connection that is stored, iterate over all of them
        //Check each connection for a message
        list<int>::iterator it;
        for (it = activeConn.begin(); it != activeConn.end(); ++it)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = 0;
            int retval = 0;
            FD_SET(*it, &rfds);

            if (maxfd < *it)
            {
                maxfd = *it;
            }
            retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1)
            {
                printf("Socket error\n");
            }
            else if (retval == 0)
            {
                //No messages were recieved from this connection
            }
            else
            {
                char buf[BUFFER_SIZE];
                memset(buf, 0, sizeof(buf));
                int len = recv(*it, buf, sizeof(buf), 0);

                //This prints the client's message on the server terminal
                printf("%s", buf);

                if (buf[0] == 'e' &&
                    buf[1] == 'x' &&
                    buf[2] == 'i' &&
                    buf[3] == 't')
                {
                    cout << "Client #" << *it << " disconnected.\n";
                    activeConn.erase(it--);
                }
                else
                {
                    try
                    {
                        //This sends the client's message to the database as a query
                        mysqlpp::StoreQueryResult result = sendQuery(buf);
                    }
                    catch (const std::exception &e)
                    {
                        //Catch an error
                        cerr << e.what() << '\n';
                        string temp = e.what();
                        temp = temp + '\n';
                        char buf[BUFFER_SIZE];
                        strcpy(buf, temp.c_str());

                        //Send the error back to the user
                        send(*it, buf, sizeof(buf), 0);
                    }
                }
            }
        }
        sleep(1);
    }
}

void sendToAll()
{
    while (1)
    {
        char buf[BUFFER_SIZE];
        fgets(buf, sizeof(buf), stdin);
        list<int>::iterator it;
        for (it = activeConn.begin(); it != activeConn.end(); ++it)
        {
            send(*it, buf, sizeof(buf), 0);
        }
    }
}

int main()
{
    //Create the socket for the connection number - ipv4 internet socket, TCP
    id = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MYPORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP);

    //bind the socket with error checking
    if (bind(id, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(id, 20) == -1)
    {
        perror("listen");
        exit(1);
    }
    length = sizeof(serverAddr);

    //thread : while ==>> accpet
    thread t(getConnection);
    t.detach();

    //thread : input ==>> send
    thread t1(sendToAll);
    t1.detach();

    //thread : recv ==>> show
    thread t2(getData);
    t2.detach();

    while (1)
    {
    }
    return 0;
}