#include <iostream>
#include <stdio.h> 
#include <string.h>   
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   
#include <arpa/inet.h>     
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <netdb.h> 
#include <chrono>
     
#define TRUE   1 
#define FALSE  0 
#define PORT 10003
#define MAX 10
#define TIMEOUT 120
#define SELECT_TIMER 5
#define USERS 5

typedef struct{
	char name[20];
	char ip[20];
	int port;
} user_info; // struct for user info

typedef struct{
	char ip[20];
	int port;
	int conn_fd;
	std::chrono::high_resolution_clock::time_point last_time;
} conn_info; // struct for connection info

user_info user_table[USERS]; // list of clients

void get_user() // get the list of clients
{
	FILE *fp;
	int i;
	
	fp = fopen("Client.txt","r");
	for(i=0; i<USERS; i++)
	{
		fscanf(fp,"%s\t%s\t%d\n",user_table[i].name,user_table[i].ip,&(user_table[i].port));
	}

	fclose(fp);
}

int main()
{
	int opt = TRUE;
	get_user();

	int main_socket, addrlen, new_socket, sd;
	int max_sd, activity, clienlen;
	char message[100];

	int fd_list[MAX];
	conn_info conn_list[MAX];

	struct timeval tv;  // time out for select.
	tv.tv_sec = SELECT_TIMER;
	tv.tv_usec = 0;

	for(int i=0; i<MAX; i++)
	{
		fd_list[i] = -1;
		conn_list[i].conn_fd = -1;
	}

	struct sockaddr_in address, client_addr;

	fd_set readfds;

	if( (main_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror(" socket failed ");
		exit (EXIT_FAILURE);
	}

	if( setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	if( bind( main_socket, (struct sockaddr *)&address, sizeof(address)) < 0) // bind the socket
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	} 

	//printf("IP Address: %s\n",inet_ntoa(address.sin_addr));

	//printf("Client listening on port %d \n", PORT);

	if( listen( main_socket, 5) , 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	} // listen for incoming connections

	addrlen = sizeof(address);
	puts ( "Waiting for incoming connections" );

	fd_list[0] = 0;
	fd_list[1] = main_socket; // add stdin and main socket to fd list

	while(TRUE)
	{
		int activ = 0;
    	for(int i=0;i<MAX;i++){
			if(conn_list[i].conn_fd != -1) activ++ ;
		}
		//printf("Active : %d\n", activ);

		FD_ZERO(&readfds); // reset readfds
		for(int i=0; i<MAX; i++) // setup readfds
			if(fd_list[i]==-1)
				continue;
			else
				FD_SET(fd_list[i], &readfds);

		max_sd = -1;

		for(int i=0; i<MAX; i++) // get the maximum active fd
		{
			if(fd_list[i]==-1)
				continue;
			else
			{
				if(fd_list[i]>max_sd)
					max_sd = fd_list[i];
			}
		}
		
		activity = select( max_sd + 1, &readfds, NULL, NULL, &tv); // select call

		if((activity < 0) && (errno!=EINTR)) // select error
		{
			printf("Select Error !");
			continue;
		}
		else // check connections for timeout
		{
			for(int i=0; i<MAX; i++)
			{
				//printf("Reached Here");
				if(conn_list[i].conn_fd==-1)
					continue;
				auto curr_time = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - conn_list[i].last_time);
				if(elapsed.count() > TIMEOUT) // timeout condition breached
				{
					//printf("Reached Here");
					for(int j=0; j<MAX; j++)
					{
						if(fd_list[j]==conn_list[i].conn_fd)
							fd_list[j] = -1;
					}
					printf("Closing connection due to timeout with:");
					for(int j=0; j<USERS; j++)
					{
						if(user_table[j].port==conn_list[i].port && strcmp(user_table[j].ip, conn_list[i].ip)==0)
							printf(" %s\n", user_table[j].name);
					}

					close(conn_list[i].conn_fd); // close the timed out connection
					conn_list[i].conn_fd = -1; // reset the connection list for the timed out connection
				}
			}

			if(activity==0) // no activity detected by select
				continue;
		}

		if (FD_ISSET(main_socket, &readfds)) // indicates an incoming connection
		{
			//printf("Entered Here");
			int client_socket;
			if((client_socket = accept(main_socket, (struct sockaddr *)&client_addr, (socklen_t *)&clienlen)) < 0)
			{
				perror("accept");
				continue;
			} // accept incoming connection request
			
			bzero(message, 100);
			int n = read(client_socket, message, 100); // read the message sent
			//printf("What I Read: %s\n", message);

			for(int i=0; i<MAX; i++)
			{
				if(fd_list[i] == -1)
				{
					fd_list[i] = client_socket;
					break;
				}
			} // add the new socket to fd list

			
			int i; // add the new connection to connection list
			for(i = 0; i<MAX; i++)
			{
				if (conn_list[i].conn_fd!=-1)
					continue;

				conn_list[i].conn_fd = client_socket;
				conn_list[i].last_time = std::chrono::high_resolution_clock::now();
				char name[100]; // insert the new connection details to connection list

				for(int j = 0; j<n; j++)
				{
					if(message[j]==':')
					{
						name[j] = '\0';
						break;
					}
					else
					{
						name[j] = message[j];
					}
				} // extract the name of the sender
				//printf("Name : %s\n", name);

				for(int j=0; j<USERS; j++)
				{
					if(strcmp(name, user_table[j].name)==0)
					{
						conn_list[i].port = user_table[j].port;
						strcpy(conn_list[i].ip, user_table[j].ip);

					} // find the user who sent the request in list of users
				}

				printf("%s", message); // display the message
				break;
			} 
		}
		else if(FD_ISSET(0, &readfds)) // change in stdin
		{
			char read_line[100];
			bzero(read_line, 100);

			int n = read(0, read_line, 100); // read the message to be sent
			//printf("Line Read : %s\n", read_line);
			//printf("n = %d\n", n);

			char name[100];
			char message[100];
			bzero(name, 100);
			bzero(message, 100);

			read_line[n] = '\0'; // null terminate
			int i, ind;
			for(ind=0; ind<n; ind++)
			{
				if(read_line[ind]=='/')
				{
					name[ind] = '\0';
					break;
				}
				else
				{
					name[ind] = read_line[ind];
				}
			} // extract the name of the recipient
			//printf("Name : %s\n", name);

			int my_port = PORT;
			for(i=0; i<USERS; i++)
			{
				if(user_table[i].port==my_port)
					break;
			} // get the index of current user in user table
			strcpy(message, user_table[i].name);
			strcat(message, ":");
			strcat(message, read_line+ind+1); // construct the message to be sent
			
			// matching user index = i
			for(i=0; i<USERS; i++)
			{
				if(strcmp(name, user_table[i].name)==0)
					break;
			} // find the index in the user table where the recipient is present


			//printf("i = %d\n", i);

			int j;
			for(j = 0; j<MAX; j++) // search if a coneection with this recipient is already present
			{
				if(conn_list[j].conn_fd==-1)
					continue;
				if(strcmp(conn_list[j].ip, user_table[i].ip)==0&&conn_list[j].port==user_table[i].port)
				{
					int n = write(conn_list[j].conn_fd, message, strlen(message));
					break; 
				} // found connection with recipient
			}

			if(j==MAX) // no active connection found with recipient
			{

				//printf("Reached Here\n");
				int sockfd, portno, n;
				struct sockaddr_in clientaddr;
				sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a new socket

				clientaddr.sin_family = AF_INET;
				clientaddr.sin_addr.s_addr = inet_addr( user_table[i].ip);
				clientaddr.sin_port = htons( user_table[i].port ); // set the ip and port to recipient ip and port

				//printf("Ip and Port are: %s %d\n", user_table[i].ip, user_table[i].port );

				if(connect(sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) 
				{
					perror("ERROR connecting");
					continue;
				} // request for new connection with recipient
		
				n = write(sockfd, message, strlen(message)); // write message to recipient
				for(int a=0; a<MAX; a++)
				{
					if(fd_list[a]==-1)
					{
						fd_list[a] = sockfd;
						break;
					}
				} // insert the socket to fd list

				for(int a=0; a<MAX; a++) // insert the socket to list of active connections
				{
					if(conn_list[a].conn_fd==-1)
					{
						conn_list[a].conn_fd=sockfd;	
						strcpy(conn_list[a].ip, user_table[i].ip);
						conn_list[a].port = user_table[i].port;
						conn_list[a].last_time = std::chrono::high_resolution_clock::now();
						break;
					} // add to list of active connections
				}
			}
		}
		else // some old client is sending a message
		{
			int i;
			for(i = 0; i<MAX; i++)
			{
				if(FD_ISSET(fd_list[i], &readfds)) // message received from client corresponding to fd_list[i]
				{
					//printf("Looping Here :-(\n");
					char message[100];
					bzero(message, 100);

					int n = read(fd_list[i], message, 100); // read the message
					//printf("%d\n", n);
					if(n<=0) // client closed so free the entries corresponding to it
					{
						for(int j=0; j<MAX; j++)
						{
							if(conn_list[j].conn_fd==fd_list[i])
								conn_list[j].conn_fd = -1;
						}
						fd_list[i] = -1;
						
					}
					else
					{
						for(int j=0; j<MAX; j++)
						{
							if(conn_list[j].conn_fd==fd_list[i]) // set last time ass current time
							{
								conn_list[j].last_time = std::chrono::high_resolution_clock::now();
							}
						}
						printf("%s", message); // display the message
					}
				}
			}
		}	
	}
}