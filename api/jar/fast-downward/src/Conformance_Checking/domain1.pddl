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

(:action moveSync#checkticket#ev3
:precondition (and (token source) (tracePointer ev3))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev3)) (tracePointer ev4))
)

(:action moveSync#checkticket#ev7
:precondition (and (token source) (tracePointer ev7))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev7)) (tracePointer ev8))
)

(:action moveSync#checkticket#ev11
:precondition (and (token source) (tracePointer ev11))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev11)) (tracePointer ev12))
)

(:action moveSync#checkticket#ev17
:precondition (and (token source) (tracePointer ev17))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev17)) (tracePointer ev18))
)

(:action moveSync#checkticket#ev22
:precondition (and (token source) (tracePointer ev22))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev22)) (tracePointer ev23))
)

(:action moveSync#checkticket#ev27
:precondition (and (token source) (tracePointer ev27))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev27)) (tracePointer ev28))
)

(:action moveSync#checkticket#ev30
:precondition (and (token source) (tracePointer ev30))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev30)) (tracePointer ev31))
)

(:action moveSync#checkticket#ev35
:precondition (and (token source) (tracePointer ev35))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev35)) (tracePointer ev36))
)

(:action moveSync#checkticket#ev39
:precondition (and (token source) (tracePointer ev39))
:effect (and (allowed) (not (token source)) (token p_3) (not (tracePointer ev39)) (tracePointer ev40))
)

(:action moveInTheModel#checkticket
:precondition (token source)
:effect (and (not (allowed)) (not (token source)) (token p_3) (increase (total-cost) 1)
)
)

(:action moveSync#decide#ev4
:precondition (and (token p_3) (tracePointer ev4))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev4)) (tracePointer ev5))
)

(:action moveSync#decide#ev8
:precondition (and (token p_3) (tracePointer ev8))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev8)) (tracePointer ev9))
)

(:action moveSync#decide#ev13
:precondition (and (token p_3) (tracePointer ev13))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev13)) (tracePointer ev14))
)

(:action moveSync#decide#ev18
:precondition (and (token p_3) (tracePointer ev18))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev18)) (tracePointer ev19))
)

(:action moveSync#decide#ev23
:precondition (and (token p_3) (tracePointer ev23))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev23)) (tracePointer ev24))
)

(:action moveSync#decide#ev28
:precondition (and (token p_3) (tracePointer ev28))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev28)) (tracePointer ev29))
)

(:action moveSync#decide#ev32
:precondition (and (token p_3) (tracePointer ev32))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev32)) (tracePointer ev33))
)

(:action moveSync#decide#ev36
:precondition (and (token p_3) (tracePointer ev36))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev36)) (tracePointer ev37))
)

(:action moveSync#decide#ev41
:precondition (and (token p_3) (tracePointer ev41))
:effect (and (allowed) (not (token p_3)) (token p_4) (not (tracePointer ev41)) (tracePointer ev42))
)

(:action moveInTheModel#decide
:precondition (token p_3)
:effect (and (not (allowed)) (not (token p_3)) (token p_4) (increase (total-cost) 1)
)
)

(:action moveSync#examinethoroughly#ev6
:precondition (and (token p_5) (tracePointer ev6))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev6)) (tracePointer ev7))
)

(:action moveSync#examinethoroughly#ev16
:precondition (and (token p_5) (tracePointer ev16))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev16)) (tracePointer ev17))
)

(:action moveSync#examinethoroughly#ev40
:precondition (and (token p_5) (tracePointer ev40))
:effect (and (allowed) (not (token p_5)) (token p_6) (not (tracePointer ev40)) (tracePointer ev41))
)

(:action moveInTheModel#examinethoroughly
:precondition (token p_5)
:effect (and (not (allowed)) (not (token p_5)) (token p_6) (increase (total-cost) 1)
)
)

(:action moveSync#examinecasually#ev2
:precondition (and (token p_4) (tracePointer ev2))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev2)) (tracePointer ev3))
)

(:action moveSync#examinecasually#ev12
:precondition (and (token p_4) (tracePointer ev12))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev12)) (tracePointer ev13))
)

(:action moveSync#examinecasually#ev21
:precondition (and (token p_4) (tracePointer ev21))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev21)) (tracePointer ev22))
)

(:action moveSync#examinecasually#ev26
:precondition (and (token p_4) (tracePointer ev26))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev26)) (tracePointer ev27))
)

(:action moveSync#examinecasually#ev31
:precondition (and (token p_4) (tracePointer ev31))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev31)) (tracePointer ev32))
)

(:action moveSync#examinecasually#ev34
:precondition (and (token p_4) (tracePointer ev34))
:effect (and (allowed) (not (token p_4)) (token p_5) (not (tracePointer ev34)) (tracePointer ev35))
)

(:action moveInTheModel#examinecasually
:precondition (token p_4)
:effect (and (not (allowed)) (not (token p_4)) (token p_5) (increase (total-cost) 1)
)
)

(:action moveSync#paycompensation#ev9
:precondition (and (token p_6) (tracePointer ev9))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev9)) (tracePointer ev10))
)

(:action moveSync#paycompensation#ev14
:precondition (and (token p_6) (tracePointer ev14))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev14)) (tracePointer ev15))
)

(:action moveSync#paycompensation#ev24
:precondition (and (token p_6) (tracePointer ev24))
:effect (and (allowed) (not (token p_6)) (token p_7) (not (tracePointer ev24)) (tracePointer ev25))
)

(:action moveInTheModel#paycompensation
:precondition (token p_6)
:effect (and (not (allowed)) (not (token p_6)) (token p_7) (increase (total-cost) 1)
)
)

(:action moveSync#reinitiaterequest#ev5
:precondition (and (token p_8) (tracePointer ev5))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev5)) (tracePointer ev6))
)

(:action moveSync#reinitiaterequest#ev29
:precondition (and (token p_8) (tracePointer ev29))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev29)) (tracePointer ev30))
)

(:action moveSync#reinitiaterequest#ev33
:precondition (and (token p_8) (tracePointer ev33))
:effect (and (allowed) (not (token p_8)) (token p_9) (not (tracePointer ev33)) (tracePointer ev34))
)

(:action moveInTheModel#reinitiaterequest
:precondition (token p_8)
:effect (and (not (allowed)) (not (token p_8)) (token p_9) (increase (total-cost) 1)
)
)

(:action moveSync#registerrequest#ev1
:precondition (and (token p_7) (tracePointer ev1))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev1)) (tracePointer ev2))
)

(:action moveSync#registerrequest#ev10
:precondition (and (token p_7) (tracePointer ev10))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev10)) (tracePointer ev11))
)

(:action moveSync#registerrequest#ev15
:precondition (and (token p_7) (tracePointer ev15))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev15)) (tracePointer ev16))
)

(:action moveSync#registerrequest#ev20
:precondition (and (token p_7) (tracePointer ev20))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev20)) (tracePointer ev21))
)

(:action moveSync#registerrequest#ev25
:precondition (and (token p_7) (tracePointer ev25))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev25)) (tracePointer ev26))
)

(:action moveSync#registerrequest#ev38
:precondition (and (token p_7) (tracePointer ev38))
:effect (and (allowed) (not (token p_7)) (token p_8) (not (tracePointer ev38)) (tracePointer ev39))
)

(:action moveInTheModel#registerrequest
:precondition (token p_7)
:effect (and (not (allowed)) (not (token p_7)) (token p_8) (increase (total-cost) 1)
)
)

(:action moveSync#rejectrequest#ev19
:precondition (and (token p_9) (tracePointer ev19))
:effect (and (allowed) (not (token p_9)) (token sink) (not (tracePointer ev19)) (tracePointer ev20))
)

(:action moveSync#rejectrequest#ev37
:precondition (and (token p_9) (tracePointer ev37))
:effect (and (allowed) (not (token p_9)) (token sink) (not (tracePointer ev37)) (tracePointer ev38))
)

(:action moveSync#rejectrequest#ev42
:precondition (and (token p_9) (tracePointer ev42))
:effect (and (allowed) (not (token p_9)) (token sink) (not (tracePointer ev42)) (tracePointer evEND))
)

(:action moveInTheModel#rejectrequest
:precondition (token p_9)
:effect (and (not (allowed)) (not (token p_9)) (token sink) (increase (total-cost) 1)
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

(:action moveInTheLog#paycompensation#ev9-ev10
:precondition (and (tracePointer ev9) (allowed))
:effect (and (not (tracePointer ev9)) (tracePointer ev10) (increase (total-cost) 1)
))

(:action moveInTheLog#registerrequest#ev10-ev11
:precondition (and (tracePointer ev10) (allowed))
:effect (and (not (tracePointer ev10)) (tracePointer ev11) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev11-ev12
:precondition (and (tracePointer ev11) (allowed))
:effect (and (not (tracePointer ev11)) (tracePointer ev12) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev12-ev13
:precondition (and (tracePointer ev12) (allowed))
:effect (and (not (tracePointer ev12)) (tracePointer ev13) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev13-ev14
:precondition (and (tracePointer ev13) (allowed))
:effect (and (not (tracePointer ev13)) (tracePointer ev14) (increase (total-cost) 1)
))

(:action moveInTheLog#paycompensation#ev14-ev15
:precondition (and (tracePointer ev14) (allowed))
:effect (and (not (tracePointer ev14)) (tracePointer ev15) (increase (total-cost) 1)
))

(:action moveInTheLog#registerrequest#ev15-ev16
:precondition (and (tracePointer ev15) (allowed))
:effect (and (not (tracePointer ev15)) (tracePointer ev16) (increase (total-cost) 1)
))

(:action moveInTheLog#examinethoroughly#ev16-ev17
:precondition (and (tracePointer ev16) (allowed))
:effect (and (not (tracePointer ev16)) (tracePointer ev17) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev17-ev18
:precondition (and (tracePointer ev17) (allowed))
:effect (and (not (tracePointer ev17)) (tracePointer ev18) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev18-ev19
:precondition (and (tracePointer ev18) (allowed))
:effect (and (not (tracePointer ev18)) (tracePointer ev19) (increase (total-cost) 1)
))

(:action moveInTheLog#rejectrequest#ev19-ev20
:precondition (and (tracePointer ev19) (allowed))
:effect (and (not (tracePointer ev19)) (tracePointer ev20) (increase (total-cost) 1)
))

(:action moveInTheLog#registerrequest#ev20-ev21
:precondition (and (tracePointer ev20) (allowed))
:effect (and (not (tracePointer ev20)) (tracePointer ev21) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev21-ev22
:precondition (and (tracePointer ev21) (allowed))
:effect (and (not (tracePointer ev21)) (tracePointer ev22) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev22-ev23
:precondition (and (tracePointer ev22) (allowed))
:effect (and (not (tracePointer ev22)) (tracePointer ev23) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev23-ev24
:precondition (and (tracePointer ev23) (allowed))
:effect (and (not (tracePointer ev23)) (tracePointer ev24) (increase (total-cost) 1)
))

(:action moveInTheLog#paycompensation#ev24-ev25
:precondition (and (tracePointer ev24) (allowed))
:effect (and (not (tracePointer ev24)) (tracePointer ev25) (increase (total-cost) 1)
))

(:action moveInTheLog#registerrequest#ev25-ev26
:precondition (and (tracePointer ev25) (allowed))
:effect (and (not (tracePointer ev25)) (tracePointer ev26) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev26-ev27
:precondition (and (tracePointer ev26) (allowed))
:effect (and (not (tracePointer ev26)) (tracePointer ev27) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev27-ev28
:precondition (and (tracePointer ev27) (allowed))
:effect (and (not (tracePointer ev27)) (tracePointer ev28) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev28-ev29
:precondition (and (tracePointer ev28) (allowed))
:effect (and (not (tracePointer ev28)) (tracePointer ev29) (increase (total-cost) 1)
))

(:action moveInTheLog#reinitiaterequest#ev29-ev30
:precondition (and (tracePointer ev29) (allowed))
:effect (and (not (tracePointer ev29)) (tracePointer ev30) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev30-ev31
:precondition (and (tracePointer ev30) (allowed))
:effect (and (not (tracePointer ev30)) (tracePointer ev31) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev31-ev32
:precondition (and (tracePointer ev31) (allowed))
:effect (and (not (tracePointer ev31)) (tracePointer ev32) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev32-ev33
:precondition (and (tracePointer ev32) (allowed))
:effect (and (not (tracePointer ev32)) (tracePointer ev33) (increase (total-cost) 1)
))

(:action moveInTheLog#reinitiaterequest#ev33-ev34
:precondition (and (tracePointer ev33) (allowed))
:effect (and (not (tracePointer ev33)) (tracePointer ev34) (increase (total-cost) 1)
))

(:action moveInTheLog#examinecasually#ev34-ev35
:precondition (and (tracePointer ev34) (allowed))
:effect (and (not (tracePointer ev34)) (tracePointer ev35) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev35-ev36
:precondition (and (tracePointer ev35) (allowed))
:effect (and (not (tracePointer ev35)) (tracePointer ev36) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev36-ev37
:precondition (and (tracePointer ev36) (allowed))
:effect (and (not (tracePointer ev36)) (tracePointer ev37) (increase (total-cost) 1)
))

(:action moveInTheLog#rejectrequest#ev37-ev38
:precondition (and (tracePointer ev37) (allowed))
:effect (and (not (tracePointer ev37)) (tracePointer ev38) (increase (total-cost) 1)
))

(:action moveInTheLog#registerrequest#ev38-ev39
:precondition (and (tracePointer ev38) (allowed))
:effect (and (not (tracePointer ev38)) (tracePointer ev39) (increase (total-cost) 1)
))

(:action moveInTheLog#checkticket#ev39-ev40
:precondition (and (tracePointer ev39) (allowed))
:effect (and (not (tracePointer ev39)) (tracePointer ev40) (increase (total-cost) 1)
))

(:action moveInTheLog#examinethoroughly#ev40-ev41
:precondition (and (tracePointer ev40) (allowed))
:effect (and (not (tracePointer ev40)) (tracePointer ev41) (increase (total-cost) 1)
))

(:action moveInTheLog#decide#ev41-ev42
:precondition (and (tracePointer ev41) (allowed))
:effect (and (not (tracePointer ev41)) (tracePointer ev42) (increase (total-cost) 1)
))

(:action moveInTheLog#rejectrequest#ev42-evEND
:precondition (and (tracePointer ev42) (allowed))
:effect (and (not (tracePointer ev42)) (tracePointer evEND) (increase (total-cost) 1)
))

)