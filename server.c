#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>




void handle_getCommand (int cliSock, char* file_name) {
  
  FILE* fp = fopen(file_name, "r");

  if (fp == NULL) {
    char* errmsg = "File DNE on server!\n";
    send(cliSock, errmsg, strlen(errmsg), 0);
  }

  else {
    char buffer[255];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      send(cliSock, buffer, 255, 0);
    }
    fclose(fp);
  }
}




void handle_putCommand (int cliSock, char* filename) {
  
}




void handle_client (int cliSock) {

  char fullcommand[50], command[5], file_name[20];
  recv(cliSock, fullcommand, sizeof(fullcommand), 0);
  sscanf(fullcommand, "%s %s", command, file_name);

  if (strncmp(command,"GET",3) == 0) {
    handle_getCommand(cliSock, file_name);
    exit(EXIT_SUCCESS);
  }

  else if (strncmp(command, "PUT", 3) == 0) {
    handle_putCommand(cliSock, file_name);
    exit(EXIT_SUCCESS);
  }

  else {
    fprintf(stderr, "Correct Command usage is : GET <file_name> or PUT <file_name>\n");
    exit(EXIT_FAILURE);
  }
}




int main (int argc, char* argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Correct Usage is: %s <port_no>\n", argv[0]);  
    exit(EXIT_FAILURE);
  }
  
  int servSock, cliSock;
  struct sockaddr_in servAddr, cliAddr;
  socklen_t addrsize;

  if ((servSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error Creating Socket on the server side\n");
    exit(EXIT_FAILURE);
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servAddr.sin_port = htons(atoi(argv[1]));
  servAddr.sin_family = AF_INET;

  if ((bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr))) < 0) {
    perror("Error in Binding the Server!\n");
    exit(EXIT_FAILURE);
  }

  if ((listen(servSock, 5)) < 0) {
    perror("Error in Listening for connections!\n");
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on PORT: %s...\n", argv[1]);

  addrsize = sizeof(cliAddr);
  while (1) {
    if ((cliSock = accept(servSock, (struct sockaddr*)&cliAddr, &addrsize)) < 0) {
      perror("Error in accepting connections!\n");
      exit(EXIT_FAILURE);
    }

    printf("New client connection accepted.\n");

    pid_t pid = fork();

    if (pid == 0) {
      close(servSock);
      handle_client(cliSock);
      exit(EXIT_SUCCESS);
    }
    
    else if (pid < 0) {
      perror("Error in fork()!\n");
      exit(EXIT_FAILURE);
    }

    close(cliSock);
  }

  close(servSock);
  return 0;
}
