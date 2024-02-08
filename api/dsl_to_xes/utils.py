import classes
import sys
import random
from datetime import datetime
import time
from flask import Flask, session
from flask_session import Session

from flask import Flask, redirect, url_for, render_template, request, session
from datetime import timedelta
import time
import os
#--------------------------------------------------------------------
#function to debug by populating the data sources
#--------------------------------------------------------------------
def fromDSLtoXES(inputDSL):
    dsl = inputDSL.split("steps:\n\t\t- ")
    pipeline_details = dsl[0]
    steps_details = dsl[1].split("\n\t\t-")
    pipeline_name = pipeline_details.split(" {")[0].replace("Pipeline ", "").replace(" ", "")
    pipeline_medium = pipeline_details.split(":")[1].replace(" ", "")
    step_phases = []
    step_phases.append(classes.StepPhase("1", "End"))
    i = 0
    steps = []
    environmentVariables = []
    step_name = ""
    step_type = ""
    risultato = ""
    for item in steps_details:
        step_type = (item.split(" step ")[0])
        step_name = (item.split(" step ")[1].split('\n')[0])
        new_step = classes.Step(str(i), step_name, "", step_type)
        #--------------- take env par if there are (DA COMPLETARE) -------------------------------------
        env_vars = item.split("environmentParameters: {\n")[1].replace("\t", "").split("\n}")[0].split(",\n")
        for env in env_vars:
            env = env.split(":")
            newEnvVar = classes.EnvironmentVariable(env[0], env[1].replace(" ", "").replace("'", ""))
        steps.append(new_step)
        i += 1
    #NOW THAT WE TRANSLATED THE DSL INTO ALL THE REQUIRED OBJECTS WE CAN GENERATE THE XES    
    for i in step_phases:
        print(i)
        risultato=risultato+generateXES("1", pipeline_name, pipeline_medium, 1, 1, steps, step_phases)

    return risultato


def debug(steps, step_phases, technologies):
    print("Debug Button clicked.")
    steps.append(classes.Step('1','1','edge','processing'))
    steps.append(classes.Step('2','2','edge','processing'))
    step_phases.append(classes.StepPhase('1','1'))
    step_phases.append(classes.StepPhase('2','2'))
    technologies.append(classes.Technology('1','1','Windows'))
    technologies.append(classes.Technology('2','2','Linux'))
    steps[0].dataSources.append(classes.DataSource('1','1','1','1'))
    steps[1].dataSources.append(classes.DataStream('2','2','2','2','IO'))
    step_phases[0].environmentVariables.append(classes.EnvironmentVariable('1','1'))
    step_phases[1].environmentVariables.append(classes.EnvironmentVariable('2','2'))
    technologies[0].cpus.append(classes.CPU('1','1','1',""))
    technologies[0].gpus.append(classes.GPU('1','1','1','1',""))
    technologies[0].rams.append(classes.RAM('1','1','1',"",'DDR4'))
    technologies[0].storages.append(classes.Storage('1','1','1',"",'HD'))
    technologies[0].networks.append(classes.Network('1','1','1'))
    technologies[1].cpus.append(classes.CPU('2','2','2','2'))
    technologies[1].gpus.append(classes.GPU('1','1','1','1',""))
    technologies[1].rams.append(classes.RAM('2','2','2','2','DDR5'))
    technologies[1].storages.append(classes.Storage('2','2','2','2','SSD'))
    technologies[1].networks.append(classes.Network('2','2','2'))
    step_phases[0].technologies.append(technologies[0])
    step_phases[1].technologies.append(technologies[1])
#--------------------------------------------------------------------
#function to generate the xes file
#--------------------------------------------------------------------
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
            

def generateXES(pipeline_id, pipeline_name, pipeline_medium, pipeline_traces, n, steps, step_phases):
        print("Generating XES file.")
        xes_result=""
        original_stdout = sys.stdout # Save a reference to the original standard output
        #check for presence of links
        presence_of_continuum_layer = 0
        presence_of_data_sources = 0
        presence_of_technologies = 0
        presence_of_cpus = 0
        presence_of_gpus = 0
        presence_of_rams = 0
        presence_of_storages = 0
        presence_of_networks = 0
        for i in steps:
            if i.continuumLayer != "":
                presence_of_continuum_layer = 1
            if len(i.dataSources) > 0:
                presence_of_data_sources = 1
        for i in step_phases:
                if len(i.technologies) > 0:
                    presence_of_technologies = 1
                    for j in i.technologies:
                        if len(j.cpus) > 0:
                            presence_of_cpus = 1
                        if len(j.gpus) > 0:
                            presence_of_gpus = 1
                        if len(j.rams) > 0:
                            presence_of_rams = 1
                        if len(j.storages) > 0:
                            presence_of_storages = 1
                        if len(j.networks) > 0:
                            presence_of_networks = 1
        
        log_result=""

        
        nuovo_path=process_string(session["directory_getdsl"][1:]+'/' + "importedpipeline" + '.xes')
        print(nuovo_path)
        with open(nuovo_path, 'w+') as f:
            sys.stdout = f # Change the standard output to the file we created.
            #standard header
            print("<?xml version='1.0' encoding='UTF-8'?>\n<log>")
            print('\t<string key="creator" value="RefModel_Generator"/>')
            #extenstions
            print('\t<extension name="Concept" prefix="concept" uri="http://code.deckfour.org/xes/concept.xesext"/>')
            print('\t<extension name="Time" prefix="time" uri="http://code.deckfour.org/xes/time.xesext"/>')
            print('\t<extension name="Organizational" prefix="org" uri="http://code.deckfour.org/xes/org.xesext"/>')
            #attributes definition at log level
            #attributes definition at trace level
            print('\t<global scope="trace">')
            print('\t\t<string key="concept:name" value="name"/>')
            print('\t</global>')
            #attributes definition at event level
            print('\t<global scope="event">')
            print('\t\t<string key="concept:name" value="name"/>')
            print('\t\t<string key="StepPhaseID" value="string"/>')
            print('\t\t<date key="time:timestamp" value="' + datetime.now().strftime('%Y-%m-%dT%H:%M:%S.%f+01:00') + '"/>')
            #pipeline
            print('\t\t<string key="PipelineName" value="name"/>')
            print('\t\t<string key="PipelineID" value="string"/>')
            print('\t\t<string key="PipelineCommunicationMedium" value="string"/>')
            #Step
            print('\t\t<string key="StepID" value="string"/>')
            if presence_of_continuum_layer == 1:
                print('\t\t<string key="StepContinuumLayer" value="string"/>')
            print('\t\t<string key="StepType" value="string"/>')
            #DataSource
            #only if there are DataSources
            if presence_of_data_sources == 1:
                print('\t\t<string key="DataSourceID" value="string"/>')
                print('\t\t<string key="DataSourceName" value="string"/>')
                print('\t\t<string key="DataSourceVolume" value="string"/>')
                print('\t\t<string key="DataSourceVelocity" value="string"/>')
                print('\t\t<string key="DataSourceType" value="string"/>')          
            #technologies
            #only if there are Technologies
            if presence_of_technologies == 1:
                print('\t\t<string key="TechnologyID" value="string"/>')
                print('\t\t<string key="TechnologyName" value="string"/>')
                print('\t\t<string key="TechnologyOS" value="string"/>')
            #cpus
            if presence_of_cpus == 1:
                print('\t\t<string key="CPUID" value="string"/>')
                print('\t\t<string key="CPUCores" value="string"/>')
                print('\t\t<string key="CPUSpeed" value="string"/>')
                print('\t\t<string key="CPUProducer" value="string"/>')
            #gpus
            if presence_of_gpus == 1:
                print('\t\t<string key="GPUID" value="string"/>')
                print('\t\t<string key="GPUCores" value="string"/>')
                print('\t\t<string key="GPUSpeed" value="string"/>')
                print('\t\t<string key="GPUMemory" value="string"/>')
                print('\t\t<string key="GPUProducer" value="string"/>')
            #rams
            if presence_of_rams == 1:
                print('\t\t<string key="RAMID" value="string"/>')
                print('\t\t<string key="RAMVolume" value="string"/>')
                print('\t\t<string key="RAMSpeed" value="string"/>')
                print('\t\t<string key="RAMProducer" value="string"/>')
                print('\t\t<string key="RAMType" value="string"/>')
            #storages
            if presence_of_storages == 1:
                print('\t\t<string key="StorageID" value="string"/>')
                print('\t\t<string key="StorageVolume" value="string"/>')
                print('\t\t<string key="StorageSpeed" value="string"/>')
                print('\t\t<string key="Storageroducer" value="string"/>')
                print('\t\t<string key="StorageType" value="string"/>')
            #networks
            if presence_of_networks == 1:
                print('\t\t<string key="NetworkID" value="string"/>')
                print('\t\t<string key="NetworkBandwidth" value="string"/>')
                print('\t\t<string key="NetworkLatency" value="string"/>')
            print('\t</global>')
            #classifiers
            print('\t<classifier name="Activity" keys="name"/>')
            print('\t<classifier name="activity classifier" keys="Activity"/>')
            #log
            #traces
            while n > 0:
                print('\t<trace>')
                print('\t\t<string key="concept:name" value="' + n.__str__() + '"/>')
                #events
                for step in steps:
                    for step_phase in step_phases:
                        print('\t\t<event>')
                        print('\t\t\t<string key="concept:name" value="' + step.name + '-' + step_phase.name + '"/>')
                        print('\t\t\t<string key="StepPhaseID" value="' + step_phase.id + '"/>')
                        #timestamp in YYYY-mm-ddTHH:MM:SS.fff+TZD"
                        print('\t\t\t<date key="time:timestamp" value="' + datetime.now().strftime('%Y-%m-%dT%H:%M:%S.%f+01:00') + '"/>')
                        #pipeline
                        print('\t\t\t<string key="PipelineID" value="' + pipeline_id + '"/>')
                        print('\t\t\t<string key="PipelineCommunicationMedium" value="' + pipeline_medium + '"/>')
                        print('\t\t\t<string key="PipelineName" value="' + pipeline_name + '"/>')
                        #Step
                        print('\t\t\t<string key="StepID" value="' + step.id + '"/>')
                        print('\t\t\t<string key="StepName" value="' + step.name + '"/>')
                        if presence_of_continuum_layer == 1:
                            print('\t\t\t<string key="StepContinuumLayer" value="' + step.continuumLayer + '"/>')
                            print('\t\t\t<string key="StepType" value="' + step.type + '"/>')
                        #data sources
                        if presence_of_data_sources == 1:
                            i = random.randint(0,len(step.dataSources)-1)
                            print('\t\t\t<string key="DataSourceID" value="' + step.dataSources[i].id + '"/>')
                            print('\t\t\t<string key="DataSourceName" value="' + step.dataSources[i].name + '"/>')
                            print('\t\t\t<string key="DataSourceVolume" value="' + step.dataSources[i].volume + '"/>')
                            if type(step.dataSources[i]) == type(classes.DataStream):
                                print('\t\t\t<string key="DataSourceVelocity" value="' + step.dataSources[i].velocity + '"/>')
                            else:
                                print('\t\t\t<string key="DataSourceVelocity" value="None"/>')
                            print('\t\t\t<string key="DataSourceType" value="' + step.dataSources[i].type + '"/>')
                        #technologies
                        if presence_of_technologies == 1:
                            i = random.randint(0,len(step_phase.technologies)-1)
                            print('\t\t\t<string key="TechnologyID" value="' + step_phase.technologies[i].id + '"/>')
                            print('\t\t\t<string key="TechnologyName" value="' + step_phase.technologies[i].name + '"/>')
                            print('\t\t\t<string key="TechnologyOS" value="' + step_phase.technologies[i].os + '"/>')
                        #cpus
                        if presence_of_cpus == 1:
                            j = random.randint(0,len(step_phase.technologies[i].cpus)-1)
                            print('\t\t\t<string key="CPUID" value="' + step_phase.technologies[i].cpus[j].id + '"/>')
                            print('\t\t\t<string key="CPUCores" value="' + step_phase.technologies[i].cpus[j].cores + '"/>')
                            print('\t\t\t<string key="CPUSpeed" value="' + step_phase.technologies[i].cpus[j].speed + '"/>')
                            print('\t\t\t<string key="CPUProducer" value="' + step_phase.technologies[i].cpus[j].producer + '"/>')
                        #gpus
                        if presence_of_gpus == 1:
                            j = random.randint(0,len(step_phase.technologies[i].gpus)-1)
                            print('\t\t\t<string key="GPUID" value="' + step_phase.technologies[i].gpus[j].id + '"/>')
                            print('\t\t\t<string key="GPUCores" value="' + step_phase.technologies[i].gpus[j].cores + '"/>')
                            print('\t\t\t<string key="GPUSpeed" value="' + step_phase.technologies[i].gpus[j].speed + '"/>')
                            print('\t\t\t<string key="GPUMemory" value="' + step_phase.technologies[i].gpus[j].memory + '"/>')
                            print('\t\t\t<string key="GPUProducer" value="' + step_phase.technologies[i].gpus[j].producer + '"/>')
                        #rams
                        if presence_of_rams == 1:
                            j = random.randint(0,len(step_phase.technologies[i].rams)-1)
                            print('\t\t\t<string key="RAMID" value="' + step_phase.technologies[i].rams[j].id + '"/>')
                            print('\t\t\t<string key="RAMVolume" value="' + step_phase.technologies[i].rams[j].volume + '"/>')
                            print('\t\t\t<string key="RAMSpeed" value="' + step_phase.technologies[i].rams[j].speed + '"/>')
                            print('\t\t\t<string key="RAMProducer" value="' + step_phase.technologies[i].rams[j].producer + '"/>')
                            print('\t\t\t<string key="RAMType" value="' + step_phase.technologies[i].rams[j].type + '"/>')
                        #storages
                        if presence_of_storages == 1:
                            j = random.randint(0,len(step_phase.technologies[i].storages)-1)
                            print('\t\t\t<string key="StorageID" value="' + step_phase.technologies[i].storages[j].id + '"/>')
                            print('\t\t\t<string key="StorageVolume" value="' + step_phase.technologies[i].storages[j].volume + '"/>')
                            print('\t\t\t<string key="StorageSpeed" value="' + step_phase.technologies[i].storages[j].speed + '"/>')
                            print('\t\t\t<string key="Storageroducer" value="' + step_phase.technologies[i].storages[j].producer + '"/>')
                            print('\t\t\t<string key="StorageType" value="' + step_phase.technologies[i].storages[j].type + '"/>')
                        #networks
                        if presence_of_networks == 1:
                            j = random.randint(0,len(step_phase.technologies[i].networks)-1)
                            print('\t\t\t<string key="NetworkID" value="' + step_phase.technologies[i].networks[j].id + '"/>')
                            print('\t\t\t<string key="NetworkBandwidth" value="' + step_phase.technologies[i].networks[j].bandwidth + '"/>')
                            print('\t\t\t<string key="NetworkLatency" value="' + step_phase.technologies[i].networks[j].latency + '"/>')
                        #close event
                        print('\t\t</event>')
                #close trace
                print('\t</trace>')
                #little wait to avoid timestamps with the same value
                time.sleep(0.2)
                n -= 1
            #close the log 
            print('</log>')
        sys.stdout = original_stdout # Reset the standard output to its original value       
        
        
        with open(nuovo_path, 'r') as file:
            log_result = file.read()
        
        return log_result
        
#--------------------------------------------------------------------
#function to generate the json file
#--------------------------------------------------------------------
def generateJSON(pipeline_id, pipeline_name, pipeline_medium, pipeline_traces, n, steps, step_phases, data_sources, environment_variables, technologies, cpus, gpus, rams, storages, networks):
    print("Generating JSON file.")
    original_stdout = sys.stdout # Save a reference to the original standard output
    with open('data/' + pipeline_name + '.json', 'w') as f:
        sys.stdout = f # Change the standard output to the file we created.
        print('{\n\t"PipelineID": "' + pipeline_id + '",\n\t"PipelineName": "' + pipeline_name + '",\n\t"PipelineCommunicationMedium": "' + pipeline_medium + '",\n\t"NumberOfTraces": "' + pipeline_traces + '",')
        for i in steps:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))    
        for i in step_phases:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in data_sources:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in environment_variables:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in technologies:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in cpus:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in gpus:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in rams:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},'))  
        for i in storages:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t},')) 
        for i in networks:
            print('\t' + i.__str__().replace('\n\t','\n\t\t').replace('\n}','\n\t}')) 
        print('}')
    sys.stdout = original_stdout # Reset the standard output to its original value