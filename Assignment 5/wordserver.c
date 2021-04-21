#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 1024

int main(){
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	
    	size_t llen = 0;
    	
	//Create socket file descriptor
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	memset(&servaddr,0,sizeof(servaddr));
	memset(&cliaddr,0,sizeof(cliaddr));
	
	socklen_t len = sizeof(cliaddr);
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	servaddr.sin_port = htons(8181);

	//Bind the socket with the server address
	bind(sockfd, (const struct  sockaddr *) &servaddr, sizeof(servaddr));
	printf("Server running...\n");
	
	int n;
	char buffer[MAXLINE];
	
	//Receive filename from client
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len);
	buffer[n] = '\0';
	
	char *word = NULL;
	FILE *fptr;
	int i = 1;
	
	//If file exists
	if (access(buffer, R_OK) != -1){
		fptr = fopen(buffer, "r");
		while(1){
			getline(&word, &llen, fptr);
			
			//If first word is not HELLO, send error message and exit
			if(i == 1 && strcmp(word, "HELLO") != 0 && strcmp(word, "HELLO\n") != 0){
				printf("Wrong file format\n");
				char *to_buffer = "WRONG_FILE_FORMAT";
				sendto(sockfd, to_buffer, strlen(to_buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
				fclose(fptr);
				break;
			}
			
			//Send the current word to client
			sendto(sockfd, word, strlen(word), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			
			//If current word is END, then exit
			if (strcmp(word, "END") == 0 || strcmp(word, "END\n") == 0){
				fclose(fptr);
				printf("File sent successfully\n");
				break;
			}
			
			memset(buffer, 0, sizeof(buffer));
			
			//Receive request for next word from client
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len);
			
			i++;
		}
	}
	
	//If file does not exist
	else{
		//Send error message to client
		char *to_buffer = "FILE_NOT_FOUND";
		sendto(sockfd, to_buffer, strlen(to_buffer), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
	}

	close(sockfd);
	return 0;
}
	
