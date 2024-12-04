// global headers for the project 
#include "global-json-api.h"

// micro httpd headers
#include <microhttpd.h>

// request handlers headers
#include "handle_get_request.h"
#include "handle_post_request.h"
#include "handle_put_request.h"
#include "handle_patch_request.h"
#include "handle_delete_request.h"

// Plugin declaration structure
static struct st_mysql_daemon json_api_plugin = {
    MYSQL_DAEMON_INTERFACE_VERSION
};

// defining the daemon structure
static struct MHD_Daemon *listener = NULL;

// Function to check if the requested URL is a valid resource
// resource format : 
// /v1/
// /v1/status/
// /v1/tables/SCHEMA/TABLE/*
// /v1/procedures/SCHEMA/PROCNAME/*
int is_exposed_resource(const char *url) {
    for (int i = 0; i < num_resources; i++) {
        if (strcmp(url, resources[i]) == 0) {
            return 1; // corresponding exposed resource found
        }
    }
    return 0; // No exposed resource found
}

static int send_json_response(struct MHD_Connection *connection, const char *json_string) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json_string), (void *)json_string, MHD_RESPMEM_MUST_COPY);
// extract http return code from json_string
    cJSON *json = cJSON_Parse(json_string);
    cJSON *field = cJSON_GetObjectItemCaseSensitive(json, "httpcode");
    unsigned int http_code = field->valueint; 
    cJSON_Delete(json);
    if (httpcode == 405) {
// mandatory allow header for 405
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

if (strcmp(method, "GET") == 0) {
// SELECT
    char *response = handle_get_request(url);
    int ret = send_json_response(connection, response);
    free(response); // Free the allocated JSON string
    return ret;
    } else if (strcmp(method, "POST") == 0) {
// INSERT
      char *response = handle_post_request(url, upload_data, upload_data_size);
      int ret = send_json_response(connection, response);
      free(response); // Free the allocated JSON string
      return ret;
    } else if (strcmp(method, "PATCH") == 0) {
// UPDATE
      char *response = handle_patch_request(url, upload_data, upload_data_size);
      int ret = send_json_response(connection, response);
      free(response); // Free the allocated JSON string
      return ret;
    } else if (strcmp(method, "PUT") == 0) {
// CALL
      char *response = handle_put_request(url, upload_data, upload_data_size);
      int ret = send_json_response(connection, response);
      free(response); // Free the allocated JSON string
      return ret;
    } else if (strcmp(method, "DELETE") == 0) {
// DELETE
      char *response = handle_delete_request(url);
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        // Method not supported
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "error", "method not allowed");
        cJSON_AddNumberToObject(json, "httpcode", HTTP_METHOD_NOT_ALLOWED);
// clean exit procedure w/ housekeeping
        char *response = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);
        return send_json_response(connection, response);
    }
}

static int json_api_plugin_init(void *p) {
    listener = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG,
                              PORT, NULL, NULL,
                              &request_handler, NULL,
                              MHD_OPTION_END);

    if (daemon == NULL) {
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
