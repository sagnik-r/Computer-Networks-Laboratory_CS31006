#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 90	//Size of server buffer
#define PORT 8080
#define SA struct sockaddr

void transfer(int sockfd) //Function to facilitate the transfer
{ 
    char FILENAME[90]; 
    int n;  
    bzero(FILENAME, 90); 
    recv(sockfd, FILENAME, 90, 0); //Get the filename
    
    int fd = open(FILENAME, O_RDONLY);//Open the File
    if(fd<0)
    {
    	close(sockfd);//File not present
    }

    while(1)
    {
    	unsigned char buff[MAX] = {0};
    	int nread = read(fd,buff,MAX);//Read from the file

    	if(nread>0)
    	{
    		printf("%d bytes read\n", nread);
    		send(sockfd, buff, nread, 0);//Send the read bytes to client
    	}
    	else
    	{
    		send(sockfd, "\0", 1, 0);//Send a null string
    		break;//Read complete
    	}
    }
    //return 1;
} 

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
	else
		while(1)
		{
			printf("[+]server listening...\n");
			connfd = accept(sockfd, (SA*)&cli, &len);//Accept the client
			if (connfd<0){
				printf("[-]server accept failed...\n");
				exit(0);
			}
			else
				printf("[+]server accepted the client...\n");

			transfer(connfd); 
			
			close(connfd);	//Close the client connection but server still listening for new clients
		}

	

	close(sockfd);//Close the socket
}