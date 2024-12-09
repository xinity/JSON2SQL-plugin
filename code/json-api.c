#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

// MariaDB headers
#ifndef MYSQL_DYNAMIC_PLUGIN
#define MYSQL_DYNAMIC_PLUGIN
#endif

#include <mysql/plugin.h>

// micro httpd headers
#include <microhttpd.h>

// cJSON headers
#include <cjson/cJSON.h>

// #define MYSQL_DAEMON_PLUGIN  3  /* The daemon/raw plugin type */
#define PLUGIN_NAME          "json2sql"
#define PLUGIN_AUTHOR        "Sylvain Arbaudie <arbaudie.it@gmail.com>"
#define PLUGIN_DESCRIPTION   "JSON-to-SQL API Plugin for MariaDB"
// #define PLUGIN_LICENSE_GPL   "GNU AGPL v3"

// TODO : managing port via a system variable
#define PORT 3000
#define ADDRESS "0.0.0.0"

// are functions corked or not
#define GETCORK     1
#define PUTCORK     1
#define POSTCORK    1
#define PATCHCORK   1
#define DELETECORK  1
#define HANDLERCORK 1
#define HANDLERCORK  1

// defining use HTTP response codes
#define HTTP_OK                     200
#define HTTP_INTERNAL_SERVER_ERROR  500
#define HTTP_BAD_REQUEST            400
#define HTTP_UNAUTHORIZED           401
#define HTTP_FORBIDDEN              403
#define HTTP_NOT_FOUND              404
#define HTTP_METHOD_NOT_ALLOWED     405
#define HTTP_UNSUPPORTED_MEDIA_TYPE 415

struct MHD_Daemon;

// TODO : managing credentials through JWTs and request body
#define APIUSER "apiadmin"
#define APIPASSWD "Ap1-4dmiN"

// ease the use of mysql_real_query
#define STRING_WITH_LEN(X) (X), ((size_t) (sizeof(X) - 1))

// Plugin declaration structure
static struct st_mysql_daemon json_api_plugin = {
    MYSQL_DAEMON_INTERFACE_VERSION
};

// Defining the resources we want to check
// TODO : ability to declare the list in a config file / system variable
const char *resources[] = {
    "/v1/",
    "/v1/tables/",
    "/v1/procedures/"
};
const int num_resources = sizeof(resources) / sizeof(resources[0]);

// defining the daemon structure
static struct MHD_Daemon *listener = NULL;

// Function to check if the requested URL is a valid resource
int is_valid_resource(const char *url) {
    for (int i = 0; i < num_resources; i++) {
        if (strcmp(url, resources[i]) == 0) {
            return 1; // Valid resource found
        }
    }
    return 0; // No valid resource found
}

// TODO : stick to the HTTP API return codes best practices
// 200 : MHD_HTTP_OK - OK
// 400 : MHD_BAD_REQUEST - bad request - malformed request
// 405 : MHD_METHOD_NOT_ALLOWED - method not allowed - method does not exist for ressource
static int send_json_response(struct MHD_Connection *connection, const char *json_string) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json_string), (void *)json_string, MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

static char* handle_get_request(const char *url) {
    char schema[64];  
    char table[64];
    char colname[64];
    char colvalue[64];
    char procname[64];
    char query[512]="";
    size_t query_len;

    // initialize the JSON answer
    cJSON *json = cJSON_CreateObject();
    
    if (sscanf(url, "/v1/tables/%64[^/]/%64[^/]/%64[^/]/%64s", schema, table, colname, colvalue) == 4) {   
        // Here query database
        query_len = snprintf(query, sizeof(query), "SELECT * FROM %s.%s WHERE %s = '%s'", schema, table, colname, colvalue);
        cJSON_AddStringToObject(json, "source", "tables");
        cJSON_AddStringToObject(json, "schema", schema);
        cJSON_AddStringToObject(json, "table", table);
        cJSON_AddStringToObject(json, "column-name", colname);
        cJSON_AddStringToObject(json, "column-value", colvalue);
//    } else if (sscanf(url, "/v1/procedures/%64[^/]/%64s", schema, procname) == 2) { 
//        // Here query database
//        snprintf(query, sizeof(query), "'use %s; call %s()'", schema, procname);
//        // initialize the JSON answer
//        cJSON *json = cJSON_CreateObject();
//        cJSON_AddStringToObject(json, "source", "procedure");
//        cJSON_AddStringToObject(json, "schema", schema);
//        cJSON_AddStringToObject(json, "procname", procname);
    }

    if (query[0] != '\0') {
        MYSQL *conn = mysql_init(NULL);
        if (conn == NULL) {
           fprintf(stderr, "mysql_init() failed\n");
           cJSON_AddStringToObject(json, "status", "INIT failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json_string; // Caller is responsible for freeing this memory
        }
        
        if (mysql_real_connect(conn, "localhost", APIUSER, APIPASSWD, schema, 0, NULL, 0) == NULL) {
           fprintf(stderr, "mysql_real_connect() failed\n");
           mysql_close(conn);
           cJSON_AddStringToObject(json, "status", "CONNECT failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json_string; // Caller is responsible for freeing this memory
        }
      
        if (mysql_real_query(conn, query, query_len)) {
           fprintf(stderr, "mysql_real_query failed\n");
           mysql_close(conn);
           cJSON_AddStringToObject(json, "status", "QUERY failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json_string; // Caller is responsible for freeing this memory
        }
        
       MYSQL_RES *result = mysql_store_result(conn);
       if (result == NULL) {
          fprintf(stderr, "mysql_store_result() failed\n");
          mysql_close(conn);
          cJSON_AddStringToObject(json, "status", "RESULT failed");
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
       }

       unsigned long long num_rows = mysql_num_rows(result);

        if (num_rows > 0) {
           cJSON_AddStringToObject(json, "status", "OK");
           cJSON_AddNumberToObject(json, "rows", (double)num_rows);
// Create a JSON array to hold all rows
           cJSON *rows_array = cJSON_CreateArray();
           unsigned int num_fields = mysql_num_fields(result);
//           MYSQL_FIELD *fields = mysql_fetch_fields(result);
           MYSQL_FIELD *field; 
           MYSQL_ROW row;
           while ((row = mysql_fetch_row(result))) {    
// Create a JSON object for each row
                 cJSON *row_object = cJSON_CreateObject();
// handling of rows in the following format : "columnname":"columnvalue"
                for (unsigned int i = 0; i < num_fields; i++) {
                     field = mysql_fetch_field(result); // Get field info
                     cJSON_AddStringToObject(row_object, field->name, row[i] ? row[i] : "NULL");
                 }
// Add the row object to the array
                cJSON_AddItemToArray(rows_array, row_object);
           }
           cJSON_AddItemToObject(json, "data", rows_array);
        } else {
           cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
           cJSON_AddNumberToObject(json, "rows", 0);
        }

    }
      // request format is KO
      return NULL;
}

static char* handle_post_request(const char *url) {

  // request format is KO  
  return NULL;
}

static char* handle_patch_request(const char *url) {

  // request format is KO  
  return NULL;
}

// v1 : "point delete" only
static char* handle_delete_request(const char *url) {

    // request format is KO
    return NULL;
}

static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls) {

#if HANDLERCORK == 1
// initialize the JSON answer
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    int ret = send_json_response(connection, response);
    free(response); // Free the allocated JSON string
    return ret;
#else

    if (strcmp(method, "GET") == 0) {
#if GETCORK == 1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    int ret = send_json_response(connection, response);
    free(response); // Free the allocated JSON string
    return ret;
#else    
    char *response = handle_get_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"error\": \"Invalid GET request\"}");
    }
#endif
    } else if (strcmp(method, "POST") == 0) {
#if POSTCORK == 1
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "status", "CORK");
        cJSON_AddStringToObject(json, "method", method);
        cJSON_AddStringToObject(json, "url", url);
        cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
        char *response = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
#else       
    char *response = handle_post_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"error\": \"Invalid POST request\"}");
    }
#endif
    } else if (strcmp(method, "PATCH") == 0) {
#if PATCHCORK == 1
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "status", "CORK");
        cJSON_AddStringToObject(json, "method", method);
        cJSON_AddStringToObject(json, "url", url);
        cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
        char *response = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
#else      
    char *response = handle_patch_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"error\": \"Invalid PUT request\"}");
    }
#endif
    } else if (strcmp(method, "DELETE") == 0) {
#if DELETECORK == 1
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "status", "CORK");
        cJSON_AddStringToObject(json, "method", method);
        cJSON_AddStringToObject(json, "url", url);
        cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
        char *response = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
#else
    char *response = handle_delete_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"error\": \"Invalid DELETE request\"}");
    }
#endif
    } else {
        // Method not supported
        return send_json_response(connection, "{\"error\": \"Unsupported HTTP method\"}");
    }
#endif 
}

static int json_api_plugin_init(void *p) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &(addr.sin_addr));
    
    listener = MHD_start_daemon(MHD_USE_POLL | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG,
                              PORT, NULL, NULL,
                              &request_handler, NULL, MHD_OPTION_SOCK_ADDR, &addr,
                              MHD_OPTION_END);

    if (listener == NULL) {
       fprintf(stderr, "Failed to start server\n");
       return 1;
    }
    printf("Server running on port %d\n", PORT);
    return 0;
}

static int json_api_plugin_deinit(void *p) {
    if (listener != NULL) {
        MHD_stop_daemon(listener);
        listener = NULL;
        printf("HTTP server stopped.\n");
    }
    return 0;
}

// Plugin descriptor
mysql_declare_plugin(json_api)
{
    MYSQL_DAEMON_PLUGIN,      /* the plugin type (a MYSQL_XXX_PLUGIN value)     */
    &json_api_plugin,         /* pointer to type-specific plugin descriptor     */
    PLUGIN_NAME,              /* plugin name                                    */
    PLUGIN_AUTHOR,            /* plugin author (for I_S.PLUGINS)                */
    PLUGIN_DESCRIPTION,       /* general descriptive text (for I_S.PLUGINS)     */
    PLUGIN_LICENSE_GPL,       /* the plugin license (PLUGIN_LICENSE_XXX)        */
    json_api_plugin_init,     /* the function to invoke when plugin is loaded   */
    json_api_plugin_deinit,   /* the function to invoke when plugin is unloaded */
    0x0100,                   /* plugin version (for I_S.PLUGINS)               */
    NULL,                     /* for status vars                                */
    NULL,                     /* for system vars                                */
    NULL,                     /* reserved for dependency checking               */
    0                         /* flags for plugin                               */
}
mysql_declare_plugin_end;
