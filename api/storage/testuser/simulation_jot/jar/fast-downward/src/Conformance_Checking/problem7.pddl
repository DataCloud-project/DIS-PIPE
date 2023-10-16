(define (problem Align) (:domain Mining)
(:objects
source - place
sink - place
p_6 - place
p_8 - place
p_12 - place
p_14 - place
p_15 - place
p_4 - place
p_5 - place
p_3 - place
p_7 - place
p_11 - place
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
(not (token p_6))
(not (token p_8))
(not (token p_12))
(not (token p_14))
(not (token p_15))
(not (token p_4))
(not (token p_5))
(not (token p_3))
(not (token p_7))
(not (token p_11))
(tracePointer evEND)
))
(:metric minimize (total-cost))
)