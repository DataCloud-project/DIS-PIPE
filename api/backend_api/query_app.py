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

############################################################
#________________________QUERY API_________________________#
############################################################

app_query = Blueprint('app_query',__name__)

@app_query.route('/queryDb', methods=['GET', 'POST'])
def queryDb():
    
    #global log_path
    #global databaseName
    print(session["databaseName"])
    print(session["log_path"])
    #databaseName="TestDB"
    #databaseName="datacloud"
    runningXesPath=session["log_path"]

    # Print the current working directory
    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))
    
    os.chdir(working_dir+'/queryJar')

    #os.system("java -jar XesToRxesPlus.jar "+databaseName+" "+runningXesPath)
    os.system("java -jar XesToRxesPlus_Postgres.jar "+session["databaseName"]+" "+runningXesPath)
    print("  \n")

    os.chdir(working_dir)

    return "query_done"

@app_query.route('/initializeQuery', methods=['GET', 'POST'])
def initializeQuery():
    queryPercentage = str(request.args.get('queryPercentage'))
    server = 'localhost' 
    port= '5432'
    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    username = 'postgres' 
    password = 'ubuntu-777' 
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    print(session["databaseName"])
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=username, password=password, host=server, port= port
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

    server = 'localhost' 
    port= '5432'
    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    username = 'postgres' 
    password = 'ubuntu-777' 
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    print(session["databaseName"])
    
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=username, password=password, host=server, port= port
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

@app_query.route('/checkDatabasePresence', methods=['GET'])
def checkDatabasePresence():

    server = 'localhost' 
    port= '5432'
    username = 'postgres' 
    password = 'ubuntu-777' 

    response = "yes" 
    connection = None
    try:
        connection = psycopg2.connect("user='postgres' host='localhost' password='ubuntu-777' port='5432'")
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
    server = 'localhost' 
    port= '5432'
    #database = 'TestDB'
    #global databaseName
    #database = 'datacloud' 
    #username = 'sa'
    username = 'postgres' 
    password = 'ubuntu-777' 
    #cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER='+server+';DATABASE='+database+';UID='+username+';PWD='+ password)
    #cnxn = pyodbc.connect('DRIVER={Devart ODBC Driver for PostgreSQL};Server='+server+';Database='+database+';User ID='+username+';Password='+password+';String Types=Unicode')
    
    print(session["databaseName"])
    #establishing the connection
    cnxn = psycopg2.connect(
        database=session["databaseName"], user=username, password=password, host=server, port= port
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
    
