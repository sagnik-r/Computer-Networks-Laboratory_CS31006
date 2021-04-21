#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SA struct sockaddr

#define MAX 30  //Client side buffer size 
#define B 20    //Block size

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
       
       //Create the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		printf("[-]socket creation failed...\n");
	else
		printf("[+]socket successfully created...\n");
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
       
       //Connect with server
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("[-]connection with server failed...\n");
		exit(0);
	}
	else 
		printf("[+]connection established...\n");
	
	// Sending the file name to the server
	char FILENAME[90];
	printf("Enter file name: ");
	scanf("%s",FILENAME); //Read filename from user
	send(sockfd, FILENAME, strlen(FILENAME)+1, 0);
	
	//Receive the first character from the server
	char a;
	recv(sockfd, &a, 1, 0);

	// If the character is 'E', then file is not found and close the socket
	if(a=='E'){
		printf("File not found\n");
		close(sockfd);
	}
	
	//Otherwise file is found and write the file contents to recv.txt
	else if(a=='L'){
		int size;
		char buff[MAX];
		int i, fd;
		
		recv(sockfd, &size, 2, MSG_WAITALL); //Receive the file size from server
		size = ntohs(size);
		fd = open("recv.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
		
		for(i=0; i<(size/B); i++){
			recv(sockfd, buff, B, MSG_WAITALL); //Receive all but the last block
			write(fd, buff, B);
		}
		recv(sockfd, buff, (size%B), MSG_WAITALL); //Receive the last block
		write(fd, buff, (size%B));

		close(fd); //Close the file recv.txt
		
		//Print the number of blocks received and the last block size
		printf("\nThe file transfer is successful. Total number of blocks received = %d, Last block size = %d\n", i, (size%B));
	}
  
	// close the socket 
	close(sockfd); 
}
