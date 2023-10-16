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

(:action moveSync#event12end#ev8
:precondition (and (token source) (tracePointer ev8))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev8)) (tracePointer evEND))
)

(:action moveInTheModel#event12end
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveSync#analyzedata#ev4
:precondition (and (token p_4) (tracePointer ev4))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#analyzedata#ev5
:precondition (and (token p_4) (tracePointer ev5))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveSync#analyzedata#ev6
:precondition (and (token p_4) (tracePointer ev6))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev6)) (tracePointer ev7))
)

(:action moveInTheModel#analyzedata
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveSync#createrecord#ev1
:precondition (and (token p_5) (tracePointer ev1))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev1)) (tracePointer ev2))
)

(:action moveInTheModel#createrecord
:precondition (token p_5)
:effect (and (not (allowed)) (not (token p_5)) (token p_6) (increase (total-cost) 1)
)
)

(:action moveInTheModel#event2start
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveInTheModel#receivetempmeasure
:precondition (token p_9)
:effect (and (not (allowed)) (not (token p_9)) (token p_10) (increase (total-cost) 1)
)
)

(:action moveInTheModel#senddatatomq
:precondition (token p_10)
:effect (and (not (allowed)) (not (token p_10)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveSync#startanalysis#ev2
:precondition (and (token p_11) (tracePointer ev2))
:effect (and (allowed) (not (token p_11)) (token sink) (not (tracePointer ev2)) (tracePointer ev3))
)

(:action moveSync#startanalysis#ev3
:precondition (and (token p_11) (tracePointer ev3))
:effect (and (allowed) (not (token p_11)) (token sink) (not (tracePointer ev3)) (tracePointer ev4))
)

(:action moveInTheModel#startanalysis
:precondition (token p_11)
:effect (and (not (allowed)) (not (token p_11)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#endanalysis#ev7
:precondition (and (token p_6) (tracePointer ev7))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev7)) (tracePointer ev8))
)

(:action moveInTheModel#endanalysis
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_7) (increase (total-cost) 1)
)
)

(:action moveInTheModel#receivebloodmeasure
:precondition (token p_7)
:effect (and (not (allowed)) (not (token p_7)) (token p_8) (increase (total-cost) 1)
)
)

(:action moveInTheModel#receivedatafrommq
:precondition (token p_8)
:effect (and (not (allowed)) (not (token p_8)) (token p_9) (increase (total-cost) 1)
)
)

(:action moveInTheLog#createrecord#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#startanalysis#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#startanalysis#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#analyzedata#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#analyzedata#ev5-ev6
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer ev6) (increase (total-cost) 1)
))

(:action moveInTheLog#analyzedata#ev6-ev7
:precondition (and (tracePointer ev6) (allowed))
:effect (and (not (tracePointer ev6)) (tracePointer ev7) (increase (total-cost) 1)
))

(:action moveInTheLog#endanalysis#ev7-ev8
:precondition (and (tracePointer ev7) (allowed))
:effect (and (not (tracePointer ev7)) (tracePointer ev8) (increase (total-cost) 1)
))

(:action moveInTheLog#event12end#ev8-evEND
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer evEND) (increase (total-cost) 1)
))

)