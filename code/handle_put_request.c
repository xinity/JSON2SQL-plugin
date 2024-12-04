//include
#include "global-json-api.h"

static char* handle_put_request(const char *url, const char *request_body, size_t *request_body_size) {
  // initializing the variables 
  char query[512]="";

  // initialize the JSON answer
  cJSON *json = cJSON_CreateObject();
  
  if (sscanf(url, "/v1/procedures/%64[^/]/%64s", schema, procname) == 2) {  
        // TODO : check that ressource is exposed  
//        if(!is_exposed(url))
//        {
//           fprintf(stderr, "resource not exposed\n");
//           cJSON_AddStringToObject(json, "status", "RESOURCE not exposed");
//           cJSON_AddStringToObject(json, "code","404");
//          // clean exit procedure         
//          char *json_string = cJSON_PrintUnformatted(json);
//          cJSON_Delete(json);
//          return json_string; // Caller is responsible for freeing this memory
//        }
        // Here query database
        snprintf(query, sizeof(query), "'use %s; CALL %s('", schema, procname);
        // looping over request body to extract 
        cJSON_AddStringToObject(json, "source", "tables");
        cJSON_AddStringToObject(json, "schema", schema);
        cJSON_AddStringToObject(json, "procname", procname);

        MYSQL *connection = mysql_init(NULL);
        if (mysql_real_connect_local(connection) == NULL) { 
           fprintf(stderr, "mysql_init/mysql_real_connect_local failed\n");
           cJSON_AddStringToObject(json, "status", "CONNECTION failed");
           cJSON_AddStringToObject(json, "code","");
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
        }
      
        if (mysql_real_query(connection, STRING_WITH_LEN(query))) {
           fprintf(stderr, "mysql_query() failed\n");
           mysql_close(connection);
           cJSON_AddStringToObject(json, "status", "QUERY failed");
           cJSON_AddStringToObject(json, "code","403");
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          return json_string; // Caller is responsible for freeing this memory
        }
        
       MYSQL_RES *resultset = mysql_store_result(connection);
       if (resultset == NULL) {
          fprintf(stderr, "mysql_store_result() failed\n");
          mysql_close(conn);
          cJSON_AddStringToObject(json, "status", "RESULT failed");
          cJSON_AddStringToObject(json, "code","");
          // clean exit procedure         
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory
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
          cJSON_AddStringToObject(json, "code","200");
          // clean exit procedure
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory;
        } else {
          cJSON_AddStringToObject(json, "status", "NO DATA FOUND");
          cJSON_AddNumberToObject(json, "rows", 0);
          cJSON_AddStringToObject(json, "code","200");
          // clean exit procedure
          char *json_string = cJSON_PrintUnformatted(json);
          cJSON_Delete(json);
          mysql_free_result(resultset);
          return json_string; // Caller is responsible for freeing this memory
        }

    }
  // request format is KO
  cJSON_AddNumberToObject(json, "error", "Invalid PUT request");
  cJSON_AddStringToObject(json, "code","400");
  // clean exit procedure
  char *json_string = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);
  return json_string; // Caller is responsible for freeing this memory    
  }
