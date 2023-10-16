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

(:action moveSync#model_generated#ev16
:precondition (and (token p_5) (tracePointer ev16))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev16)) (tracePointer evEND))
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

(:action moveSync#store_results#ev14
:precondition (and (token p_12) (tracePointer ev14))
:effect (and (allowed) (not (token p_12)) (token p_14) (not (tracePointer ev14)) (tracePointer ev15))
)

(:action moveSync#store_results#ev15
:precondition (and (token p_12) (tracePointer ev15))
:effect (and (allowed) (not (token p_12)) (token p_14) (not (tracePointer ev15)) (tracePointer ev16))
)

(:action moveInTheModel#store_results
:precondition (token p_12)
:effect (and (not (allowed)) (not (token p_12)) (token p_14) (increase (total-cost) 1)
)
)

(:action moveSync#visualization#ev12
:precondition (and (token p_15) (tracePointer ev12))
:effect (and (allowed) (not (token p_15)) (token sink) (not (tracePointer ev12)) (tracePointer ev13))
)

(:action moveSync#visualization#ev13
:precondition (and (token p_15) (tracePointer ev13))
:effect (and (allowed) (not (token p_15)) (token sink) (not (tracePointer ev13)) (tracePointer ev14))
)

(:action moveInTheModel#visualization
:precondition (token p_15)
:effect (and (not (allowed)) (not (token p_15)) (token sink) (increase (total-cost) 1)
)
)

(:action moveInTheModel#data_merge
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveSync#install_supplementary_resources#ev8
:precondition (and (token p_3) (tracePointer ev8))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev8)) (tracePointer ev9))
)

(:action moveSync#install_supplementary_resources#ev9
:precondition (and (token p_3) (tracePointer ev9))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev9)) (tracePointer ev10))
)

(:action moveInTheModel#install_supplementary_resources
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveSync#launch_algorithm#ev10
:precondition (and (token p_4) (tracePointer ev10))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev10)) (tracePointer ev11))
)

(:action moveSync#launch_algorithm#ev11
:precondition (and (token p_4) (tracePointer ev11))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev11)) (tracePointer ev12))
)

(:action moveInTheModel#launch_algorithm
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveSync#restore_accessto_database#ev2
:precondition (and (token p_11) (tracePointer ev2))
:effect (and (allowed) (not (token p_11)) (token p_12) (not (tracePointer ev2)) (tracePointer ev3))
)

(:action moveSync#restore_accessto_database#ev3
:precondition (and (token p_11) (tracePointer ev3))
:effect (and (allowed) (not (token p_11)) (token p_12) (not (tracePointer ev3)) (tracePointer ev4))
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

(:action moveInTheLog#restore_accessto_database#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#restore_accessto_database#ev3-ev4
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

(:action moveInTheLog#install_supplementary_resources#ev8-ev9
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer ev9) (increase (total-cost) 1)
))

(:action moveInTheLog#install_supplementary_resources#ev9-ev10
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer ev10) (increase (total-cost) 1)
))

(:action moveInTheLog#launch_algorithm#ev10-ev11
:precondition (and (tracePointer ev10) (allowed))
:effect (and (not (tracePointer ev10)) (tracePointer ev11) (increase (total-cost) 1)
))

(:action moveInTheLog#launch_algorithm#ev11-ev12
:precondition (and (tracePointer ev11) (allowed))
:effect (and (not (tracePointer ev11)) (tracePointer ev12) (increase (total-cost) 1)
))

(:action moveInTheLog#visualization#ev12-ev13
:precondition (and (tracePointer ev12) (allowed))
:effect (and (not (tracePointer ev12)) (tracePointer ev13) (increase (total-cost) 1)
))

(:action moveInTheLog#visualization#ev13-ev14
:precondition (and (tracePointer ev13) (allowed))
:effect (and (not (tracePointer ev13)) (tracePointer ev14) (increase (total-cost) 1)
))

(:action moveInTheLog#store_results#ev14-ev15
:precondition (and (tracePointer ev14) (allowed))
:effect (and (not (tracePointer ev14)) (tracePointer ev15) (increase (total-cost) 1)
))

(:action moveInTheLog#store_results#ev15-ev16
:precondition (and (tracePointer ev15) (allowed))
:effect (and (not (tracePointer ev15)) (tracePointer ev16) (increase (total-cost) 1)
))

(:action moveInTheLog#model_generated#ev16-evEND
:precondition (and (tracePointer ev16) (allowed))
:effect (and (not (tracePointer ev16)) (tracePointer evEND) (increase (total-cost) 1)
))

)