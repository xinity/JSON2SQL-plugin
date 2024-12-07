// global libs inclusion

//preventing inclusion duplication 
#ifndef GLOBALS_H
#define GLOBALS_H

// C shenanigans
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// MariaDB headers
#include <mysql.h>
#include <mysql/plugin.h>

// cJSON headers
#include <cjson/cJSON.h>

// global variables sharing
// Defining the resources we want to check
// TODO : ability to declare the list in a config file / system variable
extern const char *resources[] ;
extern const int num_resources;

// grant verification
// extern int has_required_privilege(const char *user);

// check for resource exposition
extern int is_exposed_resource(const char *url);

#define PLUGIN_NAME          "json2sql"
#define PLUGIN_AUTHOR        "Sylvain Arbaudie <arbaudie.it@gmail.com>"
#define PLUGIN_DESCRIPTION   "simple JSON-to-SQL API Plugin for MariaDB"

// are functions corked or not
#define GETCORK     1
#define PUTCORK     1
#define POSTCORK    1
#define PATCHCORK   1
#define DELETECORK  1
#define HANDLERCORK 1
#define METHODCORK  1

// defining use HTTP response codes
#define HTTP_OK                     200
#define HTTP_INTERNAL_SERVER_ERROR  500
#define HTTP_BAD_REQUEST            400
#define HTTP_UNAUTHORIZED           401
#define HTTP_FORBIDDEN              403
#define HTTP_NOT_FOUND              404
#define HTTP_METHOD_NOT_ALLOWED     405
#define HTTP_UNSUPPORTED_MEDIA_TYPE 415

// methodd <=> privilege translation
#define GET    'select'
#define PUT    'execute'
#define POST   'insert'
#define PATCH  'update'
#define DELETE 'delete'

// standard exposed resources
#define HEALTHCHECK  ''/v1/'
#define STATUS        '/v1/status/'
#define SUBSCRIPTION  '/v1/subscription/'
#define RESOURCES     '/v1/resources/'

// ease the use of mysql_real_query
#define STRING_WITH_LEN(X) (X), ((size_t) (sizeof(X) - 1))

// TODO : managing port via a system variable
#define PORT 3000
#define ADDRESS "0.0.0.0"

// TODO : managing credentials through JWTs and request body
#define APIUSER 'apiadmin'

#endif // GLOBALS_H
