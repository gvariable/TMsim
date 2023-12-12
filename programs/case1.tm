#Q = {0,cpa,cpb,mhl1,mhl3,mhr3,accept,clear,reject,reject1,reject2,reject3,reject4,reject5,reject6,reject7,reject8,reject9,reject10,reject11,reject12,reject13,halt_reject}

#S = {a,b}

#G = {a,b,c,_,I,l,l,e,g,a,l,_,I,n,p,u,t}

#B = _

#q0 = 0

#F = {accept}

#N = 3

; start state
0 a__ a__ *** cpa
0 b__ b__ *** reject
0 ___ ___ *** reject

; copy a 
cpa a__ aa_ rr* cpa
cpa b__ b__ *** cpb
; only a no b
cpa _** ___ *** reject
cpa _*_ ___ *** reject
cpa __* ___ *** reject
cpa ___ ___ *** reject


; copy b
cpb b__ b_b r*r cpb
cpb ___ ___ lll mhl1
cpb a__ a__ *** reject

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

; reject and write Illegal_Input on tape1
reject *** *__ r** reject
reject **_ *__ r** reject
reject *_* *__ r** reject
reject *__ *__ r** reject
reject ___ ___ l** clear

clear *** ___ l** clear
clear **_ ___ l** clear
clear *_* ___ l** clear
clear *__ ___ l** clear
clear ___ ___ *** reject1

reject1 _** I__ r** reject2
reject1 _*_ I__ r** reject2
reject1 __* I__ r** reject2
reject1 ___ I__ r** reject2

reject2 _** l__ r** reject3
reject2 _*_ l__ r** reject3
reject2 __* l__ r** reject3
reject2 ___ l__ r** reject3

reject3 _** l__ r** reject4
reject3 _*_ l__ r** reject4
reject3 __* l__ r** reject4
reject3 ___ l__ r** reject4

reject4 _** e__ r** reject5
reject4 _*_ e__ r** reject5
reject4 __* e__ r** reject5
reject4 ___ e__ r** reject5

reject5 _** g__ r** reject6
reject5 _*_ g__ r** reject6
reject5 __* g__ r** reject6
reject5 ___ g__ r** reject6

reject6 _** a__ r** reject7
reject6 _*_ a__ r** reject7
reject6 __* a__ r** reject7
reject6 ___ a__ r** reject7

reject7 _** l__ r** reject8
reject7 _*_ l__ r** reject8
reject7 __* l__ r** reject8
reject7 ___ l__ r** reject8

reject8 _** ___ r** reject9
reject8 _*_ ___ r** reject9
reject8 __* ___ r** reject9
reject8 ___ ___ r** reject9

reject9 _** I__ r** reject10
reject9 _*_ I__ r** reject10
reject9 __* I__ r** reject10
reject9 ___ I__ r** reject10

reject10 _** n__ r** reject11
reject10 _*_ n__ r** reject11
reject10 __* n__ r** reject11
reject10 ___ n__ r** reject11

reject11 _** p__ r** reject12
reject11 _*_ p__ r** reject12
reject11 __* p__ r** reject12
reject11 ___ p__ r** reject12

reject12 _** u__ r** reject13
reject12 _*_ u__ r** reject13
reject12 __* u__ r** reject13
reject12 ___ u__ r** reject13

reject13 _** t__ r** halt_reject
reject13 _*_ t__ r** halt_reject
reject13 __* t__ r** halt_reject
reject13 ___ t__ r** halt_reject