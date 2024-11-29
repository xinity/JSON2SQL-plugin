//include
#include "global-json-api.h"

static char* handle_get_request(const char *url) {
  // initializing the variables 
  char query[512]="";

  // initialize the JSON answer
  cJSON *json = cJSON_CreateObject();
  
  if (sscanf(url, "/v1/tables/%64[^/]/%64[^/]/%64[^/]/%64s", schema, table, colname, colvalue) == 4) {   
        // Here query database
        snprintf(query, sizeof(query), "SELECT * FROM %s.%s WHERE %s = '%s'", schema, table, colname, colvalue);
        cJSON_AddStringToObject(json, "source", "tables");
        cJSON_AddStringToObject(json, "schema", schema);
        cJSON_AddStringToObject(json, "table", table);
        cJSON_AddStringToObject(json, "column-name", colname);
        cJSON_AddStringToObject(json, "column-value", colvalue);
//    } else if (strcmp(url, "/v1/status/") == 0) { 
//        SHOW GLOBAL STATUS
//    } else if (strcmp(url, "/v1/") == 0) {
//        HEALTHCHECK
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

       unsigned long long num_rows = mysql_num_rows(result);

        if (num_rows > 0) {
           cJSON_AddStringToObject(json, "status", "OK");
           cJSON_AddNumberToObject(json, "rows", (double)num_rows);

           // Create a JSON array to hold all rows
           cJSON *rows_array = cJSON_CreateArray();

           unsigned int num_fields = mysql_num_fields(result);
           MYSQL_FIELD *fields = mysql_fetch_fields(result);

           MYSQL_ROW row;
           while ((row = mysql_fetch_row(result))) {
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
        } else {
           cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
           cJSON_AddNumberToObject(json, "rows", 0);
        }

    }
      // request format is KO
      return NULL;
}
