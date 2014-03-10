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
	//printf("In main\n");
	time_t rawtime;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,40,"Client-%Y-%m-%d-%H-%M-%S.log",timeinfo);
	puts (buffer);
	//printf("Time filename set\n");
	
	int serverport;
	char serverhost[64];
	char serverusername[64];
	char serverpassword [64];
	char table[20];
	char key[20];
	char value[1024];
	void *conn;
	int status = 1;
	struct storage_record r;
	int choice;
	
	/**
	 * this setup calculates elapsed time between 
	 * start and end accurate to microseconds
	 */
	struct timeval start, end;
	double secs;
	
	/**
	 * set up a config file struct pointer which reads default.conf 
	 */
	struct config_params *configfile=malloc(sizeof(struct config_params));
	int configfileReadValid=read_config2(fopen("default.conf", "r"), configfile);
	if (configfileReadValid)
	printf("Invalid configuration file or configuration file error\n"); 
	
	while (choice !=6) {
		
		prompt();
		
		//ask user for choice
		printf("please enter your selection: ");
		scanf("%d", &choice);
		
		if (choice == 1) {
			//Connect to server
			/*printf("Please enter host name:\n");
			scanf("%s", &serverhost);
			printf("Please enter port:\n");
			scanf("%i", &serverport);*/
			
			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);
			
			conn = storage_connect(configfile->server_host, configfile->server_port);
			if(!conn) {
				printf("Cannot connect to server @ %s:%d. Error code: %d.\n",
						configfile->server_host, configfile->server_port, errno);
				return -1;
			}
			printf("Connected to server.\n");
			
			gettimeofday(&end, NULL);
			secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfile, "%-.3f\n", secs*1000);
			fclose(logfile);
		}
		else if (choice == 2) {
			//Authenticate the client.
			printf("Please enter username:\n");
			scanf("%s", serverusername);
			printf("Please enter password:\n");
			scanf("%s", serverpassword);
			
			FILE *logfile=fopen("Time Log.txt", "a");
			gettimeofday(&start, NULL);
			
			status = storage_auth(serverusername, serverpassword, conn);
			if(status != 0) {
				printf("storage_auth failed with username '%s' and password '%s'. " \
					   "Error code: %d.\n", serverusername, serverpassword, errno);
				storage_disconnect(conn);
				return status;
				}
				printf("storage_auth: successful.\n");
				
				gettimeofday(&end, NULL);
				secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
				fprintf(logfile, "		%-.3f\n", secs*1000);
				fclose(logfile);
			}
			else if (choice == 3) {	
				// Issue storage_set
				printf("Please enter table name:\n");
				scanf("%s", &table);
				printf("Please enter key:\n");
				scanf("%s", &key);
			    printf("Please enter value:\n");
				scanf("%s", &value);
				
				FILE *logfile=fopen("Time Log.txt", "a");
				gettimeofday(&start, NULL);
				
				strncpy(r.value, value, sizeof r.value);
				status = storage_set(table, key, &r, conn);
				if(status != 0) {
					printf("storage_set failed. Error code: %d.\n", errno);
					storage_disconnect(conn);
					return status;
				}
				printf("storage_set: successful.\n");
				
				gettimeofday(&end, NULL);
				double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
				printf("%.3f\n", secs*1000);
				fprintf(logfile, "				%-.3f\n", secs*1000);
				fclose(logfile);
			}
			else if (choice == 4) {
				//Issue storage_get
				printf("Please enter table name:\n");
				scanf("%s", &table);
				printf("Please enter key:\n");
				scanf("%s", &key);
				
				FILE *logfile=fopen("Time Log.txt", "a");
				gettimeofday(&start, NULL);
				
				status = storage_get(table, key, &r, conn);
				if(status != 0) {
					printf("storage_get failed. Error code: %d.\n", errno);
					storage_disconnect(conn);
					return status;
				}
				printf("storage_get: the value returned for key '%s' is %s\n", key, r.value);
				
				gettimeofday(&end, NULL);
				double secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
				printf("%.3f\n", secs*1000);
				fprintf(logfile, "						"
						"%-.3f\n", secs*1000);
				fclose(logfile);
			}
			else if (choice == 5) {
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
				printf("%.3f\n", secs*1000);
				fprintf(logfile, "								"
						"%-.3f\n", secs*1000);
				fclose(logfile);
			}
			else if (choice == 6) {
				// Exit
				return 0;
			}
			else {
				printf("Invalid command\n");
				fflush(stdin);
			}
	}
	return 0;
}

/**
 * menu list for command shell
 */
void prompt() 
{
	printf("--------------------\n");
	printf("1) Connect\n");
	printf("2) Authenticate\n");
	printf("3) Set\n");
	printf("4) Get\n");
	printf("5) Disconnect\n");
	printf("6) Exit\n");
	printf("--------------------\n");
}
