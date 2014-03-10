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
//#define LOGGING 2

char message2[100];
int i; //for sprintf

//time_t tm = mktime(time(0)); //creates global time struct created at program execution

/**
 * @brief storage_connect has been modified for error handling.
 * Depending on the type of error, it will return ERR_INVALID_PARAM, ERR_CONNECTION_FAIL, or ERR_UNKNOWN.
 * It will also return -1 if the function itself fails.
 */
void* storage_connect(const char *hostname, const int port)
{
	// Create a socket.
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		errno = ERR_UNKNOWN;
		return NULL;
	}
	i = sprintf(message2, "Creating socket %i\n", sock);
	fq = fopen(buffer, "a+");
	if (fq == NULL) {
		errno = ERR_UNKNOWN;
		return NULL;
	}
	logger(LOGGING, fq, message2); // added this logger() call: reports the socket number
	int closed = fclose(fq);
	if (closed == -1) {
		errno = ERR_UNKNOWN;
		return NULL;
	}

	// Get info about the server.
	struct addrinfo serveraddr, *res;
	memset(&serveraddr, 0, sizeof serveraddr);
	serveraddr.ai_family = AF_UNSPEC;
	serveraddr.ai_socktype = SOCK_STREAM;
	char portstr[MAX_PORT_LEN];
	snprintf(portstr, sizeof portstr, "%d", port);
	int status = getaddrinfo(hostname, portstr, &serveraddr, &res);
	if (status != 0) {
		errno = ERR_INVALID_PARAM;
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
	if (sock < 0){
		errno = ERR_UNKNOWN;
		return -1;
	}

	// Send some data.
	char buf[MAX_CMD_LEN];
	memset(buf, 0, sizeof buf);
	char *encrypted_passwd = generate_encrypted_password(passwd, NULL);
	snprintf(buf, sizeof buf, "AUTH %s %s\n", username, encrypted_passwd);
	i = sprintf(message2, "In storage_auth with username %s and password %s\n", username, passwd);
	fq = fopen(buffer, "a+");
	logger(LOGGING, fq, message2); // added this logger() call: confirms entry into storage_auth method before returning
	fclose(fq);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
		return 0;
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
	if (sock < 0){
		errno = ERR_UNKNOWN;
		return -1;
	}
	// Send some data.
	char buf[MAX_CMD_LEN];
	//Check if buf points to NULL
	if (buf == NULL){
		errno = ERR_UNKNOWN;
		return -1;
	}
	memset(buf, 0, sizeof buf);
	snprintf(buf, sizeof buf, "GET %s %s\n", table, key);
	i = sprintf(message2, "In storage_get looking for table %s with key %s\n", table, key);
	fq = fopen(buffer, "a+");
	logger(LOGGING, fq, message2); // added this logger() call: confirms entry into storage_get method before returning
	fclose(fq);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
		strncpy(record->value, buf, sizeof record->value);
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
	if (sock < 0){
		errno = ERR_UNKNOWN;
		return -1;
	}

	// Send some data.
	char buf[MAX_CMD_LEN];
	//Check if buf points to NULL
	if (buf == NULL){
		errno = ERR_UNKNOWN;
		return -1;
	}
	memset(buf, 0, sizeof buf);
	snprintf(buf, sizeof buf, "SET %s %s %s\n", table, key, record->value);
	i = sprintf(message2, "In storage_set looking for table %s with key  %s\n", table, key);
	fq = fopen(buffer, "a+");
	logger(LOGGING, fq, message2); // added this logger() call: confirms entry into storage_set method before returning
	fclose(fq);
	if (sendall(sock, buf, strlen(buf)) == 0 && recvline(sock, buf, sizeof buf) == 0) {
		return 0;
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
	/**
	 * sock == 0 indicates a failed connection. Appropriate error checking has been added
	 */
	if (sock < 0){
		errno = ERR_CONNECTION_FAIL;
		return -1;
	}
	int closed = close(sock);
	if (closed == -1) {
		errno = ERR_UNKNOWN;
		return -1;		
	}
	i = sprintf(message2, "In storage_disconnect with socket %i\n", sock);
	fq = fopen(buffer, "a+");
	logger(LOGGING, fq, message2); // added this logger() call: confirms entry into storage_disconnect method before returning
	fclose(fq);

	return 0;
}
