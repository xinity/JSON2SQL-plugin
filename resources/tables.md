# TABLES resource

A tables resource represent a table in the database

# Resource operations

## Read a set of lines
```
GET /v1/tables/:schema/:name/:colname/:colvalue
```

### Response


## Insert a new set of rows
```
POST /v1/tables/:schema/:name {"colname1":"colvalue1",...,"colnamen":"colvaluen"}
```

### Response


## Update a set of rows
```
PATCH /v1/tables/:schema/:name:colname/:colvalue {"colname1":"colvalue1",...,"colnamen":"colvaluen"}
```

### Response


## Delete a set of rows
```
DELETE /v1/tables/:schema/:name/:colname/:colvalue
```

### Response

