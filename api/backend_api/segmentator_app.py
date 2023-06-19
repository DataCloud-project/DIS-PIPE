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


from backend_classes import *

app_Segmentator = Blueprint('app_Segmentator',__name__)

############################################################
#__________________SEGMENTATOR API__________________#
############################################################

@app_Segmentator.route('/segmentator', methods=['GET'])
def segmentator():

    working_dir=os.getcwd()
    #global backup_dir
    
    #print("Current working directory: {0}".format(os.getcwd()))
    
    os.chdir(working_dir+'/AutSeg/segmentator')

    #print("Current working directory: {0}".format(os.getcwd()))
    
    # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
    os.system('./al -d files/UILog.config')
    
    os.chdir(working_dir)

    print("Current working directory: {0}".format(os.getcwd()))

    return "work done"