#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main (int argc, char* argv[]) {

  if (argc != 3) {
    fprintf(stderr, " Correct usage is: %s <server_Address> <PORT_no>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int cliSock;
  struct sockaddr_in servAddr, cliAddr;

  if ((cliSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error in socket connection!\n");
    exit(EXIT_FAILURE);
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_addr.s_addr = inet_addr(argv[1]);
  servAddr.sin_port = htons(atoi(argv[2]));
  servAddr.sin_family = AF_INET;

  if ((connect(cliSock, (struct sockaddr*)&servAddr, sizeof(servAddr))) < 0) {
    perror("Error in Connecting to Server!\n");
    exit(EXIT_FAILURE);
  }
  
  printf("Client is connected to to Server(%s) at PORT(%s)\n", argv[1], argv[2]);

  while (1) {
    char fullcommand[20], buffer[255];
    printf("Enter command OR (type 'exit' to quit) : \n");
    fgets(fullcommand, sizeof(fullcommand), stdin);
    
    if (strncmp(fullcommand, "exit", 4) == 0) {
      printf("Exiting the Client...\n");
      exit(EXIT_SUCCESS);  
    }
    
    send(cliSock, fullcommand, sizeof(fullcommand), 0);
    
    FILE* fw = fopen("demoTo.txt", "w");
    int bytes_received;

    while ((bytes_received = recv(cliSock, buffer, sizeof(buffer), 0)) > 0) {
      fwrite(buffer, 1, sizeof(buffer), fw);
    }
    fclose(fw);
  }

  close(cliSock);
  return 0;
}

