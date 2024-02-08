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

(:action moveInTheModel#a
:precondition (token p1)
:effect (and (not (allowed)) (not (token p1)) (token p2) (increase (total-cost) 1)
)
)

(:action moveInTheModel#b
:precondition (token p2)
:effect (and (not (allowed)) (not (token p2)) (token p3) (increase (total-cost) 1)
)
)

(:action moveInTheModel#c
:precondition (token p3)
:effect (and (not (allowed)) (not (token p3)) (token p4) (increase (total-cost) 1)
)
)

(:action moveInTheModel#d
:precondition (token p4)
:effect (and (not (allowed)) (not (token p4)) (token p5) (increase (total-cost) 1)
)
)

(:action moveInTheLog#e#ev1-ev2
:precondition (and (tracePointer ev1) (allowed))
:effect (and (not (tracePointer ev1)) (tracePointer ev2) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev2-ev3
:precondition (and (tracePointer ev2) (allowed))
:effect (and (not (tracePointer ev2)) (tracePointer ev3) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev3-ev4
:precondition (and (tracePointer ev3) (allowed))
:effect (and (not (tracePointer ev3)) (tracePointer ev4) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev4-ev5
:precondition (and (tracePointer ev4) (allowed))
:effect (and (not (tracePointer ev4)) (tracePointer ev5) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev5-ev6
:precondition (and (tracePointer ev5) (allowed))
:effect (and (not (tracePointer ev5)) (tracePointer ev6) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev6-ev7
:precondition (and (tracePointer ev6) (allowed))
:effect (and (not (tracePointer ev6)) (tracePointer ev7) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev7-ev8
:precondition (and (tracePointer ev7) (allowed))
:effect (and (not (tracePointer ev7)) (tracePointer ev8) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev8-ev9
:precondition (and (tracePointer ev8) (allowed))
:effect (and (not (tracePointer ev8)) (tracePointer ev9) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev9-ev10
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer ev10) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev10-ev11
:precondition (and (tracePointer ev10) (allowed))
:effect (and (not (tracePointer ev10)) (tracePointer ev11) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev11-ev12
:precondition (and (tracePointer ev11) (allowed))
:effect (and (not (tracePointer ev11)) (tracePointer ev12) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev12-ev13
:precondition (and (tracePointer ev12) (allowed))
:effect (and (not (tracePointer ev12)) (tracePointer ev13) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev13-ev14
:precondition (and (tracePointer ev13) (allowed))
:effect (and (not (tracePointer ev13)) (tracePointer ev14) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev14-ev15
:precondition (and (tracePointer ev14) (allowed))
:effect (and (not (tracePointer ev14)) (tracePointer ev15) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev15-ev16
:precondition (and (tracePointer ev15) (allowed))
:effect (and (not (tracePointer ev15)) (tracePointer ev16) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev16-ev17
:precondition (and (tracePointer ev16) (allowed))
:effect (and (not (tracePointer ev16)) (tracePointer ev17) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev17-ev18
:precondition (and (tracePointer ev17) (allowed))
:effect (and (not (tracePointer ev17)) (tracePointer ev18) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev18-ev19
:precondition (and (tracePointer ev18) (allowed))
:effect (and (not (tracePointer ev18)) (tracePointer ev19) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev19-ev20
:precondition (and (tracePointer ev19) (allowed))
:effect (and (not (tracePointer ev19)) (tracePointer ev20) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev20-ev21
:precondition (and (tracePointer ev20) (allowed))
:effect (and (not (tracePointer ev20)) (tracePointer ev21) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev21-ev22
:precondition (and (tracePointer ev21) (allowed))
:effect (and (not (tracePointer ev21)) (tracePointer ev22) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev22-ev23
:precondition (and (tracePointer ev22) (allowed))
:effect (and (not (tracePointer ev22)) (tracePointer ev23) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev23-ev24
:precondition (and (tracePointer ev23) (allowed))
:effect (and (not (tracePointer ev23)) (tracePointer ev24) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev24-ev25
:precondition (and (tracePointer ev24) (allowed))
:effect (and (not (tracePointer ev24)) (tracePointer ev25) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev25-ev26
:precondition (and (tracePointer ev25) (allowed))
:effect (and (not (tracePointer ev25)) (tracePointer ev26) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev26-ev27
:precondition (and (tracePointer ev26) (allowed))
:effect (and (not (tracePointer ev26)) (tracePointer ev27) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev27-ev28
:precondition (and (tracePointer ev27) (allowed))
:effect (and (not (tracePointer ev27)) (tracePointer ev28) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev28-ev29
:precondition (and (tracePointer ev28) (allowed))
:effect (and (not (tracePointer ev28)) (tracePointer ev29) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev29-ev30
:precondition (and (tracePointer ev29) (allowed))
:effect (and (not (tracePointer ev29)) (tracePointer ev30) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev30-ev31
:precondition (and (tracePointer ev30) (allowed))
:effect (and (not (tracePointer ev30)) (tracePointer ev31) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev31-ev32
:precondition (and (tracePointer ev31) (allowed))
:effect (and (not (tracePointer ev31)) (tracePointer ev32) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev32-ev33
:precondition (and (tracePointer ev32) (allowed))
:effect (and (not (tracePointer ev32)) (tracePointer ev33) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev33-ev34
:precondition (and (tracePointer ev33) (allowed))
:effect (and (not (tracePointer ev33)) (tracePointer ev34) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev34-ev35
:precondition (and (tracePointer ev34) (allowed))
:effect (and (not (tracePointer ev34)) (tracePointer ev35) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev35-ev36
:precondition (and (tracePointer ev35) (allowed))
:effect (and (not (tracePointer ev35)) (tracePointer ev36) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev36-ev37
:precondition (and (tracePointer ev36) (allowed))
:effect (and (not (tracePointer ev36)) (tracePointer ev37) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev37-ev38
:precondition (and (tracePointer ev37) (allowed))
:effect (and (not (tracePointer ev37)) (tracePointer ev38) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev38-ev39
:precondition (and (tracePointer ev38) (allowed))
:effect (and (not (tracePointer ev38)) (tracePointer ev39) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev39-ev40
:precondition (and (tracePointer ev39) (allowed))
:effect (and (not (tracePointer ev39)) (tracePointer ev40) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev40-ev41
:precondition (and (tracePointer ev40) (allowed))
:effect (and (not (tracePointer ev40)) (tracePointer ev41) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev41-ev42
:precondition (and (tracePointer ev41) (allowed))
:effect (and (not (tracePointer ev41)) (tracePointer ev42) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev42-ev43
:precondition (and (tracePointer ev42) (allowed))
:effect (and (not (tracePointer ev42)) (tracePointer ev43) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev43-ev44
:precondition (and (tracePointer ev43) (allowed))
:effect (and (not (tracePointer ev43)) (tracePointer ev44) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev44-ev45
:precondition (and (tracePointer ev44) (allowed))
:effect (and (not (tracePointer ev44)) (tracePointer ev45) (increase (total-cost) 1)
))

(:action moveInTheLog#e#ev45-ev46
:precondition (and (tracePointer ev45) (allowed))
:effect (and (not (tracePointer ev45)) (tracePointer ev46) (increase (total-cost) 1)
))

(:action moveInTheLog#f#ev46-ev47
:precondition (and (tracePointer ev46) (allowed))
:effect (and (not (tracePointer ev46)) (tracePointer ev47) (increase (total-cost) 1)
))

(:action moveInTheLog#g#ev47-ev48
:precondition (and (tracePointer ev47) (allowed))
:effect (and (not (tracePointer ev47)) (tracePointer ev48) (increase (total-cost) 1)
))

(:action moveInTheLog#h#ev48-evEND
:precondition (and (tracePointer ev48) (allowed))
:effect (and (not (tracePointer ev48)) (tracePointer evEND) (increase (total-cost) 1)
))

)