begin_version
3
end_version
begin_metric
1
end_metric
3
begin_variable
var0
-1
2
Atom allowed()
NegatedAtom allowed()
end_variable
begin_variable
var1
-1
4
Atom token(p_3)
Atom token(p_4)
Atom token(sink)
Atom token(source)
end_variable
begin_variable
var2
-1
6
Atom tracepointer(ev1)
Atom tracepointer(ev2)
Atom tracepointer(ev3)
Atom tracepointer(ev4)
Atom tracepointer(ev5)
Atom tracepointer(evend)
end_variable
2
begin_mutex_group
4
1 0
1 1
1 2
1 3
end_mutex_group
begin_mutex_group
6
2 0
2 1
2 2
2 3
2 4
2 5
end_mutex_group
begin_state
0
3
0
end_state
begin_goal
2
1 2
2 5
end_goal
8
begin_operator
moveinthelog#checkticket#ev2-ev3 
1
0 0
1
0 2 1 2
1
end_operator
begin_operator
moveinthelog#decide#ev4-ev5 
1
0 0
1
0 2 3 4
1
end_operator
begin_operator
moveinthelog#examinethoroughly#ev3-ev4 
1
0 0
1
0 2 2 3
1
end_operator
begin_operator
moveinthelog#registerrequest#ev1-ev2 
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
moveinthelog#rejectrequest#ev5-evend 
1
0 0
1
0 2 4 5
1
end_operator
begin_operator
moveinthemodel#analyzeandcreatenotificationforhealthcarepersonnel_end 
0
2
0 0 -1 1
0 1 1 2
1
end_operator
begin_operator
moveinthemodel#generatesampledatareformatpushinmqtt_end 
0
2
0 0 -1 1
0 1 3 0
1
end_operator
begin_operator
moveinthemodel#receivedatafrommqttcheckpatientplanbuildfhirdbrecordsstoretofhirdb_end 
0
2
0 0 -1 1
0 1 0 1
1
end_operator
0
