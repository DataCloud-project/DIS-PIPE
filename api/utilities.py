import pm4py
from pm4py.objects.log.exporter.xes import exporter as xes_exporter
from flask import Flask, flash
import tkinter
from tkinter import filedialog
import datetime
import numpy as np

def clear():
    clearFile('segments.txt')
    clearFile('removeSegments.txt')

def clearFile(file):
    f = open(file,"w")
    f.close()
         
def takeActions(allActivities):
    activities = []
    for seg in allActivities:
        for act in seg: 
            if act not in activities:
                activities.append(act)
    sortActivity = sorted(activities)            
    return sortActivity    

def downloadFile():
    segments = takeSegmentFromFile()
    seg = []
    for act in segments:
        elem = act[1:-1]
        seg.append(elem)  
    log = pm4py.read_xes("static/uploads/log.xes")
    allXESActivities = {}
    list_key = []
    for trace in log:
        activities = []
        for event in trace:
            activities.append(event["concept:name"]) 
            allXESActivities[trace.attributes["concept:name"]] = activities
            list_key.append(trace.attributes["concept:name"])              
    list_ok_key = []  
    for key, value in allXESActivities.items():
        for elem in seg:
            if(elem == value): #non entra in questo if
                list_ok_key.append(key)                        
    list_ko_key = set(list_key) - set(list_ok_key)
    for fil in list_ko_key:
        log = pm4py.filter_trace_attribute_values(log, 'concept:name', {fil}, retain=False)
    xes_exporter.apply(log, "downloads/log.xes") #log contiene i segmenti che mantieni
    parent = tkinter.Tk() # Create the object
    parent.attributes("-topmost", True)
    directory_to_start_from = 'C:/Users/User/Downloads/'
    path = filedialog.askdirectory(initialdir=directory_to_start_from, title='Please select a folder:', parent=parent)
    timestamp = takeTimestamp()
    xes_exporter.apply(log, path + "/log_export_"+timestamp+".xes")

def takeSegmentFromTrace():  
    segments = []  
    with open('trace.txt', 'r') as f:
        for line in f:
            line = line.strip("[()]")
            line = line.replace("('", "")
            line = line.replace(")]", "")
            line = line.replace("' ", "'")
            line = line.replace("[", "")
            seg = line.split(",")
            s = []
            seg = filter(None, seg)
            for word in seg:
                word = word.replace(" '", "'")
                word = word.replace("' ", "'")
                word = word.rstrip("\n")
                word = word.strip("']/")
                s.append(word)
            segments.append(s)        
    f.close()        
    return segments

def takeSegmentFromFile():  
    segments = []  
    with open('segments.txt', 'r') as f:
        for line in f:
            line = line.strip("[()]")
            line = line.replace("('", "")
            line = line.replace(")]", "")
            line = line.replace("' ", "'")
            line = line.replace("[", "")
            line = line.strip("\n")
            line = line.strip("")
            seg = line.split(",")
            seg = filter(None, seg)
            s = []
            for word in seg:
                word = word.replace(" '", "'")
                word = word.replace("' ", "'")
                word = word.rstrip("\n")
                word = word.strip("']/")
                word = word.strip("")
                s.append(word)
            segments.append(s)       
    f.close()         
    return segments

def takeRemoveSegmentFromFile():  
    segments = []  
    with open('removeSegments.txt', 'r') as f:
        for line in f:
            line = line.strip("[()]")
            line = line.replace("('", "")
            line = line.replace(")]", "")
            line = line.replace("' ", "'")
            line = line.replace("[", "")
            line = line.strip("\n")
            line = line.strip("")
            seg = line.split(",")
            seg = filter(None, seg)
            s = []
            for word in seg:
                word = word.replace(" '", "'")
                word = word.replace("' ", "'")
                word = word.rstrip("\n")
                word = word.strip("']/")
                word = word.strip("")
                s.append(word)
            segments.append(s)        
    f.close()           
    return segments       

def writeOnSegmentFile(result):
    with open('segments.txt', 'w') as f:
        for line in result:
            f.write(str(line))
            f.write("\n")
    f.close()
    replaceInFile("segments.txt")
        
def writeOnRemoveSegmentFile(removeSegment):
    with open('removeSegments.txt', 'w') as f:
        for line in removeSegment:
            f.write(str(line))
            f.write("\n")
    f.close() 
    replaceInFile("removeSegments.txt")
     
        
def replaceInFile(file):
    fin = open(file, "rt")
    data = fin.read()
    data = data.replace(", '(',", "")
    data = data.replace(", ')'", "")
    data = data.replace("' ", "'")
    data = data.replace(" ('", " '")
    data = data.replace("')", "'")
    data = data.replace("]", ",")
    data = data.replace("[", "")
    fin.close()
    fin = open(file, "wt")
    fin.write(data)
    fin.close() 
    
def takeTimestamp():
    ct = datetime.datetime.now() # ct stores current time
    ts = ct.timestamp()  # ts store timestamp of current time
    return str(ts)    
    
def sortFirstAscendentOrder(actWithOccurence):
    actWithOccurence.sort(key=lambda y: y[0])
    return actWithOccurence   

def sortAscendentOrder(actWithOccurence):
    array = []
    for elem in actWithOccurence:
        arr = [int(elem[0]), tuple(elem[1:])]
        array.append(arr)
    array.sort(key=lambda y: y[0])  
    return array  

def sortDescendentOrder(actWithOccurence):
    array = []
    for elem in actWithOccurence:
        arr = [int(elem[0]), tuple(elem[1:])]
        array.append(arr)
    array.sort(key=lambda y: y[0], reverse=True)  
    return array
                        
                  
                  