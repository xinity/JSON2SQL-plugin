//include
#include "global-json-api.h"

static char* handle_put_request(const char *url, const char *upload_data, size_t *upload_data_size) {
  // initializing the variables 
  char query[512]="";

  // initialize the JSON answer
  cJSON *json = cJSON_CreateObject();
  
  if (sscanf(url, "/v1/procedures/%64[^/]/%64s", schema, procname) == 4) {  
    
        // Here query database
        snprintf(query, sizeof(query), "SELECT * FROM %s.%s WHERE %s = '%s'", schema, table, colname, colvalue);
        cJSON_AddStringToObject(json, "source", "tables");
        cJSON_AddStringToObject(json, "schema", schema);
        cJSON_AddStringToObject(json, "table", table);
        cJSON_AddStringToObject(json, "column-name", colname);
        cJSON_AddStringToObject(json, "column-value", colvalue);

        MYSQL *connection = mysql_init(NULL);
        if (mysql_real_connect_local(connection) == NULL) { 
           fprintf(stderr, "mysql_init/mysql_real_connect_local failed\n");
           cJSON_AddStringToObject(json, "status", "CONNECTION failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json; // Caller is responsible for freeing this memory
        }
      
        if (mysql_real_query(connection, STRING_WITH_LEN(query))) {
           fprintf(stderr, "mysql_query() failed\n");
           mysql_close(connection);
           cJSON_AddStringToObject(json, "status", "QUERY failed");
           char *json_string = cJSON_PrintUnformatted(json);
           cJSON_Delete(json);
           return json; // Caller is responsible for freeing this memory
        }
        
       MYSQL_RES *resultset = mysql_store_result(connection);
       if (resultset == NULL) {
          fprintf(stderr, "mysql_store_result() failed\n");
          mysql_close(conn);
          cJSON_AddStringToObject(json, "status", "RESULT failed");
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_free_result(resultset);
          return json; // Caller is responsible for freeing this memory
       }

        if (mysql_num_rows(resultset) > 0) {
           cJSON_AddStringToObject(json, "status", "OK");
           cJSON_AddNumberToObject(json, "rows", (double)num_rows);

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
          mysql_free_result(resultset);
          mysql_close(connection);
          return json;
        } else {
          cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
          cJSON_AddNumberToObject(json, "rows", 0);
          mysql_free_result(resultset);
          mysql_close(connection);
          return json:
        }

    }
      // request format is KO
      return NULL;
}
