# JSON-API-plugin
Creating an internal JSON API to access datas straight from said API

GET → SELECT
Translate API GET requests into SELECT statements
Example: GET /users/123 → SELECT * FROM users WHERE id = 123

POST → INSERT
Map POST requests to INSERT statements for creating new records
Example: POST /users (with JSON body) → INSERT INTO users (name, email) VALUES ('John', 'john@example.com')

PUT → UPDATE
Convert PUT requests to UPDATE statements for modifying existing records
Example: PUT /users/123 (with JSON body) → UPDATE users SET name = 'John Doe' WHERE id = 123

DELETE → DELETE
Transform DELETE requests into DELETE statements for removing records
Example: DELETE /users/123 → DELETE FROM users WHERE id = 123
