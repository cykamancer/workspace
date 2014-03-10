/**
 * @file
 * @brief This file contains the implementation of the storage server
 * interface as specified in storage.h.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "storage.h"
#include "utils.h"
#include "logconst.h"
#include "time.h"

/**
 * @brief storage_connect has been modified for error handling.
 * Depending on the type of error, it will return ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, or ERR_UNKNOWN.
 * It will also return -1 if the function itself fails.
 */


void* storage_connect(const char *hostname, const int port)
{
	// Create a socket.
	int sock = socket(PF_INET, SOCK_STREAM, 0);

	timevar = sprintf(message, "Creating socket\n");
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: reports the socket number
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);

	// Get info about the server.
	struct addrinfo serveraddr, *res;
	memset(&serveraddr, 0, sizeof serveraddr);
	serveraddr.ai_family = AF_UNSPEC;
	serveraddr.ai_socktype = SOCK_STREAM;
	char portstr[MAX_PORT_LEN];
	snprintf(portstr, sizeof portstr, "%d", port);
	int status = getaddrinfo(hostname, portstr, &serveraddr, &res);
	if (status != 0) {
		errno = ERR_CONNECTION_FAIL;
		return NULL;
	}

	// Connect to the server.
	status = connect(sock, res->ai_addr, res->ai_addrlen);
	if (status != 0) {
		errno = ERR_CONNECTION_FAIL;
		return NULL;
	}
	return (void*) sock;
}

/**
 * @brief storage_auth has been modified to check for ERR_AUTHENTICATION_FAILED errors.
 */
int storage_auth(const char *username, const char *passwd, void *conn)
{
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// Send some data.
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);
	char *encrypted_passwd = generate_encrypted_password(passwd, NULL);
	snprintf(buf, sizeof buf, "AUTH,%s,%s\n", username, encrypted_passwd);
	//printf("%s\n",encrypted_passwd);
	timevar = sprintf(message, "In storage_auth with username %s and password %s\n", username, passwd);
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: confirms entry into storage_auth method before returning
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
        	int i=atoi(buf);
                if(i!=0){
                errno = ERR_AUTHENTICATION_FAILED;
		return i;}
                else return 0;
	}
	errno = ERR_AUTHENTICATION_FAILED;
	return -1;
}

/**
 * @brief storage_get has been modified to work with handle_command, and also now handles the following errors: ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, ERR_TABLE_NOT_FOUND,
 * ERR_KEY_NOT_FOUND, ERR_NOT_AUTHENTICATED, ERR_UNKNOWN. Some of this error checking is in handle_command withing server.c.
 */
int storage_get(const char *table, const char *key, struct storage_record *record, void *conn)
{
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;
	// Send some data.
	char buf[MAX_CMD_LEN];
        char buf2[4];
        char value[MAX_CMD_LEN];
	//Check if buf points to NULL
	if (buf == NULL) {
		errno = ERR_UNKNOWN;
		return -1;
	}
        if(isBadTable(table)||isBadKey(key)) {
               errno=ERR_INVALID_PARAM;
               return -1;
        }
	memset(buf, 0, sizeof buf);
	memset(buf2, 0, sizeof buf2);
	snprintf(buf, sizeof buf, "GET,%s,%s\n", table, key);
	timevar = sprintf(message, "In storage_get looking for table %s with key %s\n", table, key);
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: confirms entry into storage_get method before returnin
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
                    sscanf(buf, "%s %s", value, buf2);
                 if(atoi(buf2)){
                    errno=atoi(buf2);
                    return -1;
                 }
		strncpy(record->value, value, sizeof record->value);
		return 0;
	}
	//At this point, the function has failed for unknown reasons.
	errno = ERR_UNKNOWN;
	return -1;
}

/**
 * @brief storage_set deals with many of the same errors as storage_get, with some of the error checking within handle_command.
 */
int storage_set(const char *table, const char *key, struct storage_record *record, void *conn)
{
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// Send some data.
	char buf[MAX_CMD_LEN]="\0";

	//Check if buf points to NULL
	if (buf == NULL) {
		errno = ERR_UNKNOWN;
		return -1;
	}
	memset(buf, 0, sizeof buf);
        if(isBadTable(table)||isBadKey(key)) {
               errno=ERR_INVALID_PARAM;
               return -1;
        }
        	memset(buf, 0, sizeof buf);
    		snprintf(buf, sizeof buf, "SET,%s,%s,%s\n", table, key, record->value);
	timevar = sprintf(message, "In storage_set looking for table %s with key  %s\n", table, key);
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: confirms entry into storage_set method before returning
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);
//printf("%s\n", buf);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
                if(atoi(buf)){
                  errno=atoi(buf);
                  return -1;
                }
		return 0;
	}
	//At this point, the function has failed for unknown reasons.
	errno = ERR_UNKNOWN;
	return -1;
}

int storage_query(const char *table, const char *predicates, char **keys, const int max_keys, void *conn)
{
	// Connection is really just a socket file descriptor.
	int sock = (int)conn;

	// Send some data.
	char buf[MAX_CMD_LEN];
	char buf2[4];
	char return_value[4];
	//Check if buf points to NULL
	if (buf == NULL) {
		errno = ERR_UNKNOWN;
		return -1;
	}
	memset(buf, 0, sizeof buf);
	memset(buf2, 0, sizeof buf2);
        if(isBadTable(table)) {
               errno=ERR_INVALID_PARAM;
               return -1;
        }
        	memset(buf, 0, sizeof buf);
    		snprintf(buf, sizeof buf, "QUERY,%s,%s,%d\n", table, predicates, max_keys);

	timevar = sprintf(message, "In storage_query searching with predicates %s in table %s, with max. %d keys to return", predicates, table, max_keys);
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: confirms entry into storage_set method before returning
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {

	    strncpy(return_value, strtok(buf, " "), sizeof(return_value));

     if(atoi(return_value)){
        errno=atoi(return_value);
        return -1;
        }
	    strncpy(buf2, strtok(NULL, " "), sizeof(buf2));

         int u=0;
         char *temp=strtok(NULL, ",");
         while(temp){
        strncpy(keys[u], temp, MAX_KEY_LEN); //Cannot detect the size of a dynamically allocated string, therefore just use MAX_KEY_LEN (which is the size of the malloc'd space).
         temp=strtok(NULL, ",");
         u++;
         }
         return atoi(buf2);
	}
	//At this point, the function has failed for unknown reasons.
	errno = ERR_UNKNOWN;
	return -1;
}

/**
 * @brief storage_disconnect has been modified for error handling.
 * Depending on the type of error, it will return ERR_INVALID_PARAM, ERR_UNKNOWN or ERR_CONNECTION_FAIL.
 * It will also return -1 if the function itself fails.
 */
int storage_disconnect(void *conn)
{
	/**
	 * conn == void indicates invalid parameter. Appropriate error checking has been added
	 */
	if (conn == NULL) {
		errno = ERR_INVALID_PARAM;
		return -1;
	}
	// Cleanup
	int sock = (int)conn;

	int closed = close(sock);
	if (closed == -1) {
		errno = ERR_UNKNOWN;
		return -1;
	}
	timevar = sprintf(message, "In storage_disconnect with socket %i\n", sock);
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fp = fopen(buffer, "a+");
	logger(LOGGING, fp, message); // added this logger() call: confirms entry into storage_disconnect method before returning
	/**
	 * @brief In order to avoid segfaults, fopen and fclose will only operate if LOGGING == 2,
	 * which means that there is an actual output stream to close.
	 */
	if (LOGGING == 2)
		fclose(fp);

	return 0;
}
