#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1024
  
int main() { 
	int sockfd; 
	struct sockaddr_in servaddr; 

	//Creating socket file descriptor 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&servaddr, 0, sizeof(servaddr)); 

	//Server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(8181); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 

	socklen_t len = sizeof(servaddr);  

	int n;
	char *requested_file;
	printf("Enter name of the file you want to request: ");
	scanf("%[^\n]%*c", requested_file);
	
	//Send filename to server
	sendto(sockfd, requested_file, strlen(requested_file), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)); 

	char buffer[MAXLINE];

	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *) &servaddr, &len);
	buffer[n] = '\0';

	FILE *fptr;
	char word_str[50];
	char *request = malloc(MAXLINE + 1);
	int word = 1;
	
	//If file does not exist
	if (strcmp(buffer, "FILE_NOT_FOUND") == 0)
		printf("File Not Found\n");
	
	//If first line of the file is not HELLO
	else if (strcmp(buffer, "WRONG_FILE_FORMAT") == 0)
		printf("Wrong File Format\n");	

	else {
		//Writing data received from server in recv.txt
		fptr = fopen("recv.txt", "w");
		fprintf(fptr, "%s", buffer);

		do {
			strcpy(request, "WORD");
			sprintf(word_str, "%d", word);
			strcat(request, word_str);
			
			//Send request for next word to server
			sendto(sockfd, (char *)request, strlen(request), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

			memset(buffer, 0, sizeof(buffer));
			
			//Receive next word from server
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *) &servaddr, &len);
			buffer[n] = '\0';
			
			//Print the received word to recv.txt
			fprintf(fptr, "%s", buffer);
			word++;
		} while(strcmp(buffer, "END") != 0 && strcmp(buffer, "END\n") != 0);

		printf("File received from server and written to recv.txt\n");
		free(request);
		fclose(fptr);
	}

	close(sockfd); 
	return 0; 
} 
    
