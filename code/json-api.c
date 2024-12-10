// global headers for the project 
#include "common.h"
#include <arpa/inet.h>

// MariaDB headers
#ifndef MYSQL_DYNAMIC_PLUGIN
#define MYSQL_DYNAMIC_PLUGIN
#endif

#include <mysql/plugin.h>

// micro httpd headers
#include <microhttpd.h>

// request handlers headers
// #include "handle_get_request.h"
// #include "handle_post_request.h"
// #include "handle_put_request.h"
// #include "handle_patch_request.h"
// #include "handle_delete_request.h"

#define PLUGIN_NAME          "json2sql"
#define PLUGIN_AUTHOR        "Sylvain Arbaudie <arbaudie.it@gmail.com>"
#define PLUGIN_DESCRIPTION   "simple JSON-to-SQL API Plugin for MariaDB"

// TODO : managing port via a system variable
#define PORT 3000
#define ADDRESS "0.0.0.0"

// are functions corked or not
// #define GETCORK     1
// #define PUTCORK     1
// #define POSTCORK    1
// #define PATCHCORK   1
// #define DELETECORK  1
// #define HANDLERCORK 1

// defining use HTTP response codes
// #define HTTP_OK                     200
// #define HTTP_INTERNAL_SERVER_ERROR  500
// #define HTTP_BAD_REQUEST            400
// #define HTTP_UNAUTHORIZED           401
// #define HTTP_FORBIDDEN              403
// #define HTTP_NOT_FOUND              404
// #define HTTP_METHOD_NOT_ALLOWED     405
// #define HTTP_UNSUPPORTED_MEDIA_TYPE 415

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

// defining the daemon structure
static struct MHD_Daemon *listener = NULL;

// Function to check if the requested URL is a valid resource
// resource format : 
// /v1/
// /v1/registration/
// /v1/subscription/
// /v1/status/
// /v1/tables/%
// /v1/tables/SCHEMA/%
// /v1/tables/SCHEMA/TABLE/
// /v1/procedures/%
// /v1/procedures/PROCNAME/
// int is_exposed_resource(const char *url) {
//    for (int i = 0; i < num_resources; i++) {
//        if (strcmp(url, resources[i]) == 0) {
//            return 1; // corresponding exposed resource found
//        }
//    }
//    return 0; // No exposed resource found
// }

// sends back answer to client
static int send_json_response(struct MHD_Connection *connection, const char *json_string) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json_string), (void *)json_string, MHD_RESPMEM_MUST_COPY);
// extract http return code from json_string
    cJSON *json = cJSON_Parse(json_string);
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, "httpcode");
    unsigned int http_code = field->valueint; 
    cJSON_Delete(json);
    if (httpcode == 405) {
// mandatory allow header for HTTP 405
        MHD_add_response_header(response, MHD_HTTP_HEADER_ALLOW, "GET, POST, PUT, PATCH, DELETE");
    }
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, http_code, response);
    MHD_destroy_response(response);
    return ret;
}


static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls) {

#if HANDLERCORK == 1
// initialize the JSON answer
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer), "method:%s, url:%s",method,url);
    printf("%s\n", buffer);
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
     if (strcmp(method, "POST") == 0) {
#if GETCORK ==1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);      
#else
      char *response = handle_get_request(url);
#endif // GETCORK
    } else if (strcmp(method, "POST") == 0) {
// INSERT
#if POSTCORK == 1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
#else
      char *response = handle_post_request(url, upload_data, upload_data_size);
#endif //POSTCORK
    } else if (strcmp(method, "PATCH") == 0) {
// UPDATE
#if PATCHCORK == 1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
#else
      char *response = handle_patch_request(url, upload_data, upload_data_size);
#endif //PATCHCORK
    } else if (strcmp(method, "PUT") == 0) {
// CALL
#if PUTCORK == 1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
#else 
      char *response = handle_put_request(url, upload_data, upload_data_size);
#endif //PUTCORK
    } else if (strcmp(method, "DELETE") == 0) {
// DELETE
#if DELETECORK == 1
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", method);
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *response = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
#else
      char *response = handle_delete_request(url);
#endif // DELETECORK
    } else {
// Method not supported
      cJSON *json = cJSON_CreateObject();
      cJSON_AddStringToObject(json, "error", "method not allowed");
      cJSON_AddNumberToObject(json, "httpcode", HTTP_METHOD_NOT_ALLOWED);
// clean exit procedure w/ housekeeping
      char *response = cJSON_PrintUnformatted(json);
      cJSON_Delete(json);
    }
int ret = send_json_response(connection, response);
free(response); // Free the allocated JSON string
return ret;
#endif // HANDLERCORK
}

// API bootstrap : httpd startup
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

// API shutdown : housekeeping
static int json_api_plugin_deinit(void *p) {
    if (listener != NULL) {
        MHD_stop_daemon(listener);
        listener = NULL;
        printf("HTTP server stopped.\n");
    }
    return 0;
}

// Plugin descriptor
// Plugin descriptor
mariadb_declare_plugin(json_api)
{
    MYSQL_DAEMON_PLUGIN,              /* the plugin type (a MYSQL_XXX_PLUGIN value)     */
    &json_api_plugin,                 /* pointer to type-specific plugin descriptor     */
    PLUGIN_NAME,                      /* plugin name                                    */
    PLUGIN_AUTHOR,                    /* plugin author (for I_S.PLUGINS)                */
    PLUGIN_DESCRIPTION,               /* general descriptive text (for I_S.PLUGINS)     */
    PLUGIN_LICENSE_GPL,               /* the plugin license (PLUGIN_LICENSE_XXX)        */
    json_api_plugin_init,             /* the function to invoke when plugin is loaded   */
    json_api_plugin_deinit,           /* the function to invoke when plugin is unloaded */
    0x0100,                           /* plugin version (for I_S.PLUGINS)               */
    NULL,                             /* for status vars                                */
    NULL,                             /* for system vars                                */
    NULL,                             /* reserved for dependency checking               */
    MariaDB_PLUGIN_MATURITY_ALPHA     /* maturity flags for plugin                      */
}
mariadb_declare_plugin_end;
