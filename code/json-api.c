#include <mysql.h>
#include <ctype.h>
#include <string.h>
#include <microhttpd.h>
#include <cjson/cJSON.h>

#define PLUGIN_AUTHOR        "Sylvain Arbaudie"
#define PLUGIN_DESCRIPTION   "JSON API Plugin for MariaDB"
#define PLUGIN_LICENSE_GPL   "GNU AGPL v3"

// TODO : managing port via a system variable
#define PORT 8080

// TODO : managing credentials through JWTs and request body
#define APIUSER "apiadmin"
#define APIPASSWD "Ap1-4dmiN"

// Plugin declaration structure
static struct st_mysql_daemon json_api_plugin = {
    MYSQL_DAEMON_INTERFACE_VERSION
};

// TODO : stick to the HTTP API return codes best practices
static int send_json_response(struct MHD_Connection *connection, const char *json_string) {
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json_string),
                                                                    (void *)json_string,
                                                                    MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

// v1 : point select only
// TODO : multirows resultset using arrays
static char* handle_get_request(const char *url) {
    char schema[64];  
    char table[64];
    char colname[64];
    char colvalue[64];
    char query[512];
    
    if (sscanf(url, "/v1/%64[^/]/%64[^/]/%64[^/]/%64s", schema, table, pkname, pkavlue) == 4) {
        // Here query database
        snprintf(query, sizeof(query), "SELECT * FROM %s.%s WHERE %s = '%s'", schema, table, pkname, pkvalue);

        // initialize the JSON answer
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "schema", schema);
        cJSON_AddStringToObject(json, "table", table);
        cJSON_AddStringToObject(json, "pkname", pkname);
        cJSON_AddStringToObject(json, "pkvalue", pkvalue);

        if (mysql_real_connect(conn, "localhost", APIUSER, APIPASSWD, schema, 0, NULL, 0) == NULL) {
           fprintf(stderr, "mysql_real_connect() failed\n");
           mysql_close(conn);
           cJSON_AddStringToObject(json, "status", "CONNECT failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json_string; // Caller is responsible for freeing this memor
        }
      
        if (mysql_query(conn, query)) {
           fprintf(stderr, "mysql_query() failed\n");
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
      
   MYSQL_ROW row = mysql_fetch_row(result);
   if (row) {
      // Process the row data and create JSON response
      cJSON_AddStringToObject(json, "status", "OK");
     
      unsigned int num_fields = mysql_num_fields(result);
      MYSQL_FIELD *fields = mysql_fetch_fields(result);
      for (unsigned int i = 0; i < num_fields; i++) {
        cJSON_AddStringToObject(json, fields[i].name, row[i] ? row[i] : "");
      }
} else { 
    cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
}
      // here we factor the end of query processing
    char *json_string = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    mysql_free_result(result);
    mysql_close(conn);
    return json_string; // Caller is responsible for freeing this memory
}
  // request format is KO
    return NULL;
}

static char* handle_post_request(const char *url) {
  // request format is KO  
  return NULL;
}

static char* handle_put_request(const char *url) {
  // request format is KO  
  return NULL;
}

// v1 : "point delete" only
static char* handle_delete_request(const char *url) {
    char schema[64];
    char table[64];
    char pkname[64];
    char pkvalue[64];
    char query[512];

    // request format is KO
    return NULL;
}

static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls) {

if (strcmp(method, "GET") == 0) {
char *response = handle_get_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"message\": \"Invalid GET request, expected format is /v1/SCHEMA/TABLE/PKNAME/PKVALUE\"}");
    }
    } else if (strcmp(method, "POST") == 0) {
char *response = handle_post_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"message\": \"Invalid POST request, expected format is /v1/SCHEMA/TABLE + JSON body\"}");
    }
    } else if (strcmp(method, "PUT") == 0) {
char *response = handle_put_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"message\": \"Invalid PUT request, expected format is /v1/SCHEMA/TABLE/PKNAME/PKVALUE + JSON body\"}");
    }
    } else if (strcmp(method, "DELETE") == 0) {
char *response = handle_delete_request(url);
    if (response) {
        int ret = send_json_response(connection, response);
        free(response); // Free the allocated JSON string
        return ret;
    } else {
        return send_json_response(connection, "{\"message\": \"Invalid DELETE request, expected format is /v1/SCHEMA/TABLE/PKNAME/PKVALUE\"}");
    }
    } else {
        // Method not supported
        return send_json_response(connection, "{\"error\": \"Unsupported HTTP method, expected methodes are GET,POST,PUT or DELETE\"}");
    }
}

static int json_api_plugin_init(void *p) {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG,
                              PORT, NULL, NULL,
                              &request_handler, NULL,
                              MHD_OPTION_END);

    if (daemon == NULL) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    printf("Server running on port %d\n", PORT);

  MYSQL *conn = mysql_init(NULL);
  if (conn == NULL) {
    fprintf(stderr, "mysql_init() failed\n");
    return NULL;
}
 
    return 0;
}

static int json_api_plugin_deinit(void *p) {
    if (daemon != NULL) {
        MHD_stop_daemon(daemon);
        daemon = NULL;
        printf("HTTP server stopped.\n");
    }
    return 0;
}

// Plugin descriptor
mysql_declare_plugin(json_api)
{
    MYSQL_DAEMON_PLUGIN,
    &json_api_plugin,
    "json_api",
    PLUGIN_AUTHOR,
    PLUGIN_DESCRIPTION,
    PLUGIN_LICENSE_GPL,
    json_api_plugin_init,
    json_api_plugin_deinit,
    0x0100,
    NULL,
    NULL,
    NULL,
    0
}
mysql_declare_plugin_end;
