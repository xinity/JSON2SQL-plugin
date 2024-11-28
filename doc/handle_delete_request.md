# Translate DELETE requests into DELETE statements
* DELETE /v1/tables/ {"schema":SCHEMA, "table":TABLE, "column-name":COLNAME, "column-value":COLVALUE} → DELETE FROM SCHEMA.TABLE WHERE COLNAME = COLVALUE

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
