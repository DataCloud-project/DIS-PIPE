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
Atom token(p_11)
Atom token(p_12)
Atom token(p_14)
Atom token(p_15)
Atom token(p_3)
Atom token(p_4)
Atom token(p_5)
Atom token(p_6)
Atom token(p_7)
Atom token(p_8)
Atom token(sink)
Atom token(source)
end_variable
begin_variable
var2
-1
11
Atom tracepointer(ev1)
Atom tracepointer(ev10)
Atom tracepointer(ev2)
Atom tracepointer(ev3)
Atom tracepointer(ev4)
Atom tracepointer(ev5)
Atom tracepointer(ev6)
Atom tracepointer(ev7)
Atom tracepointer(ev8)
Atom tracepointer(ev9)
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
11
2 0
2 1
2 2
2 3
2 4
2 5
2 6
2 7
2 8
2 9
2 10
end_mutex_group
begin_state
0
11
0
end_state
begin_goal
2
1 10
2 10
end_goal
34
begin_operator
moveinthelog#data_merge#ev6-ev7 
1
0 0
1
0 2 6 7
1
end_operator
begin_operator
moveinthelog#data_merge#ev7-ev8 
1
0 0
1
0 2 7 8
1
end_operator
begin_operator
moveinthelog#query#ev2-ev3 
1
0 0
1
0 2 2 3
1
end_operator
begin_operator
moveinthelog#query#ev3-ev4 
1
0 0
1
0 2 3 4
1
end_operator
begin_operator
moveinthelog#report_generated#ev10-evend 
1
0 0
1
0 2 1 10
1
end_operator
begin_operator
moveinthelog#request#ev1-ev2 
1
0 0
1
0 2 0 2
1
end_operator
begin_operator
moveinthelog#trasformation#ev4-ev5 
1
0 0
1
0 2 4 5
1
end_operator
begin_operator
moveinthelog#trasformation#ev5-ev6 
1
0 0
1
0 2 5 6
1
end_operator
begin_operator
moveinthelog#visualization#ev8-ev9 
1
0 0
1
0 2 8 9
1
end_operator
begin_operator
moveinthelog#visualization#ev9-ev10 
1
0 0
1
0 2 9 1
1
end_operator
begin_operator
moveinthemodel#data_merge 
0
2
0 0 -1 1
0 1 11 4
1
end_operator
begin_operator
moveinthemodel#install_supplementary_resources 
0
2
0 0 -1 1
0 1 4 5
1
end_operator
begin_operator
moveinthemodel#launch_algorithm 
0
2
0 0 -1 1
0 1 5 6
1
end_operator
begin_operator
moveinthemodel#model_generated 
0
2
0 0 -1 1
0 1 6 7
1
end_operator
begin_operator
moveinthemodel#query 
0
2
0 0 -1 1
0 1 1 0
1
end_operator
begin_operator
moveinthemodel#query 
0
2
0 0 -1 1
0 1 7 8
1
end_operator
begin_operator
moveinthemodel#report_generated 
0
2
0 0 -1 1
0 1 8 9
1
end_operator
begin_operator
moveinthemodel#request 
0
2
0 0 -1 1
0 1 9 0
1
end_operator
begin_operator
moveinthemodel#restore_accessto_database 
0
2
0 0 -1 1
0 1 0 1
1
end_operator
begin_operator
moveinthemodel#store_results 
0
2
0 0 -1 1
0 1 1 2
1
end_operator
begin_operator
moveinthemodel#trasformation 
0
2
0 0 -1 1
0 1 2 3
1
end_operator
begin_operator
moveinthemodel#visualization 
0
2
0 0 -1 1
0 1 3 10
1
end_operator
begin_operator
movesync#data_merge#ev6 
0
3
0 0 -1 0
0 1 11 4
0 2 6 7
0
end_operator
begin_operator
movesync#data_merge#ev7 
0
3
0 0 -1 0
0 1 11 4
0 2 7 8
0
end_operator
begin_operator
movesync#query#ev2 
0
3
0 0 -1 0
0 1 1 0
0 2 2 3
0
end_operator
begin_operator
movesync#query#ev2 
0
3
0 0 -1 0
0 1 7 8
0 2 2 3
0
end_operator
begin_operator
movesync#query#ev3 
0
3
0 0 -1 0
0 1 1 0
0 2 3 4
0
end_operator
begin_operator
movesync#query#ev3 
0
3
0 0 -1 0
0 1 7 8
0 2 3 4
0
end_operator
begin_operator
movesync#report_generated#ev10 
0
3
0 0 -1 0
0 1 8 9
0 2 1 10
0
end_operator
begin_operator
movesync#request#ev1 
0
3
0 0 -1 0
0 1 9 0
0 2 0 2
0
end_operator
begin_operator
movesync#trasformation#ev4 
0
3
0 0 -1 0
0 1 2 3
0 2 4 5
0
end_operator
begin_operator
movesync#trasformation#ev5 
0
3
0 0 -1 0
0 1 2 3
0 2 5 6
0
end_operator
begin_operator
movesync#visualization#ev8 
0
3
0 0 -1 0
0 1 3 10
0 2 8 9
0
end_operator
begin_operator
movesync#visualization#ev9 
0
3
0 0 -1 0
0 1 3 10
0 2 9 1
0
end_operator
0
