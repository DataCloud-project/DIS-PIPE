(define (problem Align) (:domain Mining)
(:objects
source - place
sink - place
p_4 - place
p_3 - place
ev1 - event
ev2 - event
ev3 - event
ev4 - event
ev5 - event
ev6 - event
ev7 - event
ev8 - event
ev9 - event
evEND - event
)
(:init
(tracePointer ev1)
(allowed)
(token source)
(= (total-cost) 0)
)
(:goal
(and
(not (token source))
(token sink)
(not (token p_4))
(not (token p_3))
(tracePointer evEND)
))
(:metric minimize (total-cost))
)