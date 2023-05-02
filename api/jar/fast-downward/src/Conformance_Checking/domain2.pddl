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

(:action moveInTheModel#generatesampledatareformatpushinmqtt_end
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveInTheModel#analyzeandcreatenotificationforhealthcarepersonnel_end
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token sink) (increase (total-cost) 1)
)
)

(:action moveInTheModel#receivedatafrommqttcheckpatientplanbuildfhirdbrecordsstoretofhirdb_end
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveInTheLog#registerrequest#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#paycompensation#ev5-evEND
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer evEND) (increase (total-cost) 1)
))

)