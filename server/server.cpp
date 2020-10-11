/*
Patrick Bald, John Quinn, Rob Reutiman
pbald, jquin13, rreutima
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../lib/pg3lib.h"

#define MAX_LINE 4096
#define MAX_THREAD 10

/* GLOBALs */
int NUM_THREADS = 0;
char * AUTH_FILE = "authfile.txt"

/*
 * @func   client_authenticate
 * @desc   logs in user or creates account
 * --
 * @param  s  socket desc
 */
void client_authenticate (int s) {
	
	// Receive Client Username
	char uname [BUFSIZ];
	if(recv(client_sock, uname, sizeof(uname), 0) < 0) {
		fprintf(stderr, "Unable to get client username\n");
		exit(1);
	}
	printf("Username: %s\n", username);

	// Generate Server Public Key
	char * skey = getPubKey();
	
	// Send Public Key
	if (send(s, skey, sizeof(skey), 0) < 0) {
		fprintf(stdout, "Unable to send server public key\n");
		exit(1);
	}

	// Receive Client Password
	if (recv(s, epass, sizeof(epass), 0) < 0) {
		fprintf(stderr, "Unable to get client username\n");
		exit(1);
	}

	// Decrypt Password
	char * pass = decrypt(epass);

	// Open Authentication File
	FILE * fp = fopen(AUTH_FILE, "rw");
	if (!fp) {
		fprintf(stderr, "Unable to open Auth File\n");
		exit(1);
	}

	// Loop Through File
	char fline [BUFSIZ];
	char *fuser; char *fpass;
	while (fgets(fline, sizeof(fline), fp)) {
		fuser = strtok(fline, "\t");
		fpass = strtok(NULL, "\n");
		fprintf(stdout, "FUser: %s; FPass: %s\n");
		bzero((char *)&fline, sizeof(fline));
	}

	// Login
	if (userFound) {
		
	// Create Account
	} else {

	}

	// Send Acknowledgement
	short ack = 1; ack = htons(ack);
	if (send(s, ack, sizeof(ack), 0) < 0) {
		fprintf(stdout, "Unable to send server public key\n");
		exit(1);
	}

	// Receive Client Public Key
	if (recv(s, ckey, sizeof(ckey), 0) < 0) {
		fprintf(stderr, "Unable to get client pubkey\n");
		exit(1);
	}
	

	fclose(fp);

}

/*
* @func   client_interaction
* @desc   thread function for handling user messages
*/
void* client_interaction(void* arg){

	int len;
	char command[MAX_LINE] = "";
	int client_sock = *(int*)arg;

	/* Loop to get commands */
	while(1) {

		if((len = recv(client_sock, command, sizeof(command), 0)) == -1) {
			perror("Server Received Error!"); 
			exit(1);
		}
  	if(len == 0) break;

		printf("%s\n", command);

		/* Command specific functions */
		if(!strncmp(command, "BM", 2)) {

		} else if(!strncmp(command, "PM", 2)) {

		} else if(!strncmp(command, "EX", 2)) {

		}

		bzero((char*)command, sizeof(command));

	}
  return NULL;
}

/*
* @func   main
* @desc   main driver, 
*         main thread for socket listener for incoming connections
*/
int main(int argc, char* argv[]){

  // Get port number
  int port;
  if(argc == 2){
    port = atoi(argv[1]);
  } else {
    fprintf(stderr, "Usage: ./myserver [PORT]\n");
    exit(1);
  }

  // Set up socket for connections
  struct sockaddr_in sin, client_addr;
  int s, client_sock;

  // Build Address Data Structure
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	
	// Set passive option 
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("myserver: socket");
		exit(1);
	}
	
	// Set Socket Option
	int opt = 1;
	if((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(int))) < 0) {
		perror("myserver: setsocket"); 
		exit(1);
	}

	// Bind Socket
	if((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
		perror("myserver: bind"); 
		exit(1);
	}

	// Listen
	if((listen(s, MAX_THREAD)) < 0) {
		perror("myserver: listen"); 
		exit(1);
	} 
	
	/* wait for connection, then receive and print text */
	socklen_t addr_len = sizeof(client_addr);
	printf("Waiting for connections on port %d\n", port);

  while(1) {

		if((client_sock = accept(s, (struct sockaddr *)&client_addr, &addr_len)) < 0){
			perror("myserver: accept"); 
			exit(1);
		}
 
    if(NUM_THREADS == 10){
      fprintf(stdout, "Connection Refused: Max clients online.\n");
      continue;
    }

    // Create new thread for each accepted client
   	pthread_t user_thread;
    NUM_THREADS++;
    if(pthread_create(&user_thread, NULL, client_interaction, &client_sock) < 0){
      perror("Error creating user thread\n");
      return 1;
    }

  }

  return 0;
}
