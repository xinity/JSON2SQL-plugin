# Translate API POST requests into INSERT statements  
  
* POST /v1/tables/ {"schema":SCHEMA, "table":TABLE, "COLNAME1":COLVALUE1, ... "COLNAMEn":COLVALUEn} → INSERT INTO SCHEMA.TABLE (COLNAME1,...,COLNAMEn) VALUES (COLVALUE1,...,COLVALUEn)  

## RESULT  
* If failure  
 {
  "request": "POST",
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  ...
  "status": "MYSQL_ERROR_CODE",
  "rows": 0
} 

* If OK  
{
  "request":"POST",
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "status": "OK",
  "rows": n
}

* POST /v1/procedures/ {"schema":"SCHEMA, "procedure":PROCEDURE, "arg1":val1, ... , "argn":valn } → USE SCHEMA; CALL PROCEDURE(arg1, ... , argn);

## RESULT
* If failure  
{
  "request": "POST",
} 

* If OK  
{
  "request":"POST",
}
