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

(:action moveSync#event12end#ev2
:precondition (and (token source) (tracePointer ev2))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev2)) (tracePointer evEND))
)

(:action moveInTheModel#event12end
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveInTheModel#analyzedata
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
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

(:action moveInTheModel#startanalysis
:precondition (token p_11)
:effect (and (not (allowed)) (not (token p_11)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#endanalysis#ev1
:precondition (and (token p_6) (tracePointer ev1))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev1)) (tracePointer ev2))
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

(:action moveInTheLog#endanalysis#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#event12end#ev2-evEND
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer evEND) (increase (total-cost) 1)
))

)