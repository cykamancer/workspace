/**
 * @file
 * @brief This file declares various utility functions that are
 * can be used by the storage server and client library.
 */

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Now includes errno.h and errno
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "storage.h"

#define MAX_LISTENQUEUELEN 20	//< The maximum number of queued connections.
#define MAX_DATABASE_LEN 1000   //< The maxinum number of tables, and entries in each table.

/**
 * @brief Any lines in the config file that start with this character
 * are treated as comments.
 */
static const char CONFIG_COMMENT_CHAR = '#';

/**
 * @brief The max length in bytes of a command from the client to the server.
 */
#define MAX_CMD_LEN (1024 * 8)

/**
 * @brief A macro to log some information.
 * Use it like this:  LOG(("Hello %s", "world\n"))
 * Don't forget the double parentheses, or you'll get weird errors!
 */
#define LOG(x)  {printf x; fflush(stdout);}

/**
 * @brief A macro to output debug information.
 * It is only enabled in debug builds.
 */
#ifdef NDEBUG
#define DBG(x)  {}
#else
#define DBG(x)  {printf x; fflush(stdout);}
#endif

/**
 * @brief A struct to store config parameters.
 */
struct config_params {
	// The hostname of the server.
	char server_host[MAX_HOST_LEN];

	// The listening port of the server.
	int server_port;

	// The storage server's username
	char username[MAX_USERNAME_LEN];

	// The storage server's encrypted password
	char password[MAX_ENC_PASSWORD_LEN];

	//Changed by Danny to load table name and formats

	// The name of a table.
	char table_name[MAX_TABLES][MAX_TABLE_LEN];

	// The column format of a table.
	char column_list[MAX_TABLES][MAX_COLUMNS_PER_TABLE][MAX_COLNAME_LEN];

	/// The datatypes of each column of a table, currently used: "char" and "int".
	char datatype_list[MAX_TABLES][MAX_COLUMNS_PER_TABLE][5];

	/// The datasizes of each column of a table, listed in byte size.
	int datasize_list[MAX_TABLES][MAX_COLUMNS_PER_TABLE];


};

/**
 * @brief Entries to store data.
 * @param key string key of the entry.
 * @param value string value of the entry.
 */
struct Entry {
	char *key;
  	char *value;

};

typedef struct Entry Entry;

/**
 * @brief A Table to store arrays of entries.
 * @param name Name of table.
 * @param entry Array of table's entries.
 * @param format column format of the table's entries.
 */
struct Table{
	char *name;
  	Entry entry[MAX_DATABASE_LEN];
        /// The name of each column of a table. Array size: MAX_COLUMNS_PER_TABLE*MAX_COLNAME_LEN.
  	char **columns;
	/// The datatype of each column of a table, currently used: "char" and "int". Array size: MAX_COLUMNS_PER_TABLE*5.
	char **datatypes;
	/// The datasize of each column of a table, listed in byte size. Array size: MAX_COLUMNS_PER_TABLE.
	int *datasizes;
};

typedef struct Table Table;

/**
 * @brief Exit the program because a fatal error occured.e
 *
 * @param msg The error message to print.
 * @param code The program exit return value.
 */
static inline void die(char *msg, int code)
{
	printf("%s\n", msg);
	exit(code);
}


/**
  * @brief Allocates memory for a string (char**) array.
  * @param arraysizeX Size of the array.
  * @param arraysizeY Size of each array element.
  * @return Returns the pointer to the allocated memory.
  */
char** mallocStringArray(int arraySizeX, int arraySizeY);

/**
  * @brief Frees memory from a string(char**) array.
  * @param theArray Pointer to the array.
  * @param numofelements Size of the array.
  * @return Returns 0 if successful.
  */
int freeStringArray(char** theArray, int numofelements);

/**
 * @brief Checks if string is a number.
 * @param key String to check.
 * @return Return 0 if valid, -1 otherwise.
 */
int isNum(char* key);

/**
 * @brief Checks if name of table is valid.
 * @param key String to check.
 * @return Return 0 if valid, -1 otherwise.
 */
int isBadTable(char* key);

/**
 * @brief Checks if name of key is valid.
 * @param key String to check.
 * @return Return 0 if valid, -1 otherwise.
 */
int isBadKey(char* key);

/**
 * @brief Checks if name of value is valid.
 * @param key String to check.
 * @return Return 0 if valid, -1 otherwise.
 */
int isBadValue(char* key);

/**
 * @brief Retrieve a column value from a value string, and writes it into the col_value string.
 * @param value Pointer to the value string of a data entry.
 * @param index The index of the column from which to retrieve.
 * @param col_value Pointer to the string to write the retrieved value into.
 */
int column_value(char* value, int index, char* col_value);


/**
 * @brief Keep sending the contents of the buffer until complete.
 * @return Return 0 on success, -1 otherwise.
 *
 * The parameters mimic the send() function.
 */
int sendall(const int sock, const char *buf, const size_t len);

/**
 * @brief Receive an entire line from a socket.
 * @return Return 0 on success, -1 otherwise.e
 */
int recvline(const int sock, char *buf, const size_t buflen);

/**
 * @brief The following function is a different version of read_config used in client.c.
 * It takes in a file pointer instead of the config file itself.
 */
int read_config2(FILE *file, struct config_params *params);

/**
 * @brief Read and load configuration parameters.
 *
 * @param config_file The name of the configuration file.
 * @param params The structure where config parameters are loaded.
 * @return Return 0 on success, -1 otherwise.
 */
int read_config(const char *config_file, struct config_params *params);

/**
 * @brief Generates a log message.
 *
 * @param file The output stream
 * @param message Message to log.
 */
void logger(int logging, FILE *file, char *message);

/**
 * @brief Default two character salt used for password encryption.
 */
#define DEFAULT_CRYPT_SALT "xx"

/**
 * @brief Generates an encrypted password string using salt CRYPT_SALT.
 *
 * @param passwd Password before encryption.
 * @param salt Salt used to encrypt the password. If NULL default value
 * DEFAULT_CRYPT_SALT is used.
 * @return Returns encrypted password.
 */
char *generate_encrypted_password(const char *passwd, const char *salt);

#endif
