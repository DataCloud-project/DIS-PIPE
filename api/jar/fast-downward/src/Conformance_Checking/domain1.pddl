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

(:action moveSync#registerrequest#ev1
:precondition (and (token source) (tracePointer ev1))
:effect (and (allowed) (not (token source)) (token p_5) (not (tracePointer ev1)) (tracePointer ev2))
)

(:action moveInTheModel#registerrequest
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveInTheModel#tausplit_3
:precondition (token p_5)
:effect (and (not (allowed)) (not (token p_5)) (token p_10) (token p_8) (increase (total-cost) 0)
)
)

(:action moveSync#checkticket#ev3
:precondition (and (token p_8) (tracePointer ev3))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev3)) (tracePointer ev4))
)

(:action moveSync#checkticket#ev7
:precondition (and (token p_8) (tracePointer ev7))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev7)) (tracePointer ev8))
)

(:action moveInTheModel#checkticket
:precondition (token p_8)
:effect (and (not (allowed)) (not (token p_8)) (token p_9) (increase (total-cost) 1)
)
)

(:action moveSync#examinecasually#ev2
:precondition (and (token p_10) (tracePointer ev2))
:effect (and (allowed) (not (token p_10)) (token p_11) (not (tracePointer ev2)) (tracePointer ev3))
)

(:action moveInTheModel#examinecasually
:precondition (token p_10)
:effect (and (not (allowed)) (not (token p_10)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveSync#reinitiaterequest#ev5
:precondition (and (token p_6) (tracePointer ev5))
:effect (and (allowed) (not (token p_6)) (token p_5) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveInTheModel#reinitiaterequest
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveInTheModel#skip_5
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_4) (increase (total-cost) 0)
)
)

(:action moveInTheModel#rejectrequest
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#paycompensation#ev9
:precondition (and (token p_4) (tracePointer ev9))
:effect (and (allowed) (not (token p_4)) (token sink) (not (tracePointer ev9)) (tracePointer evEND))
)

(:action moveInTheModel#paycompensation
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#examinethoroughly#ev6
:precondition (and (token p_10) (tracePointer ev6))
:effect (and (allowed) (not (token p_10)) (token p_11) (not (tracePointer ev6)) (tracePointer ev7))
)

(:action moveInTheModel#examinethoroughly
:precondition (token p_10)
:effect (and (not (allowed)) (not (token p_10)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveSync#decide#ev4
:precondition (and (token p_11) (token p_9) (tracePointer ev4))
:effect (and (allowed) (not (token p_11)) (not (token p_9)) (token p_6) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#decide#ev8
:precondition (and (token p_11) (token p_9) (tracePointer ev8))
:effect (and (allowed) (not (token p_11)) (not (token p_9)) (token p_6) (not (tracePointer ev8)) (tracePointer ev9))
)

(:action moveInTheModel#decide
:precondition (and (token p_11) (token p_9))
:effect (and (not (allowed)) (not (token p_11)) (not (token p_9)) (token p_6) (increase (total-cost) 1)
)
)

(:action moveInTheLog#registerrequest#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#reinitiaterequest#ev5-ev6
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer ev6) (increase (total-cost) 1)
))

(:action moveInTheLog#examinethoroughly#ev6-ev7
:precondition (and (tracePointer ev6) (allowed))
:effect (and (not (tracePointer ev6)) (tracePointer ev7) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev7-ev8
:precondition (and (tracePointer ev7) (allowed))
:effect (and (not (tracePointer ev7)) (tracePointer ev8) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev8-ev9
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer ev9) (increase (total-cost) 1)
))

(:action moveInTheLog#paycompensation#ev9-evEND
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer evEND) (increase (total-cost) 1)
))

)