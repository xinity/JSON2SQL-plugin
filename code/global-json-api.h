// global libs inclusion

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
const char *resources[] = {
    "/v1/",
    "/v1/status/",
    "/v1/tables/",
    "/v1/procedures/"
};
const int num_resources = sizeof(resources) / sizeof(resources[0]);

// global connexion : no creds, full privileges
static MYSQL *global_mysql;
