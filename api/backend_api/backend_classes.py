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

