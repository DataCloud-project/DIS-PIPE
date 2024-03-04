############################################################
#____________________ALL IMPORT SECTION____________________#
############################################################
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
import shutil

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

from flask import Flask, session
from flask_session import Session

from flask import Flask, redirect, url_for, render_template, request, session
from datetime import timedelta
import time

sys.path.insert(1, 'database')
sys.path.insert(2, 'rule_filter')
sys.path.insert(3, 'backend_api')
sys.path.insert(4, 'dsl_to_xes')

from backend_classes import *
from databaseFunctions import *
from utilities import *
from rule import *
from classes import *
from utils import *

from pm4py.objects.conversion.dfg import converter as dfg_mining
from pm4py.objects.conversion.log import converter as log_converter
from pm4py.algo.discovery.inductive import algorithm as inductive_miner
from pm4py.objects.conversion.process_tree import converter as pt_converter
from pm4py.objects.petri_net.exporter import exporter as pnml_exporter


############################################################
#_____________________APP CONFIGUARTION____________________#
############################################################

app = flask.Flask(__name__)

LOGS_FOLDER = "/event logs"
IMAGE_FOLDER = os.path.join('static', 'images')

storage = '/storage'

app.config["DEBUG"] = True
app.config['UPLOAD_FOLDER'] = LOGS_FOLDER
app.config['SESSION_TYPE'] = 'filesystem'


f = open('client_secrets.json')
client_secret_json = json.load(f)
secret_key_client=client_secret_json['web']['client_secret']
secret_server_url=client_secret_json['web']['server_url']
secret_client_id=client_secret_json['web']['client_id']
secret_realm_name=client_secret_json['web']['realm_name']
f.close()

app.config.update({
    'SECRET_KEY': secret_key_client,
    'TESTING': True,
    'DEBUG': True,
    'OIDC_CLIENT_SECRETS': 'client_secrets.json',
    'OIDC_ID_TOKEN_COOKIE_SECURE': False,
    'OIDC_REQUIRE_VERIFIED_EMAIL': False,
    'OIDC_USER_INFO_ENABLED': True,
    'OIDC_OPENID_REALM': 'user-authentication',
    'OIDC_SCOPES': ['openid', 'email', 'profile'],
    'OIDC_INTROSPECTION_AUTH_METHOD': 'client_secret_post'
})
oidc = OpenIDConnect(app)

app.app_context().push()
#Session(app)
app.secret_key = "hello"
sess = Session()
sess.init_app(app)


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


############################################################
#______________________API APPLICATION_____________________#
############################################################

from backend_classes import *

from rule_app import app_scan
app.register_blueprint(app_scan)

from start_app import app_start
app.register_blueprint(app_start)

from creategraph_app import app_creategraph
app.register_blueprint(app_creategraph)

from durFreq_app import app_durFreq
app.register_blueprint(app_durFreq)

from filter_app import app_filter
app.register_blueprint(app_filter)

from conformance_app import app_conformance
app.register_blueprint(app_conformance)

from query_app import app_query
app.register_blueprint(app_query)

from segmentator_app import app_Segmentator
app.register_blueprint(app_Segmentator)


############################################################
#_________________________API LOGIN________________________#
############################################################


def copy_folder(source_folder, destination_folder):
    try:
        shutil.copytree(source_folder, destination_folder)
        print("Folder copied successfully!")
    except shutil.Error as e:
        print(f"Folder copy failed: {e}")
    except OSError as e:
        print(f"Folder copy failed: {e}")

#added by SimoneONE
@app.route('/',methods=["POST", "GET"])
@oidc.require_login
def indice():
    """ 
    Example for protected endpoint that extracts private information
    from the OpenID Connect id_token. Uses the accompanied access_token
    to access a backend service.
    """
    
    info = oidc.user_getinfo(['preferred_username', 'sub'])

    username = info.get('preferred_username')
    user_id = info.get('sub')
    greeting = "Hello %s" % username
    #access_token= ""
    

    if user_id in oidc.credentials_store:
            from oauth2client.client import OAuth2Credentials
            session["access_token"] = OAuth2Credentials.from_json(oidc.credentials_store[user_id]).access_token
            session["refresh_token"] = OAuth2Credentials.from_json(oidc.credentials_store[user_id]).refresh_token
            #print(greeting, user_id, session["access_token"])
            session.permanent = True
            session["user"] = username
            session["log_name"] = "Example.xes"
            session["log_name_clear"] = session["log_name"].replace(".xes","")
            session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()
            session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
            session["directory_getdsl"]=session["directory_log"]+"/"+"getdsl"
            session["directory_net_pnml"]=session["directory_log"]+"/"+"net"
            session["segmentator"]=session["directory_log"]+"/"+"SEG"
            session["conformace_jar"] = session["directory_log"]+"/"+"jar"
            session["plans_path"]= process_string(session["conformace_jar"])+"/fast-downward/src/plans"

            session["database_jar"] = session["directory_log"]+"/"+"db_jar"
            
            session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
            session["nomeupload"] = ""
            session["backup_dir"] = os.getcwd()
            #session["plans_path"] = ""
            session["boolean_case"] = False
            session["activity_list"] = []
            session["dfg"] = None
            session["dfg_f"] = None
            process_jar= None
            session["pid_jar"] = None
            session["pid_script"] = None
            session["pid_database"] = None
            session["pid_segmentator"] = None
            session["process_script"]= None
            session["pnml_path"] = "net/petri_final.pnml"
            session["marking_path"]="net/marking.txt"
            session["xes_path"] = "net/petri_log.xes"
            session["cost_file_path"]="jar/cost_file"
            session["TIMESTAMP_FOLDER"] = 'timestamp/'
            session["NODOsegmen"] = False

            
            folder_path="storage"+"/"+session["user"]
            source_folder="Example"

            if os.path.exists(folder_path) and os.path.isdir(folder_path):
                print(f"The folder '{folder_path}' exists.")
            else:
                print(f"The folder '{folder_path}' does not exist.")
                os.makedirs(folder_path)
                try:
                    shutil.copytree(source_folder, os.path.join(folder_path, os.path.basename(source_folder)))
                    print(f"Contents from '{source_folder}' have been copied to '{folder_path}'.")
                except Exception as e:
                    print(f"An error occurred while copying: {e}")

            #print(session["directory_log"])


            return home(session["log_name"])
    else:
            return logout()


#added by SimoneONE
@app.route('/logout')
@oidc.require_login
def logout():
    session.pop("user", None)
    session.pop("log_name", None)
    session.pop("log_name_clear", None)
    session.pop("databaseName", None)
    session.pop("directory_log", None)
    session.pop("log", None)
    session.pop("log_duplicate", None)
    session.pop("nomeupload", None)
    session.pop("backup_dir", None)
    session.pop("boolean_case", None)
    session.pop("activity_list", None)
    session.pop("dfg", None)
    session.pop("pnml_path", None)
    session.pop("marking_path", None)
    session.pop("xes_path", None)
    session.pop("cost_file_path", None)
    session.pop("TIMESTAMP_FOLDER", None)
    session.pop("directory_getdsl",None)
    session.pop("directory_net_pnml",None)
    session.pop("segmentator",None)
    session.pop("NODOsegmen",None)
    
    keycloak_openid = KeycloakOpenID(server_url=secret_server_url,
                                    client_id=secret_client_id,
                                    realm_name=secret_realm_name,
                                    client_secret_key=secret_key_client)

    try:
        keycloak_openid.logout(session["refresh_token"])
    except:
        print("An exception on keycloak_openid.logout occurred")
    

    oidc.logout()
    
    session.pop("access_token", None)
    session.pop("refresh_token", None)
    
    return render_template("logout.html")



############################################################
#_____________________USE XES IN THE APP___________________#
############################################################

def home(file):
    #global nomeupload 
    session["nomeupload"]=session["log_name"]
    print("[home]")
    
    return (render_template("index.html", \
        stringF = "", \
        stringP = "", \
        traceDt = "", \
        varalternative ="",\
        stringDuration = "", \
        stringUsedVarible = "", \
        stringEdgeDuration = "", \
        stringEdgeFrequency = "", \
        stringFrequency = "", \
        stringPetriNet = "", \
        median = "", \
        total = "", \
        myPathF_init = "100", \
        myActF_init = "100", \
        myPathP_init = "100", \
        myActP_init = "100", \
        perf_checked = "false" , \
        path = http, \
        filename = file, \
        nameupload = session["log_name"]     ) )

@app.route('/index')
@oidc.require_login
def index():
    session.permanent = True
    '''
    if session.get('user') != True:
        session["user"] = "testuser"
    
    if session.get('log_name') != True:
        session["log_name"] = "Example.xes"

    if session.get('log_name_clear') != True:
        session["log_name_clear"] = session["log_name"].replace(".xes","")
    
    if session.get('databaseName') != True:
        session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()

    if session.get('directory_log') != True:
        session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
    
    if session.get('log_path') != True:
        session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]

    if session.get('nomeupload') != True:
        session["nomeupload"] = ""

    if session.get('backup_dir') != True:
        session["backup_dir"] = os.getcwd()

    if session.get('plans_path') != True:
        session["plans_path"] = ""

    if session.get('boolean_case') != True:
        session["boolean_case"] = False

    if session.get('activity_list') != True:
        session["activity_list"] = []

    if session.get('dfg') != True:
        session["dfg"] = None

    if session.get('dfg_f') != True:
        session["dfg_f"] = None

    if session.get('process_script') != True:
        session["process_script"]= None

    if session.get("directory_getdsl") != True:
        session["directory_getdsl"]= session["directory_log"]+"/"+"getdsl"

    if session.get("directory_net_pnml") != True:
        session["directory_net_pnml"] = session["directory_log"]+"/"+"net"

    if session.get("segmentator") != True:
        session["segmentator"]=session["directory_log"]+"/"+"SEG"

    if session.get("conformace_jar") != True:
        session["conformace_jar"] = session["directory_log"]+"/"+"jar"

    if session.get("database_jar") != True:
        session["database_jar"] = session["directory_log"]+"/"+"db_jar"
    '''
    #session["pnml_path"] = "net/petri_final.pnml"
    #session["marking_path"] = "net/marking.txt"
    #session["xes_path"] = "net/petri_log.xes"
    #session["cost_file_path"] = "jar/cost_file"


    os.chdir(session["backup_dir"])
    #print("Current working directory: {0}".format(os.getcwd()))

    log_name = session["log_name"]
    filename = log_name
    #print("index")

    #global process_jar
    #process_jar= None
    #session["process_script"]= None
    try:
        print('killing', session["pid_jar"])
        os.kill(session["pid_jar"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global process_script
    try:
        print('killing', session["pid_script"])
        #process_script.kill()
        os.kill(session["pid_script"], signal.SIGKILL)
    except :
        print("process alreay killed")


    try:
        print('killing', session["pid_database"])
        #process_script.kill()
        os.kill(session["pid_database"], signal.SIGKILL)
    except :
        print("process alreay killed")

    return home(filename)


process_jar= None
@app.route('/index', methods = ['POST'])
def upload_file():

    #global process_jar
    try:
        print('killing', session["pid_jar"])
        #process_jar.kill()
        os.kill(session["pid_jar"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global process_script
    try:
        print('killing', session["pid_script"])
        #process_script.kill()
        os.kill(session["pid_script"], signal.SIGKILL)
    except :
        print("process alreay killed")

    try:
        print('killing', session["pid_database"])
        #process_script.kill()
        os.kill(session["pid_database"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global backup_dir
    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    f = request.files['file']
    print(f.filename)

    regex_expression="^[\(\)\w\.,\s-]+\.(xes|csv)$"
    check = re.search(regex_expression, f.filename)

    #global nomeupload
    if(check):
        print("file allowed")
    else:
        print("file not allowed")
        return home(session["nomeupload"])

    if f.filename == '':
        print("empty")
    #f.save("event logs/" + f.filename)

    session["nomeupload"] = f.filename

    #global user
    #global log_name
    '''
    import calendar
    import time

    current_GMT = time.gmtime()

    time_stamp = calendar.timegm(current_GMT)
    '''

    now = datetime.datetime.now()
    time_stamp = now.strftime("%Y%m%d%H%M%S%f")

    print("Current timestamp:", time_stamp)

    session["log_name"]=session["nomeupload"]
    #session["log_name"]=session["nomeupload"].replace(".xes","")+str(time_stamp)+".xes"
    #global log_name_clear    
    session["log_name_clear"]=session["log_name"].replace(".xes","").replace(".csv","")
    #global databaseName
    session["databaseName"]=session["log_name_clear"].lower()+"_"+session["user"].lower()
    #global log_path

    directory_user=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]
    isExistUser = os.path.exists(directory_user)
    if(not(isExistUser)):
        print("create directory User")
        os.mkdir(directory_user)
    #print(isExistUser)
    #global directory_log
    
    #TESTLIONE
    #session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
    session["directory_log"]=directory_user+"/"+session["log_name_clear"]
    isExistLog = os.path.exists(session["directory_log"])
    if(not(isExistLog)):
        print("create directory Log")
        os.mkdir(session["directory_log"])
        #createDatabase(session["databaseName"])
        #applyDbSchema(session["databaseName"])
        
        session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
        
        f.save(session["log_path"])
        
        now = datetime.datetime.now()
        time_stamp = now.strftime("%Y%m%d%H%M%S%f")
        
        
        f.save(os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+time_stamp+".txt")
        # Open the file in write mode
        file = open(os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+time_stamp+".txt", "w")

        # Write the string to the file
        file.write(time_stamp)

        # Close the file
        file.close()


        print("upload_file")
        #Process(target=queryDb).start()
    else: 
        session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
        f.save(session["log_path"])
        print("upload_file")
    #print(isExistLog)

    session["directory_getdsl"]=session["directory_log"]+"/"+"getdsl"
    isExistGETDSL = os.path.exists(session["directory_getdsl"])
    if(not(isExistGETDSL)):
        print("create directory getdsl")
        os.mkdir(session["directory_getdsl"])

    
    session["directory_net_pnml"] = session["directory_log"]+"/"+"net"
    isExistNETPNML = os.path.exists(session["directory_net_pnml"])
    if(not(isExistNETPNML)):
        print("create directory net")
        os.mkdir(session["directory_net_pnml"])

    
    session["segmentator"] = session["directory_log"]+"/"+"SEG"
    isExistSEGMENTATOR = os.path.exists(session["segmentator"])
    if(not(isExistSEGMENTATOR)):
        print("create directory segmentator")
        os.mkdir(session["segmentator"])

    session["conformace_jar"] = session["directory_log"]+"/"+"jar"
    copy_folder(os.path.dirname(os.path.realpath(__file__))+"/jar",session["conformace_jar"])
    session["plans_path"]="./"+process_string(session["conformace_jar"])+"/fast-downward/src/plans"

    session["database_jar"] = session["directory_log"]+"/"+"db_jar"
    copy_folder(os.path.dirname(os.path.realpath(__file__))+"/queryJar",session["database_jar"])

    
    
    return home(f.filename)
    #return redirect("http://127.0.0.1:8080", code=200)    

@app.route('/indexSeg')
@oidc.require_login
def indexSeg():
    session.permanent = True
    '''
    if session.get('user') != True:
        session["user"] = "testuser"
    
    if session.get('log_name') != True:
        session["log_name"] = "Example.xes"

    if session.get('log_name_clear') != True:
        session["log_name_clear"] = session["log_name"].replace(".xes","")
    
    if session.get('databaseName') != True:
        session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()

    if session.get('directory_log') != True:
        session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
    
    if session.get('log_path') != True:
        session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]

    if session.get('nomeupload') != True:
        session["nomeupload"] = ""

    if session.get('backup_dir') != True:
        session["backup_dir"] = os.getcwd()

    if session.get('plans_path') != True:
        session["plans_path"] = ""

    if session.get('boolean_case') != True:
        session["boolean_case"] = False

    if session.get('activity_list') != True:
        session["activity_list"] = []

    if session.get('dfg') != True:
        session["dfg"] = None

    if session.get('dfg_f') != True:
        session["dfg_f"] = None

    if session.get('process_script') != True:
        session["process_script"]= None

    if session.get("directory_getdsl") != True:
        session["directory_getdsl"]= session["directory_log"]+"/"+"getdsl"

    if session.get("directory_net_pnml") != True:
        session["directory_net_pnml"] = session["directory_log"]+"/"+"net"

    if session.get("segmentator") != True:
        session["segmentator"]=session["directory_log"]+"/"+"SEG"

    if session.get("conformace_jar") != True:
        session["conformace_jar"] = session["directory_log"]+"/"+"jar"

    if session.get("database_jar") != True:
        session["database_jar"] = session["directory_log"]+"/"+"db_jar"
    
    '''
    #session["pnml_path"] = "net/petri_final.pnml"
    #session["marking_path"] = "net/marking.txt"
    #session["xes_path"] = "net/petri_log.xes"
    #session["cost_file_path"] = "jar/cost_file"


    os.chdir(session["backup_dir"])
    #print("Current working directory: {0}".format(os.getcwd()))

    log_name = session["log_name"]
    filename = log_name
    #print("index")

    #global process_jar
    #process_jar= None
    #session["process_script"]= None
    try:
        print('killing', session["pid_jar"])
        #process_jar.kill()
        os.kill(session["pid_jar"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global process_script
    try:
        print('killing', session["pid_script"])
        #process_script.kill()
        os.kill(session["pid_script"], signal.SIGKILL)
    except :
        print("process alreay killed")


    try:
        print('killing', session["pid_database"])
        #process_script.kill()
        os.kill(session["pid_database"], signal.SIGKILL)
    except :
        print("process alreay killed")

    return home(filename)


process_jar= None
@app.route('/indexSeg', methods = ['POST'])
def upload_file_seg():

    #global process_jar
    try:
        print('killing', session["pid_jar"])
        #process_jar.kill()
        os.kill(session["pid_jar"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global process_script
    try:
        print('killing',session["pid_script"])
        #process_script.kill()
        os.kill(session["pid_script"], signal.SIGKILL)
    except :
        print("process alreay killed")

    try:
        print('killing', session["pid_database"])
        #process_script.kill()
        os.kill(session["pid_database"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global backup_dir
    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    log_name = session["log_name"]
    filename = log_name

    session["nomeupload"] = filename

    #global user
    #global log_name
    '''
    import calendar
    import time

    current_GMT = time.gmtime()

    time_stamp = calendar.timegm(current_GMT)
    '''

    now = datetime.datetime.now()
    time_stamp = now.strftime("%Y%m%d%H%M%S%f")

    print("Current timestamp:", time_stamp)

    session["log_name"]=session["nomeupload"]
    #session["log_name"]=session["nomeupload"].replace(".xes","")+str(time_stamp)+".xes"
    #global log_name_clear    
    session["log_name_clear"]=session["log_name"].replace(".xes","")
    #global databaseName
    session["databaseName"]=session["log_name_clear"].lower()+"_"+session["user"].lower()
    #global log_path

    directory_user=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]
    isExistUser = os.path.exists(directory_user)
    if(not(isExistUser)):
        print("create directory User")
        os.mkdir(directory_user)
    #print(isExistUser)
    #global directory_log
    
    #TESTLIONE
    #session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
    session["directory_log"]=directory_user+"/"+session["log_name_clear"]
    isExistLog = os.path.exists(session["directory_log"])
    if(not(isExistLog)):
        print("create directory Log")
        os.mkdir(session["directory_log"])
        #createDatabase(session["databaseName"])
        #applyDbSchema(session["databaseName"])
        
        session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
        
        #f.save(session["log_path"])
        
        now = datetime.datetime.now()
        time_stamp = now.strftime("%Y%m%d%H%M%S%f")
        
        
        f.save(os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+time_stamp+".txt")
        # Open the file in write mode
        file = open(os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+time_stamp+".txt", "w")

        # Write the string to the file
        file.write(time_stamp)

        # Close the file
        file.close()


        print("upload_file")
        #Process(target=queryDb).start()
    else: 
        session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
        #f.save(session["log_path"])
        print("upload_file")
    #print(isExistLog)

    session["directory_getdsl"]=session["directory_log"]+"/"+"getdsl"
    isExistGETDSL = os.path.exists(session["directory_getdsl"])
    if(not(isExistGETDSL)):
        print("create directory getdsl")
        os.mkdir(session["directory_getdsl"])

    
    session["directory_net_pnml"] = session["directory_log"]+"/"+"net"
    isExistNETPNML = os.path.exists(session["directory_net_pnml"])
    if(not(isExistNETPNML)):
        print("create directory net")
        os.mkdir(session["directory_net_pnml"])

    
    session["segmentator"] = session["directory_log"]+"/"+"SEG"
    isExistSEGMENTATOR = os.path.exists(session["segmentator"])
    if(not(isExistSEGMENTATOR)):
        print("create directory segmentator")
        os.mkdir(session["segmentator"])

    session["conformace_jar"] = session["directory_log"]+"/"+"jar"
    copy_folder(os.path.dirname(os.path.realpath(__file__))+"/jar",session["conformace_jar"])
    session["plans_path"]="./"+process_string(session["conformace_jar"])+"/fast-downward/src/plans"
    
    session["database_jar"] = session["directory_log"]+"/"+"db_jar"
    copy_folder(os.path.dirname(os.path.realpath(__file__))+"/queryJar",session["database_jar"])


    return home(filename)
    #return redirect("http://127.0.0.1:8080", code=200)    









@app.route('/loadProject', methods = ['POST'])
def loadProject():

    #global process_jar
    try:
        print('killing', session["pid_jar"])
        #process_jar.kill()
        os.kill(session["pid_jar"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global process_script
    try:
        print('killing',session["pid_script"])
        #process_script.kill()
        os.kill(session["pid_script"], signal.SIGKILL)
    except :
        print("process alreay killed")

    try:
        print('killing', session["pid_database"])
        #process_script.kill()
        os.kill(session["pid_database"], signal.SIGKILL)
    except :
        print("process alreay killed")

    #global backup_dir
    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    nameclear = request.args.get('namelog')
    #global log_name 
    #global nomeupload
    
    session["log_name"]=nameclear+".xes"
    session["nomeupload"]=session["log_name"]
    #global databaseName
    session["databaseName"]=nameclear.lower()
    #global log_path
    session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+nameclear+"/"+nameclear+".xes"
        
    return (render_template("index.html", \
        stringF = "", \
        stringP = "", \
        traceDt = "", \
        varalternative ="",\
        stringDuration = "", \
        stringUsedVarible = "", \
        stringEdgeDuration = "", \
        stringEdgeFrequency = "", \
        stringFrequency = "", \
        stringPetriNet = "", \
        median = "", \
        total = "", \
        myPathF_init = "100", \
        myActF_init = "100", \
        myPathP_init = "100", \
        myActP_init = "100", \
        perf_checked = "false" , \
        path = http, \
        filename = session["nomeupload"], \
        nameupload = session["nomeupload"]     ) )


from flask import Flask, send_file
import io
import tempfile

############################################################
#_____________________LATERAL FEATURES_____________________#
############################################################

@app.route('/exportXes', methods=['GET'])
def exportXes():
    #global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    pm4py.write_xes(log, 'exported.xes')
    path = "exported.xes"
    xes_exporter.apply(log, "exported.xes") #log contiene i segmenti che mantieni
    directory_to_start_from = '/home/ubuntu/Downloads/'
    #path = filedialog.askdirectory(initialdir=directory_to_start_from, title='Please select a folder:', parent=parent)
    timestamp = "1111"
    xes_exporter.apply(log, directory_to_start_from + "/log_export_"+timestamp+".xes")

    return "done"

@app.route('/downloadXes', methods=['GET'])
def downloadXes():
    #global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    # Create a temporary file in memory
    temp_file = tempfile.NamedTemporaryFile(delete=False)
    
    # Replace this with code that generates the content you want to serve as a file.
    file_content = log

    pm4py.write_xes(log, temp_file.name)
    
    # Close the temporary file
    temp_file.close()

    return send_file(temp_file.name, as_attachment=True, download_name='exported.xes')



@app.route('/downloadCsv', methods=['GET'])
def downloadCsv():
    #global log
    dataframe1=pd.DataFrame(session["log"])

    # Create a temporary file in memory
    temp_file = tempfile.NamedTemporaryFile(delete=False)
    
    # Replace this with code that generates the content you want to serve as a file.
    dataframe1.to_csv(temp_file.name)

    # Close the temporary file
    temp_file.close()

    return send_file(temp_file.name, as_attachment=True, download_name='exported.csv')
   
@app.route('/saveProject', methods=['GET'])
def saveProject():
    #global log

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    pm4py.write_xes(log, session["log_path"])
    return "work_done"

@app.route('/renameProject/<newname>', methods=['POST'])
@oidc.require_login
def renameProject(newname):
    headers = {
        'Authorization': session["access_token"]
    }

    '''
    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt") as f:
        lines = f.read() 

    files = {
        'dsl': (None, lines),
    }
    '''
    path = "storage"+"/"+session["user"]+"/"+session["log_name_clear"]
    dirs = os.listdir( path )

    pipelineID=""
    for file in dirs:        
        #if(len(str(file))==24):
        #    print(file)
        matches = re.findall(r'(\d+|\D+)', file)
        matches = [ int(x) if x.isdigit() else x for x in matches ] #elemento 0 è il ts elemento 1 .txt
        if(len(matches)!=1 and len(str(matches[0]))==20):
            pipelineID=(str(matches[0]))

    print(pipelineID)

    #response = requests.post('https://195.231.61.196:7779/renamePipeline/'+session["user"]+'/:pipelineID/'+newname, headers=headers, verify=False)
    
    response = rename_pipeline(session["user"],pipelineID,newname)

    session["log_name"] = newname+".xes"
    session["log_name_clear"] = session["log_name"].replace(".xes","")
    session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()
    session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
    session["directory_getdsl"]=session["directory_log"]+"/"+"getdsl"
    session["directory_net_pnml"]=session["directory_log"]+"/"+"net"
    session["segmentator"] = session["directory_log"]+"/"+"SEG"
    session["conformace_jar"] = session["directory_log"]+"/"+"jar"
    session["plans_path"]="./"+process_string(session["conformace_jar"])+"/fast-downward/src/plans"
    session["database_jar"] = session["directory_log"]+"/"+"db_jar"


    session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]


    #return response.text
    return session["log_name"]

@app.route('/sendDsl')
@oidc.require_login
def sendDsl():

    headers = {
        'Authorization': 'Bearer '+session["access_token"]
    }

    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt") as f:
        lines = f.read() 

    files = {
        'dsl': (None, lines),
    }

    response = requests.post('https://crowdserv.sys.kth.se/api/repo/'+session["user"]+'/import', headers=headers, files=files, verify=False)
    
    return response.text

@app.route('/dslPost', methods=['POST'])
def dslPost():
    dslHeader=request.headers.get('Dsl')
    dslJson=(json.loads(dslHeader))
 
    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt", "w") as f:
        f.write(dslJson['pipeline'])    
        f.close()
    
    return "work_done"    



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
            

@app.route('/getDslStructure')
@oidc.require_login
def getDslStructure():

    dslName = request.args.get('dslName')

    headers = {
        'Authorization': 'Bearer '+session["access_token"]
    }
    

    #response = requests.get('https://crowdserv.sys.kth.se/api/repo/export/'+session["user"]+"/"+str(dslName), headers=headers, verify=False)

    '''
    import asyncio
    import aiohttp

   
    risposta=""
    
    async def make_request():
        async with aiohttp.ClientSession() as sz:
            async with sz.get('https://crowdserv.sys.kth.se/api/repo/export/'+session["user"]+"/"+str(dslName), headers=headers) as response:
                if response.status == 200:  # Check if the request was successful
                    print("Request successful")
                    risposta=await response.text()
                    print(risposta)  # Access the response body
                else:
                    print(f"Request failed with status code {response.status}")

    loop = asyncio.new_event_loop()
    loop.run_until_complete(make_request())
    '''
    response = requests.get('https://crowdserv.sys.kth.se/api/repo/export/'+session["user"]+"/"+str(dslName), headers=headers, verify=False)  # Send the GET request

    if response.status_code == 200:  # Check if the request was successful
        print("Request successful")
        print(response.text)  # Access the response body
   


        try:
            new_xes=fromDSLtoXES(json.loads(response.text)["data"])
        except Exception as e:
            print(e)
            return(str("error"))
         
        ######
        
        dfg=session["dfg"]

        dataframe1=pd.DataFrame(session["log"])
        log = pm4py.convert_to_event_log(dataframe1)

        activities = None
        activities = pm4py.get_event_attribute_values(log, "concept:name")
        dfg_f=session["dfg_f"]

        if(dfg_f!=None):
            dfg_conf =dfg_f
        else:
            dfg_conf = dfg

        #Print the current working directory
        working_dir=os.getcwd()
        #global backup_dir
        
        session["backup_dir"]=working_dir
        print("Current working directory: {0}".format(os.getcwd()))
        # os.chdir(working_dir+'/jar')
        

        # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
        # subprocess.call(['bash', './run_SYMBA_all'])
        os.chdir(working_dir)
        
        
        ######


        
        path=process_string(session["directory_getdsl"][1:]+"/importedpipeline"+".xes")
        
        new_xes_log = xes_importer.apply(path)

        #dfg=session["dfg"]
        #session["dfg"]=dfg

        new_dfg, new_start_activities, new_end_activities = pm4py.discover_dfg(new_xes_log)
        new_parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
        new_gviz_freq = dfg_visualization.apply(new_dfg, log=new_xes_log, variant=dfg_visualization.Variants.FREQUENCY,
                                                parameters={new_parameters.FORMAT: "svg", new_parameters.START_ACTIVITIES: new_start_activities,
                                                    new_parameters.END_ACTIVITIES: new_end_activities})

        new_grafo_frequency=(str(new_gviz_freq))




        parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
        static_event_stream = log_converter.apply(new_xes_log, variant=log_converter.Variants.TO_EVENT_STREAM)
        tree = inductive_miner.apply_tree(new_xes_log, variant=inductive_miner.Variants.IMf)
        net, im, fm = pt_converter.apply(tree)

        gviz = pn_visualizer.apply(net, im, fm)
        # pn_visualizer.view(gviz)
        places = net.places
        transitions = net.transitions
        arcs = net.arcs
        trst=[]


        for tr in transitions:
            trst.append([str(tr.name), str(tr.label)])


        #print(session["marking_path"])
        #print(session["cost_file_path"])

        pnml_new_path=process_string(session["directory_net_pnml"][1:]+'/petri_final.pnml')
        pnml_exporter.apply(net, im, pnml_new_path, final_marking=fm)
        
        with open(session["marking_path"], 'w') as f:
            f.write(str(im))
            f.write('\n')
            f.write(str(fm))

        xes_exporter.apply(new_xes_log, session["xes_path"])

        
        with open(session["cost_file_path"], "w") as f:
            for index in trst:
                if(index[1].lower().replace(" ", "")=="none"):
                    f.write(index[0].lower().replace(" ", "")+" 0 0")
                else:
                    f.write(index[1].lower().replace(" ", "")+" 1 1")    
                f.write('\n')
            #f.write("none"+" 0 0") remove comment to consider invisible transitions
            f.close()

        #session["dfg"]=dfg

        
        #print(str(gviz)+"£"+str(im)+"£"+str(fm)+"£"+str(list(activities))+"£"+str(trst))

        #print(trst)
        #print(activities)
    
    else:
        print(f"Request failed with status code {response.status_code}")

    return str(gviz)+"£"+str(im)+"£"+str(fm)+"£"+str(list(activities))+"£"+str(trst)+"£"+str(new_grafo_frequency)



@app.route('/getDslName')
@oidc.require_login
def getDslName():

    headers = {
        'Authorization': 'Bearer '+session["access_token"]
    }
    

    response = requests.get('https://crowdserv.sys.kth.se/api/repo/'+session["user"], headers=headers, verify=False)
    

    #fromDSLtoXES("Pipeline TELLU {\n\tcommunicationMedium: medium \n\tsteps:\n\t\t-  step GenerateSampledataReformatPushinMQTT\n\t\t\timplementation:  image: ''\n\t\t\tenvironmentParameters: {\n\t\t\t\tMQTT_HOST: ''oslo.sct.sintef.no'',\n\t\t\t\tMQTT_USERNAME: ''TGW000000000'',\n\t\t\t\tMQTT_CLIENT_ID: ''TGWDATACLOUD'',\n\t\t\t\tMQTT_PASS: ''???'',\n\t\t\t\tMQTT_PORT: ''1883''\n\t\t\t}\n\t\t\tresourceProvider: TelluGateway0\n\t\t\texecutionRequirement:\n\n\n\t\t-  step ReceiveDataFromMQTTCheckPatientPlanBuildFhirDBrecordsStoretoFhirDB\n\t\t\timplementation:  image: ''\n\t\t\tenvironmentParameters: {\n\t\t\t\tRABBITMQ_HOST: ''oslo.sct.sintef.no',\n\t\t\t\t: '5672'',\n\t\t\t\tRABBITMQ_USERNAME: ''tellucareapi'',\n\t\t\t\tRABBITMQ_PASSWORD: ''???'',\n\t\t\t\tFHIR_URL: ''https//tellucloud-fhir.sintef.cloud''\n\t\t\t}\n\t\t\tresourceProvider: TelluCloudProvider\n\t\t\texecutionRequirement:\n\n\n\t\t-  step AnalyzeandCreateNotificationforHealthcarePersonnel\n\t\t\timplementation:  image: ''\n\t\t\tenvironmentParameters: {\n\t\t\t\tRABBITMQ_HOST: ''oslo.sct.sintef.no',\n\t\t\t\t: '5672'',\n\t\t\t\tRABBITMQ_USERNAME: ''tellucareapi'',\n\t\t\t\tRABBITMQ_PASSWORD: ''???'',\n\t\t\t\tFHIR_URL: ''https//tellucloud-fhir.sintef.cloud''\n\t\t\t}\n\t\t\tresourceProvider: TelluCloudProvider\n\t\t\texecutionRequirement:\n\n}\n\n")

    return response.text





def rename_pipeline(user,id,newname):
    repo = 'storage/'
    user_dir = ""
    find = False
    folders_and_files=os.listdir(repo)
    for username in folders_and_files:
            if os.path.isdir(repo+username) and user==username:
                    find = True
                    pipelines = os.listdir(repo+user)
                    user_dir = repo+user

    if find is False:
            return {"data": {}}

    diz={}
    dsl=""

    for project in pipelines:
            project_files = os.listdir(repo+user+"/"+project)
            for file in project_files:
                    matches = re.findall(r'(\d+|\D+)', file)
                    
                    matches = [ int(x) if x.isdigit() else x for x in matches ] #elemento 0 è il ts elemento 1 .txt
                    if(len(matches)!=1 and len(str(matches[0]))==20):
                            diz[matches[0]]=project

    if int(id) in diz:
            dsl = diz[int(id)]
            new_dsl_name=renameDSL(user_dir,dsl,newname)
            data = {"data": "pipeline with id "+id+" was correctly renamed from "+dsl+" to "+newname}                                               
    else:
            data = {"data": "pipeline with id "+id+" was not correctly renamed"}     
                
    return data


def renameDSL(folderName,vecchioNome,nuovoNome):
        os.rename(folderName+"/"+vecchioNome+"/"+vecchioNome+".txt",folderName+"/"+vecchioNome+"/"+nuovoNome+".txt")
        os.rename(folderName+"/"+vecchioNome+"/"+vecchioNome+".xes",folderName+"/"+vecchioNome+"/"+nuovoNome+".xes")
        os.rename(folderName+"/"+vecchioNome, folderName+"/"+nuovoNome )

        concatenazione = ""
        f = open(folderName+"/"+nuovoNome+"/"+nuovoNome+".txt")
        l = f.read()
        first_line=0
        for elem in l:
                if(first_line==0 and elem=="\n"):
                        concatenazione += "Pipeline "+nuovoNome+" {\n"
                        first_line=1
                elif(first_line!=0):
                        concatenazione += elem
        f.close()

        with open(folderName+"/"+nuovoNome+"/"+nuovoNome+".txt", 'w') as f1:
                f1.write(concatenazione)
        
        return concatenazione


@app.before_request
def filter_non_http_requests():
    # Check if the request is an HTTP request
    if not request.method or not request.url:
        # Reject non-HTTP requests with a 400 Bad Request response
        abort(400)


############################################################
#_____________________START APPLICATION_____________________#
############################################################

if(path_f=="127.0.0.1"):
    print("local")
    #context = ('key/localhost/localhost.crt', 'key/localhost/localhostd.key')
    #pp.run(host=path_f, port=int(port_f), debug=True, ssl_context=context)
    app.run(host=path_f, port=int(port_f), debug=True)
elif(path_f=="0.0.0.0"):
    print("DOCKER")
    app.run(host='0.0.0.0', port=7778, debug=True)
else:
    port = int(os.environ.get('PORT', port_f))
    context = ('key/certificate.crt', 'key/private.key')    #certificate and key files
    app.run(host=path_f, port=port, debug=True, ssl_context=context)



