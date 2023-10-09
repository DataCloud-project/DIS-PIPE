import pandas as pd
import pm4py

log = pm4py.read_xes('exported.xes')
#log to dataframe
dataframe = pm4py.convert_to_dataframe(log)
dataframe.to_csv('exported.csv')

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