# Translate PATCH requests into UPDATE statements
* PATCH /v1/tables/ {"schema":SCHEMA, "table":TABLE, "COLNAME":COLVALUE, "set":["COLNAME1":COLVALUE1, ... "COLNAMEn":COLVALUEn}  → UPDATE SCHEMA.TABLE SET COLs = VALs, ... WHERE COLNAME = COLVALUE

## RESULT  
* If failure
{
  "request":"PUT",
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "status": "MYSQL_ERROR_CODE",
  "rows": 0
} 

* If OK  
{
  "request":"PUT",
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "status": "OK"
}
