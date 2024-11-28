# Translate API POST requests into INSERT statements  
  
* POST /v1/tables/ /v1/tables/ {"schema":SCHEMA, "table":TABLE, "COLNAME1":COLVALUE1, ... "COLNAMEn":COLVALUEn} → INSERT INTO SCHEMA.TABLE (COLNAME1,...,COLNAMEn) VALUES (COLVALUE1,...,COLVALUEn)  

## RESULT  
* If failure  
 {
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  ...
  "status": "MYSQL_ERROR_CODE",
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
