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
12
Atom token(p_10)
Atom token(p_11)
Atom token(p_12)
Atom token(p_3)
Atom token(p_4)
Atom token(p_5)
Atom token(p_6)
Atom token(p_7)
Atom token(p_8)
Atom token(p_9)
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
12
1 0
1 1
1 2
1 3
1 4
1 5
1 6
1 7
1 8
1 9
1 10
1 11
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
11
0
end_state
begin_goal
2
1 10
2 5
end_goal
16
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
moveinthemodel#analyzedata_end 
0
2
0 0 -1 1
0 1 4 5
1
end_operator
begin_operator
moveinthemodel#createrecord_end 
0
2
0 0 -1 1
0 1 5 6
1
end_operator
begin_operator
moveinthemodel#endanalysis_end 
0
2
0 0 -1 1
0 1 6 7
1
end_operator
begin_operator
moveinthemodel#event_12_end_end 
0
2
0 0 -1 1
0 1 11 3
1
end_operator
begin_operator
moveinthemodel#event_2_start_end 
0
2
0 0 -1 1
0 1 3 4
1
end_operator
begin_operator
moveinthemodel#receivebloodmeasure_end 
0
2
0 0 -1 1
0 1 7 8
1
end_operator
begin_operator
moveinthemodel#receivedatafrommq_end 
0
2
0 0 -1 1
0 1 8 9
1
end_operator
begin_operator
moveinthemodel#receivescalemeasure_end 
0
2
0 0 -1 1
0 1 9 0
1
end_operator
begin_operator
moveinthemodel#receivetempmeasure_end 
0
2
0 0 -1 1
0 1 0 1
1
end_operator
begin_operator
moveinthemodel#senddatatomq_end 
0
2
0 0 -1 1
0 1 1 2
1
end_operator
begin_operator
moveinthemodel#startanalysis_end 
0
2
0 0 -1 1
0 1 2 10
1
end_operator
0
