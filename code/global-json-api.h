// global libs inclusion

//preventing inclusion duplication 
#ifndef GLOBALS_H
#define GLOBALS_H

// C shenanigans
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// MariaDB headers
#include <mysql.h>
#include <mysql/plugin.h>

// micro httpd headers
#include <microhttpd.h>

// cJSON headers
#include <cjson/cJSON.h>

// global variables sharing
// Defining the resources we want to check
// TODO : ability to declare the list in a config file / system variable
extern const char *resources[] ;
extern const int num_resources;

// global connexion : no creds, full privileges
extern static MYSQL *global_mysql;

#define PLUGIN_NAME          "json2sql"
#define PLUGIN_AUTHOR        "Sylvain Arbaudie <arbaudie.it@gmail.com>"
#define PLUGIN_DESCRIPTION   "JSON-to-SQL API Plugin for MariaDB"

// ease the use of mysql_real_query
#define STRING_WITH_LEN(X) (X), ((size_t) (sizeof(X) - 1))

// TODO : managing port via a system variable
#define PORT 3000

// TODO : managing credentials through JWTs and request body
#define APIUSER "apiadmin"
#define APIPASSWD "Ap1-4dmiN"

#endif // GLOBALS_H
