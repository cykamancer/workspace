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
#include <time.h>
#include <sys/time.h>
#include "utils.h"
#include "logconst.h"

#define MAX_LISTENQUEUELEN 20	///< The maximum number of queued connections.
#define MAX_DATABASE_LEN 1000   ///< The maxinum number of tables, and entries in each table->


/**
  * @brief Searches for a table with specified name.
  * @param name Specified name of table->
  * @param database The pointer to root of the database.
  * @return Returns the pointer to entry if found, NULL otherwise.
  */
Table* table_search(char* name, Table* tables)
{
	int i=0;
	for(i=0; i<MAX_DATABASE_LEN; i++)
        	if(tables[i].name)
  			if(!strcmp(tables[i].name, name))
  				return &tables[i];
	return NULL;
}

/**
  * @brief Searches for a column in a table with specified name.
  * @param name Specified name of column.
  * @param table The pointer to root of the database.
  * @return Returns the pointer to entry if found, NULL otherwise.
  */
int column_search(Table* table, char* name)
{
	int i=0;
	for(i=0; i<MAX_COLUMNS_PER_TABLE; i++){
        	if(*(table->columns[i])){
                     //printf("%s %s\n", table->columns[i], name);
  		     if(!strcmp(table->columns[i], name)){
  				return i;
                      }
                 }
        }
	return -1;
}

/**
  * @brief Searches for a entry with specified key.
  * @param key Specified key.
  * @param database The pointer to root of the database.
  * @return Returns the pointer to entry if found, NULL otherwise.
  */
Entry* entry_search(char* key, Entry* entries)
{
	int i=0;
	for(i=0; i<MAX_RECORDS_PER_TABLE; i++)
		if(entries[i].key)
			if(!strcmp(entries[i].key,key))
				return &entries[i];
	return NULL;
}

/**
 * this setup calculates elapsed time between
 * start and end accurate to microseconds
 */
struct timeval start, end;
double secs;

/**
 * @brief Process a command from the client. Some error checking has been added; see documentation in storage.c.
 *
 * @param sock The socket connected to the client.
 * @param cmd The command received from the client.
 * @param database Pointer to the server's database.
 * @param username username in default.conf.
 * @param password encrypted password in default.conf.
 * @return Returns 0 on success, -1 otherwise.
 */
int handle_command(int sock, char *cmd, Table* database, char* username, char* password, int *auth)
{
//printf("asdasdasdasdasd\n");
	timevar = sprintf(message, "Processing command '%s'\n", cmd);
	fp = fopen(buffer, "a+");
	logger(LOGGING2, fp, message); // replace LOG commands with logger() calls
	fclose(fp);

	// For now, just send back the command to the client.
	char split[100]="\0";
	char args[15][100];
	//char args[1][100];
	//char args[2][100];
	char errstr[4]="\0"; //stores the possible errno.
        int index, index2;
        for (index=0; index<15; index++){
             for(index2=0; index2<100; index2++){
                 //args[index][index2]='\n';
             }
             //printf("%s\n", args[index]);
        }
	//strcpy(args[0], "");
	//strcpy(args[1], "");
	//strcpy(args[2], "");
       //strtok(cmd, "\n");
        strncpy(split, strtok(cmd, ","), sizeof(split));

        int arg_index=0;
        char *temp2=strtok(NULL, ",");
        while(temp2){
              if(*temp2){
            strncpy(args[arg_index], temp2, sizeof(args[arg_index]));
               printf("%s\n", args[arg_index]);

            arg_index++;}
            temp2=strtok(NULL, ",");
        }

	//sscanf (cmd, "%s %s %s %s", split, args[0], args[1], args[2]);
	if(!strcmp(split,"AUTH")) {
		gettimeofday(&start, NULL);

		if(!strcmp(username,args[0])&&!strcmp(password,args[1])) {
    			*auth=1;

			gettimeofday(&end, NULL);
			FILE *logfiles=fopen("Server log Time.txt", "a");
			secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfiles, "A %-.1f\n", secs*1000000);
			fclose(logfiles);

			sendall(sock, "0", 1);
			sendall(sock, "\n", 1);
			return 0;
    		}
    		else
    			errno = ERR_AUTHENTICATION_FAILED;
	}
	else if(!strcmp(split,"GET")) {
		char str[MAX_CMD_LEN];

		if(!*auth) {
			errno = ERR_NOT_AUTHENTICATED;
			sprintf(errstr, "%d", errno);
			strcpy(str, "FAIL ");
			strcat(str, errstr);
			strcat(str, "\n");
			sendall(sock, str, strlen(str));
			return -1;
		}
		else {
			gettimeofday(&start, NULL);
			Table* table=table_search(args[0], database);

    			if(table) {
    				Entry* entry=entry_search(args[1], table->entry);

				if(entry) {
					gettimeofday(&end, NULL);
					FILE *logfiles=fopen("Server log Time.txt", "a");
					secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
					fprintf(logfiles, "G %-.1f\n", secs*1000000);
					fclose(logfiles);
					strcpy(str, entry->value);
					strcat(str, " 0\n");
					sendall(sock, str, strlen(str));
					return 0;
				}
				else {
					errno = ERR_KEY_NOT_FOUND;
					sprintf(errstr, "%d", errno);
					strcpy(str, "FAIL ");
					strcat(str, errstr);
					strcat(str, "\n");
					sendall(sock, str, strlen(str));
					return -1;
				}
			}
			else {
				errno = ERR_TABLE_NOT_FOUND;
				sprintf(errstr, "%d", errno);
				strcpy(str, "FAIL ");
				strcat(str, errstr);
				strcat(str, "\n");
				sendall(sock, str, strlen(str));
				return -1;
			}
		}
	}
	else if(!strcmp(split,"SET")) {
 			sendall(sock, "0", 1);
			sendall(sock, "\n", 1);
			return 0;

		if(!*auth)
			errno = ERR_NOT_AUTHENTICATED;
                else{
			Table* table=table_search(args[0], database);
		if(!table){

                         errno = ERR_TABLE_NOT_FOUND;

                         //old code.
				/*int i;

				for(i=0; i<MAX_DATABASE_LEN; i++) {
    					if(!database[i].name) {
    					    	table=&database[i];
						break;
    	    				}
    				}
				table->name=(char*)malloc(strlen(args[0])+1);
				strcpy(table->name, args[0]);*/
		      }
		else {
			gettimeofday(&start, NULL);



			Entry* entry=entry_search(args[1], table->entry);
                                 char arg_output[MAX_VALUE_LEN]="";

			/*if(!entry) {
                                 arg_index=2;
                                 while(args[arg_index][0]){
    			    	      int isBadColumnorValue=0;
                                 char colname[MAX_COLNAME_LEN]="";
                                 char colvalue[MAX_VALUE_LEN]="";
                                 printf("---%s---\n", args[arg_index]);
                                 sscanf(args[arg_index], "%s %s", colname, colvalue);
    			    	int column_index=column_search(table, colname);
                         //printf("asdasd%d\n", column_index);
    			    	if(column_index==-1){
                                         printf("%s: No such column\n", colname);
    			    		isBadColumnorValue=1;
    			    	}
    			    	else if(!strcmp(table->datatypes[column_index], "int")){
    			    		if(!isNum(colvalue)){
                                         printf("Predicate value not a number\n");
                                     isBadColumnorValue=1;
                                         }
    			    	}
    			    	else if(!strcmp(table->datatypes[column_index], "char")){
    			    		if(table->datasizes[column_index]<sizeof(colvalue)+1){
                                         printf("Predicate datasize not correct\n");
                                    isBadColumnorValue=1;
                                         }
    			    	}

    			    	//Checks if the predicate is valid.
    			    	if(isBadColumnorValue){
    						errno = ERR_INVALID_PARAM;
	                                         sprintf(errstr, "%d", errno);
	                                         sendall(sock, errstr, strlen(errstr));
	                                         sendall(sock, "\n", 1);

	                                         return -1;
    			    	}
                                 strcat(arg_output, colvalue);
                                 strcat(arg_output, ",");
                                 arg_index++;

                                 }


				int i;

				for(i=0; i<MAX_DATABASE_LEN; i++) {
					if(!table->entry[i].key) {
						entry=&(table->entry[i]);
						break;
		    	    		}
				}
				entry->key=(char*)malloc(MAX_KEY_LEN);
				entry->value=(char*)malloc(MAX_VALUE_LEN);
				strcpy(entry->key, args[1]);
				strcpy(entry->value, arg_output);
			}
			else { printf("using storage_set delete\n");

				if(!args[2][0]) {
                                        printf("using storage_set deleeete\n");
                                        printf("-----%s\n",entry->key);
                                        printf("-----%s\n",entry->value);

					//entry->key=memset(entry->key, '\0', MAX_KEY_LEN);
                                         //free (entry->key);
					//entry->value=memset(entry->value, '\0', MAX_VALUE_LEN);
                                   //free (entry->value);
                                        printf("---------%d\n",sizeof(entry->key));
				}
				else {
                                        printf("using storage_set update\n");
					free(entry->value);
					entry->value=(char*)malloc(MAX_VALUE_LEN);
					strcpy(entry->value, arg_output);
				}
			}*/

			gettimeofday(&end, NULL);
			FILE *logfiles=fopen("Server log Time.txt", "a");
			secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfiles, "S %-.1f\n", secs*1000000);
			fclose(logfiles);

			sendall(sock, "0", 1);
			sendall(sock, "\n", 1);
			return 0;
		}
                }
	}

	else if(!strcmp(split,"QUERY")) {
		//args[0]: table-> args[1]: predicates. args[2]: max_number_of_keys.
		char str[MAX_CMD_LEN];

		if(!*auth) {
			errno = ERR_NOT_AUTHENTICATED;
			sprintf(errstr, "%d", errno);
			strcpy(str, errstr);
			strcat(str, " 0 FAIL\n");
			sendall(sock, str, strlen(str));
			return -1;
		}
		else {
			gettimeofday(&start, NULL);
			Table* table=table_search(args[0], database);

    			if(table) {
//printf("columns: %s %s %s\n", table->name, table->columns[0], table->columns[1]);
    				char predicate[MAX_COLNAME_LEN+MAX_VALUE_LEN];
    				char predicate_column[MAX_COLNAME_LEN];
    				char predicate_value[MAX_VALUE_LEN];
                    int max_keys=atoi(args[2]);
    				char predicate_columns[MAX_COLUMNS_PER_TABLE][MAX_COLNAME_LEN];
    				char predicate_values[MAX_COLUMNS_PER_TABLE][MAX_VALUE_LEN];
    				int predicate_operators[MAX_COLUMNS_PER_TABLE];
    				int predicate_indices[MAX_COLUMNS_PER_TABLE];

    				char*temp=strtok(args[1], ",");
//printf("1--%s\n", predicate);
    				int i=0;
    			       while(i<MAX_DATABASE_LEN && temp){
                                 strncpy(predicate, temp, sizeof(predicate));
    			    	int predicate_operator=0; //0 represents no operator, 1 represents a less-than comparison, 2 represents a equals comparison, 3 represents a greater-than comparison.
    			    	int u=0;
    			    	//First, remove any whitespaces from the predicate input.
    			    	int nowhitespace_index=0;
    			    	while (predicate[u]){
    			    		if(u!=' '){
    			    			predicate[nowhitespace_index]=predicate[u];
    			    			nowhitespace_index++;
                                                 u++;
    			    		}
    			    	}
	                    predicate[nowhitespace_index]='\0';
//printf("1--%s\n", predicate);
	                    //Determines the operator of this predicate.
    			    	u=0;
    			    	while(predicate[u]){

    			    		if(predicate[u]=='<'){
    			    			predicate_operator=1;
                                                 break;
    			    		}
    			    		if(predicate[u]=='='){
    			    			predicate_operator=2;
                                          break;
    			    		}
    			    		if(predicate[u]=='>'){
    			    			predicate_operator=3;
                                          break;
    			    		}
                                         u++;
    			    	}

//printf("extracting %d\n", u);
    			    	//Extracts the column and value from the predicate.
    			    	strncpy(predicate_column, predicate, u);
                                 predicate_column[u]='\0';
    			  //printf("predicate_column: %s\n", predicate_column);
    			    	strncpy(predicate_value, predicate+u+1, sizeof(predicate_value));
    			  //printf("predicate_value: %s\n", predicate_value);

    			    	//Checks if the predicate's column exists in the structure, and if yes, if the datatype of that column matches the predicate's value.
    			    	int isBadColumnorValue=0;
    			    	int column_index=column_search(table, predicate_column);
                         //printf("asdasd%d\n", column_index);
    			    	if(column_index==-1){
                                         //printf("No such column\n");
    			    		isBadColumnorValue=1;
    			    	}
    			    	else if(!strcmp(table->datatypes[column_index], "int")){
    			    		if(!isNum(predicate_value)){
                                         //printf("Predicate value not a number\n");
                                     isBadColumnorValue=1;
                                         }
    			    	}
    			    	else if(!strcmp(table->datatypes[column_index], "char")){
    			    		if(table->datasizes[column_index]<sizeof(predicate_value)+1){
                                         //printf("Predicate datasize not correct\n");
                                    isBadColumnorValue=1;
                                         }
    			    	}

    			    	//Checks if the predicate is valid.
    			    	if(predicate_operator==0||isBadColumnorValue){//No operator detected, which means an invalid parameter.
    						errno = ERR_INVALID_PARAM;
    						sprintf(errstr, "%d", errno);
    						strcpy(str, errstr);
    						strcat(str, " 0 FAIL\n");
    						sendall(sock, str, strlen(str));
    						return -1;
    			    	}
    			    	//All good, copying decomposed values of this predicate into arrays for later use.
//printf("2--%d\n", i);
                        strncpy(predicate_columns[i], predicate_column, sizeof(predicate_columns[i]));
    			   //printf("predicate_columns: %s\n", predicate_columns[i]);
                        predicate_operators[i]=predicate_operator;
                        predicate_indices[i]=column_index;
    			    	strncpy(predicate_values[i], predicate_value, sizeof(predicate_values[i]));
    			   //printf("predicate_values: %s\n", predicate_values[i]);
    			   temp=strtok(NULL, ",");
                         i++;
                                 //printf("---------%s", predicate);
    			   }
    			    //Now, we traverse through the table's entries to determine the entries that meets the predicates and store them in a string.
    			    int entry_index=0;
    			    char keys[MAX_CMD_LEN];
    			    int number_of_keys=0;
    			    while(entry_index<MAX_RECORDS_PER_TABLE&&table->entry[entry_index].key){
    			    	int i=0;
    			    	while(strcmp(predicate_columns[i], "")){

    			    			char *col_value=malloc(MAX_VALUE_LEN);
    			    			if(column_value(table->entry[entry_index].value, predicate_indices[i], col_value)){
    			    				//printf("column_value failed :P\n");
    			    			}
                                                 printf("%s\n", col_value);
    			    			if(predicate_operators[i]==1){ //Perform the less-than comparison.
    			    				if(strcmp(table->datatypes[predicate_indices[i]], "int")){
    			    					if(atoi(col_value)>=atoi(predicate_values[i])){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    				else if(strcmp(table->datatypes[predicate_indices[i]], "char")){
    			    					if(strcmp(col_value, predicate_values[i])>=0){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    			}
    			    			else if(predicate_operators[i]==2){//Perform the equals comparison.
    			    				if(strcmp(table->datatypes[predicate_indices[i]], "int")){
    			    					if(atoi(col_value)!=atoi(predicate_values[i])){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    				else if(strcmp(table->datatypes[predicate_indices[i]], "char")){
    			    					if(strcmp(col_value, predicate_values[i])!=0){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    			}
    			    			else if(predicate_operators[i]==3){//Performs the greater-than comparison
    			    				if(strcmp(table->datatypes[predicate_indices[i]], "int")){
    			    					if(atoi(col_value)<=atoi(predicate_values[i])){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    				else if(strcmp(table->datatypes[predicate_indices[i]], "char")){
    			    					if(strcmp(col_value, predicate_values[i])<=0){
    			    						goto next; //this entry does not meet the requirements, move on to next entry
    			    					}
    			    				}
    			    			}
                                      free(col_value);
    			    	    i++;  //Moves on to the next predicate.
    			    	}
    			    	if(number_of_keys<max_keys){
    			    	    strcat(keys, table->entry[entry_index].key);
    			    	    strcat(keys, ",");
    			    	    number_of_keys++;
    			    	}
    			        next:
    			    	entry_index++; //Moves on to the next entry
    			    }
//printf("3--\n");
				if(number_of_keys==0) {
					errno = ERR_KEY_NOT_FOUND;
					sprintf(errstr, "%d", errno);
					strcpy(str, errstr);
					strcat(str, " 0 FAIL\n");
					sendall(sock, str, strlen(str));
					return -1;
				}
				else
				    sprintf(str, "0 %d %s\n", number_of_keys, keys);
				    sendall(sock, str, strlen(str));
				   return 0;
			}
			else {
				errno = ERR_TABLE_NOT_FOUND;
					sprintf(errstr, "%d", errno);
					strcpy(str, errstr);
					strcat(str, " 0 FAIL\n");
					sendall(sock, str, strlen(str));
					return -1;

			}
		}
	}
	sprintf(errstr, "%d", errno);
	sendall(sock, errstr, strlen(errstr));
	sendall(sock, "\n", 1);

	return -1;
}

/**
 * @brief Loads the census data into the server's database.
 * @database the server's database.
 */
int census_load(Table* database){
	char* census_path="census.dat";
	FILE* census=fopen(census_path, "r");
	database->name=(char*)malloc(7);
	strcpy(database->name, "census");
	return census_load_helper(census, database);
}

/**
 * @brief Start the storage server.
 *
 * This is the main entry point for the storage server.  It reads the
 * configuration file, starts listening on a port, and proccesses
 * commands from clients.
 */
int main(int argc, char *argv[])
{
       int i=0, u=0;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime (buffer,40,"Server-%Y-%m-%d-%H-%M-%S.log",timeinfo);
	puts (buffer);

	int auth=0; //indicates whether the user is authenticated!!!
	Table database[MAX_DATABASE_LEN];

	/*if (census_load(database)) {
		//printf("loading census failed\n");
	}*/

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

    	printf("chk\n");
	i=0;
	//Created by Danny
	//Loads the params into data structure and contains various test printfs.
	while(i<MAX_DATABASE_LEN && strcmp(params.table_name[i], "")) {

		//printf("Table #%d\n", i);
		database[i].name=(char*)malloc(MAX_TABLE_LEN);
		database[i].columns=mallocStringArray(MAX_COLUMNS_PER_TABLE,MAX_COLNAME_LEN);
		database[i].datatypes=mallocStringArray(MAX_COLUMNS_PER_TABLE,5);
		database[i].datasizes=(int*)malloc(MAX_COLUMNS_PER_TABLE*sizeof(int));

    	strncpy(database[i].name, params.table_name[i], sizeof(database[i].name));
    	u=0;
    	while(strcmp(params.column_list[i][u], "")){
             //("---%s\n", params.column_list[i][u]);
               //printf("----%d\n",sizeof(database[i].columns[u]));
    	      strncpy(database[i].columns[u], params.column_list[i][u], sizeof(params.column_list[i][u])+1);
    	      strncpy(database[i].datatypes[u], params.column_list[i][u], sizeof(params.column_list[i][u])+1);
              //printf("----%s\n",database[i].columns[u]);
    	      database[i].datasizes[u]=params.datasize_list[i][u];
              //printf("--------%d\n", database[i].datasizes[u]);
             u++;
    	}
    	i++;
    }


	timevar = sprintf(message, "Server on %s:%d\n", params.server_host, params.server_port);
	//printf("%i", n);
	fp = fopen(buffer, "a+");
	logger(LOGGING2, fp, message);
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

		timevar = sprintf(message, "Got a connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		fp = fopen(buffer, "a+");
		logger(LOGGING2, fp, message);
		fclose(fp);

		//printf("ggggggggg");
		// Get commands from client.
		int wait_for_commands = 1;
		do {
			// Read a line from the client.
			char cmd[MAX_CMD_LEN];
			int status = recvline(clientsock, cmd, MAX_CMD_LEN);
                                                 //printf("cmd is %s\n", cmd);

			if (status != 0) {
				// Either an error occurred or the client closed the connection.
				wait_for_commands = 0;
			} else {
				// Handle the command from the client.
                                                 //printf("%s\n", params.username);
                                                 //printf("%s\n", params.password);
                                                 //printf("%s\n", cmd);
                                                 //printf("%s\n", cmd);
                                                 printf("%s\n", cmd);

				int status = handle_command(clientsock, cmd, database, params.username, params.password, &auth);
                printf("%s\n", cmd);
                                                // printf("chk\n");
				if(status){
                                                 //printf("chk\n");
					wait_for_commands=0;                                                               }
			}
		} while (wait_for_commands);

		// Close the connection with the client.
		close(clientsock);
              auth=0;

		timevar = sprintf(message, "Closed connection from %s:%d.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		fp = fopen(buffer, "a+");
		logger(LOGGING2, fp, message);
		fclose(fp);
	}

	// Stop listening for connections.
	close(listensock);

	//frees any malloc'd memory.
        i=0;
        while(i<MAX_DATABASE_LEN && database[i].name){
                u=0;
		while(database[i].entry[u].key){
			free(database[i].entry[u].key);
			free(database[i].entry[u].value);
                         u++;
		}
		free(database[i].name);
                freeStringArray(database[i].columns, MAX_COLUMNS_PER_TABLE);
                freeStringArray(database[i].datatypes, MAX_COLUMNS_PER_TABLE);
                free(database[i].datasizes);
                i++;
        }



/*	for(i=0; i<MAX_DATABASE_LEN; i++){
		Entry* entry=database[i].entry;
		for(u=0; u<MAX_DATABASE_LEN; u++){
			free(entry[u].key);
			free(entry[u].value);

		}
		free(database[i].name);
        //free((database[i].columns));
        //free(database[i].datatypes);
        //free(database[i].datasizes);
	}
*/
	return EXIT_SUCCESS;
}

