//include
#include "global-json-api.h"

static char* handle_get_request(const char *url) {
// initialize the JSON answer
    cJSON *json = cJSON_CreateObject();

#if GETCORK == 1
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", "GET");
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *json_string = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return json_string; // Caller is responsible for freeing this memory
#endif
    
// initializing the variables 
    char schema[64];  
    char table[64];
    char colname[64];
    char colvalue[64];  
// check request format, parameter extraction & statement exec
  if (sscanf(url, "/v1/tables/%64[^/]/%64[^/]/%64[^/]/%64s", schema, table, colname, colvalue) == 4) {
// initializing the variables 
      char query[512]="";
// building query
      snprintf(query, sizeof(query), "SELECT * FROM %s.%s WHERE %s = '%s'", schema, table, colname, colvalue);
    } else if (strcmp(url, "/v1/status/") == 0) {
      snprintf(query, sizeof(query), "SHOW GLOBAL STATUS");
    } else if (strcmp(url, "/v1/") == 0) {
// healthcheck
      cJSON_AddNumberToObject(json, "status", "OK");
      cJSON_AddStringToObject(json, "httpcode", HTTP_OK);
// clean exit procedure w/ housekeeping
      char *json_string = cJSON_PrintUnformatted(json);
      cJSON_Delete(json);
      return json_string; // Caller is responsible for freeing this memory
    } else {
// request format is KO
  cJSON_AddNumberToObject(json, "error", "Invalid GET request");
  cJSON_AddStringToObject(json, "httpcode", HTTP_BAD_REQUEST);
// clean exit procedure w/ housekeeping
  char *json_string = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);
  return json_string; // Caller is responsible for freeing this memory 
  }
// we establish internal local connexion
        MYSQL *connection = mysql_init(NULL);
        if (mysql_real_connect_local(connection) == NULL) { 
          fprintf(stderr, "mysql_init/mysql_real_connect_local failed\n");
          cJSON_AddStringToObject(json, "status", "CONNECTION failed");
          cJSON_AddNumberToObject(json, "mariadbcode", mysql_errno(connection));
          cJSON_AddNumberToObject(json, "httpcode", HTTP_INTERNAL_SERVER_ERROR);
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
        }
// we execute the query      
        if (mysql_real_query(connection, STRING_WITH_LEN(query))) {
          fprintf(stderr, "mysql_query() failed\n");
          cJSON_AddStringToObject(json, "status", "QUERY failed");
          cJSON_AddNumberToObject(json, "mariadbcode",mysql_errno(connection));
          cJSON_AddNumberToObject(json, "httpcode", HTTP_INTERNAL_SERVER_ERROR);
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          mysql_close(connection);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
        }
// check resulset existence       
       MYSQL_RES *resultset = mysql_store_result(connection);
       if (resultset == NULL) {
          fprintf(stderr, "mysql_store_result() failed\n");
          cJSON_AddStringToObject(json, "status", "RESULT failed");
          cJSON_AddNumberToObject(json, "mariadbcode",mysql_errno(connection));
          cJSON_AddNumberToObject(json, "httpcode", HTTP_INTERNAL_SERVER_ERROR);
// clean exit procedure w/ housekeeping         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_close(connection);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory
       } else {
// resulset to json translation
           cJSON_AddStringToObject(json, "status", "OK");
           cJSON_AddNumberToObject(json, "rows", mysql_num_rows(resultset));
           // Create a JSON array to hold all rows
           cJSON *rows_array = cJSON_CreateArray();
           unsigned int num_fields = mysql_num_fields(resultset);
           MYSQL_FIELD *fields = mysql_fetch_fields(resultset);
           MYSQL_ROW row;
           while ((row = mysql_fetch_row(resultset))) {
                 // Create a JSON object for each row
                 cJSON *row_object = cJSON_CreateObject();
                 // handling of rows in the following format : "columnname":"columnvalue"
                 for (unsigned int i = 0; i < num_fields; i++) {
                     cJSON_AddStringToObject(row_object, fields[i].name, row[i] ? row[i] : "");
                 }
                // Add the row object to the array
                cJSON_AddItemToArray(rows_array, row_object);
           }
          cJSON_AddItemToObject(json, "data", rows_array);
          cJSON_AddNumberToObject(json, "mariadbcode",0);
          cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
// clean exit procedure w/ housekeeping
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_close(connection);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory;
        } else {
          cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
          cJSON_AddNumberToObject(json, "rows", 0);
          cJSON_AddNumberToObject(json, "mariadbcode", 0); 
          cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
// clean exit procedure w/ housekeeping
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_close(connection);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory
        }
}
