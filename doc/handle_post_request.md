# Translate API POST requests into INSERT statements  
  
* POST /v1/tables/SCHEMA/TABLE {"COLs":"VALs" ... } → INSERT INTO SCHEMA.TABLE (COLs,...) VALUES (VALs,...)  

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
