
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "IRCServer.h"
//#include "LinkedList.h"

//Data structs for 3 LinkedLists
struct ListNode {
	char * value;
	struct ListNode * next;
};

typedef struct ListNode ListNode;

struct LinkedList {
	ListNode * head;
};
typedef struct LinkedList LinkedList;

struct Rooms {
	char * roomName;
	int countMessage;
	int getMessage;
	struct Rooms * n;
	char * name[100];
	char * msg[100];
	//LinkedList * name;
	//LinkedList * msg;
};

struct RoomList {
	Rooms * heading;
};

typedef struct RoomList RoomList;


int QueueLength = 5;
int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);
  
	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			     (char *) &optval, sizeof( int ) );
	
	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}
	
	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
	while ( 1 ) {
		
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);
		
		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
	
}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//


//Declaring LinkedList functions  
void llist_init(LinkedList * list) 
{
	list->head = NULL;
}
void room_init(RoomList * room)
{
	room->heading = NULL;
}
void llist_add(LinkedList * list, char * value) {
	//Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value= value;

	//Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}
int llist_remove_first(LinkedList * list, char * value) {
	ListNode * s;
	s = list->head;
	if(s != NULL) {
		value = s->value;
		list->head = s->next;
		free(s);
		return 1;
	}
	return 0;
}
void llist_clear(LinkedList * list)
{
	list->head = NULL;
	free(list->head);
}
int llist_exists(LinkedList * list, char * value) {
	ListNode * e;
	e = list->head;
	while(e != NULL) {
		if(e->value == value) {
			return 1;
		}
		e = e->next;
	}
	return 0;
}
void llist_print(LinkedList * list) {
	ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return;
	}
}
bool checkInRoom(char * x[100], char * y) {
	int i = 0;
	while(i < 100) {
		if(strcmp(strdup(x[i]),strdup(y)) == 0) {
			return true;
		}
		i++;
	}
	return false;
}
void sorter(char * array[], int n) {
	int i,j;
	char * temp = (char *)malloc(sizeof(char)*50);
	for(i = 0; i< (n -1); i++) {
		for(j = 0; j < n-i-1; j++) {
			if(strcmp(array[j],array[j+1]) > 0) {
				temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp;
			}
		}
	}
}

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	char * pointer;
	char * pointer2;
	char * pointer3;
	char * pointer4;
	int i = 0;
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) {
				
		if (newChar == '\n' && prevChar == '\r') {
			break;
		}
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	
	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
        commandLine[ commandLineLength ] = 0;
	pointer = strtok(commandLine, " ");
	while(pointer != NULL) {
		if(i == 0) {
			pointer = strtok(NULL, " ");
			i++;
		}
		
		if(i == 1) {
			pointer2 = pointer;
			pointer = strtok(NULL, " ");
			i++;
		}

		if(i == 2) {
			pointer3 = pointer;
			pointer = strtok(NULL, "\n");
			i++;
		}

		if(i == 3) {
			pointer4 = pointer;
			i++;
			break;
			//pointer = strtok(NULL, "");
		}
		//pointer = strtok(NULL, " ");
	}
	if(i != 4) {
		const char * m = "DENIED\r\n";
		write(fd,m,strlen(m));
	}
	char * command = commandLine;
	char * user = pointer2;
	char * password = pointer3;
	char * args = pointer4;
	printf("command: %s\n", commandLine);
	printf("user: %s\n", pointer2);
	printf("password: %s\n", pointer3);
	printf("args: %s\n", pointer4);

	printf("RECEIVED: %s\n", commandLine);

	//printf("The commandLine has the following format:\n");
	//printf("COMMAND <user> <password> <arguments>. See below.\n");
	//printf("You need to separate the commandLine into those components\n");
	//printf("For now, command, user, and password are hardwired.\n");

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRooms(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer
	//const char * msg =  "OK\n";
	//write(fd, msg, strlen(msg));

	close(fd);	
}
//Declaring Roomlist as global variable
RoomList * rlist = (RoomList *) malloc(sizeof(RoomList));
char *  username[1000]; //stores char * array of usernames
char *  pass[1000]; //stores char * array of password
char *  room[1000]; //stores char * array of rooms
int messageCounter;
#define MAXUSER 100
#define MAXPASSWORD 100
void
IRCServer::initialize()
{
	//Initialize user in room
	room_init(rlist);
	char c = 0;
	int i = 0; //username and password counter
	//int order = 0;
	//char * pointer;
	//pointer = (char *)malloc(50);
	//Initialize message list
	//Open password file
	FILE *fp;
	fp = fopen("password.txt","a+");
	//while((c = fgetc(fp)) != EOF) {
	//	ungetc(fgetc(fp),fp);
			
		username[i] = (char *)malloc(sizeof(char)* MAXUSER);
		pass[i] = (char *)malloc(sizeof(char) * MAXPASSWORD);
	while(fscanf(fp, "%s %s", username[i], pass[i]) == 2) {
		//fscanf(fp, "%s\n", pass[i]);
		i++;
	}
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
	// Here check the password
	int i = 0;
	FILE *fp;
	fp = fopen("password.txt","a");
	//char * u = (char *)malloc(sizeof(char));
	//strcpy(u,strdup(user));
	//char * p = (char *)malloc(sizeof(char));
	//char * line = (char *)malloc(sizeof(char));
	//strcpy(p, strdup(password));
	//while not end of file, scans file and stores into username and password
	while(username[i] != NULL && pass[i] != NULL) {
		//username[i] = (char *)malloc(sizeof(char));
		//pass[i] = (char *)malloc(sizeof(char));
		//fscanf(fp, "%s %s", username[i], pass[i]);
		//fscanf(fp, "%s", pass[i]);
		if(strcmp(username[i], user) == 0) {
			if(strcmp(pass[i],password) == 0) {
				return true;
			}
		}
		i++;
	}
	//if strcmp fails for all usernames and passwords
	//const char * error = "ERROR(Wrong Password)\r\n";
	//write(fd, error, strlen(error));
	return false;
}

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
	// Here add a new user. For now always return OK.
	int i = 0;
	FILE *fp;
	char * u = (char *)malloc(sizeof(char));
	strcpy(u,strdup(user));
	char * p = (char *)malloc(sizeof(char));
	strcpy(p,strdup( password));
	fp = fopen("password.txt", "a+");
	
	//if(fgetc(fp) == EOF) {
		//Print to file in that order at EOF
	//	fprintf(fp, "%s %s\n", user, password);
	//	fflush(fp);
	//}
	//else {
		//Gets to end of file and puts username and password into char * arrays
		while(fgetc(fp) != EOF) {
			username[i] = (char *)malloc(sizeof(char)* 1000);
			pass[i] = (char *)malloc(sizeof(char));
			fscanf(fp, "%s %s", username[i], pass[i]);
			fflush(fp);
			//fscanf(fp, "%s\n", pass[i]);
			if(!(strcmp(username[i],u))) {
				const char * error = "ERROR, user already exists\r\n";
				write(fd,error,strlen(error));
				return;
			}
			i++;
		}
		//Prints to file in that order at EOF
		fprintf(fp," %s %s\n", user, password);
		fflush(fp);
		username[i] = (char *)malloc(sizeof(char));
		pass[i] = (char *)malloc(sizeof(char));
		strcpy(username[i],strdup(u));
		strcpy(pass[i], strdup(p));
	//}
	char * pointer = username[i];
	//Prints if no segv
	const char * msg =  "OK\r\n";
	write(fd,msg, strlen(msg));
//	write(fd,strdup(username[i]),strlen(username[i]));
	return;		
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{
	Rooms * e = (Rooms *) malloc(sizeof(Rooms));
	e = rlist->heading;
	int x = 0;
	char * u = (char *)malloc(sizeof(char));
	strcpy(u, strdup(user));
	char * rname = (char *)malloc(sizeof(char));
	strcpy(rname, strdup(args));
	if(checkPassword(fd, user, password) == true) {
		while(e != NULL) {
			if((strcmp(rname, e->roomName) == 0)) {
				if(e->name[0] == NULL) {
					const char * m = "OK\r\n";
					write(fd,m,strlen(m));
					e->name[0] = (char *)malloc(sizeof(char));
					strcpy(e->name[0],strdup(u));
					return;
				}
				while(e->name[x] != NULL) {
					if((strcmp(e->name[x],u) == 0)) {
						const char * ok = "OK\r\n";
						write(fd,ok,strlen(ok));
						return;
					}
					x++;
				}
				e->name[x] = (char *)malloc(sizeof(char));
				strcpy(e->name[x], strdup(u));
				const char * ms = "OK\r\n";
				write(fd, ms, strlen(ms));
				return;
			}
			e = e->n;
		}
		const char * error = "ERROR (No room)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	const char * m = "OK\r\n";
	write(fd,m,strlen(m));
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{
	Rooms * e = (Rooms *) malloc(sizeof(Rooms));
	e = rlist->heading;
	int i = 0;
	int x = 0;
	int j = 0;
	char * u = (char *)malloc(sizeof(char));
	strcpy(u, strdup(user));
	char * rname = (char *)malloc(sizeof(char));
	strcpy(rname, strdup(args));
	if(checkPassword(fd, user, password) == true) {	
		while(e != NULL) {
			if((strcmp(rname, e->roomName)) == 0) {
				while(e->name[x] != NULL) {
					//found user who wants to leave
					if((strcmp(e->name[x],u)) == 0) {
						j = 1;
						e->name[x] = NULL;
						while(i < 100) {
							if(e->name[i] == NULL && e->name[i+1] != NULL) {
								e->name[i] = (char *)malloc(sizeof(char));
								e->name[i] = e->name[i+1];
								e->name[i+1] = NULL;
							}
							i++;
						}
					}
					x++;
				}
			}
			e = e->n;
		}
	}
	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	if(j == 0) {
		const char * a ="ERROR (No user in room)\r\n";
		write(fd,a,strlen(a));
		return;
	}
	const char * m = "OK\r\n";
	write(fd,m,strlen(m));
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
	Rooms * e = (Rooms *) malloc(sizeof(Rooms));
	char * total = (char *)malloc(sizeof(char)* 100);
	strcpy(total, args);
	int p;
	char * rname = (char *)malloc(sizeof(char)* 100);
	char * linenumber = (char *)malloc(sizeof(char)* 100);
	rname = strtok(total, " ");
	total = strtok(NULL, "");
	if(checkPassword(fd, user, password) == true) {	
		int i = 0;
		e = rlist->heading;
		while(e != NULL) {
			if((strcmp(rname, e->roomName)) == 0) {
				while(e->name[i] != NULL) {
					//checks if user is in room
					if(strcmp(e->name[i],user) == 0) {
						int x = 0;
						char * pointer;
						char * message = (char *) malloc(sizeof(char) * (strlen(args) + strlen(user)));
						while(e->msg[x] != NULL) {
							x++;
						}
						e->msg[x] = (char *) malloc(sizeof(char) * (strlen(args) + strlen(user)));
						char * counter = (char *)malloc(sizeof(char));
						p = sprintf(counter,"%d ",e->countMessage);
						strcat(message, counter);
						strcat(message, strdup(user));
						strcat(message, " ");
						strcat(message, strdup(total));
						strcpy(e->msg[x], strdup(message));
						const char * ok = "OK\r\n";
						write(fd, ok, strlen(ok));
						e->countMessage++;
						return;
					}
					i++;
				}
			}
			e = e->n;
		}
		const char * m = "ERROR (user not in room)\r\n";
		write(fd, m, strlen(m));	
		return;
	}

	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
	if(checkPassword(fd, user, password) == true) {
		Rooms * e = (Rooms *) malloc(sizeof(Rooms));
		e = rlist->heading;
		char * total = (char *)malloc(sizeof(char));
		strcpy(total, args);
		char * rname = (char *)malloc(sizeof(char));
		char *linenumber = (char *)malloc(sizeof(char));
		int number;
		linenumber = strtok(total, " ");
		total = strtok(NULL, "");
		number = atoi(linenumber);
		strcpy(rname,strdup(total));
		int i = 0;
		while(e != NULL) {
			if(!(strcmp(rname,e->roomName))) {
				while(e->name[i] != NULL) {
					if(strcmp(e->name[i], user) == 0) {
						int x = 0;
						if(e->countMessage == e->getMessage || number >= e->countMessage) {
							const char * e = "NO-NEW-MESSAGES\r\n";
							write(fd, e, strlen(e));
							return;
						}
						while(e->msg[number] != NULL) {
							write(fd, strdup(e->msg[number]),strlen(e->msg[number]));
							write(fd, "\r\n", strlen("\r\n"));
							number++;
						}
						write(fd, "\r\n", strlen("\r\n"));
						//e->getMessage = e->countMessage;
						return;
					}
					i++;
				}
			}
			e = e->n;
		}
		const char * m = "ERROR (User not in room)\r\n";
		write(fd, m, strlen(m));
		return;
	}

	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	//const char * msg =  "OK\r\n";
	//write(fd,msg, strlen(msg));
}

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
	//Dont forget to fix end of line printing
	Rooms * e = (Rooms *) malloc(sizeof(Rooms));
	e = rlist->heading;
	char * rname = (char *)malloc(sizeof(char));
	strcpy(rname, strdup(args));
	int x = 0;
	int i = 0;
	if(checkPassword(fd, user, password) == true) {
		while(e != NULL) {
			if(!(strcmp(rname,e->roomName))) {
				while(e->name[i] != NULL) {
					i++;
				}
				sorter(e->name, i);
				while(e->name[x] != NULL) {
					write(fd,strdup(e->name[x]),strlen(e->name[x]));
					write(fd,"\r\n",strlen("\r\n"));
					x++;
				}
				write(fd,"\r\n",strlen("\r\n"));
				return;
			}
			e= e->n;
		}
	}
	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	write(fd,"\r\n",strlen("\r\n"));
	//Prints if no segv
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password, const  char * args)
{
	int i = 1;
	int x = 0;
	if(checkPassword(fd,user,password) == true) {
		while(username[x] != NULL) {
			x++;
		}
		sorter(username,x);
		while(username[i] != NULL) {
			//Prints out all users to socket
			//write(fd,"\r\n",strlen("\r\n"));
			write(fd, strdup(username[i]),strlen(username[i]));
			write(fd,"\r\n",strlen("\r\n"));
			i++;
		}
		write(fd,"\r\n",strlen("\r\n"));
	}
	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	//Prints if no segv
}

void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args) 
{
		
	int i = 0;
	int x = 0;
	if(checkPassword(fd, user, password) == true) {
		Rooms * e = (Rooms *) malloc(sizeof(Rooms));
		Rooms * next = (Rooms *) malloc(sizeof(Rooms));
		//setting list head to e
		
		//setting heading at e to next NULL entry
		e->roomName = (char *)malloc(sizeof(char));
		e->roomName = strcpy(e->roomName, strdup(args));
		while(room[x] != NULL) {
			x++;
		}
		room[x] = (char *)malloc(sizeof(char));
		strcpy(room[x],e->roomName);
		//make all char * empty and malloc them
		while(i < 100) {
			e->name[i] = NULL;
			e->msg[i] = NULL;
			i++;
		}
		e->getMessage = 0;
		e->countMessage = 0;
		e->n = rlist->heading;
		rlist->heading = e;
	}
	else {
		const char * error = "ERROR (Wrong password)\r\n";
		write(fd, error, strlen(error));
		return;
	}
	//Prints if no segv
	const char * msg =  "OK\r\n";
	write(fd,msg, strlen(msg));
}
void
IRCServer::listRooms(int fd, const char * user, const char * password, const char * args)
{
	int i = 0;
	int x = 0;
	Rooms * e = (Rooms *) malloc(sizeof(Rooms));
	e = rlist->heading;
	while(room[i] != NULL) {
		i++;
	}
	sorter(room,i);
	while(room[x] != NULL) {
		write(fd, strdup(room[x]),strlen(room[x]));
		write(fd,"\r\n",strlen("\r\n"));
		x++;
	}
/*		
	sorter(room, x);
	while(e != NULL) {		
		write(fd,strdup(e->roomName),strlen(e->roomName));
		write(fd,"\r\n",strlen("\r\n"));
		e = e->n;
	}*/
	//Prints if no segv
	const char * msg =  "OK\r\n";
	write(fd,msg, strlen(msg));
}

