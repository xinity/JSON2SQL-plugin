# JSON2SQL-plugin
## Creating an internal JSON API to access datas from remote

* [GET → SELECT](https://github.com/SylvainA77/JSON-API-plugin/blob/main/doc/handle_get_request.md)  

* [POST → INSERT](https://github.com/SylvainA77/JSON2SQL-plugin/blob/main/doc/handle_post_request.md)  

* [PUT → UPDATE](https://github.com/SylvainA77/JSON2SQL-plugin/blob/main/doc/handle_put_request.md)  
Convert PUT requests to UPDATE statements for modifying existing records
Example: PUT /v1/tables/SCHEMA/TABLE/PKNAME/PKVALUE (with JSON body) → UPDATE SCHEMA.TABLE SET name = 'John Doe' WHERE PKNAME = PKVALUE

* [DELETE → DELETE](https://github.com/SylvainA77/JSON2SQL-plugin/blob/main/doc/handle_delete_request.md)  
Transform DELETE requests into DELETE statements for removing records
Example: DELETE /v1/tables/SCHEMA/TABLE/PKNAME/PKVALUE → DELETE FROM SCHEMA.TABLE WHERE PKNAME = PKVALUE

## compilation instructions

* [instructions](https://github.com/SylvainA77/JSON-API-plugin/blob/main/doc/compilation.md)

## upcoming features

* [TODO](https://github.com/SylvainA77/JSON-API-plugin/blob/main/doc/TODO.md)
