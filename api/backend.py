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


#aggiunto da simone
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
from utilities import *
from rule import *
from databaseFunctions import *
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


#Global variable
nomeupload=""
storage = '/storage'
user="testuser"
log_name="Example.xes"
log_name_clear=log_name.replace(".xes","")
databaseName=log_name_clear.lower()
directory_log=storage+"/"+user+"/"+log_name_clear
dfg_f=None
process_script=None
process_jar=None
backup_dir=os.getcwd()

if platform.system() == "Windows":
        log_path = 'event logs\\running-example.xes'
        pnml_path="net\\petri_final.pnml"
        xes_path="net\\petri_log.xes"
        marking_path="net\\marking.txt"
        cost_file_path="jar\\cost_file"
if platform.system() == "Linux":
        #log_path = 'event logs/running-example.xes'
        log_path=os.path.dirname(os.path.realpath(__file__))+storage+"/"+user+"/"+log_name_clear+"/"+log_name
        pnml_path="net/petri_final.pnml"
        xes_path="net/petri_log.xes"
        marking_path="net/marking.txt"
        cost_file_path="jar/cost_file"

#Configuartion variable
LOGS_FOLDER = "/event logs"
IMAGE_FOLDER = os.path.join('static', 'images')
TIMESTAMP_FOLDER = 'timestamp/'

app = flask.Flask(__name__)

#SESSION_TYPE = 'redis'
#app.config.from_object(__name__)
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

#add by SimoneONE vvvvvvvvvvvvv
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
#add by SimoneONE ^^^^^^^^^

app.app_context().push()
#Session(app)
app.secret_key = "hello"
sess = Session()
sess.init_app(app)




#user1 = request.form["nm"]
#with app.app_context():
'''
session.permanent = True
session["user"] = "testuser"
session["log_name"] = "Example.xes"
session["log_name_clear"] = session["log_name"].replace(".xes","")
session["databaseName"] = session["log_name_clear"].lower()+"_"+session["user"].lower()
session["directory_log"] = storage+"/"+session["user"]+"/"+session["log_name_clear"]
session["log_path"] = os.path.dirname(os.path.realpath(__file__))+storage+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name"]
session["nomeupload"] = ""
session["backup_dir"] = os.getcwd()
session["plans_path"] = ""
session["boolean_case"] = False
session["activity_list"] = []
session["dfg"] = None
session["dfg_f"] = None
process_jar= None
session["process_script"]= None
'''



with open('../properties.txt') as f:
    lines = f.readlines()
    backend=lines[1]
    backend = backend.split(': ')
    path = backend[1]
    port_n = backend[1].split(':')[1]
    port_n = port_n.split('/')[0]
    frontend=lines[0]
    frontend = frontend.split(': ')
    http = frontend[1]
    frontend = frontend[1]
    frontend = frontend.split('//')
    path_f = frontend[1].split(':')[0]
    port_n = frontend[1].split(':')[1]
    port_n = port_n.split('/')[0]
f.close()

class ModelEncoder( json.JSONEncoder) :
    def default( self , obj ) :
        if isinstance( obj , Model ):
            return obj.to_json()
        # Let the base class default method raise the TypeError
        return json.JSONEncoder.default( self , obj )

class Model( json.JSONEncoder)  :
    
    def to_json( self ) :
        """
        to_json transforms the Model instance into a JSON string
        """
        return jsonpickle.encode( self )


#Class objects
class DateTimeAwareEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, datetime):
            return o.isoformat()

        return json.JSONEncoder.default(self, o)

CONVERTERS = {
    'datetime': dateutil.parser.parse,
    'decimal': decimal.Decimal,
    'dict': dict,
    'list': list,
    'trace': pm4py.objects.log.obj.Trace,
    'string': str
}

class MyJSONEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, (datetime.datetime,)):
            return {"val": str(obj), "_spec_type": "string"}
        elif isinstance(obj, (decimal.Decimal,)):
            return {"val": str(obj), "_spec_type": "decimal"}
        elif isinstance(obj, (pm4py.objects.log.obj.Event,)):
            return {"val": dict(obj), "_spec_type": "dict"}
        elif isinstance(obj, (pm4py.objects.log.obj.Trace,)):
            return {"val": {'attribute':obj.attributes, 'events':list(obj)}, "_spec_type": "dict"}
        else:
            return super().default(obj)

def object_hook(obj):
    _spec_type = obj.get('_spec_type')
    if not _spec_type:
        return obj

    if _spec_type in CONVERTERS:
        return CONVERTERS[_spec_type](obj['val'])
    else:
        raise Exception('Unknown {}'.format(_spec_type))

'''
@app.route('/set/')
def set():
    session['key'] = 'value'
    return 'ok'

@app.route('/get/')
def get():
    return session.get('key', 'not set')

thisdict = {
  "brand": "Ford",
  "model": "Mustang",
  "year": 1964
}
'''
'''
@app.before_request
def before_request():
    print("before req")
    if not request.is_secure:
        url = request.url.replace('http://', 'https://', 1)
        code = 301
        print("after")
        return redirect(url, code=code)
'''


#added by SimoneONE
@app.route('/',methods=["POST", "GET"])
@oidc.require_login
def indice():
    """Example for protected endpoint that extracts private information from the OpenID Connect id_token.
       Uses the accompanied access_token to access a backend service.
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
            #filename = 'running-example.xes'

            #session.permanent = True
            #user1 = request.form["nm"]
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

            print(session["directory_log"])


            return home(session["log_name"])
    else:
            return logout()


@app.route('/sendDsl')
@oidc.require_login
def sendDsl():

    #mydsl = request.args.get('dsl')
    import requests

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


#added by SimoneONE
@app.route('/logout')
@oidc.require_login
def logout():
    """Performs local logout by removing the session cookie."""
    session.pop("user", None)
    session.pop("log_name", None)
    session.pop("log_name_clear", None)
    session.pop("databaseName", None)
    session.pop("directory_log", None)
    session.pop("log", None)
    session.pop("log_duplicate", None)
    session.pop("nomeupload", None)
    session.pop("backup_dir", None)
    #session.pop("plans_path", None)
    session.pop("boolean_case", None)
    session.pop("activity_list", None)
    session.pop("dfg", None)


    #oidc.logout()


    
    
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
    
    
    #return 'Hi, you have been logged out! <a href="/">Return</a>'
    return render_template("logout.html")







#session
'''
@app.route("/login", methods=["POST", "GET"])
def login():
    if request.method == "POST":
        session.permanent = True
        user1 = request.form["nm"]
        session["user"] = user1
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
        session["process_script"]= Noneg

        print(session["directory_log"])
        return redirect(url_for("user"))
    else:
        if "user" in session:
            return redirect(url_for("user"))

        return render_template("login.html")

'''

@app.route("/user")
def user():
    if "user" in session:
        user1 = session["user"]
        return f"<h1>{user1}</h1>"
    else:
        return redirect(url_for("login"))

'''
@app.route("/logout")
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
    #session.pop("plans_path", None)
    session.pop("boolean_case", None)
    session.pop("activity_list", None)
    session.pop("dfg", None)
    
    return redirect(url_for("login"))
'''

#Server functions
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


    #global backup_dir
    os.chdir(session["backup_dir"])
    print("Current working directory: {0}".format(os.getcwd()))

    log_name = session["log_name"]
    #filename = 'running-example.xes'
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

    session["log_name"]=session["nomeupload"].replace(".xes","")+str(time_stamp)+".xes"
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

@app.route('/start', methods=['GET', 'POST'])
def start():
    start =requests.get(path.strip('\n')+'start')
    return start.text

@app.route('/end', methods=['GET', 'POST'])
def end():
    end =requests.get(path.strip('\n')+'end')
    return end.text


@app.route('/petriFreq', methods=['GET', 'POST'])
def petriFreq():
    petriF = requests.get(path.strip('\n')+'petriNetFreq')
    return str(petriF.text)
    
@app.route('/petriPerf', methods=['GET', 'POST'])
def petriPerf():
    petriP = requests.get(path.strip('\n')+'petriNetPerf')
    return str(petriP.text)
    
@app.route('/bpmn', methods=['GET', 'POST'])
def bpmn():
    bpmn = requests.get(path.strip('\n')+'bpmn')
    return str(bpmn.text)
    
def createGraphF(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    
    #DFG - process discovery
    #dfg_freq = dfg_discovery.apply(log)
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    return gviz_freq

def createGraphP(log):
    # log = xes_importer.apply(log_path) 
    # log = interval_lifecycle.to_interval(log)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
                                                        
    #DFG - process discovery
    #dfg_perf = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    #parameters = {dfg_visualization.Variants.PERFORMANCE.value.Parameters.FORMAT: "svg"}
    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    
    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    
    #gviz_perf = dfg_visualization.apply(dfg_perf, log=log, variant=dfg_visualization.Variants.PERFORMANCE)
    #dfg_visualization.view(gviz)
    #dfg_visualization.save(gviz_perf, "static/images/performance.png")
    #perf_img = os.path.join(app.config['UPLOAD_FOLDER'], 'performance.png')
    
    #return render_template("index.html", img_freq = freq_img, img_perf = perf_img, string = str(gviz_freq))
    #string_html = render_template("string.html", string = str(gviz_freq))
    #frequency = render_template("img_freq.html", img_freq = freq_img)
    #performance = render_template("img_perf.html", img_perf = perf_img)
    
    return gviz_perf

@app.route('/dfgFrequency', methods=['GET', 'POST'])
def dfgFrequency():
    #global log_path
    log = xes_importer.apply(session["log_path"])
    return str(createGraphF(log))

@app.route('/dfgPerformance', methods=['GET', 'POST'])
def dfgPerformance():
    #global log_path
    log = xes_importer.apply(session["log_path"])
    return str(createGraphP(log))

def createGraphFReduced(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if request.args.get('myActF') == None:
        act = 100;
    else:
        act = int(request.args.get('myActF'))
    if request.args.get('myPathF') == None:
        path = 100;
    else:
        path = int(request.args.get('myPathF'))
    #print("Freq: "+str(act)+" "+str(path))

    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)
    gviz_f = dfg_visualization.apply(dfg_f, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
                                                
    return gviz_f

def createGraphPReduced(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if request.args.get('myActP') == None:
        act = 100;
    else:
        act = int(request.args.get('myActP'))
    if request.args.get('myPathP') == None:
        path = 100;
    else:
        path = int(request.args.get('myPathP'))
    #print("Perf: "+str(act)+" "+str(path))
    

    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    dfg_p = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    gviz_f = dfg_visualization.apply(dfg_p, log=log, variant=dfg_visualization.Variants.PERFORMANCE, 
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})
                                                
    return gviz_f

@app.route('/dfgFreqReduced', methods=['GET', 'POST'])
def dfgFreqReduced():
    # GET request
    #if request.method == 'GET':
    #f = requests.get('http://127.0.0.1:7777/dfgFrequency')
    #myPathF = request.form.get('myPathF')
    myPathF = request.args.get('myPathF')
    #myActF = request.form.get('myActF')
    myActF = request.args.get('myActF')
    #perfCheck = request.form.get('perf_checked')
    perfCheck = request.args.get('perf_checked')

    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    ##path = request.args.get('myPathF')
    #paramsF = {'myPathF' : myPathF, 'myActF' : myActF}

    #f = requests.get(path.strip('\n')+'dfgFreqReduced', params = paramsF)

    ##if request.form.get('updated') != None:
    #if request.args.get('updated') != None:
    #    f = request.files['file']
    #    if f.filename != '': 
    #      #f.save("event logs/" + f.filename)
    #      f.save(log_path)
    #      return home(f.filename)      
      
        
    # log = xes_importer.apply(log_path)

    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if myActF == None:
        act = 100;
    else:
        act = int(myActF)
    
    if myPathF == None:
        path = 100;
    else:
        path = int(myPathF)
    #print("Freq: "+str(act)+" "+str(path))
    #global dfg_f
    
    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)
    gviz_f = dfg_visualization.apply(dfg_f, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
    session["dfg_f"]=dfg_f
                                                
    return str(gviz_f)

@app.route('/dfgPerfReduced', methods=['GET', 'POST'])
def dfgPerfReduced():
    # if request.method == 'GET':
    myPathP = request.args.get('myPathP')
    myActP = request.args.get('myActP')
    perfCheck = request.args.get('perf_checked')
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathP' : myPathP, 'myActP' : myActP}
    # p = requests.get(path.strip('\n')+'dfgPerfReduced', params = paramsP)

    # #print(request.form.get('updated'))
    # if request.form.get('updated') != None:
    #     f = request.files['file']
    #     if f.filename != '': 
    #       #f.save("event logs/" + f.filename)
    #       f.save(log_path)
    #       return home(f.filename)      
    
    # log = xes_importer.apply(log_path)

    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    #global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if myActP == None:
        act = 100;
    else:
        act = int(myActP)
    
    if myPathP == None:
        path = 100;
    else:
        path = int(myPathP)
    #print("Perf: "+str(act)+" "+str(path))
    

    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    gviz_f = dfg_visualization.apply(dfg_p, log=log, variant=dfg_visualization.Variants.PERFORMANCE, 
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})
                                                
    return str(gviz_f)

@app.route('/allduration', methods=['GET'])
def allduration():
    import time
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    #global activity_list
    session["activity_list"] = []
    for i in range(0, len(log)):
        for j in range(0, len(log[i])):
            if(log[i][j]['concept:name'] not in session["activity_list"]):
                session["activity_list"].append(log[i][j]['concept:name'])

    # global activity_dictionary
    activity_dictionary=dict.fromkeys(session["activity_list"])
    
    for a in session["activity_list"]:
        activity_dictionary[a]=[]
    
    for i in range(0, len(log)):
        for p in range(0, len(log[i])):
            activity_dictionary[log[i][p]['concept:name']].append(log[i][p]['@@duration'])


    mean_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        mean_dizionario[j]=statistics.mean(activity_dictionary[j])

        
    total_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        total_dizionario[j]=sum(activity_dictionary[j])
        
    median_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        median_dizionario[j]=activity_dictionary[j][int(len(activity_dictionary[j])/2)]
        
    max_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        max_dizionario[j]=max(activity_dictionary[j])
        
        
    min_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        min_dizionario[j]=min(activity_dictionary[j])

    return str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario)

@app.route('/alledgeduration', methods=['GET'])
def alledgeduration():
    import time
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #global activity_list
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    follower_dictionary={}
    follower_list=[]
    follower_used_list=[]
    import datetime
    
    already_sum=[]

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            for k in range(j, len(log[i])):
                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg and j!=k and ((log[i][j]['concept:name']==log[i][k]['concept:name'] and j+1==k )or (log[i][j]['concept:name'],log[i][k]['concept:name']) not in already_sum )):  
                    already_sum.append((log[i][j]['concept:name'],log[i][k]['concept:name']))         
                    if((log[i][j]['concept:name'],log[i][k]['concept:name']) not in follower_list):
                        follower_list.append((log[i][j]['concept:name'],log[i][k]['concept:name']))
                        follower_used_list.append(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'])
                        follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=[]

                    time_result=log[i][k]['start_timestamp']-log[i][j]['time:timestamp']
                    time_result=time_result.total_seconds()
                    follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']].append(time_result)
                    # print(follower_dictionary)
        already_sum=[]

   
    mean_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        mean_edge_dizionario[j]=sum(follower_dictionary[j])/len(follower_dictionary[j])

    total_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        total_edge_dizionario[j]=sum(follower_dictionary[j])

    median_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        median_edge_dizionario[j]=statistics.median(follower_dictionary[j])

    max_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        max_edge_dizionario[j]=max(follower_dictionary[j])

    min_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        min_edge_dizionario[j]=min(follower_dictionary[j])

    return str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario)

@app.route('/allfrequency', methods=['GET'])
def allfrequency():
    import time
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    dictionary_absolute={}

    for trace in log:
        for ev in trace: 
            if(ev['concept:name'] in dictionary_absolute):
                dictionary_absolute[ev['concept:name']]=str(int(dictionary_absolute[ev['concept:name']])+1)
            else:
                dictionary_absolute[ev['concept:name']]=str(1)

    
    ###################

    #CASE FREQ
    dictionary_case={}
    already_case=[]

    for trace_case in log:
        for ev_case in trace_case:
            if(ev_case['concept:name'] not in already_case):
                already_case.append(ev_case['concept:name'])
                if(ev_case['concept:name'] in dictionary_case):
                    dictionary_case[ev_case['concept:name']]=str(int(dictionary_case[ev_case['concept:name']])+1)
                else:
                    dictionary_case[ev_case['concept:name']]=str(1)
                    
        already_case=[]

    ###################

    #max repetititions
    dictionary_max_trace={}
    dictionary_max={}


    for trace_max in log:
        for ev_max in trace_max:
    #         print(precedente)
    #         if(ev_max['concept:name']!=precedente):
            if(ev_max['concept:name'] in dictionary_max_trace):
                dictionary_max_trace[ev_max['concept:name']]=str(int(dictionary_max_trace[ev_max['concept:name']])+1)
            else:
                dictionary_max_trace[ev_max['concept:name']]=str(1)
                
            if(ev_max['concept:name'] in dictionary_max):
                if(int(dictionary_max_trace[ev_max['concept:name']])>int(dictionary_max[ev_max['concept:name']])):
                    dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
                
            else:
                dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
        
        dictionary_max_trace={}

    return str(dictionary_absolute)+"*"+str(dictionary_case)+"*"+str(dictionary_max)

@app.route('/alledgefrequency', methods=['GET'])
def alledgefrequency():
    import time
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #absolute frequency
    follower_dictionary_absolute={}
    

    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_absolute):
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)

    for i in range(0, len(log)):
        j=0
        if(log[i][j]['concept:name'] in start_activities):

            if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_absolute):
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
            else:
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']])+1)

        k=len(log[i])-1
        if(log[i][k]['concept:name'] in end_activities):

            if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_absolute):
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
            else:
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"])+1)


    ###################

    # case frequency
    follower_dictionary_case={}
    already_use_case=[]

    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):

            k=j+1
            if(log[i][j]['concept:name'] not in already_use_case):
                already_use_case.append(log[i][j]['concept:name'])

                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                    if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_case):
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                    else:
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
        already_use_case=[]

    for i in range(0, len(log)):
        j=0
        
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
        
            if(log[i][j]['concept:name'] in start_activities):

                if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_case):
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
                else:
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']])+1)
        already_use_case=[]
                    
            
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
                    
            k=len(log[i])-1
            if(log[i][k]['concept:name'] in end_activities):

                if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_case):
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
                else:
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"])+1)

        already_use_case=[]
                        
    ###################

    #max frequency
    follower_dictionary_max={}
    follower_max_dictionary_trace={}
    content=""

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
    #         for k in range(j, len(log[i])):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                content=log[i][j]['concept:name']+"#"+log[i][k]['concept:name']
                if(content not in follower_max_dictionary_trace):
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
                    
            if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] in follower_dictionary_max):
                if(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])>int(follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])):
                    follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
                
            else:
                follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
        
        follower_max_dictionary_trace={}

    return str(follower_dictionary_absolute)+"*"+str(follower_dictionary_case)+"*"+str(follower_dictionary_max)

@app.route('/variants', methods=['GET', 'POST'])
def variants():
    if(1):
        #global log_path
        log = xes_importer.apply(session["log_path"])
        log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
        variants = variants_filter.get_variants(log)
        # global start_case
        # start_case= False

    
    variantsDict = '{'

    j=0
    for var, trace in variants.items():

        
        cases = len(trace)
        info = (list(variants.values())[j][0])
        info = info.__getattribute__('attributes')
        #Apri la variante
        if("variant-index" in info):
            variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            caseName = info['concept:name']

            
            variantsDict = variantsDict + '{"'+str(caseName)+'":['

            for x in trace[i]:
                timestamp = x['time:timestamp']
                x['time:timestamp'] = str(timestamp)
                start_timestamp = x['start_timestamp']
                start_timestamp_back=start_timestamp
                x['start_timestamp'] = str(start_timestamp)

                stringX = str(x).replace("'",'"')

                variantsDict = variantsDict + '' + stringX #+', '
            variantsDict = variantsDict + ']}' # chiude ogni caso
        variantsDict = variantsDict + ']' # chiude ogni variante
        j =j+1
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')        
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')

    return variantsDict

@app.route('/filterScan', methods=['GET'])
def filterScan():
    variant_list= request.args.get('variantList') #
    variant_list_array=json.loads(variant_list)
    # global log
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    #global boolean_case
    session["boolean_case"]=False
    #variants = variants_filter.get_variants(log)
    filtered_log = pm4py.filter_variants(log,variant_list_array)
    log = filtered_log
    
    dataframe = pm4py.convert_to_dataframe(filtered_log)
    session["log"]= dataframe.to_dict()
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    session["boolean_case"]=True

    # start_case=True
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    
    #print("Freq: "+str(act)+" "+str(path))

    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)

    gviz_f = dfg_visualization.apply(dfg_f, log=log,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
    f=str(gviz_f)


    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    # dfg_p = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    
    gviz_p = dfg_visualization.apply(dfg_p, log=log,  
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})

    p=gviz_p
    


    alternative_variants_array=[]

    result = str(f)+"|||"+str(p)
    # result=str(variantsDict)
    # print(variantsDict)
    # start_case=True
    return result 
 
@app.route('/filter', methods=['GET', 'POST'])
def filter():
    # GET request
    if request.method == 'GET':
        __min_sec = request.args.get('min')
        __max_sec = request.args.get('max')

        __min_event = request.args.get('minevent')
        __max_event = request.args.get('maxevent')
        # print(__min_event);
        # print(__max_event)

        myPathF = request.args.get('myPathF')
        myActF = request.args.get('myActF')
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        
        perfCheck = request.args.get('perf_checked')

        __Start = request.args.get('start')
        __End = request.args.get('end')

        filterTime = request.args.get('filterTime')
        timeframe = request.args.get('timeframe')
        
        filterPerf = request.args.get('filterPerf')  
        perfFrame = request.args.get('perfFrame') #

        filterAttr = request.args.get('filterAttr')  
        attrFrame = request.args.get('attrFrame') #
        attrFrameFilt = request.args.get('attrFilt') #

        list_attr= request.args.get('listattr') #
        plusMode= request.args.get('plusmode')

    list_attr=list_attr.replace("@",".")
              
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    if filterPerf == None:
        filterPerf = "false";
        
    if filterTime == None:
        filterTime = "false";

    if filterAttr == None:
        filterAttr = "false";
        
    
    
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathF' : myPathF, 'myActF' : myActF, 'myPathP' : myPathP, 'myActP' : myActP, 'min' : __min_sec, 'max' : __max_sec, 'start' : __Start, 'end' : __End, 'timeframe' : timeframe, "filterTime": filterTime, "filterPerf": filterPerf}
    # p = requests.get(path.strip('\n')+'filter', params = paramsP)

    # #print(request.form.get('updated'))
    # if request.form.get('updated') != None:
    #     f = request.files['file']
    #     if f.filename != '': 
    #       #f.save("event logs/" + f.filename)
    #       f.save(log_path)
    #       return home(f.filename)      
    # global start_case
    
    # global log
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    

    # if(start_case==False):
        # log = xes_importer.apply(log_path)
        # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
        #                                                     constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
        #                                                     constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
        #open text file
    # text_file = open('C:\\Users\\jacop\\Desktop\\xp.txt', "w")
    
    # #write string to file
    # text_file.write(str(log))
    
    # #close file
    # text_file.close()
    variants = variants_filter.get_variants(log)
    
    
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    from pm4py.algo.filtering.log.cases import case_filter
    from dateutil.parser import parse
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    from pm4py.algo.filtering.log.attributes import attributes_filter
    
    # GET
    if __Start == None:
        start = 0;
    else:
        start = __Start
    
    if __End == None:
        end = 0;
    else:
        end = __End

    if __min_sec == None:
        min_sec = 0;
    else:
        min_sec = float(__min_sec)
    
    if __max_sec == None:
        max_sec = 100*3,154e+7;
    else:
        max_sec = float(__max_sec)
        

    if __max_event == None or __max_event == "":
        max_event = 100*3,154e+7;
    else:
        max_event = int(__max_event)

    if __min_event == None or __min_event == "":
        min_event = 0;
    else:
        min_event = int(__min_event)

    #global boolean_case
    session["boolean_case"]=False

    # log = xes_importer.apply(log_path)

    if filterTime == "true":
        if timeframe == 'contained':
            # print("timeframe contained")
            
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_contained(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            
    
        elif timeframe == 'intersecting':
            # print("timeframe intersecting")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'started':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            session["boolean_case"]=True
            variants = variants_filter.get_variants(log)
            filtered_log= pm4py.filter_log(lambda x: str(x[0]['start_timestamp'])>=(start) and str(x[0]['start_timestamp'])<=(end), log)
            # filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'completed':
            # print("timeframe completed")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            session["boolean_case"]=True
            variants = variants_filter.get_variants(log)
            # print(x[len(x)-1]['time:timestamp'])
            # print(end)
            # print(parse(end))
            filtered_log= pm4py.filter_log(lambda x: str(x[len(x)-1]['time:timestamp'])<=(end) and str(x[len(x)-1]['time:timestamp'])>=(start), log)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'trim':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.apply_events(log, start, end)
            # print(start)
            # print(end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            
            # print(len(log))
    # else:
        # print("else")
        # log = xes_importer.apply(log_path)
        # variants = variants_filter.get_variants(log)
        # filtered_log = log



    if filterPerf == "true":

        if perfFrame=="caseDuration":
            # print("performance case duration")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = case_filter.filter_case_performance(log, min_sec, max_sec)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif perfFrame == 'eventsNumber':
            # __min_event
            # print("performance eventsNumber")
            # log = xes_importer.apply(session["log_path"])
            variants = variants_filter.get_variants(log)
            
    
            filtered_log = pm4py.filter_log(lambda x: len(x) >=min_event and len(x) <=max_event , log)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

    # else:
        # print("else")
        # log = xes_importer.apply(log_path)
        # variants = variants_filter.get_variants(log)
        # filtered_log = log


    activity_list=[]
    resource_list=[]
    cost_list=[]
    cost_list_2=[]
    
    caseId_list=[]
    resource_list=list_attr.split(",")
   
    variant_list=[]

    if filterAttr == "true":

        if attrFrame == "activity":
            # print("attr activity")
            activity_list=list_attr.split(",")
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            
        elif attrFrame == 'resource':
            # print("attr resource")
            
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                
                # print(log)

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                


        elif attrFrame == 'costs':
            # print("attr costs")
            cost_list=list_attr.split(",")
            # print(cost_list)
            
            for i in cost_list:
                cost_list_2.append(float(i))
            
            '''
            if attrFrameFilt=="selected":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, cost_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="mandatory":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="forbidden":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log) 
                filtered_log = attributes_filter.apply(log, cost_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
            '''
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, cost_list_2 ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2 ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

        elif attrFrame == 'variants':
            # print("attr variants")
            
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            #print(plusMode)
            if(plusMode=="1"):
                #print("plusmode")
                variant_list_array=json.loads(list_attr)
                filtered_log = pm4py.filter_variants(log,variant_list_array)
                #filtered_log = attributes_filter.apply_trace_attribute(log, variant_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
            else:
                variant_list=list_attr.split(",")
                filtered_log = attributes_filter.apply_trace_attribute(log, variant_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "variant", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            


        elif attrFrame == 'caseID':
            # print("attr caseID")
            caseId_list=list_attr.split(",")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            filtered_log = attributes_filter.apply_trace_attribute(log, caseId_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log  
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)     
                 

    
    # else:
        # print("else")
        # filtered_log = log

    # f = createGraphFReduced(filtered_log)
    # p = createGraphPReduced(filtered_log)
    # variants = variants_filter.get_variants(filtered_log)


    # print(len(log))

    # f = createGraphFReduced(log)
    # p = createGraphPReduced(log)
    variants = variants_filter.get_variants(log)
    # text_file = open('C:\\Users\\jacop\\Desktop\\xp2.txt', "w")
        
    #write string to file
    # text_file.write(str(log))
    
    #close file
    # text_file.close()

    # prova f
    # if(start_case==False):
        # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
        #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
        #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    session["boolean_case"]=True

    # start_case=True
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    
    #print("Freq: "+str(act)+" "+str(path))

    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)

    gviz_f = dfg_visualization.apply(dfg_f, log=log,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
    f=str(gviz_f)


    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    # dfg_p = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    
    gviz_p = dfg_visualization.apply(dfg_p, log=log,  
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})

    p=gviz_p
    
    
    
    alternative_variants_array=[]
    
    #result = str(f)+"|||"+str(p)+"|||"+str(variantsDict)
    result = str(f)+"|||"+str(p)
    

    # result=str(variantsDict)
    # print(variantsDict)
    # start_case=True
    #return result+"£"+str(alternative_variants_array)
    
    return result   

    # ******************************************************************************
    # ******************************************************************************

@app.route('/usedvariable', methods=['GET', 'POST'])
def usedvariable():
    #global log_path
    log = xes_importer.apply(session["log_path"])
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    activities = attributes_filter.get_attribute_values(log, "concept:name")
    resources = attributes_filter.get_attribute_values(log, "org:resource")
    resources_cost = json.dumps(attributes_filter.get_attribute_values(log, "resourceCost")) 
    caseid = attributes_filter.get_trace_attribute_values(log, "concept:name")
    variant = attributes_filter.get_trace_attribute_values(log, "Variant")

    stringX = str(str(activities)+"*"+str(resources)+"*"+str(resources_cost)+"*"+str(caseid)+"*"+str(variant)).replace("'",'"')

    return stringX

@app.route('/initialVariantAction', methods=['GET', 'POST'])
def initialVariantAction():
    #global log
    #global log_duplicate
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    #log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                        constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                        constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dataframe_duplicate=pd.DataFrame(session["log_duplicate"])
    log_duplicate= pm4py.convert_to_event_log(dataframe_duplicate)
    
    variants = variants_filter.get_variants(log)
    
    alternative_variants_array=[]
    
    response_string = json.dumps(variants, cls=MyJSONEncoder)
    response_json=json.loads(response_string, object_hook=object_hook)

    return response_json

@app.route('/initialAction', methods=['GET', 'POST'])
def initialAction():
    #global log
    #global log_duplicate
    #global log_path
    import simplejson
    print(session["log_path"])
    #log=xes_importer.apply(session["log_path"])
    
    log_clone = xes_importer.apply(session["log_path"])
    #log_clone = log

    log_clone = interval_lifecycle.assign_lead_cycle_time(log_clone, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dataframe = pm4py.convert_to_dataframe(log_clone).replace(np.nan, 'None')
    #print(pd.DataFrame.from_dict(dataframe.to_dict()).replace(np.nan, 'None'))
    #print(dataframe)
    session["log"]= (dataframe.to_dict())
    session["log_duplicate"] = (dataframe.to_dict())
    
    

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    

    #session["log_duplicate"]=session["log"]
    #dataframe_duplicate=pd.DataFrame(session["log_duplicate"])
    #log_duplicate= pm4py.convert_to_event_log(dataframe_duplicate)

    print(log_clone==log)
    
    #log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                        constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                        constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    
    #log_duplicate=log

    #global dfg
    dfg=session["dfg"]
    
    #DFG - process discovery
    #dfg_freq = dfg_discovery.apply(log)
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    session["dfg"]=dfg
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    grafo_frequency=(str(gviz_freq))
    #fine graph frequency______________________________________________________________________________________________________________

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters

    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})

    

    grafo_performance=(str(gviz_perf))
    #fine graph performance______________________________________________________________________________________________________________

    activities = attributes_filter.get_attribute_values(log, "concept:name")
    resources = attributes_filter.get_attribute_values(log, "org:resource")
    resources_cost = json.dumps(attributes_filter.get_attribute_values(log, "resourceCost")) 
    caseid = attributes_filter.get_trace_attribute_values(log, "concept:name")
    variant = attributes_filter.get_trace_attribute_values(log, "variant")

    stringX = str(str(activities)+"*"+str(resources)+"*"+str(resources_cost)+"*"+str(caseid)+"*"+str(variant)).replace("'",'"')

    variabili_usare=(stringX)
    #fine all used variable______________________________________________________________________________________________________________  

    
    import time
    

    
    #global activity_list
    session["activity_list"] = []
    for i in range(0, len(log)):
        for j in range(0, len(log[i])):
            if(log[i][j]['concept:name'] not in session["activity_list"]):
                session["activity_list"].append(log[i][j]['concept:name'])

    # global activity_dictionary
    activity_dictionary=dict.fromkeys(session["activity_list"])
    
    for a in session["activity_list"]:
        activity_dictionary[a]=[]
    
    for i in range(0, len(log)):
        for p in range(0, len(log[i])):
            activity_dictionary[log[i][p]['concept:name']].append(log[i][p]['@@duration'])

    
    mean_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        mean_dizionario[j]=statistics.mean(activity_dictionary[j])
        
    total_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        total_dizionario[j]=sum(activity_dictionary[j])
        
    median_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        median_dizionario[j]=activity_dictionary[j][int(len(activity_dictionary[j])/2)]
        
    max_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        max_dizionario[j]=max(activity_dictionary[j])
        
        
    min_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        min_dizionario[j]=min(activity_dictionary[j])

    activity_durata=(str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario))
    #fine all duration_______________________________________________________________________________________________________________________

    import time    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    follower_dictionary={}
    follower_list=[]
    follower_used_list=[]
    import datetime
    

    already_sum=[]

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            for k in range(j, len(log[i])):
                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg and j!=k and ((log[i][j]['concept:name']==log[i][k]['concept:name'] and j+1==k )or (log[i][j]['concept:name'],log[i][k]['concept:name']) not in already_sum )):  
                    already_sum.append((log[i][j]['concept:name'],log[i][k]['concept:name']))         
                    if((log[i][j]['concept:name'],log[i][k]['concept:name']) not in follower_list):
                        follower_list.append((log[i][j]['concept:name'],log[i][k]['concept:name']))
                        follower_used_list.append(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'])
                        follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=[]

                    time_result=log[i][k]['start_timestamp']-log[i][j]['time:timestamp']
                    time_result=time_result.total_seconds()
                    follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']].append(time_result)
                    
        already_sum=[]


   
    mean_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        mean_edge_dizionario[j]=sum(follower_dictionary[j])/len(follower_dictionary[j])

    total_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        total_edge_dizionario[j]=sum(follower_dictionary[j])

    median_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        median_edge_dizionario[j]=statistics.median(follower_dictionary[j])

    max_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        max_edge_dizionario[j]=max(follower_dictionary[j])

    min_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        min_edge_dizionario[j]=min(follower_dictionary[j])

    durata_edge=(str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario))
    

    #___________________________________________________________________________


    
    
    import time
    


    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    dictionary_absolute={}

    for trace in log:
        for ev in trace: 
            if(ev['concept:name'] in dictionary_absolute):
                dictionary_absolute[ev['concept:name']]=str(int(dictionary_absolute[ev['concept:name']])+1)
            else:
                dictionary_absolute[ev['concept:name']]=str(1)

    
    ###################

    #CASE FREQ
    dictionary_case={}
    already_case=[]

    for trace_case in log:
        for ev_case in trace_case:
            if(ev_case['concept:name'] not in already_case):
                already_case.append(ev_case['concept:name'])
                if(ev_case['concept:name'] in dictionary_case):
                    dictionary_case[ev_case['concept:name']]=str(int(dictionary_case[ev_case['concept:name']])+1)
                else:
                    dictionary_case[ev_case['concept:name']]=str(1)
                    
        already_case=[]

    ###################

    #max repetititions
    dictionary_max_trace={}
    dictionary_max={}


    for trace_max in log:
        for ev_max in trace_max:
    #         print(precedente)
    #         if(ev_max['concept:name']!=precedente):
            if(ev_max['concept:name'] in dictionary_max_trace):
                dictionary_max_trace[ev_max['concept:name']]=str(int(dictionary_max_trace[ev_max['concept:name']])+1)
            else:
                dictionary_max_trace[ev_max['concept:name']]=str(1)
                
            if(ev_max['concept:name'] in dictionary_max):
                if(int(dictionary_max_trace[ev_max['concept:name']])>int(dictionary_max[ev_max['concept:name']])):
                    dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
                
            else:
                dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
        
        dictionary_max_trace={}


    activity_frequency= str(dictionary_absolute)+"*"+str(dictionary_case)+"*"+str(dictionary_max)

    #___________________________________________________________________________


    
    
    import time
    

    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    #absolute frequency
    follower_dictionary_absolute={}
    

    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_absolute):
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)

    for i in range(0, len(log)):
        j=0
        if(log[i][j]['concept:name'] in start_activities):

            if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_absolute):
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
            else:
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']])+1)

        k=len(log[i])-1
        if(log[i][k]['concept:name'] in end_activities):

            if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_absolute):
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
            else:
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"])+1)



   
   
   
   
   
   
    ###################

    # case frequency
    follower_dictionary_case={}
    already_use_case=[]


    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):

            k=j+1
            if(log[i][j]['concept:name'] not in already_use_case):
                already_use_case.append(log[i][j]['concept:name'])

                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                    if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_case):
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                    else:
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
        already_use_case=[]
                        


    for i in range(0, len(log)):
        j=0
        
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
        
            if(log[i][j]['concept:name'] in start_activities):

                if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_case):
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
                else:
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']])+1)
        already_use_case=[]
                    
            
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
                    
            k=len(log[i])-1
            if(log[i][k]['concept:name'] in end_activities):

                if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_case):
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
                else:
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"])+1)

        already_use_case=[]

    ###################

    #max frequency
    follower_dictionary_max={}
    follower_max_dictionary_trace={}
    content=""
   

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
    #         for k in range(j, len(log[i])):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                content=log[i][j]['concept:name']+"#"+log[i][k]['concept:name']
                if(content not in follower_max_dictionary_trace):
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
                    
            if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] in follower_dictionary_max):
                if(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])>int(follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])):
                    follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
                
            else:
                follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
        
        follower_max_dictionary_trace={}

    

    frequency_edge= str(follower_dictionary_absolute)+"*"+str(follower_dictionary_case)+"*"+str(follower_dictionary_max)



    #___________________________________________________________________________
     
    variants = variants_filter.get_variants(log)
    # global start_case
    # start_case= False
    
    alternative_variants_array=[]
    
   
    variantsDict=jsonify({"variants": str(variants)})  
    varianti=(variantsDict)
    print(varianti)

    #variants fine_______________________________________________________________________________________________________________________




    session["dfg"]=dfg
    #___________________________________________________________________________

    #return jsonify({"grafo_frequency": grafo_frequency, "grafo_performance":grafo_performance, "variabili_usare":variabili_usare, "activity_durata":activity_durata,  "alternative_variants_array":str(alternative_variants_array)})
    return grafo_frequency+"£"+grafo_performance+"£"+variabili_usare+"£"+activity_durata+"£"+durata_edge+"£"+activity_frequency+"£"+frequency_edge

@app.route('/swipeRemoveAction', methods=['GET', 'POST'])
def swipeRemoveAction():
    # global log
    # global log_duplicate

    # log=log_duplicate

    # dataframe1=pd.DataFrame(session["log"])
    # log = pm4py.convert_to_event_log(dataframe1)

    dataframe_duplicate=pd.DataFrame(session["log_duplicate"])
    log_duplicate= pm4py.convert_to_event_log(dataframe_duplicate)

    log=log_duplicate
    
    dataframe = pm4py.convert_to_dataframe(log)
    session["log"]= dataframe.to_dict()
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    ####start frequency_______________________________________________________________________________________
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    grafo_frequency=(str(gviz_freq))

    ####end frequency_________________________________________________________________________________________


    ####start performance_____________________________________________________________________________________
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters

    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})

    

    grafo_performance=(str(gviz_perf))

    ####end performance_______________________________________________________________________________________

    #return grafo_frequency+"£"+grafo_performance+"£"+varianti+"£"+str(alternative_variants_array)
    return grafo_frequency+"£"+grafo_performance

@app.route('/conformanceChecking', methods=['GET', 'POST'])
def conformanceChecking():
    # global log
    # global dfg
    dfg=session["dfg"]

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    activities = pm4py.get_event_attribute_values(log, "concept:name")
    from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
    from pm4py.algo.discovery.alpha import algorithm as alpha_miner
    dfg_f=session["dfg_f"]
    if(dfg_f!=None):
        dfg_conf =dfg_f
    else:
        dfg_conf = dfg

    # Print the current working directory
    working_dir=os.getcwd()
    #global backup_dir
    session["backup_dir"]=working_dir
    print("Current working directory: {0}".format(os.getcwd()))
    os.chdir(working_dir+'/jar')
    

    # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
    
    # subprocess.call(['bash', './run_SYMBA_all'])

    os.chdir(working_dir)

    from pm4py.objects.conversion.dfg import converter as dfg_mining
    # net, im, fm = dfg_mining.apply(dfg_conf)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    from pm4py.objects.conversion.log import converter as log_converter
    static_event_stream = log_converter.apply(log, variant=log_converter.Variants.TO_EVENT_STREAM)
    from pm4py.algo.discovery.inductive import algorithm as inductive_miner
    tree = inductive_miner.apply_tree(log, variant=inductive_miner.Variants.IMf)
    from pm4py.objects.conversion.process_tree import converter as pt_converter
    net, im, fm = pt_converter.apply(tree)

    # net, im, fm = alpha_miner.apply(log)
 
    gviz = pn_visualizer.apply(net, im, fm)
    # pn_visualizer.view(gviz)
    places = net.places
    transitions = net.transitions
    arcs = net.arcs
    trst=[]

    for tr in transitions:
        # print((str(tr.name)+" "+str(tr.label)))
        trst.append([str(tr.name), str(tr.label)])

    from pm4py.objects.petri_net.exporter import exporter as pnml_exporter
    # pnml_exporter.apply(net, im, "petri.pnml")




    pnml_exporter.apply(net, im, pnml_path, final_marking=fm)
    with open(marking_path, 'w') as f:
        f.write(str(im))
        f.write('\n')
        f.write(str(fm))

    xes_exporter.apply(log, xes_path)

    

    with open(cost_file_path, "w") as f:
        for index in trst:
            if(index[1].lower().replace(" ", "")=="none"):
                f.write(index[0].lower().replace(" ", "")+" 0 0")
            else:
                f.write(index[1].lower().replace(" ", "")+" 1 1")    
            f.write('\n')
        #f.write("none"+" 0 0") remove comment to consider invisible transitions
        f.close()

    session["dfg"]=dfg

    return str(gviz)+"£"+str(im)+"£"+str(fm)+"£"+str(list(activities))+"£"+str(trst)

@app.route('/jarCalling', methods=['GET', 'POST'])
def jarCalling():
    minLen = str(request.args.get('minLen'))
    # print(minLen)
    maxLen = str(request.args.get('maxLen'))
    # print(maxLen)
    planner = str(request.args.get('planner'))
    # print(planner)
    duplicate = str(request.args.get('duplicate'))
    # print("duplicate: "+duplicate)


    import os

    # Print the current working directory
    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))
    os.chdir(working_dir+'/jar')
    pnmlPath="../net/petri_final_remap.pnml"
    xesPath="../net/petri_log.xes"
    costPath="cost_file"

    #os.system("java -jar traceAligner.jar align "+pnmlPath+" "+xesPath+" "+costPath+" "+minLen +" "+maxLen+" "+planner+" "+duplicate)
    global process_jar
    process_jar = subprocess.Popen( "java -jar traceAligner11.jar align "+pnmlPath+" "+xesPath+" "+costPath+" "+minLen +" "+maxLen+" "+planner+" "+duplicate , shell=True)
    
    try:
        print('Running in process', process_jar.pid)
        process_jar.wait()
    except subprocess.TimeoutExpired:
        print('Timed out - killing', process_jar.pid)
        process_jar.kill()
    print("\njar done")
    
    
    print("  \n")
    global plans_path
    global process_script
    
    if(planner=="FD"):    
        plans_path="./jar/fast-downward/src/plans"
        
        process_script=subprocess.Popen(['bash', './run_FD_all'])
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
    
    elif(planner=="SYMBA"):
        plans_path="./jar/seq-opt-symba-2/plans"
        
        process_script=subprocess.Popen(['bash', './run_SYMBA_all'])
        #os.system("bash ./run_SYMBA_all")
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
        
    else:
        plans_path="./jar/fast-downward/src/plans"
        
        process_script=subprocess.Popen(['bash', './run_FD_all'])
        #os.system("bash ./run_FD_all")
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
       
    os.chdir(working_dir)

    return "done"

@app.route('/costFile', methods=['POST'])
def costFile():
    costHeader=request.headers.get('Contenuto')
    costJson = json.loads(costHeader)

    with open(cost_file_path, "w") as f:
        for singleCost in costJson:
            f.write(singleCost.lower().replace(" ", "")+" "+str(costJson[singleCost][0])+" "+str(costJson[singleCost][1]))    
            f.write('\n')
        f.close()

    return "done"

@app.route('/traceDetail', methods=['GET'])
def traceDetail():
    allTraceName=""

    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))

    global plans_path

    os.chdir(plans_path)
    for fileName in glob.glob("*.txt"):
        # print(fileName)
        traceIndex=(fileName.replace("out","").replace(".txt",""))
        allTraceName=allTraceName+traceIndex+"#"
        
    allTraceName = allTraceName[:-1]
    os.chdir(working_dir)
    
    return allTraceName

@app.route('/updateTraceDetail', methods=['GET'])
def updateTraceDetail():
    nameTrace = str(request.args.get('nameTrace'))
    global plans_path
  
    file_name = os.path.basename(plans_path+'/'+nameTrace)
    with open(plans_path+'/'+nameTrace) as f:
        trace = f.readlines()

    start_index=len(trace)
    start_string="Actual search time:"

    end_index=len(trace)
    end_string="Plan length:"

    plan_cost_string="Plan cost:"
    
    plan_cost=""
    plan_length=""
    search_time=""

    alignment=""
    i=0

    while(i<len(trace)-1):
        # print(trace[i].strip())
        if(start_string in trace[i].strip()):
            # print("\ninizio: ")
            start_index=i
            search_time=trace[i].strip().split(" [",1)[1].replace("t=","").replace("]","")

        if(end_string in trace[i+1].strip()):
            # print("fine: \n")
            end_index=i+1
            plan_length=trace[i+1].strip().split("Plan length: ",1)[1].replace(".","").strip()

        
        if(plan_cost_string in trace[i+1].strip()):
            plan_cost=trace[i+1].strip().split("Plan cost:",1)[1].strip()
        
        if(i>start_index and i<end_index):
            alignment=alignment+trace[i].strip()+"\n"
        
        i=i+1
    
    return alignment+"$"+search_time+"$"+plan_length+"$"+plan_cost

@app.route('/generalTraceInfo', methods=['GET'])
def generalTraceInfo():
    dict_event = {}
    dict_trace = {} 
    temp_array = [] 
  
    dict_skip_ins ={} 

    working_dir=os.getcwd()

    # plans_path="./jar/fast-downward/src/plans" 
    # plans_path="./jar/seq-opt-symba-2/plans"
    global plans_path
    os.chdir(plans_path)
    for fileName in glob.glob("*.txt"):
        # print(fileName)
        traceIndex=(fileName.replace("out","").replace(".txt",""))
        # print(traceIndex)

        with open('./'+str(fileName)) as f:
            trace = f.readlines()

        start_index=len(trace)
        start_string="Actual search time:"

        end_index=len(trace)
        end_string="Plan length:"

        alignment=""
        i=0

        while(i<len(trace)-1):
            # print(trace[i].strip())
            trace_to_add=trace[i].strip()
            if(start_string in trace_to_add):
                # print("\ninizio: ")
                start_index=i

            if(end_string in trace[i+1].strip()):
                # print("fine: \n")
                end_index=i+1
            
            if(i>start_index and i<end_index):
                alignment=alignment+trace_to_add+"\n"

            trace_to_analyse=trace_to_add.split("#")

            if(trace_to_analyse[0]=="movesync"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    # temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][0]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][0]+1

                
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][0]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][0]+1
                #print("nessuna mossa: "+trace_to_analyse[1] )
                #print(dict_event)
                #print(dict_trace)    



            elif(trace_to_analyse[0]=="moveinthemodel"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][1]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][1]+1
                
            
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][1]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][1]+1
                #print("mossa nel modello: "+trace_to_analyse[1].split("  ")[0].strip() )
                #print(trace_to_analyse[1].split("  ")[0].strip())

                if (trace_to_analyse[1].split("  ")[0].strip() not in dict_skip_ins):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()]=[[],[]] 
                
                if(('Trace '+str(traceIndex)) not in dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][1]):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][1].append('Trace '+str(traceIndex))

            
            elif(trace_to_analyse[0]=="moveinthelog"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][2]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][2]+1

                #print(trace_to_analyse[1].split("  ")[0].strip())
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][2]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][2]+1
                #print("mossa nel log: "+trace_to_analyse[1].split("  ")[0].strip() )
                #print(dict_event)
                #print(dict_trace)

                if (trace_to_analyse[1].split("  ")[0].strip() not in dict_skip_ins):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()]=[[],[]] 
                
                if(('Trace '+str(traceIndex)) not in dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][0]):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][0].append('Trace '+str(traceIndex))
            
            
            i=i+1
        temp_array=[]
    
    os.chdir(working_dir)
    # print(dict_event)
    # print("-----------------------------------------------") 
    # print(dict_trace)
    # print("-----------------------------------------------")
    # print(dict_skip_ins)

    return str(dict_event)+"#"+str(dict_trace)+"#"+str(dict_skip_ins)

@app.route('/mapPnml', methods=['POST'])
def mapPnml():
    costHeader=request.headers.get('Replace_content')
    
    replace_array=costHeader.split("#")

    x=replace_array[0] 
    y=replace_array[1] 

    with open('./net/petri_final.pnml') as f:
        trace = f.readlines()

    response=""

    for line in trace:
        if "<text>"+x+"</text>" in line:
            substi=line.replace(x,y)
        else:
            substi=line
           
        response=response+substi

    f = open("./net/petri_final_remap.pnml", "w")
    f.write(response)
    f.close()
    
    return "prova"

@app.route('/getPnmlExistence', methods=['GET'])
def getPnmlExistence():
    file_exists = exists("./net/petri_final_remap.pnml")
    return str(file_exists)

@app.route('/createRemap', methods=['POST'])
def createRemap():

    with open('./net/petri_final.pnml') as f:
        trace = f.readlines()

    response=""

    for line in trace:
        substi=line
        
        response=response+substi

    f = open("./net/petri_final_remap.pnml", "w")
    f.write(response)
    f.close()

    return "prova"
    
@app.route('/deleteRemap', methods=['POST'])
def deleteRemap():
    file_exists = exists("./net/petri_final_remap.pnml")

    if(file_exists):
        os.remove("./net/petri_final_remap.pnml")    

    return "prova"

@app.route('/queryDb', methods=['GET', 'POST'])
def queryDb():
    #global log_path
    #global databaseName
    print(session["databaseName"])
    print(session["log_path"])
    #databaseName="TestDB"
    #databaseName="datacloud"
    #runningXesPath="../event\ logs/running-example.xes"
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

@app.route('/initializeQuery', methods=['GET', 'POST'])
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
        # print(row[1])
        print(row)
        response=response+str(row)+"\n"
        row = cursor1.fetchone()

    cursor1.close()
    cnxn.close()

    return response



@app.route('/makeQuery', methods=['GET', 'POST'])
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

    ##Sample select query DROP TABLE IF EXISTS log;
    cursor.execute("DROP TABLE IF EXISTS log_db CASCADE;")
    cursor.execute("select e.trace_id, e.name as EventName, e.time, a.key, eha.value into log_db \
                    from attribute a, event e, event_has_attribute eha \
                    where e.id=eha.event_id and a.id=eha.attr_id") 
    cursor.close()

 

    if (("delete" in queryTODO) or ("drop" in queryTODO) or ("insert" in queryTODO)):
        return "errore,sql"+"£"+"errore,sql"

      

    sql_query = sqlvalidator.parse(queryTODO)
    if not sql_query.is_valid():
        print(sql_query.errors)
        return "errore,query"+"£"+"errore,query"
    else:
        cursor1 = cnxn.cursor()
        cursor1.execute(queryTODO) 

        response=""
        row = cursor1.fetchone() 
        while row: 
            # print(row[1])
            for a in row:
                if(isinstance(a, datetime.datetime)):
                    response=response+a.strftime("%d/%m/%Y %H:%M:%S")+","
                else:
                    response=response+str(a)+","
            response=response[:-1]+"\n"

            #print(row[0])

            #print(row[1])
            #response=response+str(row)+"\n"
            row = cursor1.fetchone()

        cursor1.close()
        cnxn.close()

    return response+"£"+querySELECT



@app.route('/checkDatabasePresence', methods=['GET'])
def checkDatabasePresence():

    print("ultimotest")
    print(session["databaseName"])

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


p=None

@app.route('/translation1', methods=['GET'])
def translation1():
    global p
    #p=Process(target=queryDb)
    #p.start()
    queryDb()
    print("greve")
    print("ho finito")
    return("done")


@app.route('/translation2', methods=['GET'])
def translation2():
    global p
    createDatabase(session["databaseName"])
    applyDbSchema(session["databaseName"])
    #p=Process(target=queryDb)
    queryDb()

    #p.start()
    print("greve")
    print("ho finito")

    return("done")

@app.route('/checkTranslationEnd', methods=['GET'])
def checkTranslationEnd():
    if(p==None):
        return "false"

    if(p.is_alive()):
        return "true"
    else:
        return "false"


@app.route('/createEventLog', methods=["GET"])
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

@app.route('/dslPost', methods=['POST'])
def dslPost():
    dslHeader=request.headers.get('Dsl')
    dslJson=(json.loads(dslHeader))
    
    #global directory_log

    with open("storage"+"/"+session["user"]+"/"+session["log_name_clear"]+"/"+session["log_name_clear"]+".txt", "w") as f:
        f.write(dslJson['pipeline'])    
        f.close()
    
    return "work_done"

@app.route('/scan', methods=['POST'])
def scan():
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    #log = pm4py.read_xes(log_path)  
    allXESActivities = []
    for trace in log:
        activities = []
        for event in trace:
            activities.append(event["concept:name"])
        allXESActivities.append(activities)           
    allActivities = []
    actWithOccurence = []
    c = Counter()
    for act in allXESActivities:
        c[tuple(act)] += 1
        if act not in allActivities:
            allActivities.append(act)        
    listActivity = takeActions(allActivities) 
    for elem in c:
        list = [c[elem], elem]
        actWithOccurence.append(list)    
    clear()  
    actWithOccurence = sortFirstAscendentOrder(actWithOccurence)
    with open('segments.txt', 'w') as f:
        i = 0
        for line in actWithOccurence:
            f.write(str(line))
            f.write("segment_"+str(i))
            f.write('\n')
            i += 1
    f.close()
    replaceInFile("segments.txt")
    fileHandle = open("segments.txt", "r")
    texts = fileHandle.readlines()
    fileHandle.close()
    fileHandle = open("trace.txt", "w")
    for s in texts:
        fileHandle.write(s)
    fileHandle.close()
    timestamp = takeTimestamp()
    with open(os.path.join(TIMESTAMP_FOLDER, 'log_' + timestamp + '.txt'), 'w+') as file:
        file.write("UPLOAD: " + timestamp + "\n")
    file.close()
    # flash("Successfully loaded", "success")       
    result = takeSegmentFromFile()
    #print(result)
    #print("start scan")
    #global nomeupload
    return jsonify({"activity": listActivity, "nameFile": session["nomeupload"], "segments": result}) 

#__________________________________________________________________________________________________________________________________________________________________________________________________________________

@app.route('/scan/start_activity', methods=['POST'])
def start_activity():
    result, removeSegment = rule_start_activity()
    return jsonify({"result": result, "remove": removeSegment}) 

@app.route('/scan/end_activity', methods=['POST'])
def end_activity():
    result, removeSegment = rule_end_activity()
    return jsonify({"result": result, "remove": removeSegment}) 
          
@app.route('/scan/existence', methods=['POST'])
def existence():
    result, removeSegment = rule_existence()
    return jsonify({"result": result, "remove": removeSegment})  

@app.route('/scan/absence', methods=['POST'])
def absence():
    result, removeSegment = rule_absence()
    return jsonify({"result": result, "remove": removeSegment})               
   
@app.route('/scan/choice', methods=['POST'])
def choice():
    result, removeSegment = rule_choice()
    return jsonify({"result": result, "remove": removeSegment})  

@app.route('/scan/exclusive_choice', methods=['POST'])
def exclusive_choice():
    result, removeSegment = rule_exclusive_choice()
    return jsonify({"result": result, "remove": removeSegment}) 

@app.route('/scan/responded_existence', methods=['POST'])
def responded_existence():
    result, removeSegment = rule_responded_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/response', methods=['POST'])
def response():
    result, removeSegment = rule_response()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/alternate_response', methods=['POST'])
def alternate_response():
    result, removeSegment = rule_alternate_response()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/chain_response', methods=['POST'])
def chain_response():
    result, removeSegment = rule_chain_response()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/precedence', methods=['POST'])
def precedence():
    result, removeSegment = rule_precedence()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/alternate_precedence', methods=['POST'])
def alternate_precedence():
    result, removeSegment = rule_alternate_precedence()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/chain_precedence', methods=['POST'])
def chain_precedence():
    result, removeSegment = rule_chain_precedence()
    return jsonify({"result": result, "remove": removeSegment})  

@app.route('/scan/co_existence', methods=['POST'])
def co_existence():
    result, removeSegment = rule_co_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/succession', methods=['POST'])
def succession():
    result, removeSegment = rule_succession()
    return jsonify({"result": result, "remove": removeSegment})     
 
@app.route('/scan/alternate_succession', methods=['POST'])
def alternate_succession():
    result, removeSegment = rule_alternate_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/chain_succession', methods=['POST'])
def chain_succession():
    result, removeSegment = rule_chain_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/not_co_existence', methods=['POST'])
def not_co_existence():
    result, removeSegment = rule_not_co_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/not_succession', methods=['POST'])
def not_succession():
    result, removeSegment = rule_not_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app.route('/scan/not_chain_succession', methods=['POST'])
def not_chain_succession():
    result, removeSegment = rule_not_chain_succession()
    return jsonify({"result": result, "remove": removeSegment})     

@app.route('/scan/del_rule', methods=['POST'])
def delete_rule():
    result, removeSegment = del_rule()
    order = request.form["order"]
    if order == "ascending": result = sortDescendentOrder(result)
    else: result = sortAscendentOrder(result)
    writeOnSegmentFile(result)
    return jsonify({"result": result, "remove": removeSegment}) 

@app.route('/scan/write_apply', methods=['POST'])
def write_apply():
    fun = request.form["fun"]
    act1 = request.form["act1"]
    act2 = request.form["act2"]
    list_of_files = glob.glob(TIMESTAMP_FOLDER + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("APPLY: " + timestamp + "," + fun + "," + act1 + "," + act2 + "\n")
    file.close()
    return render_template('index.html');

@app.route('/scan/write_delete', methods=['POST'])
def write_delete():
    fun = request.form["fun"]
    act1 = request.form["act1"]
    act2 = request.form["act2"]
    list_of_files = glob.glob(TIMESTAMP_FOLDER + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("DELETE: " + timestamp + "," + fun + "," + act1 + "," + act2 + "\n")
    file.close()
    return render_template('index.html');

@app.route('/scan/show_trace', methods=['POST'])
def show_trace():      
    order = request.form["order"]
    result = takeSegmentFromTrace()
    removeSegment = []
    if(order == "ascending"): result = sortDescendentOrder(result)
    else: result = sortAscendentOrder(result)
    writeOnSegmentFile(result)     
    return jsonify({"result": result, "remove": removeSegment}) 
              
@app.route('/scan/ascending_order', methods=['POST'])
def ascending_order():
    segments = takeSegmentFromFile()
    remove = takeRemoveSegmentFromFile()
    seg_ord = sortAscendentOrder(segments)
    rem_ord = sortAscendentOrder(remove)
    writeOnSegmentFile(seg_ord)
    writeOnRemoveSegmentFile(rem_ord)
    return jsonify({"result": seg_ord, "remove": rem_ord}) 

@app.route('/scan/descending_order', methods=['POST'])
def descending_order():
    segments = takeSegmentFromFile()
    remove = takeRemoveSegmentFromFile()
    seg_ord = sortDescendentOrder(segments)
    rem_ord = sortDescendentOrder(remove)
    writeOnSegmentFile(seg_ord)
    writeOnRemoveSegmentFile(rem_ord)
    return jsonify({"result": seg_ord, "remove": rem_ord}) 
          
@app.route('/scan/download_file')      
def download():
    downloadFile()
    list_of_files = glob.glob(TIMESTAMP_FOLDER + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("EXPORT: " + timestamp + "\n")
    file.close()
    return render_template('index.html');  

@app.route('/scan/clear')
def clearDiv():
    clear()
    list_of_files = glob.glob(TIMESTAMP_FOLDER + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("CLEAR: " + timestamp + "\n")
    file.close()
    return render_template('index.html');
    

#app.run(host=path_f, port=int(port_n))

#context = ('key/certificate.crt', 'key/private.key')    #certificate and key files
context = ('key/localhost/localhost.crt', 'key/localhost/localhostd.key')
app.run(host=path_f, port=int(port_n), debug=True, ssl_context=context)
