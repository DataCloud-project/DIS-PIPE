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

(:action moveInTheModel#model_generated
:precondition (token p_5)
:effect (and (not (allowed)) (not (token p_5)) (token p_6) (increase (total-cost) 1)
)
)

(:action moveSync#query#ev4
:precondition (and (token p_6) (tracePointer ev4))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#query#ev5
:precondition (and (token p_6) (tracePointer ev5))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveInTheModel#query
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_7) (increase (total-cost) 1)
)
)

(:action moveSync#report_generated#ev12
:precondition (and (token p_7) (tracePointer ev12))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev12)) (tracePointer evEND))
)

(:action moveInTheModel#report_generated
:precondition (token p_7)
:effect (and (not (allowed)) (not (token p_7)) (token p_8) (increase (total-cost) 1)
)
)

(:action moveSync#request#ev1
:precondition (and (token p_8) (tracePointer ev1))
:effect (and (allowed) (not (token p_8)) (token p_11) (not (tracePointer ev1)) (tracePointer ev2))
)

(:action moveInTheModel#request
:precondition (token p_8)
:effect (and (not (allowed)) (not (token p_8)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveInTheModel#store_results
:precondition (token p_12)
:effect (and (not (allowed)) (not (token p_12)) (token p_14) (increase (total-cost) 1)
)
)

(:action moveSync#visualization#ev10
:precondition (and (token p_15) (tracePointer ev10))
:effect (and (allowed) (not (token p_15)) (token sink) (not (tracePointer ev10)) (tracePointer ev11))
)

(:action moveSync#visualization#ev11
:precondition (and (token p_15) (tracePointer ev11))
:effect (and (allowed) (not (token p_15)) (token sink) (not (tracePointer ev11)) (tracePointer ev12))
)

(:action moveInTheModel#visualization
:precondition (token p_15)
:effect (and (not (allowed)) (not (token p_15)) (token sink) (increase (total-cost) 1)
)
)

(:action moveSync#data_merge#ev8
:precondition (and (token source) (tracePointer ev8))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev8)) (tracePointer ev9))
)

(:action moveSync#data_merge#ev9
:precondition (and (token source) (tracePointer ev9))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev9)) (tracePointer ev10))
)

(:action moveInTheModel#data_merge
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveInTheModel#install_supplementary_resources
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveInTheModel#launch_algorithm
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveInTheModel#restore_accessto_database
:precondition (token p_11)
:effect (and (not (allowed)) (not (token p_11)) (token p_12) (increase (total-cost) 1)
)
)

(:action moveSync#query#ev4
:precondition (and (token p_12) (tracePointer ev4))
:effect (and (allowed) (not (token p_12)) (token p_11) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#query#ev5
:precondition (and (token p_12) (tracePointer ev5))
:effect (and (allowed) (not (token p_12)) (token p_11) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveInTheModel#query
:precondition (token p_12)
:effect (and (not (allowed)) (not (token p_12)) (token p_11) (increase (total-cost) 1)
)
)

(:action moveSync#trasformation#ev6
:precondition (and (token p_14) (tracePointer ev6))
:effect (and (allowed) (not (token p_14)) (token p_15) (not (tracePointer ev6)) (tracePointer ev7))
)

(:action moveSync#trasformation#ev7
:precondition (and (token p_14) (tracePointer ev7))
:effect (and (allowed) (not (token p_14)) (token p_15) (not (tracePointer ev7)) (tracePointer ev8))
)

(:action moveInTheModel#trasformation
:precondition (token p_14)
:effect (and (not (allowed)) (not (token p_14)) (token p_15) (increase (total-cost) 1)
)
)

(:action moveInTheLog#request#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#restoreaccessto_database#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#restoreaccessto_database#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#query#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#query#ev5-ev6
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer ev6) (increase (total-cost) 1)
))

(:action moveInTheLog#trasformation#ev6-ev7
:precondition (and (tracePointer ev6) (allowed))
:effect (and (not (tracePointer ev6)) (tracePointer ev7) (increase (total-cost) 1)
))

(:action moveInTheLog#trasformation#ev7-ev8
:precondition (and (tracePointer ev7) (allowed))
:effect (and (not (tracePointer ev7)) (tracePointer ev8) (increase (total-cost) 1)
))

(:action moveInTheLog#data_merge#ev8-ev9
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer ev9) (increase (total-cost) 1)
))

(:action moveInTheLog#data_merge#ev9-ev10
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer ev10) (increase (total-cost) 1)
))

(:action moveInTheLog#visualization#ev10-ev11
:precondition (and (tracePointer ev10) (allowed))
:effect (and (not (tracePointer ev10)) (tracePointer ev11) (increase (total-cost) 1)
))

(:action moveInTheLog#visualization#ev11-ev12
:precondition (and (tracePointer ev11) (allowed))
:effect (and (not (tracePointer ev11)) (tracePointer ev12) (increase (total-cost) 1)
))

(:action moveInTheLog#report_generated#ev12-evEND
:precondition (and (tracePointer ev12) (allowed))
:effect (and (not (tracePointer ev12)) (tracePointer evEND) (increase (total-cost) 1)
))

)