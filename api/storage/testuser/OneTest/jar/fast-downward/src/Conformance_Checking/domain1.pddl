(define (domain Mining)
(:requirements :typing :equality)
(:types place event)

(:predicates
(token ?p - place)
(tracePointer ?e - event)
(allowed)
)

(:functions
(total-cost)
)

(:action moveSync#event12end#ev21
:precondition (and (token source) (tracePointer ev21))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev21)) (tracePointer evEND))
)

(:action moveInTheModel#event12end
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveSync#analyzedata#ev12
:precondition (and (token p_4) (tracePointer ev12))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev12)) (tracePointer ev13))
)

(:action moveInTheModel#analyzedata
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveSync#createrecord#ev7
:precondition (and (token p_5) (tracePointer ev7))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev7)) (tracePointer ev8))
)

(:action moveSync#createrecord#ev11
:precondition (and (token p_5) (tracePointer ev11))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev11)) (tracePointer ev12))
)

(:action moveInTheModel#createrecord
:precondition (token p_5)
:effect (and (not (allowed)) (not (token p_5)) (token p_6) (increase (total-cost) 1)
)
)

(:action moveSync#event2start#ev1
:precondition (and (token p_3) (tracePointer ev1))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev1)) (tracePointer ev2))
)

(:action moveSync#event2start#ev5
:precondition (and (token p_3) (tracePointer ev5))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveSync#event2start#ev14
:precondition (and (token p_3) (tracePointer ev14))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev14)) (tracePointer ev15))
)

(:action moveSync#event2start#ev18
:precondition (and (token p_3) (tracePointer ev18))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev18)) (tracePointer ev19))
)

(:action moveInTheModel#event2start
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveSync#receivetempmeasure#ev19
:precondition (and (token p_9) (tracePointer ev19))
:effect (and (allowed) (not (token p_9)) (token p_10) (not (tracePointer ev19)) (tracePointer ev20))
)

(:action moveInTheModel#receivetempmeasure
:precondition (token p_9)
:effect (and (not (allowed)) (not (token p_9)) (token p_10) (increase (total-cost) 1)
)
)

(:action moveSync#senddatatomq#ev3
:precondition (and (token p_10) (tracePointer ev3))
:effect (and (allowed) (not (token p_10)) (token p_11) (not (tracePointer ev3)) (tracePointer ev4))
)

(:action moveSync#senddatatomq#ev8
:precondition (and (token p_10) (tracePointer ev8))
:effect (and (allowed) (not (token p_10)) (token p_11) (not (tracePointer ev8)) (tracePointer ev9))
)

(:action moveSync#senddatatomq#ev16
:precondition (and (token p_10) (tracePointer ev16))
:effect (and (allowed) (not (token p_10)) (token p_11) (not (tracePointer ev16)) (tracePointer ev17))
)

(:action moveInTheModel#senddatatomq
:precondition (token p_10)
:effect (and (not (allowed)) (not (token p_10)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveSync#startanalysis#ev10
:precondition (and (token p_11) (tracePointer ev10))
:effect (and (allowed) (not (token p_11)) (token sink) (not (tracePointer ev10)) (tracePointer ev11))
)

(:action moveSync#startanalysis#ev13
:precondition (and (token p_11) (tracePointer ev13))
:effect (and (allowed) (not (token p_11)) (token sink) (not (tracePointer ev13)) (tracePointer ev14))
)

(:action moveInTheModel#startanalysis
:precondition (token p_11)
:effect (and (not (allowed)) (not (token p_11)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#endanalysis#ev20
:precondition (and (token p_6) (tracePointer ev20))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev20)) (tracePointer ev21))
)

(:action moveInTheModel#endanalysis
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_7) (increase (total-cost) 1)
)
)

(:action moveSync#receivebloodmeasure#ev2
:precondition (and (token p_7) (tracePointer ev2))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev2)) (tracePointer ev3))
)

(:action moveSync#receivebloodmeasure#ev6
:precondition (and (token p_7) (tracePointer ev6))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev6)) (tracePointer ev7))
)

(:action moveSync#receivebloodmeasure#ev15
:precondition (and (token p_7) (tracePointer ev15))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev15)) (tracePointer ev16))
)

(:action moveInTheModel#receivebloodmeasure
:precondition (token p_7)
:effect (and (not (allowed)) (not (token p_7)) (token p_8) (increase (total-cost) 1)
)
)

(:action moveSync#receivedatafrommq#ev4
:precondition (and (token p_8) (tracePointer ev4))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#receivedatafrommq#ev9
:precondition (and (token p_8) (tracePointer ev9))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev9)) (tracePointer ev10))
)

(:action moveSync#receivedatafrommq#ev17
:precondition (and (token p_8) (tracePointer ev17))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev17)) (tracePointer ev18))
)

(:action moveInTheModel#receivedatafrommq
:precondition (token p_8)
:effect (and (not (allowed)) (not (token p_8)) (token p_9) (increase (total-cost) 1)
)
)

(:action moveInTheLog#event2start#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#receivebloodmeasure#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#senddatatomq#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#receivedatafrommq#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#event2start#ev5-ev6
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer ev6) (increase (total-cost) 1)
))

(:action moveInTheLog#receivebloodmeasure#ev6-ev7
:precondition (and (tracePointer ev6) (allowed))
:effect (and (not (tracePointer ev6)) (tracePointer ev7) (increase (total-cost) 1)
))

(:action moveInTheLog#createrecord#ev7-ev8
:precondition (and (tracePointer ev7) (allowed))
:effect (and (not (tracePointer ev7)) (tracePointer ev8) (increase (total-cost) 1)
))

(:action moveInTheLog#senddatatomq#ev8-ev9
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer ev9) (increase (total-cost) 1)
))

(:action moveInTheLog#receivedatafrommq#ev9-ev10
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer ev10) (increase (total-cost) 1)
))

(:action moveInTheLog#startanalysis#ev10-ev11
:precondition (and (tracePointer ev10) (allowed))
:effect (and (not (tracePointer ev10)) (tracePointer ev11) (increase (total-cost) 1)
))

(:action moveInTheLog#createrecord#ev11-ev12
:precondition (and (tracePointer ev11) (allowed))
:effect (and (not (tracePointer ev11)) (tracePointer ev12) (increase (total-cost) 1)
))

(:action moveInTheLog#analyzedata#ev12-ev13
:precondition (and (tracePointer ev12) (allowed))
:effect (and (not (tracePointer ev12)) (tracePointer ev13) (increase (total-cost) 1)
))

(:action moveInTheLog#startanalysis#ev13-ev14
:precondition (and (tracePointer ev13) (allowed))
:effect (and (not (tracePointer ev13)) (tracePointer ev14) (increase (total-cost) 1)
))

(:action moveInTheLog#event2start#ev14-ev15
:precondition (and (tracePointer ev14) (allowed))
:effect (and (not (tracePointer ev14)) (tracePointer ev15) (increase (total-cost) 1)
))

(:action moveInTheLog#receivebloodmeasure#ev15-ev16
:precondition (and (tracePointer ev15) (allowed))
:effect (and (not (tracePointer ev15)) (tracePointer ev16) (increase (total-cost) 1)
))

(:action moveInTheLog#senddatatomq#ev16-ev17
:precondition (and (tracePointer ev16) (allowed))
:effect (and (not (tracePointer ev16)) (tracePointer ev17) (increase (total-cost) 1)
))

(:action moveInTheLog#receivedatafrommq#ev17-ev18
:precondition (and (tracePointer ev17) (allowed))
:effect (and (not (tracePointer ev17)) (tracePointer ev18) (increase (total-cost) 1)
))

(:action moveInTheLog#event2start#ev18-ev19
:precondition (and (tracePointer ev18) (allowed))
:effect (and (not (tracePointer ev18)) (tracePointer ev19) (increase (total-cost) 1)
))

(:action moveInTheLog#receivetempmeasure#ev19-ev20
:precondition (and (tracePointer ev19) (allowed))
:effect (and (not (tracePointer ev19)) (tracePointer ev20) (increase (total-cost) 1)
))

(:action moveInTheLog#endanalysis#ev20-ev21
:precondition (and (tracePointer ev20) (allowed))
:effect (and (not (tracePointer ev20)) (tracePointer ev21) (increase (total-cost) 1)
))

(:action moveInTheLog#event12end#ev21-evEND
:precondition (and (tracePointer ev21) (allowed))
:effect (and (not (tracePointer ev21)) (tracePointer evEND) (increase (total-cost) 1)
))

)