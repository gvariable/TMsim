#Q = {0,cpa,cpb,mhl1,mhl3,mhr3,accept}

#S = {a,b}

#G = {a,b,c,_}

#B = _

#q0 = 0

#F = {accept}

#N = 3

; start state
0 a__ a__ *** cpa

; copy a 
cpa a__ aa_ rr* cpa
cpa b__ b__ *** cpb


; copy b
cpb b__ b_b r*r cpb
cpb ___ ___ lll mhl1

; move 1st head to left
mhl1 *** _** l** mhl1
mhl1 _** _** r** mhl3

; move 3st head to left
mhl3 _ab cab r*l mhl3
mhl3 _a_ ___ *lr mhr3
mhl3 __* __* l** accept

; move 3st head to right
mhr3 _ab cab r*r mhr3
mhr3 _a_ ___ *ll mhl3
mhr3 __* __* l** accept