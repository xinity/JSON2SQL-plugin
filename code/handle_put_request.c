//include
#include "common.h"

static char* handle_put_request(const char *url, const char *request_body, size_t *request_body_size) {
// initialize the JSON answer
    cJSON *json = cJSON_CreateObject();  

#if PUTCORK == 1
    cJSON_AddStringToObject(json, "status", "CORK");
    cJSON_AddStringToObject(json, "method", "PUT");
    cJSON_AddStringToObject(json, "url", url);
    cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
    char *json_string = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return json_string; // Caller is responsible for freeing this memory
#else
    
// check request format, parameter extraction & statement exec
  if (sscanf(url, "/v1/procedures/%64[^/]/%64s", schema, procname) == 2) {  
    // initializing the variables 
    char query[512]="";
// Here query database
    snprintf(query, sizeof(query), "'use %s; CALL %s('", schema, procname);
// looping over request body to extract 
// CODE TO BE WRITTEN
      
// we establish internal local connexion
        MYSQL *connection = mysql_init(NULL);
        if (mysql_real_connect_local(connection) == NULL) { 
          fprintf(stderr, "mysql_init/mysql_real_connect_local failed\n");
          cJSON_AddStringToObject(json, "status", "CONNECTION failed");
          cJSON_AddNumberToObject(json, "mysqlcode",mysql_errno(connection));
          cJSON_AddNumberToObject(json, "httpcode","");
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
        }
// we execute the query      
        if (mysql_real_query(connection, STRING_WITH_LEN(query))) {
          fprintf(stderr, "mysql_query() failed\n");
          cJSON_AddStringToObject(json, "status", "QUERY failed");
          cJSON_AddNumberToObject(json, "mysqlcode",mysql_errno(connection));
          cJSON_AddNumberToObject(json, "code","403");
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
          cJSON_AddNumberToObject(json, "code","");
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
          cJSON_AddNumberToObject(json, "httpcode", HTTP_OK);
// clean exit procedure w/ housekeeping
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_close(connection);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory
        }
    }
// request format is KO
  cJSON_AddStringToObject(json, "status", "Invalid PUT request");
  cJSON_AddNumberToObject(json, "httpcode", HTTP_BAD_REQUEST);
// clean exit procedure w/ housekeeping
  char *json_string = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);
  return json_string; // Caller is responsible for freeing this memory 
#endif // PUTCORK
  }
