# Translate PUT requests into UPDATE statements
* PUT /v1/tables/SCHEMA/TABLE/COLNAME/COLVALUE {"COLs":"VALs", ... }  → UPDATE SCHEMA.TABLE SET COLs = VALs, ... WHERE COLNAME = COLVALUE

## RESULT  
* If failure  
 {
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  ...
  "status": " ",
  "rows": 0
} 

* If OK  
{
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "status": "OK",
  "rows": n
}
