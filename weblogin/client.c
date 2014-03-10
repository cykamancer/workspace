/**
 * @file
 * @brief This file implements a "very" simple sample client.
 *
 * The client connects to the server, running at SERVERHOST:SERVERPORT
 * and performs a number of storage_* operations. If there are errors,
 * the client exists.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "storage.h"
#include "logconst.h"
#include "utils.h"

/**
 * @brief Start a client to interact with the storage server.
 *
 * If connect is successful, the client performs a storage_set/get() on
 * TABLE and KEY and outputs the results on stdout. Finally, it exists
 * after disconnecting from the server.
 */
int main(int argc, char *argv[]) {
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime (buffer,40,"Client-%Y-%m-%d-%H-%M-%S.log",timeinfo);
	puts (buffer);

	int serverport=3334;
	int maxnumberofkeys;
	char serverhost[64]="localhost";
	char serverusername[64]="admin";
	char serverpassword [64]="lol";
	char table[20]="\n";
	char key[20]="\n";
	char predicates[1024]="\n";
	char value[1024]="\n";
	char **keys;
        int choice;
        char flush;
	void *conn;
	int status = 1;
	struct storage_record r;

	/**
	 * @brief This setup calculates elapsed time between
	 * start and end accurate to microseconds
	 */
	struct timeval start, end;
	double secs;

	/**
	 * @brief Set up a config file struct pointer which reads default.conf
	 */
	struct config_params *configfile=malloc(sizeof(struct config_params));
	int configfileReadValid=read_config2(fopen("default.conf", "r"), configfile);
	if (configfileReadValid)
		printf("Invalid configuration file or configuration file error\n");

	while (1) {
		prompt();

		//ask user for choice
		printf("Please enter your selection: ");
		scanf("%d",&choice);

		while ((flush=fgetc(stdin)) != '\0' && flush != '\n') { }

		if (choice==1) {
			//Connect to server
			/*printf("Please enter host name:\n");
			fgets(serverhost, sizeof(serverhost), stdin);
			strtok(serverhost, "\n");
			printf("Please enter port:\n");
			scanf("%d",&serverport);*/

			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);

			//conn = storage_connect(configfile->server_host, configfile->server_port);
			conn = storage_connect(serverhost, serverport);
			if(!conn) {
				printf("Cannot connect to server @ %s:%d. Error code: %d.\n",
					serverhost, serverport, errno);
				return -1;
			}
			printf("Connected to server.\n");

			gettimeofday(&end, NULL);
			secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfile, "%-.3f\n", secs*1000);
			fclose(logfile);
		}
		else if (choice==2) {
			//Authenticate the client.
			/*printf("Please enter username:\n");
			fgets(serverusername, sizeof(serverusername), stdin);
			strtok(serverusername, "\n");
			printf("Please enter password:\n");
			fgets(serverpassword, sizeof(serverpassword), stdin);
			strtok(serverpassword, "\n");*/

			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);

			status = storage_auth(serverusername, serverpassword, conn);
			if(status != 0) {
				printf("storage_auth failed with username '%s' and password '%s'. " \
					"Error code: %d.\n", serverusername, serverpassword, errno);
			}
			else {
				printf("storage_auth: successful.\n");

				gettimeofday(&end, NULL);
				secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
				fprintf(logfile, "		%-.3f\n", secs*1000);
				fclose(logfile);
			}
		}
		else if (choice==3) {
			// Issue storage_set
			printf("Please enter table name:\n");
			fgets(table, sizeof(table), stdin);
			strtok(table, "\n");
			printf("Please enter key:\n");
			fgets(key, sizeof(key), stdin);
			strtok(key, "\n");
			printf("Please enter value:\n");
			fgets(value, sizeof(value), stdin);


			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);
			if(value[0]!='\n'){
                         printf("value copied\n");
                         strtok(value, "\n");
			strncpy(r.value, value, sizeof r.value);
                         }
                         else{
                         printf("value NOT copied\n");
                         strcpy(r.value, "\0");
                         }
			status = storage_set(table, key, &r, conn);
			if(status != 0) {
				printf("storage_set failed. Error code: %d.\n", errno);
			}
			else {
				printf("storage_set: successful.\n");

				gettimeofday(&end, NULL);
				double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
				fprintf(logfile, "				%-.3f\n", secs*1000);
				fclose(logfile);
			}
		}
		else if (choice==4) {//
			//Issue storage_get
			printf("Please enter table name:\n");
			fgets(table, sizeof(table), stdin);
			strtok(table, "\n");
			printf("Please enter key:\n");
			fgets(key, sizeof(key), stdin);
			strtok(key, "\n");

			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);

			status = storage_get(table, key, &r, conn);
			if(status != 0) {
				printf("storage_get failed. Error code: %d.\n", errno);
			}
			else {
				printf("storage_get: the value returned for key '%s' is %s\n", key, r.value);
			}
			gettimeofday(&end, NULL);
			double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfile, "						"
					"%-.3f\n", secs*1000);
			fclose(logfile);
		}
		else if (choice==5) {
			//Issue storage_query
			printf("Please enter table name:\n");
			fgets(table, sizeof(table), stdin);
			strtok(table, "\n");
			printf("Please enter predicates:\n");
			fgets(predicates, sizeof(predicates), stdin);
			strtok(predicates, "\n");
			printf("Please enter maximum number of keys:\n");
			fgets(key, sizeof(key), stdin);
			strtok(key, "\n");
	//printf("1---%s %s %s\n", table, predicates, key);
			maxnumberofkeys=atoi(key);
//printf("max number of keys is %d\n", maxnumberofkeys);
//printf("%d\n",sizeof(key));
			keys=mallocStringArray(maxnumberofkeys, sizeof(key));

			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);

			status = storage_query(table, predicates, keys, maxnumberofkeys, conn);
			if(status < 0) {
				printf("storage_query failed. Error code: %d.\n", errno);
			}
			else {
				printf("storage_query: the matching keys for the predicates are:\n");
                                 int i;
 				for(i=0; i<status&&i<maxnumberofkeys; i++){
                                      printf("%s\n", keys[i]);

 				}
			}
			gettimeofday(&end, NULL);
			double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfile, "						"
					"%-.3f\n", secs*1000);
			fclose(logfile);

			freeStringArray(keys, maxnumberofkeys);
		}
		else if (choice==6) {
			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);

			//Disconnect
			status = storage_disconnect(conn);
			printf("storage_disconnect: successful.\n");
			if(status != 0) {
				printf("storage_disconnect failed. Error code: %d.\n", errno);
				return status;
			}
			gettimeofday(&end, NULL);
			double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfile, "								"
					"%-.3f\n", secs*1000);
			fclose(logfile);
		}
		else if (choice==7) {
			// Exit
			return 0;
		}
		else
			printf("Invalid command\n");
	}
	return 0;
}

/**
 * @brief menu list for command shell
 */
void prompt()
{
	printf("--------------------\n");
	printf("1) Connect\n");
	printf("2) Authenticate\n");
	printf("3) Set\n");
	printf("4) Get\n");
	printf("5) Query\n");
	printf("6) Disconnect\n");
	printf("7) Exit\n");
	printf("--------------------\n");
}
