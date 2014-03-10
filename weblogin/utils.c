/**
 * @file
 * @brief This file implements various utility functions that are
 * can be used by the storage server and client library.
 */

#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "utils.h"
#include "storage.h"

/**
 * @brief This setup calculates elapsed time between
 * start and end accurate to microseconds
 */
struct timeval start, end;
double secs;

/**
  * @brief Allocates memory for a string (char**) array.
  * @param arraysizeX Size of the array.
  * @param arraysizeY Size of each array element.
  * @return Returns the pointer to the allocated memory.
  */
char** mallocStringArray(int arraySizeX, int arraySizeY) {
         char** theArray;
         theArray = (char**) malloc(arraySizeX*sizeof(char*));
         int i;
         for (i = 0; i < arraySizeX; i++)  {
             theArray[i] = (char*) malloc(arraySizeY);
         }
         return theArray;
}

/**
  * @brief Frees memory from a string(char**) array.
  * @param theArray Pointer to the array.
  * @return Returns 0 if successful.
  */
int freeStringArray(char** theArray, int numofelements) {

           int i;
         for (i = 0; i < numofelements; i++){
             free(theArray[i]);
          }
           free(theArray);
           return 0;
}


int isNum(char* key)
{
	if(!*key)
	   return -1;
	int i;
	for(i=0; i<strlen(key); i++)
		if(!isdigit(key[i]))
			return -1;
	return 0;
}

int isBadTable(char* key)
{
	if(!*key)
	   return -1;
	int i;
	for(i=0; i<strlen(key); i++)
		if(!isalnum(key[i]))
			return -1;
	return 0;
}

int isBadKey(char* key)
{
	if(!*key)
	   return -1;
	int i;
	for(i=0; i<strlen(key); i++)
		if(!isalnum(key[i]))
			return -1;
	return 0;
}

int isBadValue(char* key)
{
	if(!*key)
	   return -1;
	int i;
	for(i=0; i<strlen(key); i++)
		if(!isalnum(key[i])&&key[i]!=' ')
			return -1;
	return 0;
}

/**
 * @brief Retrieve a column value from a value string, and writes it into the col_value string.
 * @param value Pointer to the value string of a data entry.
 * @param index The index of the column from which to retrieve.
 * @param col_value Pointer to the string to write the retrieved value into.
 */
int column_value(char* value, int index, char* col_value)
{      //printf("value: %s\nindex:%d\n", value, index);
	int commacounter=0;
	int start=0, end=0;
        int startFound=0;
	int i, u;
	int length=strlen(value);
	for(i=0; i<length; i++){
		if(value[i]==','){
			commacounter++;
		}
		if(commacounter==index&&!startFound){
			start=i+1;
                         startFound=1;
		}
		if(commacounter==index+1){
			end=i-1;
                        break;
		}
	}
	if(start<end){
		u=0;
		for(i=start; i<=end; i++){
			col_value[u]=value[i];
                         u++;
		}
		col_value[u+1]='\0';
		return 0;
	}
	else if(startFound&&start<length){
		u=0;
		for(i=start; i<length; i++){
			col_value[u]=value[i];
                         u++;
		}
		col_value[u+1]='\0';
		return 0;
	}
	return -1;
}

int sendall(const int sock, const char *buf, const size_t len)
{
	size_t tosend = len;
	while (tosend > 0) {
		ssize_t bytes = send(sock, buf, tosend, 0);
		if (bytes <= 0)
			break; // send() was not successful, so stop.
		tosend -= (size_t) bytes;
		buf += bytes;
	}
	return tosend == 0 ? 0 : -1;
}

/**
 * In order to avoid reading more than a line from the stream,
 * this function only reads one byte at a time.  This is very
 * inefficient, and you are free to optimize it or implement your
 * own function.
 */
int recvline(const int sock, char *buf, const size_t buflen)
{
	int status = 0; // Return status.
	size_t bufleft = buflen;

	while (bufleft > 1) {
		// Read one byte from scoket.
		ssize_t bytes = recv(sock, buf, 1, 0);
		if (bytes <= 0) {
			// recv() was not successful, so stop.
			status = -1;
			break;
		} else if (*buf == '\n') {
			// Found end of line, so stop.
			*buf = 0; // Replace end of line with a null terminator.
			status = 0;
			break;
		} else {
			// Keep going.
			bufleft -= 1;
			buf += 1;
		}
	}
	*buf = 0; // add null terminator in case it's not already there.

	return status;
}

/**
 * @brief Parse and process a line in the config file.
 */

//Modified by Danny to accomodate new configuration file.
int process_config_line(char *line, struct config_params *params, int *Shost, int *Sport, int *Uname, int *Pword, int *num_of_tables)
{
	// Ignore comments.
	if (line[0] == CONFIG_COMMENT_CHAR)
		return 0;

	// Extract config parameter name and value.
	char name[MAX_CONFIG_LINE_LEN];
	char value[MAX_CONFIG_LINE_LEN];
	int items = sscanf(line, "%s %s\n", name, value);

	// Line wasn't as expected.
	if (items != 2)
		return 0;

	// Process this line.
	if (strcmp(name, "server_host") == 0) {
		if ((*Shost))
			return -1;
		else {
			strncpy(params->server_host, value, sizeof params->server_host);
			(*Shost)=1;
		}
	} else if (strcmp(name, "server_port") == 0) {
		if(params->server_port && (*Sport))
          	    	return -1;
		else {
			params->server_port = atoi(value);
			(*Sport)=1;
		}
	} else if (strcmp(name, "username") == 0) {
		if ((*Uname))
			return -1;
		else {
			strncpy(params->username, value, sizeof params->username);
			(*Uname)=1;
		}
	} else if (strcmp(name, "password") == 0) {
		if ((*Pword))
			return -1;
		else {
			strncpy(params->password, value, sizeof params->password);
			(*Pword)=1;
		}
	} else if (strcmp(name, "table") == 0) { //Adds a configuration line for a table.
		if((*num_of_tables)==MAX_TABLES){

			return -1;
                }
                int i=0;
                while(params->table_name[i][0]){
                      if(!strcmp(params->table_name[i], value)){
                      //return -1;
                      }
                      i++;
                }
		    //printf("%s\n",line);
			strncpy(params->table_name[*num_of_tables], value, sizeof params->table_name[*num_of_tables]);

        		//Determines if the table's entries have additional columns.
        		//If yes, proceed to parse and store the columns and their datatypes.
			if(strlen(line)>(strlen(name)+strlen(value)+2)) {
				line=line+(strlen(name)+strlen(value)+2);
				strtok(line, "\n");
				char tempND[MAX_COLUMNS_PER_TABLE][31]={'\0'}; //Temporary strings for Name:Datatype.
				char *temp;
				char temp2[10];
				temp=strtok(line, " ,");

				i=0;
				while(i<10 && temp) {
					strncpy(tempND[i], temp, sizeof(tempND[i]));
					temp=strtok(NULL, " ,");
					i++;
				}
				i=0;

				while(i<10 && tempND[i]){
					temp=strtok(tempND[i], ":"); //Stores the name of a column.
					if(temp){
						strncpy(params->column_list[*num_of_tables][i], temp, sizeof params->column_list[*num_of_tables][i]);
					}
					temp=strtok(NULL, "\n"); //Stores the datatype of a column.
					if(temp){
						if(!strcmp(temp, "int")){
							strcpy(params->datatype_list, "int");
							params->datasize_list[*num_of_tables][i]=4;
						}
						else{
							strncpy(temp2, temp, sizeof (temp2));
							if(!strcmp(strtok(temp2,"["), "char")){
								strcpy(params->datatype_list, "char");
								temp=strtok(NULL,"\n");
								if(temp){
									params->datasize_list[*num_of_tables][i]=atoi(temp);
								}
							}
						}
					}
					//strncpy(params->data_directory[(*num_of_tables)], line, sizeof params->data_directory[(*num_of_tables)]);
					i++;
		 		}
			}
			(*num_of_tables)++;

	}
	else {
		// Ignore unknown config parameters.
	}

	return 0;
}

int census_load_helper(FILE *census_file, Table *database)
{
	int error_occurred = 0;

	// Open file for reading.
	if (census_file == NULL)
		error_occurred = 1;

	FILE *logfiles=fopen("Consensus load Time.txt", "a");

	// Process the config file.
	int i=0; //index for the census table's entries.
	while (!error_occurred && !feof(census_file)) {
		gettimeofday(&start, NULL);

		// Read a line from the file.
		char line[10000];
		char *l = fgets(line, sizeof line, census_file);

		// Process the line.
		char *lineptr;
		Table* table=database;
		if (l == line) {
                        strtok(line, "\n");
			lineptr=strtok(line, " ");
			Entry* entry=&(table->entry[i]);
			entry->key=(char*)malloc(strlen(lineptr)+1);
			strcpy(entry->key, lineptr);
			lineptr=strtok(NULL, " ");
			entry->value=(char*)malloc(11);
			strcpy(entry->value, lineptr);
			i++;

			gettimeofday(&end, NULL);
			secs = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			fprintf(logfiles, "S %-.1f\n", secs*1000000);
		}
		else if (!feof(census_file))
			error_occurred = 1;
	}
	fclose(logfiles);

	return error_occurred ? -1 : 0;
}

int read_config2(FILE *file, struct config_params *params)
{
	int error_occurred = 0;

	int *ShostV=(int *)malloc(sizeof(int));
	int *SportV=(int *)malloc(sizeof(int));
	int *UnameV=(int *)malloc(sizeof(int));
	int *PwordV=(int *)malloc(sizeof(int));
	int *num_of_tables=(int *)malloc(sizeof(int));
	(*ShostV)=0;
	(*SportV)=0;
	(*UnameV)=0;
	(*PwordV)=0;
	(*num_of_tables)=0;

	// Process the config file.

	while (!error_occurred && !feof(file)) {
		// Read a line from the file.
		char line[MAX_CONFIG_LINE_LEN];
		char *l = fgets(line, sizeof line, file);

		// Process the line.
		if (l == line){
			if(process_config_line(line, params, ShostV, SportV, UnameV, PwordV, num_of_tables)){
			 error_occurred = 1;
                         }
                }
		else if (!feof(file))
			error_occurred = 1;
	}
	free(ShostV);
	free(SportV);
	free(UnameV);
	free(PwordV);
	free(num_of_tables);
	return error_occurred ? -1 : 0;
}

int read_config(const char *config_file, struct config_params *params)
{
	int error_occurred = 0;

	int *ShostV=(int *)malloc(sizeof(int));
	int *SportV=(int *)malloc(sizeof(int));
	int *UnameV=(int *)malloc(sizeof(int));
	int *PwordV=(int *)malloc(sizeof(int));
	int *num_of_tables=(int *)malloc(sizeof(int));
	(*ShostV)=0;
	(*SportV)=0;
	(*UnameV)=0;
	(*PwordV)=0;
	(*num_of_tables)=0;

	// Open file for reading.
	FILE *file = fopen(config_file, "r");
	if (file == NULL)
		error_occurred = 1;
        params->server_port=0;
	// Process the config file.
	while (!error_occurred && !feof(file)) {
		// Read a line from the file.
		char line[MAX_CONFIG_LINE_LEN];
		char *l = fgets(line, sizeof line, file);
		//printf("Finished reading line from file\n"); //this line is for debugging

		// Process the line.
		if (l == line) {
			if(process_config_line(line, params, ShostV, SportV, UnameV, PwordV, num_of_tables)){
                           error_occurred=1;
                         }
			//printf("Finished processing line from file\n"); //this line is for debugging
		}
		else if (!feof(file)) {
			//printf("Error occurred\n"); //this line is for debugging
			error_occurred = 1;
		}
	}
	free(ShostV);
	free(SportV);
	free(UnameV);
	free(PwordV);
	free(num_of_tables);
	return error_occurred ? -1 : 0;
}

void logger(int logging, FILE *file, char *message)
{
	if (logging == 0) return;
	if (logging == 1) {
		printf(message);
	}
	if (logging == 2) {
		fprintf(file,"%s",message);
		fflush(file);
	}
}

char *generate_encrypted_password(const char *passwd, const char *salt)
{
	if(salt != NULL)
		return crypt(passwd, salt);
	else
		return crypt(passwd, DEFAULT_CRYPT_SALT);
}
