begin_version
3
end_version
begin_metric
1
end_metric
11
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
2
Atom token(p_10)
NegatedAtom token(p_10)
end_variable
begin_variable
var2
-1
2
Atom token(p_11)
NegatedAtom token(p_11)
end_variable
begin_variable
var3
-1
2
Atom token(p_4)
NegatedAtom token(p_4)
end_variable
begin_variable
var4
-1
2
Atom token(p_5)
NegatedAtom token(p_5)
end_variable
begin_variable
var5
-1
2
Atom token(p_6)
NegatedAtom token(p_6)
end_variable
begin_variable
var6
-1
2
Atom token(p_8)
NegatedAtom token(p_8)
end_variable
begin_variable
var7
-1
2
Atom token(p_9)
NegatedAtom token(p_9)
end_variable
begin_variable
var8
-1
2
Atom token(sink)
NegatedAtom token(sink)
end_variable
begin_variable
var9
-1
2
Atom token(source)
NegatedAtom token(source)
end_variable
begin_variable
var10
-1
6
Atom tracepointer(ev1)
Atom tracepointer(ev2)
Atom tracepointer(ev3)
Atom tracepointer(ev4)
Atom tracepointer(ev5)
Atom tracepointer(evend)
end_variable
1
begin_mutex_group
6
10 0
10 1
10 2
10 3
10 4
10 5
end_mutex_group
begin_state
0
1
1
1
1
1
1
1
1
0
0
end_state
begin_goal
10
1 1
2 1
3 1
4 1
5 1
6 1
7 1
8 0
9 1
10 5
end_goal
20
begin_operator
moveinthelog#checkticket#ev2-ev3 
1
0 0
1
0 10 1 2
1
end_operator
begin_operator
moveinthelog#decide#ev4-ev5 
1
0 0
1
0 10 3 4
1
end_operator
begin_operator
moveinthelog#examinethoroughly#ev3-ev4 
1
0 0
1
0 10 2 3
1
end_operator
begin_operator
moveinthelog#registerrequest#ev1-ev2 
1
0 0
1
0 10 0 1
1
end_operator
begin_operator
moveinthelog#rejectrequest#ev5-evend 
1
0 0
1
0 10 4 5
1
end_operator
begin_operator
moveinthemodel#checkticket 
0
3
0 0 -1 1
0 6 0 1
0 7 -1 0
1
end_operator
begin_operator
moveinthemodel#decide 
0
4
0 0 -1 1
0 2 0 1
0 5 -1 0
0 7 0 1
1
end_operator
begin_operator
moveinthemodel#examinecasually 
0
3
0 0 -1 1
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
moveinthemodel#examinethoroughly 
0
3
0 0 -1 1
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
moveinthemodel#paycompensation 
0
3
0 0 -1 1
0 3 0 1
0 8 -1 0
1
end_operator
begin_operator
moveinthemodel#registerrequest 
0
3
0 0 -1 1
0 4 -1 0
0 9 0 1
1
end_operator
begin_operator
moveinthemodel#reinitiaterequest 
0
3
0 0 -1 1
0 4 -1 0
0 5 0 1
1
end_operator
begin_operator
moveinthemodel#rejectrequest 
0
3
0 0 -1 1
0 3 0 1
0 8 -1 0
1
end_operator
begin_operator
moveinthemodel#skip_5 
0
3
0 0 -1 1
0 3 -1 0
0 5 0 1
0
end_operator
begin_operator
moveinthemodel#tausplit_3 
0
4
0 0 -1 1
0 1 -1 0
0 4 0 1
0 6 -1 0
0
end_operator
begin_operator
movesync#checkticket#ev2 
0
4
0 0 -1 0
0 6 0 1
0 7 -1 0
0 10 1 2
0
end_operator
begin_operator
movesync#decide#ev4 
0
5
0 0 -1 0
0 2 0 1
0 5 -1 0
0 7 0 1
0 10 3 4
0
end_operator
begin_operator
movesync#examinethoroughly#ev3 
0
4
0 0 -1 0
0 1 0 1
0 2 -1 0
0 10 2 3
0
end_operator
begin_operator
movesync#registerrequest#ev1 
0
4
0 0 -1 0
0 4 -1 0
0 9 0 1
0 10 0 1
0
end_operator
begin_operator
movesync#rejectrequest#ev5 
0
4
0 0 -1 0
0 3 0 1
0 8 -1 0
0 10 4 5
0
end_operator
0
