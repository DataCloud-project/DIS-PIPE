import pandas as pd
import pm4py
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.objects.log.util import interval_lifecycle
from pm4py.util import constants

#log = pm4py.read_xes('RJTS.xes')
#log to dataframe
#dataframe = pm4py.convert_to_dataframe(log)


log_clone = xes_importer.apply('RJTS.xes')

log_clone = interval_lifecycle.assign_lead_cycle_time(log_clone, parameters={
                                                        constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                        constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

dataframe = pm4py.convert_to_dataframe(log_clone) #.replace(np.nan, 'None')
dataframe.to_csv('RJTS.csv')
'''
#log to petrinet
net, im, fm = pm4py.discover_petri_net_inductive(log)
pm4py.save_vis_petri_net(net, im, fm, "net.svg")

#log to bpmn
tree = pm4py.discover_process_tree_inductive(log)
bpmn_graph = pm4py.convert_to_bpmn(tree)
pm4py.write_bpmn(bpmn_graph, "ru.bpmn")


#dataframe to log
dataframe1 = pd.read_csv('exported.csv', sep=',')
event_log = pm4py.convert_to_event_log(dataframe1)
pm4py.write_xes(event_log, 'exported777.xes')
'''