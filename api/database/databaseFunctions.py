

import pyodbc
import psycopg2
import json
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT

with open('../properties.txt') as f:
    lines = f.readlines()
    frontend = lines[0]
    frontend = frontend.split(': ')
    http = frontend[1]
    frontend = frontend[1]
    frontend = frontend.split('//')
    path_f = frontend[1].split(':')[0]
    port_f = frontend[1].split(':')[1]
    port_f = port_f.split('/')[0]
f.close()



# Path to the JSON file
json_file_path = "database_config.json"

# Read the JSON file
with open(json_file_path, "r") as json_file:
    database_config = json.load(json_file)


serverDB= database_config['database']['database_host']
portDB=database_config['database']['port']
databaseDB = 'dove' 
usernameDB=database_config['database']['user']
passwordDB=database_config['database']['password']

if (database_config['database']['populate']=="true"):
   serverDB= database_config['database']['database_host']
   portDB=database_config['database']['port']
   databaseDB = 'dove' 
   usernameDB=database_config['database']['user']
   passwordDB=database_config['database']['password']
   URL_DATABASE= "jdbc:postgresql://"+serverDB+":"+portDB
elif (path_f=="0.0.0.0"):
   serverDB = 'flask_db' 
   portDB = '5432'
   databaseDB = 'dove' 
   usernameDB = 'postgres' 
   passwordDB = 'postgres' 
   URL_DATABASE= "jdbc:postgresql://"+serverDB+":"+portDB
else:
   serverDB = 'localhost' 
   portDB = '5432'
   databaseDB = 'dove' 
   usernameDB = 'postgres' 
   passwordDB = 'postgres' 
   URL_DATABASE= "jdbc:postgresql://"+serverDB+":"+portDB

def createDatabase(databaseName):
   cnxn = psycopg2.connect(
      user=usernameDB, password=passwordDB, host=serverDB, port= portDB
   )

   cnxn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
   cursor = cnxn.cursor()
   name_Database   = databaseName

   sqlCreateDatabase = "create database "+name_Database+";"

   cursor.execute(sqlCreateDatabase);

   cursor.close()

   cnxn.close()

def applyDbSchema(databaseName):

   cnxn = psycopg2.connect(
      database=databaseName, user=usernameDB, password=passwordDB, host=serverDB, port = portDB
   )
   cnxn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT);

   cursor = cnxn.cursor()

   cursor.execute("do $$ "+
                  "begin "+
                     "WHILE( EXISTS(SELECT 1 FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE CONSTRAINT_TYPE = 'FOREIGN KEY') ) LOOP "+

                     "PREPARE sql_alterTable_1 AS "+
                     "SELECT ('ALTER TABLE ' || table_schema || '.[' || TABLE_NAME || '] DROP CONSTRAINT [' || constraint_name || ']') "+
                     "FROM information_schema.table_constraints "+
                     "WHERE CONSTRAINT_TYPE = 'FOREIGN KEY' "+
                     "LIMIT 1; "+
                     "execute sql_alterTable_1; "+

                     "end loop; "+
                  "end$$;")

   cursor.execute("DROP TABLE IF EXISTS log;")
   cursor.execute("CREATE TABLE log ( \
                     id bigint NOT NULL, \
                     name varchar(250) NULL, \
                     CONSTRAINT log_PK PRIMARY KEY (id) \
                  );") 

   cursor.execute("DROP TABLE IF EXISTS classifier;")
   cursor.execute("CREATE TABLE classifier ( \
                     id bigint NOT NULL, \
                     name varchar(50) NOT NULL, \
                     keys varchar(250) NOT NULL, \
                     log_id bigint NOT NULL, \
                     CONSTRAINT classifier_PK PRIMARY KEY (id), \
                     CONSTRAINT classifier_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id) \
                  );")
            
   cursor.execute("DROP TABLE IF EXISTS extension;")
   cursor.execute("CREATE TABLE extension ( \
                     id bigint NOT NULL, \
                     name varchar(50) NOT NULL, \
                     prefix varchar(50) NOT NULL, \
                     uri varchar(250) NOT NULL, \
                     CONSTRAINT extension_PK PRIMARY KEY (id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS attribute;")
   cursor.execute("CREATE TABLE attribute ( \
                     id bigint NOT NULL, \
                     type varchar(50) NOT NULL, \
                     key varchar(50) NOT NULL, \
                     ext_id bigint NULL, \
                     parent_id bigint NULL, \
                     CONSTRAINT attribute_PK PRIMARY KEY (id), \
                     CONSTRAINT attribute_FK_ext_id FOREIGN KEY (ext_id) REFERENCES extension(id), \
                     CONSTRAINT attribute_FK_parent_id FOREIGN KEY (parent_id) REFERENCES attribute(id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS log_has_attribute;")
   cursor.execute("CREATE TABLE log_has_attribute ( \
                     log_id bigint NOT NULL, \
                     trace_global bit NOT NULL, \
                     event_global bit NOT NULL, \
                     attr_id bigint NOT NULL, \
                     value varchar(250) NOT NULL, \
                     CONSTRAINT log_has_attribute_PK PRIMARY KEY (log_id,trace_global,event_global,attr_id), \
                     CONSTRAINT log_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id), \
                     CONSTRAINT log_has_attribute_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS trace;")
   cursor.execute("CREATE TABLE trace ( \
                     id bigint NOT NULL, \
                     name varchar(250) NULL, \
                     log_id bigint NOT NULL, \
                     CONSTRAINT trace_PK PRIMARY KEY (id), \
                     CONSTRAINT trace_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS trace_has_attribute;")
   cursor.execute("CREATE TABLE trace_has_attribute ( \
                     trace_id bigint NOT NULL, \
                     attr_id bigint NOT NULL, \
                     value varchar(250) NOT NULL, \
                     CONSTRAINT trace_has_attribute_PK PRIMARY KEY (trace_id,attr_id), \
                     CONSTRAINT trace_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id), \
                     CONSTRAINT trace_has_attribute_FK_trace_id FOREIGN KEY (trace_id) REFERENCES trace(id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS event_collection;")
   cursor.execute("CREATE TABLE event_collection ( \
                     id bigint NOT NULL, \
                     name varchar(50) NULL, \
                     CONSTRAINT event_collection_PK PRIMARY KEY (id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS event;")
   cursor.execute("CREATE TABLE event ( \
                     id bigint NOT NULL, \
                     trace_id bigint NOT NULL, \
                     name varchar(250) NOT NULL, \
                     transition varchar(50) NOT NULL, \
                     event_coll_id bigint NULL, \
                     time timestamp(3) NOT NULL, \
                     CONSTRAINT event_PK PRIMARY KEY (id), \
                     CONSTRAINT event_FK_event_coll_id FOREIGN KEY (event_coll_id) REFERENCES event_collection(id), \
                     CONSTRAINT event_FK_trace_id FOREIGN KEY (trace_id) REFERENCES trace(id) \
                  );")

   cursor.execute("DROP TABLE IF EXISTS event_has_attribute;")
   cursor.execute("CREATE TABLE event_has_attribute ( \
                     event_id bigint NOT NULL, \
                     attr_id bigint NOT NULL, \
                     value varchar(250) NOT NULL, \
                     CONSTRAINT event_has_attribute_PK PRIMARY KEY (event_id,attr_id), \
                     CONSTRAINT event_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id), \
                     CONSTRAINT event_has_attribute_FK_event_id FOREIGN KEY (event_id) REFERENCES event(id) \
                  );")

   cursor.close()

   cnxn.close()