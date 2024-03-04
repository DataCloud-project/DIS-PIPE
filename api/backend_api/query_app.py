############################################################
#____________________ALL IMPORT SECTION____________________#
############################################################
from flask import Blueprint, render_template, session,abort
import flask
import os
import sys
import requests
import json
import re
import glob
import subprocess
import datetime
import graphviz
import platform
import dateutil.parser
import decimal
import pm4py
import statistics
import ast
import pyodbc
import psycopg2
import sqlvalidator
import pandas as pd
from flask_oidc import OpenIDConnect
from flask_cors import CORS
from flask import Flask,redirect
from keycloak import KeycloakOpenID
from graphviz import Digraph
from flask import request, jsonify, render_template, redirect, send_file
from pm4py.algo.filtering.log.attributes import attributes_filter
from distutils.command.build_scripts import first_line_re
from jinja2 import Undefined
from collections import Counter
from multiprocessing import Process
from time import sleep
from os.path import exists
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
from pm4py.visualization.dfg import visualizer as dfg_visualization
from pm4py.objects.dfg.filtering import dfg_filtering
from pm4py.algo.discovery.inductive import algorithm as inductive_miner
from pm4py.visualization.petri_net import visualizer as pn_visualizer
from pm4py.objects.conversion.process_tree import converter
from pm4py.algo.filtering.log.start_activities import start_activities_filter
from pm4py.algo.filtering.log.end_activities import end_activities_filter
from pm4py.algo.discovery.footprints import algorithm as footprints_discovery
from pm4py.objects.log.util import interval_lifecycle
from pm4py.algo.filtering.log.variants import variants_filter
from pm4py.util import constants
from pm4py.statistics.traces.generic.log import case_statistics
from pm4py.algo.organizational_mining.resource_profiles import algorithm
from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
from pm4py.objects.log.exporter.xes import exporter as xes_exporter
sys.path.insert(1, 'database')
sys.path.insert(2, 'rule_filter')
sys.path.insert(3, 'backend_api')
from databaseFunctions import *
from utilities import *
from rule import *

from backend_classes import *
import databaseFunctions

############################################################
#________________________QUERY API_________________________#
############################################################

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

app_query = Blueprint('app_query',__name__)

def process_string(input_string):
    split_string = input_string.split("api/", 1)
    if len(split_string) > 1:
        result = split_string[1]
        return result
    else:
        if(input_string.startswith("/storage")):
            return input_string[1:]
        else:
            return input_string
            

@app_query.route('/queryDb', methods=['GET', 'POST'])
def queryDb():
    
    #global log_path
    #global databaseName
    #print(session["databaseName"])
    #print(session["log_path"])
    #databaseName="TestDB"
    #databaseName="datacloud"
    #runningXesPath=session["log_path"]
    

    # Print the current working directory
    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))
    
    #os.chdir(working_dir+'/queryJar')

    #os.system("java -jar XesToRxesPlus.jar "+databaseName+" "+runningXesPath)
    #os.system("java -jar XesToRxesPlus_Postgres.jar "+session["databaseName"]+" "+runningXesPath)
    #print("  \n")
    
    
    if (path_f=="0.0.0.0"):
        db_data_jar_path=process_string(session["database_jar"]+"/XesToRxesPlus_PostgresDocker.jar")
        if(db_data_jar_path[0]=="/"):
            #print("start database jar path")
            db_data_jar_path=process_string(session["database_jar"]+"/XesToRxesPlus_PostgresDocker.jar")[1:]

        runningXesPath=process_string(session["directory_log"]+"/"+session["log_name"])
        if(runningXesPath[0]=="/"):
            #print("start xes path")
            runningXesPath=process_string(session["directory_log"]+"/"+session["log_name"])[1:]

        db_data_jar = subprocess.Popen( "java -jar " + db_data_jar_path +" "+session["databaseName"]+" "+runningXesPath+" "+"'"+databaseFunctions.URL_DATABASE+"/"+session["databaseName"]+"?user="+databaseFunctions.usernameDB+"&password="+databaseFunctions.passwordDB+"'", shell=True)
    else:
        db_data_jar_path=process_string(session["database_jar"]+"/XesToRxesPlus_Postgres.jar")
        if(db_data_jar_path[0]=="/"):
            #print("start database jar path")
            db_data_jar_path=process_string(session["database_jar"]+"/XesToRxesPlus_Postgres.jar")[1:]

        runningXesPath=process_string(session["directory_log"]+"/"+session["log_name"])
        if(runningXesPath[0]=="/"):
            #print("start xes path")
            runningXesPath=process_string(session["directory_log"]+"/"+session["log_name"])[1:]    

        db_data_jar = subprocess.Popen( "java -jar " + db_data_jar_path +" "+session["databaseName"]+" "+runningXesPath, shell=True)

    session["pid_database"]=db_data_jar.pid

    try:
        print('Running in process', session["pid_database"])
        db_data_jar.wait()    
    except subprocess.TimeoutExpired:
        print('Timed out - killing', session["pid_database"])
        #process_jar.kill()
        os.kill(session["pid_database"], signal.SIGKILL) #or signal.SIGKILL     
    print("\njar database done")
    
    #os.chdir(working_dir)

    return "query_done"

@app_query.route('/initializeQuery', methods=['GET', 'POST'])
def initializeQuery():
    queryPercentage = str(request.args.get('queryPercentage'))
    #server = 'localhost' 
    #port= '5432'
    #database = 'TestDB'
    #global databaseName
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    #print(session["databaseName"])
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
    )


    cursor = cnxn.cursor()

    ##Sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

    cursor1 = cnxn.cursor()
    cursor1.execute("select distinct a1.trace_id, a1.EventName, a2.EventName \
                from log_db a1, log_db a2 \
                where a1.trace_id=a2.trace_id and a1.EventName!=a2.EventName and (EXTRACT(EPOCH from a1.time::timestamp)-EXTRACT(EPOCH from a2.time::timestamp))>"+queryPercentage) 

    response=""
    row = cursor1.fetchone() 
    while row: 
        response=response+str(row)+"\n"
        row = cursor1.fetchone()

    cursor1.close()
    cnxn.close()

    return response

@app_query.route('/makeQuery', methods=['GET', 'POST'])
def makeQuery():
    
    queryTODO = str(request.args.get('query'))
    querySELECT = str(request.args.get("selectpart"))
    querySELECT = querySELECT.replace("select distinct ","")

    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
    )


    cursor = cnxn.cursor()

    #sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

 

    if (("delete" in queryTODO) or ("drop" in queryTODO) or ("insert" in queryTODO)):
        return "errore,sql"+"£"+"errore,sql"

      
    print("############")
    print(queryTODO)
    print("############")

    sql_query = sqlvalidator.parse(queryTODO)
    if not sql_query.is_valid():
        print(sql_query.errors)
        return "errore,query"+"£"+"errore,query"
    else:
        try:
            cursor1 = cnxn.cursor()
            cursor1.execute(queryTODO) 

            response=""
            row = cursor1.fetchone() 
            while row: 
            
                for a in row:
                    if(isinstance(a, datetime.datetime)):
                        response=response+a.strftime("%d/%m/%Y %H:%M:%S")+","
                    else:
                        response=response+str(a)+","
                response=response[:-1]+"\n"

                #response=response+str(row)+"\n"
                row = cursor1.fetchone()

            cursor1.close()
            cnxn.close()
        except Exception as e:
            # Handle the exception
            print("An error occurred:", str(e))
            # Return an appropriate error message or value
            return "errore,query"+"£"+"errore,query"

    return response+"£"+querySELECT



@app_query.route('/doQuery1', methods=['GET', 'POST'])
def doQuery1():
    
    #queryTODO = str(request.args.get('query'))
    #querySELECT = str(request.args.get("selectpart"))
    #querySELECT = querySELECT.replace("select distinct ","")

    #server = 'localhost' 
    #port= '5432'
    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"],user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
    )


    cursor = cnxn.cursor()
    #sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

    
    cursor1 = cnxn.cursor()
    cursor1.execute("DROP TABLE IF EXISTS new_log_db CASCADE;");
    cursor1.execute("select distinct  log1.trace_id, log1.EventName, log1.time, log1.key, log1.value into new_log_db \
                     from log_db log1 ;")
    cursor1.close()


    cursor2 = cnxn.cursor()
    cursor2.execute("select distinct nldb1.eventname, nldb1.key, nldb1.value \
                     from new_log_db nldb1 \
                     where key='DataSourceName' and NOT EXISTS( \
                         select * \
                         from new_log_db nldb2 \
                         where nldb1.eventname=nldb2.eventname and ((nldb2.key='DataSourceType' and nldb2.value='Input') or (nldb2.key='DataSourceType' and nldb2.value='Both')) \
                     )")



    response=""
    row = cursor2.fetchone() 
    while row: 
        for a in row:
            if(isinstance(a, datetime.datetime)):
                response=response+a.strftime("%d/%m/%Y %H:%M:%S")+","
            else:
                response=response+str(a)+","
        response=response[:-1]+"\n"

        #response=response+str(row)+"\n"
        row = cursor2.fetchone()

    cursor2.close()
    cnxn.close()

    print(response)
    querySELECT="nldb1.eventname, nldb1.key, nldb1.value"

    return response+"£"+querySELECT



@app_query.route('/doQuery2', methods=['GET', 'POST'])
def doQuery2():
    
    #queryTODO = str(request.args.get('query'))
    #querySELECT = str(request.args.get("selectpart"))
    #querySELECT = querySELECT.replace("select distinct ","")

    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
    )


    cursor = cnxn.cursor()
    #sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

    
    cursor1 = cnxn.cursor()
    cursor1.execute("DROP TABLE IF EXISTS new_log_db CASCADE;");
    cursor1.execute("select distinct  log1.trace_id, log1.EventName, log1.time, log1.key, log1.value into new_log_db \
                     from log_db log1 ;")
    cursor1.close()


    cursor2_1 = cnxn.cursor()
    cursor2_1.execute("DROP TABLE IF EXISTS dsname CASCADE;")
    cursor2_1.execute("select nldb1.eventname, nldb1.key, nldb1.value into dsname \
                        from new_log_db nldb1 \
                        where key='DataSourceName' and EXISTS( \
                            select * \
                            from new_log_db nldb2 \
                            where nldb1.eventname=nldb2.eventname and nldb2.key='StepContinuumLayer' and nldb2.value='Cloud' \
                        ) and NOT EXISTS ( \
                            select * \
                            from new_log_db nldb3 \
                            where nldb1.eventname=nldb3.eventname and ((nldb3.key='DataSourceType' and nldb3.value='Input') or (nldb3.key='DataSourceType' and nldb3.value='Both')) \
                        );")
    cursor2_1.close()


    cursor2_2 = cnxn.cursor()
    cursor2_2.execute("DROP TABLE IF EXISTS dsvolume CASCADE;")
    cursor2_2.execute("select nldb1.eventname, nldb1.key, nldb1.value into dsvolume \
                     from new_log_db nldb1 \
                     where key='DataSourceVolume' and EXISTS( \
                         select * \
                         from new_log_db nldb2 \
                         where nldb1.eventname=nldb2.eventname and nldb2.key='StepContinuumLayer' and nldb2.value='Cloud' \
                     ) and NOT EXISTS ( \
                         select * \
                         from new_log_db nldb3 \
                         where nldb1.eventname=nldb3.eventname and ((nldb3.key='DataSourceType' and nldb3.value='Input') or (nldb3.key='DataSourceType' and nldb3.value='Both')) \
                     );")
    cursor2_2.close()

    cursor3 = cnxn.cursor()
    cursor3.execute("select distinct dsname.key, dsname.value, dsvolume.key, dsvolume.value \
                     from dsname, dsvolume \
                     where dsname.eventname = dsvolume.eventname;")



    response=""
    row = cursor3.fetchone() 
    while row: 
        for a in row:
            if(isinstance(a, datetime.datetime)):
                response=response+a.strftime("%d/%m/%Y %H:%M:%S")+","
            else:
                response=response+str(a)+","
        response=response[:-1]+"\n"

        #response=response+str(row)+"\n"
        row = cursor3.fetchone()

    cursor3.close()
    cnxn.close()

    print(response)
    querySELECT="dsname.key, dsname.value, dsvolume.key, dsvolume.value"

    return response+"£"+querySELECT



@app_query.route('/checkDatabasePresence', methods=['GET'])
def checkDatabasePresence():

    response = "yes" 
    connection = None
    try:
        connection=psycopg2.connect(
                    user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
                )        
        print('Database connected.')

    except:
        print('Database not connected.')

    if connection is not None:
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

p=None

@app_query.route('/translation1', methods=['GET'])
def translation1():
    global p
    #p=Process(target=queryDb)
    #p.start()
    queryDb()

    return("done")

@app_query.route('/translation2', methods=['GET'])
def translation2():
    global p
    createDatabase(session["databaseName"])
    applyDbSchema(session["databaseName"])
    #p=Process(target=queryDb)
    queryDb()

    #p.start()

    return("done")

@app_query.route('/checkTranslationEnd', methods=['GET'])
def checkTranslationEnd():
    if(p==None):
        return "false"

    if(p.is_alive()):
        return "true"
    else:
        return "false"

@app_query.route('/createEventLog', methods=["GET"])
def createEventLog():

    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')

    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=databaseFunctions.usernameDB, password=databaseFunctions.passwordDB, host=databaseFunctions.serverDB, port= databaseFunctions.portDB
    )


    cursor = cnxn.cursor()

    ##Sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

    array=['trace_id', 'EventName', 'time', 'key', 'value' ]

    return jsonify({"campi":array}) 
    
