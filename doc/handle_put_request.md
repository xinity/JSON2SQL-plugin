# Translate API PUT requests into CALL statements  
  
* PUT /v1/procedures/:schema/:name {"arg1":ARGV1,...,"argn":ARGVn} → USE :schema;CALL :name(argv1,...argvn)

## RESULT  

