# JSON2SQL-plugin
## Creating an internal JSON API to access datas from remote

* [GET → SELECT](https://github.com/SylvainA77/JSON-API-plugin/blob/main/handle_get_request.md)  

* [POST → INSERT]()  
Map POST requests to INSERT/CALL statements for creating new records
Example: POST /v1/tables/SCHEMA/TABLE (with JSON body) → INSERT INTO SCHEMA.TABLE (name, email) VALUES ('John', 'john@example.com')
         POST /v1/procedures/SCHEMA/PROCEDURE → USE SCHEMA;CALL PROCEDURE

* [PUT → UPDATE]()  
Convert PUT requests to UPDATE statements for modifying existing records
Example: PUT /v1/tables/SCHEMA/TABLE/PKNAME/PKVALUE (with JSON body) → UPDATE SCHEMA.TABLE SET name = 'John Doe' WHERE PKNAME = PKVALUE

* [DELETE → DELETE]()  
Transform DELETE requests into DELETE statements for removing records
Example: DELETE /v1/tables/SCHEMA/TABLE/PKNAME/PKVALUE → DELETE FROM SCHEMA.TABLE WHERE PKNAME = PKVALUE
