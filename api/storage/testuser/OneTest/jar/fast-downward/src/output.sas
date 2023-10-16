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
11
Atom token(p_10)
Atom token(p_11)
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
9
Atom tracepointer(ev1)
Atom tracepointer(ev2)
Atom tracepointer(ev3)
Atom tracepointer(ev4)
Atom tracepointer(ev5)
Atom tracepointer(ev6)
Atom tracepointer(ev7)
Atom tracepointer(ev8)
Atom tracepointer(evend)
end_variable
2
begin_mutex_group
11
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
end_mutex_group
begin_mutex_group
9
2 0
2 1
2 2
2 3
2 4
2 5
2 6
2 7
2 8
end_mutex_group
begin_state
0
10
0
end_state
begin_goal
2
1 9
2 8
end_goal
26
begin_operator
moveinthelog#analyzedata#ev4-ev5 
1
0 0
1
0 2 3 4
1
end_operator
begin_operator
moveinthelog#analyzedata#ev5-ev6 
1
0 0
1
0 2 4 5
1
end_operator
begin_operator
moveinthelog#analyzedata#ev6-ev7 
1
0 0
1
0 2 5 6
1
end_operator
begin_operator
moveinthelog#createrecord#ev1-ev2 
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
moveinthelog#endanalysis#ev7-ev8 
1
0 0
1
0 2 6 7
1
end_operator
begin_operator
moveinthelog#event12end#ev8-evend 
1
0 0
1
0 2 7 8
1
end_operator
begin_operator
moveinthelog#startanalysis#ev2-ev3 
1
0 0
1
0 2 1 2
1
end_operator
begin_operator
moveinthelog#startanalysis#ev3-ev4 
1
0 0
1
0 2 2 3
1
end_operator
begin_operator
moveinthemodel#analyzedata 
0
2
0 0 -1 1
0 1 3 4
1
end_operator
begin_operator
moveinthemodel#createrecord 
0
2
0 0 -1 1
0 1 4 5
1
end_operator
begin_operator
moveinthemodel#endanalysis 
0
2
0 0 -1 1
0 1 5 6
1
end_operator
begin_operator
moveinthemodel#event12end 
0
2
0 0 -1 1
0 1 10 2
1
end_operator
begin_operator
moveinthemodel#event2start 
0
2
0 0 -1 1
0 1 2 3
1
end_operator
begin_operator
moveinthemodel#receivebloodmeasure 
0
2
0 0 -1 1
0 1 6 7
1
end_operator
begin_operator
moveinthemodel#receivedatafrommq 
0
2
0 0 -1 1
0 1 7 8
1
end_operator
begin_operator
moveinthemodel#receivetempmeasure 
0
2
0 0 -1 1
0 1 8 0
1
end_operator
begin_operator
moveinthemodel#senddatatomq 
0
2
0 0 -1 1
0 1 0 1
1
end_operator
begin_operator
moveinthemodel#startanalysis 
0
2
0 0 -1 1
0 1 1 9
1
end_operator
begin_operator
movesync#analyzedata#ev4 
0
3
0 0 -1 0
0 1 3 4
0 2 3 4
0
end_operator
begin_operator
movesync#analyzedata#ev5 
0
3
0 0 -1 0
0 1 3 4
0 2 4 5
0
end_operator
begin_operator
movesync#analyzedata#ev6 
0
3
0 0 -1 0
0 1 3 4
0 2 5 6
0
end_operator
begin_operator
movesync#createrecord#ev1 
0
3
0 0 -1 0
0 1 4 5
0 2 0 1
0
end_operator
begin_operator
movesync#endanalysis#ev7 
0
3
0 0 -1 0
0 1 5 6
0 2 6 7
0
end_operator
begin_operator
movesync#event12end#ev8 
0
3
0 0 -1 0
0 1 10 2
0 2 7 8
0
end_operator
begin_operator
movesync#startanalysis#ev2 
0
3
0 0 -1 0
0 1 1 9
0 2 1 2
0
end_operator
begin_operator
movesync#startanalysis#ev3 
0
3
0 0 -1 0
0 1 1 9
0 2 2 3
0
end_operator
0
