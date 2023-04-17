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

from backend_classes import *
from databaseFunctions import *
from utilities import *
from rule import *



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

############################################################
#_________________________API LOGIN________________________#
############################################################

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
            print(greeting, user_id, session["access_token"])
            #session.permanent = True
            session["user"] = username
            session["log_name"] = "Example.xes"
            session["log_name_clear"] = session["log_name"].replace(".xes","")
            session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()
            session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
            session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
            session["nomeupload"] = ""
            session["backup_dir"] = os.getcwd()
            #session["plans_path"] = ""
            session["boolean_case"] = False
            session["activity_list"] = []
            session["dfg"] = None
            session["dfg_f"] = None
            process_jar= None
            session["process_script"]= None
            session["pnml_path"] = "net/petri_final.pnml"
            session["marking_path"]="net/marking.txt"
            session["xes_path"] = "net/petri_log.xes"
            session["cost_file_path"]="jar/cost_file"
            session["TIMESTAMP_FOLDER"] = 'timestamp/'

            print(session["directory_log"])


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
    #session.permanent = True
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

    #session["pnml_path"] = "net/petri_final.pnml"
    #session["marking_path"] = "net/marking.txt"
    #session["xes_path"] = "net/petri_log.xes"
    #session["cost_file_path"] = "jar/cost_file"


    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    log_name = session["log_name"]
    filename = log_name
    print("index")

    global process_jar
    #process_jar= None
    #session["process_script"]= None
    try:
        print('killing', process_jar.pid)
        #process_jar.kill()
    except :
        print("process alreay killed")

    global process_script
    try:
        print('killing', process_script.pid)
        process_script.kill()
    except :
        print("process alreay killed")

    return home(filename)


process_jar= None
@app.route('/index', methods = ['POST'])
def upload_file():

    global process_jar
    try:
        print('killing', process_jar.pid)
        #process_jar.kill()
    except :
        print("process alreay killed")

    global process_script
    try:
        print('killing', process_script.pid)
        process_script.kill()
    except :
        print("process alreay killed")

    #global backup_dir
    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    f = request.files['file']
    print(f.filename)

    regex_expression="^[\(\)\w\.,\s-]+\.xes$"
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
        print("greve:", time_stamp)

        print("upload_file")
        #Process(target=queryDb).start()
    else:
        
        session["log_path"]=os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
        f.save(session["log_path"])
        print("upload_file")
    #print(isExistLog)
    print("sto printando "+session["log_name"])
    return home(f.filename)
    #return redirect("http://127.0.0.1:8080", code=200)    

@app.route('/loadProject', methods = ['POST'])
def loadProject():

    global process_jar
    try:
        print('killing', process_jar.pid)
        #process_jar.kill()
    except :
        print("process alreay killed")

    global process_script
    try:
        print('killing', process_script.pid)
        process_script.kill()
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

    pm4py.write_xes(log, 'exported.xes')
    path = 'exported.xes'
    return send_file(path, as_attachment=True)
   
@app.route('/saveProject', methods=['GET'])
def saveProject():
    #global log

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    pm4py.write_xes(log, session["log_path"])
    return "work_done"

@app.route('/sendDsl')
@oidc.require_login
def sendDsl():

    headers = {
        'Authorization': 'Bearer '+session["access_token"]
    }

    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt") as f:
        lines = f.read()
    print(lines)    

    files = {
        'dsl': (None, lines),
    }

    response = requests.post('https://crowdserv.sys.kth.se/api/repo/testuser/import', headers=headers, files=files)
    
    return response.text

@app.route('/dslPost', methods=['POST'])
def dslPost():
    dslHeader=request.headers.get('Dsl')
    dslJson=(json.loads(dslHeader))
 
    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt", "w") as f:
        f.write(dslJson['pipeline'])    
        f.close()
    
    return "work_done"    




############################################################
#_____________________START APPLICATION_____________________#
############################################################

if(path_f=="127.0.0.1"):
    context = ('key/localhost/localhost.crt', 'key/localhost/localhostd.key')
    app.run(host=path_f, port=int(port_f), debug=True, ssl_context=context)
else:
    app.run(host=path_f, port=int(port_f))
    context = ('key/certificate.crt', 'key/private.key')    #certificate and key files

