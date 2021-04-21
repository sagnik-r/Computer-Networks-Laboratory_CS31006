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

#define MAX 80  //Client side buffer size (May be different from server too)
#define PORT 8080
#define SA struct sockaddr

void transfer(int sockfd) //Function facilitating file transfer
{ 
    char buff[MAX]; 
    int n; 
    
    bzero(buff, sizeof(buff)); 
    printf("Enter the filename : "); //Getting the filename
    n = 0; 
    while ((buff[n++] = getchar()) != '\n') 
        ; 
    buff[n-1] = '\0';
    send(sockfd, buff, sizeof(buff), 0); //Sending the filename to the server
    
    char recv_buff[MAX] = {0};
    int fd = open("client_file.txt", O_WRONLY | O_CREAT, 0666);//Creat ehte client side file
    int total_bytes = 0;//Running byte count
    int total_words = 0;//Running Word Count
    char last_char = ' ', curr_char;
    int bytes_received; 

    while((bytes_received = recv(sockfd, recv_buff, MAX, 0))>0)
    {
        total_bytes += bytes_received;//Increment bytes received
        if(!(bytes_received==1 && recv_buff[0]=='\0'))
        {
    	   write(fd, recv_buff, bytes_received);
        }

    	for(int i=0; i<bytes_received; i++)
    	{
    		if (recv_buff[i]!=',' && recv_buff[i]!='.' && recv_buff[i]!=' ' && recv_buff[i]!='\0' && recv_buff[i] != '\n' && recv_buff[i] != ':' && recv_buff[i] != ';' && (last_char=='.' || last_char==';' || last_char==':' || last_char==' ' || last_char==',' || last_char=='\n'))
    			total_words++;
    		last_char = recv_buff[i];
    	}//Running word count
    }
    close(fd);
    if(total_bytes<0)
    	printf("Error");
    else if(total_bytes==0)
    	printf("ERR 01: File Not Found\n");//File not found
    else//Success message
    {
    	printf("The file transfer is successful. Size of file = %d bytes, no. of words = %d\n", total_bytes-1, total_words);
    }


    
} 

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

	// function for transfer
    transfer(sockfd); 
  
    // close the socket 
    close(sockfd); 
	
}
