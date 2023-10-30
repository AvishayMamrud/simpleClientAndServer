#include <string.h>
#include <stdio.h>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <pthread.h>

using namespace std;

// typedef struct conn
// {
//   int connection;
//   int sockfd;
// } conn;


void* clientReception(void*);

void* readAndRespond(void* con);

int main() {
  pthread_t t;
  int res = pthread_create(&t, NULL, &clientReception, NULL);
  cout << "main res - " << res << endl;
  pthread_exit(NULL);
}

void* clientReception(void* nul){

 
  // Create a socket (IPv4, TCP)
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  cout << "sockfd - " << serv_sock << endl;
  if (serv_sock == -1) {
    cout << "Failed to create socket. errno: " << errno << endl;
    exit(EXIT_FAILURE);
  }

  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
                                    // network byte order

  // Listen to port 9999 on any address
  if (bind(serv_sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    cout << "Failed to bind to port 9999. errno: " << errno << endl;
    exit(EXIT_FAILURE);
  }

  // Start listening. Hold at most 10 connections in the queue
  if (listen(serv_sock, 10) < 0) {
    cout << "Failed to listen on socket. errno: " << errno << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Server is running..." << endl;

  // Grab a connection from the queue
  auto addrlen = sizeof(sockaddr);
  while(true){
    cout << sockaddr.sin_family << " ?= "
    << AF_INET << "\n"
    << sockaddr.sin_addr.s_addr << " ?= "
    << htonl(INADDR_ANY) << "\n"
    << sockaddr.sin_port << " ?= "
    << htons(9999) << endl;

    int connection = accept(serv_sock, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      cout << "Failed to grab connection. errno: " << errno << endl;
      exit(EXIT_FAILURE);
    }
    
    cout << "A client has connected...\n" << endl;

    int* con = (int*)malloc(sizeof(int));
    *con = connection;

    cout << "connection - " << connection << endl;

    pthread_t t;
    int res = pthread_create(&t, NULL, &readAndRespond, (void*)con);
    cout << "clientReception res - " << res << endl;
  }
  close(serv_sock);
  pthread_exit(NULL);
}

void* readAndRespond(void* con){
  int connection = *(int*)con;
  free(con);
  // Read from the connection
  int maxlen = 100;
  char buffer[maxlen];
  while(true){
    int bytesRead = read(connection, buffer, maxlen - 1);
    if(bytesRead == -1){
        cout << "error reading" << endl;
        break;
    }

    buffer[bytesRead] = '\0';
    while (bytesRead > 0 && (buffer[bytesRead-1] == '\n' || buffer[bytesRead-1] == '\r')) buffer[--bytesRead] = '\0';
    
    cout << "The message was: " << buffer << endl;

    // Send a message to the connection
    string response = "what the hell is - " + (string)buffer + '\n';
    // cout << "response.size() - " + to_string(response.size()) << endl;
    send(connection, response.c_str(), response.size(), 0);

    // cout << "str compare - " << (const char*)"quit" << " ?= " << (const char*)buffer << " = " << strcmp((const char*)"quit", (const char*)buffer) << endl;
    // cout << "str length - " << strlen((const char*)"quit") << " ?= " << strlen((const char*)buffer) << endl;
    // cout << (int)(buffer[0]) << (int)(buffer[1]) << (int)(buffer[2]) << (int)(buffer[3]) << (int)(buffer[4]) << " " << (int)(buffer[5]) << " " << (int)(buffer[6]) << endl;
    if(((string)"quit").compare((string)buffer) == 0){
      cout << "quitting..." << endl;
      break;
    }
  }

  // Close the connections
  close(connection);
  pthread_exit(NULL);
}