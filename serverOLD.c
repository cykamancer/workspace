/**
 * @file
 * @brief This file implements the storage server.
 *
 * The storage server should be named "server" and should take a single
 * command line argument that refers to the configuration file.
 *
 * The storage server should be able to communicate with the client
 * library functions declared in storage.h and implemented in storage.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "utils.h"
#include "logconst.h"

#define LOGGING 2
#define MAX_LISTENQUEUELEN 20	///< The maximum number of queued connections.
#define MAX_DATABASE_LEN 1000   ///< The maxinum number of tables, and entries in each table.

  time_t rawtime;
  struct tm * timeinfo2;
  char buffer2 [40];
  FILE *fp;

  char message[100];
  int n; //for sprintf

  /**
   * @brief Global array of entries.
   */

  struct Entry {
  	char *key;
  	char *value;
  };

  typedef struct Entry Entry;
  /**
   * @brief Global array of tables.
   */
  struct Table{
  	char *name;
  	Entry entry[MAX_DATABASE_LEN];
  };

  typedef struct Table Table;


//time_t tm = mktime(time(0)); //creates global time struct created at program execution

 /**
   * @brief Searches for a entry with specified key.
   * @param key Specified key.
   * @param database The pointer to root of the database.
   * @return Returns the pointer to entry if found, NULL otherwise.
   */

Table* table_search(char* name, Table tables[MAX_DATABASE_LEN]){
  int i=0;
  for(i=0; i<MAX_DATABASE_LEN; i++){
  	if(tables[i].name==name){
  		return &tables[i];
  	}
  }
  return NULL;
 }

/**
  * @brief Searches for a entry with specified key.
  * @param key Specified key.
  * @param database The pointer to root of the database.
  * @return Returns the pointer to entry if found, NULL otherwise.
  */

Entry* entry_search(char* key, Entry entries[MAX_DATABASE_LEN]){
	int i=0;
	for(i=0; i<MAX_DATABASE_LEN; i++){
		if(entries[i].key==key){
			return &entries[i];
		}
	}
	return NULL;
}
/**
 * @brief Process a command from the client.
 *
 * @param sock The socket connected to the client.
 * @param cmd The command received from the client.
 * @return Returns 0 on success, -1 otherwise.
 */



int handle_command(int sock, char *cmd, Table* database, char* username, char* password)
{

	n = sprintf(message, "Processing command '%s'\n", cmd);
	fp = fopen(buffer2, "a+");
	logger(LOGGING, fp, message); // replace LOG commands with logger() calls
	fclose(fp);

	// For now, just send back the command to the client.
	int isReadingCMD=0; //boolean that indicated whether it is reading the command indicator (AUTH, SET, GET)

	char* split, arg1, arg2, arg3;
	split=strtok(cmd, " ");
    if(split=="AUTH")
    {
    	arg1=strtok(NULL, " ");//Get username
    	arg2=strtok(NULL, " ");//Get password
    	if(username==arg1&&password==arg2){
    		sendall(sock, arg1, strlen(arg1));
    		return 0;
    	}
    	else return -1;

    }
    else if(split=="GET")
    {
    	arg1=strtok(NULL, " ");//Get name of table
    	arg2=strtok(NULL, " ");//Get key of entry
        Table* table=table_search(arg1, database);
    	if(table){
    		Entry* entry=entry_search(arg2, table->entry);
    	    if(entry){
    			sendall(sock, entry->value, strlen(entry->value));
    	    }
    	}
    }
    else if(split=="SET")
    {
    	arg1=strtok(NULL, " ");//Get name of table
    	arg2=strtok(NULL, " ");//Get key of entry
    	arg3=strtok(NULL, " ");//Get value to set

    	Table* table=table_search(arg1, database);
    	if(!table){
    		int i;
    		for(i=0; i<MAX_DATABASE_LEN; i++){
    		    if(database[i].name==""){
    		    	table=&database[i];
                    break;
    	    	}
    		}
    	}
    	table->name=arg1;
    	Entry* entry=entry_search(arg2, table->entry);
    	if(!entry){
    		int i;
    		for(i=0; i<MAX_DATABASE_LEN; i++){
    		    if(table->entry[i].key==""){
    		    	entry=&(table->entry[i]);
                    break;
    	    	}
    		}
    	}
    	entry->key=arg2;
        entry->value=arg3;
        sendall(sock, entry->value, strlen(entry->value));
    }
	/*
	sendall(sock, cmd, strlen(cmd));
	sendall(sock, "\n", 1);*/

	return 0;
}

/**
 * @brief Creates the data structure to be used in our tables.
 *
 * @param key A string, the entry's key.
 * @param value An integer, the entry's value. The data type of this parameter might be changed for later versions of the storage server.
 */



/**
 * @brief Creates the data structure to berver.c -o server.o
server.c:69: error: expected identifier or â€?â€?before â€˜[â€?token
server.c:70: error: expected â€?â€?before â€˜intâ€?
server.c:84: error: expected â€?â€? â€?â€? â€?â€? â€˜asmâ€?or â€˜__attribute__â€?before â€˜databaseâ€?
make[1]: *** [server.o] Error 1
 * e used in our tables.
 *
 * @param key A string, the entry's key.
 * @param value An integer, the entry's value. The data type of this parameter might be changed for later versions of the storage server.
 */



/**
 * @brief Start the storage server.
 *
 * This is the main entry point for the storage server.  It reads the
 * configuration file, starts listening on a port, and proccesses
 * commands from clients.
 */
int main(int argc, char *argv[])
{


	Table database[MAX_DATABASE_LEN];

	//printf("Setting time\n");//remove
	  time ( &rawtime );
	  timeinfo2 = localtime ( &rawtime );
	  //printf("Finished rawtime stuff\n"); //remove

	  //printf("About to do strftime\n"); //remove
	  strftime (buffer2,40,"Server-%Y-%m-%d-%H-%M-%S.log",timeinfo2);
	  puts (buffer2);
	  //printf("Finished setting time\n");//remove

	// Process command line arguments.
	// This program expects exactly one argument: the config file name.
	assert(argc > 0);
	if (argc != 2) {
		printf("Usage %s <config_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *config_file = argv[1];

	// Read the config file.
	struct config_params params;
	int status = read_config(config_file, &params);
	if (status != 0) {
		printf("Error processing config file.\n");
		exit(EXIT_FAILURE);
	}

	n = sprintf(message, "Server on %s:%d\n", params.server_host, params.server_port);
	//printf("%i", n);
	fp = fopen(buffer2, "a+");
	logger(LOGGING, fp, message);
	fclose(fp);

	// Create a socket.
	int listensock = socket(PF_INET, SOCK_STREAM, 0);
	if (listensock < 0) {
		printf("Error creating socket.\n");
		exit(EXIT_FAILURE);
	}

	// Allow listening port to be reused if defunct.
	int yes = 1;
	status = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	if (status != 0) {
		printf("Error configuring socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind it to the listening port.
	struct sockaddr_in listenaddr;
	memset(&listenaddr, 0, sizeof listenaddr);
	listenaddr.sin_family = AF_INET;
	listenaddr.sin_port = htons(params.server_port);
	inet_pton(AF_INET, params.server_host, &(listenaddr.sin_addr)); // bind to local IP address
	status = bind(listensock, (struct sockaddr*) &listenaddr, sizeof listenaddr);
	if (status != 0) {
		printf("Error binding socket.\n");
		exit(EXIT_FAILURE);
	}

	// Listen for connections.
	status = listen(listensock, MAX_LISTENQUEUELEN);
	if (status != 0) {
		printf("Error listening on socket.\n");
		exit(EXIT_FAILURE);
	}

	// Listen loop.
	int wait_for_connections = 1;
	while (wait_for_connections) {
		// Wait for a connection.
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof clientaddr;
		int clientsock = accept(listensock, (struct sockaddr*)&clientaddr, &clientaddrlen);
		if (clientsock < 0) {
			printf("Error accepting a connection.\n");
			exit(EXIT_FAILURE);
		}
		n = sprintf(message, "Got a connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		fp = fopen(buffer2, "a+");
		logger(LOGGING, fp, message);
		fclose(fp);

		// Get commands from client.
		int wait_for_commands = 1;
		do {
			// Read a line from the client.
			char cmd[MAX_CMD_LEN];
			int status = recvline(clientsock, cmd, MAX_CMD_LEN);
			if (status != 0) {
				// Either an error occurred or the client closed the connection.
				wait_for_commands = 0;
			} else {
				// Handle the command from the client.
				int status = handle_command(clientsock, cmd, database, params.username, params.password);
				if (status != 0)
					wait_for_commands = 0; // Oops.  An error occured.
			}
		} while (wait_for_commands);

		// Close the connection with the client.
		close(clientsock);
		n = sprintf(message, "Closed connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		fp = fopen(buffer2, "a+");
		logger(LOGGING, fp, message);
		fclose(fp);
	}

	// Stop listening for connections.
	close(listensock);

	return EXIT_SUCCESS;
}

