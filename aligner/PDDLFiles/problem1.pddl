(define (problem Align) (:domain Mining)
(:objects
p1 - place
p2 - place
p3 - place
p4 - place
p5 - place
ev1 - event
ev2 - event
ev3 - event
ev4 - event
ev5 - event
ev6 - event
ev7 - event
ev8 - event
ev9 - event
ev10 - event
ev11 - event
ev12 - event
ev13 - event
ev14 - event
ev15 - event
ev16 - event
ev17 - event
ev18 - event
ev19 - event
ev20 - event
ev21 - event
ev22 - event
ev23 - event
ev24 - event
ev25 - event
ev26 - event
ev27 - event
ev28 - event
ev29 - event
ev30 - event
ev31 - event
ev32 - event
ev33 - event
ev34 - event
ev35 - event
ev36 - event
ev37 - event
ev38 - event
ev39 - event
ev40 - event
ev41 - event
ev42 - event
ev43 - event
ev44 - event
ev45 - event
ev46 - event
ev47 - event
ev48 - event
evEND - event
)
(:init
(tracePointer ev1)
(allowed)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(token p1)
(= (total-cost) 0)
)
(:goal
(and
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(not (token p1))
(not (token p2))
(not (token p3))
(not (token p4))
(token p5)
(tracePointer evEND)
))
(:metric minimize (total-cost))
)