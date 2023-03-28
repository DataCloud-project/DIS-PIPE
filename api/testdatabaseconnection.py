import psycopg2
print("ultimotest")
#print(session["databaseName"])

server = 'localhost' 
port= '5432'
username = 'postgres' 
password = 'ubuntu-777' 

response = "yes" 
connection = None
#try:
connection = psycopg2.connect("user='postgres' host='localhost' password='ubuntu-777' port='5432'")
#connection = psycopg2.connect("user='postgres' host='127.0.0.1' password='' port='5432'")
print('Database connected.')
'''
except:
    print('Database not connected.')

if connection is not None:
    print(connection)
    connection.autocommit = True

    cur = connection.cursor()

    cur.execute("SELECT datname FROM pg_database;")

    list_database = cur.fetchall()

    database_name = session["databaseName"]

    if (database_name,) in list_database:
        print("'{}' Database already exist".format(database_name))
        response = "yes"
    else:
        print("'{}' Database not exist.".format(database_name))
        #createDatabase(session["databaseName"])
        #applyDbSchema(session["databaseName"])
        response = "no"
    connection.close()
    print('Done')

    return jsonify({"presence":response}) 

else:
    return jsonify({"presence":"error connection"}) 
    '''