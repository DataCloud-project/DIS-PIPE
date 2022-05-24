import flask

from flask import request, jsonify, render_template
import os
import requests

IMAGE_FOLDER = os.path.join('static', 'images')

app = flask.Flask(__name__)
app.config["DEBUG"] = True
app.config['UPLOAD_FOLDER'] = IMAGE_FOLDER


from graphviz import Digraph
#import file xes
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
from pm4py.visualization.dfg import visualizer as dfg_visualization

import pm4py
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
#log = xes_importer.apply(log_path)


with open('../properties.txt') as f:
    lines = f.readlines()
    backend=lines[1]
    backend = backend.split(': ')
    backend = backend[1]
    backend = backend.split('//')
    path = backend[1].split(':')[0]
    port_n = backend[1].split(':')[1]
    port_n = port_n.split('/')[0]
    #print(path)
    #print(port_n)
    
f.close()

import platform
if platform.system() == "Windows":
        log_path = 'event logs\\running-example.xes'
if platform.system() == "Linux":
        log_path = 'event logs/running-example.xes'
        
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
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #DFG - process discovery
    #dfg_perf = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    #parameters = {dfg_visualization.Variants.PERFORMANCE.value.Parameters.FORMAT: "svg"}
    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
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
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    gviz_f = dfg_visualization.apply(dfg_p, log=log, variant=dfg_visualization.Variants.PERFORMANCE, 
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})
                                                
    return gviz_f
    
        
@app.route('/dfgFrequency', methods=['GET'])
def dfgFrequency():
    log = xes_importer.apply(log_path)
    return str(createGraphF(log))

@app.route('/dfgPerformance', methods=['GET'])
def dfgPerformance():
    log = xes_importer.apply(log_path)
    return str(createGraphP(log))

@app.route('/variants', methods=['GET'])
def variants():
    log = xes_importer.apply(log_path)

    variants = variants_filter.get_variants(log)
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

@app.route('/dfgFreqReduced', methods=['GET', 'POST'])
def dfgFreqReduced():
    log = xes_importer.apply(log_path)
    return str(createGraphFReduced(log))

@app.route('/dfgPerfReduced', methods=['GET', 'POST'])
def dfgPerfReduced():
    log = xes_importer.apply(log_path)
    return str(createGraphPReduced(log))


@app.route('/filter', methods=['GET', 'POST'])
def filter():
    log = xes_importer.apply(log_path)
    variants = variants_filter.get_variants(log)
    
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    from pm4py.algo.filtering.log.cases import case_filter
    
    # GET
    if request.args.get('start') == None:
        start = 0;
    else:
        start = request.args.get('start')
    
    if request.args.get('end') == None:
        end = 0;
    else:
        end = request.args.get('end')

    if request.args.get('min') == None:
        min_sec = 0;
    else:
        min_sec = float(request.args.get('min'))
    
    if request.args.get('max') == None:
        max_sec = 100*3,154e+7;
    else:
        max_sec = float(request.args.get('max'))
        

    if request.args.get('filterTime') == "true":
        if request.args.get('timeframe') == 'contained':
            filtered_log = timestamp_filter.filter_traces_contained(log, start, end)
            log = filtered_log
    
        elif request.args.get('timeframe') == 'intersecting':
            filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log
    else:
        filtered_log = log

    if request.args.get('filterPerf') == "true":
        filtered_log = case_filter.filter_case_performance(log, min_sec, max_sec)
        log = filtered_log
    
    else:
        filtered_log = log

    f = createGraphFReduced(filtered_log)
    p = createGraphPReduced(filtered_log)
    
    '''
    variantsDict = '{'

    cases = len(filtered_log)
    print(cases)
    j=0
    for i in range(0, cases):
        j=j+1
        info = filtered_log[i].__getattribute__('attributes')
        caseName = info['concept:name']
        if ("variant-index" in info):
            varIndex = info['variant-index']
            if (i == 0):
                variantsDict = variantsDict + '"' + str(varIndex) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        variantsDict = variantsDict + '{"' + str(caseName) + '":['

        for x in filtered_log[i]:
            timestamp = x['time:timestamp']
            x['time:timestamp'] = str(timestamp)
            stringX = str(x).replace("'", '"')
            variantsDict = variantsDict + '' + stringX  # +', '
        
        variantsDict = variantsDict + ']}'  # chiude ogni caso
        if ("variant-index" not in info):
            variantsDict = variantsDict + ']' # chiude ogni variante
    if ("variant-index" in info):
        variantsDict = variantsDict + ']' # chiude ogni variante
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    '''
    variantsDict = '{'

    j=-1
    for var, trace in variants.items():

        j =j+1
        cases = len(trace)
        #print("Numero cases var "+str(j)+": "+str(cases))
        varEmpty = True
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            #print("info: "+str(info))
            caseName = info['concept:name']
            #print(trace[i])
            inFilter = False
            for k in range(0,len(filtered_log)):
                infoFiltered = filtered_log[k].__getattribute__('attributes')
                filteredCaseName = infoFiltered['concept:name']
                if(filteredCaseName == caseName):
                    inFilter = True
                    break

            if(inFilter == False):
                break
            if(i==0):
                varEmpty = False
                if("variant-index" in info):
                    variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
                else:
                    variantsDict = variantsDict + '"' + str(j) + '": ['
                #variantsDict = variantsDict + '"' + str(j) + '": ['
            variantsDict = variantsDict + '{"'+str(caseName)+'":['
            #print("Trac i len: "+str(len(trace[i])))
            for x in trace[i]:
                timestamp = x['time:timestamp']
                x['time:timestamp'] = str(timestamp)
                stringX = str(x).replace("'",'"')
                variantsDict = variantsDict + '' + stringX #+', '
            variantsDict = variantsDict + ']}' # chiude ogni caso
        if(varEmpty == False):
            variantsDict = variantsDict + ']' # chiude ogni variante

    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    #variantsDict = variantsDict.replace('}"','},"')
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')
    
    result = str(f)+"|||"+str(p)+"|||"+str(variantsDict)
    
    return result

'''
@app.route('/petriNetFreq', methods=['GET'])
def petriNetFreq():
    log = xes_importer.apply(log_path)
    
    net, initial_marking, final_marking = inductive_miner.apply(log)
    parameters = {pn_visualizer.Variants.FREQUENCY.value.Parameters.FORMAT: "png"}
    gviz_pnf = pn_visualizer.apply(net, initial_marking, final_marking, parameters=parameters, variant=pn_visualizer.Variants.FREQUENCY, log=log)
    
    return str(gviz_pnf)
    
@app.route('/petriNetPerf', methods=['GET'])
def petriNetPerf():
    log = xes_importer.apply(log_path)
    
    net, initial_marking, final_marking = inductive_miner.apply(log)
    parameters = {pn_visualizer.Variants.PERFORMANCE.value.Parameters.FORMAT: "png"}
    gviz_pnp = pn_visualizer.apply(net, initial_marking, final_marking, parameters=parameters, variant=pn_visualizer.Variants.PERFORMANCE, log=log)
    
    return str(gviz_pnp)
    
@app.route('/bpmn', methods=['GET'])
def bpmn():
    log = xes_importer.apply(log_path)
    
    tree = pm4py.discover_process_tree_inductive(log)
    bpmn_graph = converter.apply(tree, variant=converter.Variants.TO_BPMN)
    gviz_bpmn = pm4py.visualization.bpmn.visualizer.apply(bpmn_graph)
    
    return str(gviz_bpmn)

@app.route('/start', methods=['GET'])
def start():
    log = xes_importer.apply(log_path)
    
    log_start = start_activities_filter.get_start_activities(log)
    
    return log_start
    
@app.route('/end', methods=['GET'])
def end():
    log = xes_importer.apply(log_path)
    
    end_activities = end_activities_filter.get_end_activities(log)
    
    return end_activities
    
@app.route('/median', methods=['GET', 'POST'])
def median():
    #MEDIAN CASE
    from pm4py.statistics.traces.generic.log import case_statistics
    import time
    log = xes_importer.apply(log_path)
    median_case_duration = case_statistics.get_median_case_duration(log, parameters={
        case_statistics.Parameters.TIMESTAMP_KEY: "time:timestamp"
    })
    return str(median_case_duration)
    
@app.route('/total', methods=['GET', 'POST'])
def total():
    #ALL CASES
    from pm4py.statistics.traces.generic.log import case_statistics
    import time
    log = xes_importer.apply(log_path)
    all_case_durations = case_statistics.get_all_case_durations(log, parameters={
        case_statistics.Parameters.TIMESTAMP_KEY: "time:timestamp"})
    total = 0
    for i in range(0, len(all_case_durations)):
       total = total + all_case_durations[i];

    return str(total)  

@app.route('/filterPerformance', methods=['GET', 'POST'])
def filterPerformance():
    log = xes_importer.apply(log_path)
    from pm4py.algo.filtering.log.cases import case_filter
    
    # GET
    if request.args.get('min') == None:
        min_sec = 0;
    else:
        min_sec = float(request.args.get('min'))
    if request.args.get('max') == None:
        max_sec = 100*3,154e+7;
    else:
        max_sec = float(request.args.get('max'))

    filtered_log = case_filter.filter_case_performance(log, min_sec, max_sec)
    
    f = createGraphFReduced(filtered_log)
    p = createGraphPReduced(filtered_log)
    
    variantsDict = '{'

    cases = len(filtered_log)
    j=0
    for i in range(0, cases):
        info = filtered_log[i].__getattribute__('attributes')
        caseName = info['concept:name']

        if ("variant-index" in info):
            varIndex = info['variant-index']
            if (i == 0):
                variantsDict = variantsDict + '"' + str(varIndex) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j+1) + '": ['
        
        variantsDict = variantsDict + '{"' + str(caseName) + '":['

        for x in filtered_log[i]:
            timestamp = x['time:timestamp']
            x['time:timestamp'] = str(timestamp)
            stringX = str(x).replace("'", '"')
            variantsDict = variantsDict + '' + stringX  # +', '
        j=j+1
        variantsDict = variantsDict + ']}'  # chiude ogni caso
        if ("variant-index" not in info):
            if (i == cases):
                variantsDict = variantsDict + ']' # chiude ogni variante
    if ("variant-index" in info):
        variantsDict = variantsDict + ']' # chiude ogni variante
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    
    return result
    
@app.route('/filterTimeframe', methods=['GET', 'POST'])
def filterTimeframe():
    log = xes_importer.apply(log_path)
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    
    # GET
    if request.args.get('start') == None:
        start = 0;
    else:
        start = request.args.get('start')
    if request.args.get('end') == None:
        end = 0;
    else:
        end = request.args.get('end')
        
        
    if request.args.get('timeframe') == 'contained':
        filtered_log = timestamp_filter.filter_traces_contained(log, start, end)
    
    if request.args.get('timeframe') == 'intersecting':
        filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
        
    f = createGraphFReduced(filtered_log)
    p = createGraphPReduced(filtered_log)
    
    variantsDict = '{'

    cases = len(filtered_log)
    j=0
    for i in range(0, cases):
        info = filtered_log[i].__getattribute__('attributes')
        caseName = info['concept:name']

        if ("variant-index" in info):
            varIndex = info['variant-index']
            if (i == 0):
                variantsDict = variantsDict + '"' + str(varIndex) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j+1) + '": ['
        
        variantsDict = variantsDict + '{"' + str(caseName) + '":['

        for x in filtered_log[i]:
            timestamp = x['time:timestamp']
            x['time:timestamp'] = str(timestamp)
            stringX = str(x).replace("'", '"')
            variantsDict = variantsDict + '' + stringX  # +', '
        j=j+1
        variantsDict = variantsDict + ']}'  # chiude ogni caso
        if ("variant-index" not in info):
            if (i == cases):
                variantsDict = variantsDict + ']' # chiude ogni variante
    if ("variant-index" in info):
        variantsDict = variantsDict + ']' # chiude ogni variante
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    
    result = str(f)+"|||"+str(p)+"|||"+str(variantsDict)
    
    return result
'''
app.run(host=path, port=int(port_n))