# Translate API GET requests into SELECT statements  
  
* GET /v1/tables/SCHEMA/TABLE/COLNAME/COLVALUE → SELECT * FROM SCHEMA.TABLE WHERE COLNAME = COLVALUE  

## RESULT  

* If 0 row found  
{
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "pkname": "primary_key_name",
  "pkvalue": "primary_key_value",
  "status": "NO DATA FOUND",
  "rows": 0
}

* If 1+ rows found  
{
  "source": "tables",
  "schema": "schema_name",
  "table": "table_name",
  "pkname": "primary_key_name",
  "pkvalue": "primary_key_value",
  "status": "OK",
  "rows": 3,
  "data": [
    {
      "column1": "value1",
      "column2": "value2",
      ...
    },
    {
      "column1": "value1",
      "column2": "value2",
      ...
    },
    ...
  ]
}
