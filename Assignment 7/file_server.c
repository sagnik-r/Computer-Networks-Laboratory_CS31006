#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define SA struct sockaddr

#define MAX 30	//Size of server buffer
#define B 20	//Block size

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	
	/*Code to create the socket*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		printf("[-]socket creation failed...\n");
	else
		printf("[+]socket successfully created...\n");
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	//Binding the socket
	if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
		printf("[-]socket bind failed...\n");
	else
		printf("[+]socket successfully bound...\n");

	//Listening for client connection
	if((listen(sockfd, 5))!=0)
	{
		printf("[-]listen failed...\n");
		exit(0);
	}
	else{
		printf("[+]server listening...\n");
		connfd = accept(sockfd, (SA*)&cli, &len);//Accept the client
		if (connfd<0){
			printf("[-]server accept failed...\n");
			exit(0);
		}
		else
			printf("[+]server accepted the client...\n");
		
		short int FSIZE;
		char buff[MAX];
		char file[MAX];
		int i;
		char FILENAME[90]; 
    		
    		bzero(FILENAME, 90); 
    		recv(connfd, FILENAME, 90, 0); //Get the filename
    		int fd = open(FILENAME, O_RDONLY);//Open the File
		
		//If file is not present, send error message "E" and close the connection
		if(fd<0)
		{
			printf("File not found\n");
			send(connfd, "E", 1, 0);
			close(connfd);
		}
		//Else send error message "L"
		else{
			send(connfd, "L", 1, 0);
			FSIZE = lseek(fd, 0, SEEK_END); //Get the file size
			FSIZE = htons(FSIZE);
			send(connfd, &FSIZE, 2, 0); //Send the file size to the client
			lseek(fd, 0, SEEK_SET); //Bring the file pointer back to the beginning of the file

			// Find the number of full-sized blocks to be sent and the size of the last block
			int blocks = ntohs(FSIZE)/B;
			int last = ntohs(FSIZE)%B;

			/* Sending the contents of the file */
			for(i=0; i<blocks; i++){
				read(fd, buff, B);
				send(connfd, buff, B, 0); //Send all blocks except the last block
			}
			read(fd, buff, last);
			send(connfd, buff, last, 0); //Send the last block

			printf("\nFile sent\n");
			close(fd); //Close the file
			
			close(connfd); //Close the connection	
		}
	}
	
	close(sockfd);//Close the socket
}
