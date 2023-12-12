; TODO(gpl): handle cases that multiple c occur

#Q = {0, cp, mhl1, cmp, reject, reject2, reject3, reject4, reject5, halt_reject, accept, accept2, accept3, accept4, halt_accept}

#S = {a,b,c}

#G = {a,b,c,_,t,r,u,e,f,a,l,s,e}

#B = _

#q0 = 0

#F = {halt_accept}

#N = 2


; start state
0 a_ a_ ** cp
0 b_ b_ ** cp
0 c_ __ r* mhl1

; copy tape1 content to tape2
cp a_ _a rr cp
cp b_ _b rr cp
cp c_ __ rl mhl1


; move head1 to left
mhl1 ** ** *l mhl1
mhl1 *_ *_ *r cmp

; compare the contents of tape1 and tape2 
cmp aa __ rr cmp
cmp bb __ rr cmp 
cmp ab __ rr reject
cmp a_ __ rr reject
cmp ba __ rr reject
cmp b_ __ rr reject
cmp c* _* r* reject
cmp __ __ ** accept

; reject and write false on tape1
reject ** __ r* reject
reject *_ __ r* reject
reject _* f_ r* reject2
reject __ f_ r* reject2
reject2 _* a_ r* reject3
reject2 __ a_ r* reject3
reject3 _* l_ r* reject4
reject3 __ l_ r* reject4
reject4 _* s_ r* reject5
reject4 __ s_ r* reject5
reject5 _* e_ ** halt_reject
reject5 __ e_ ** halt_reject

; accept and write true on tape1
accept __ t_ r* accept2
accept2 __ r_ r* accept3
accept3 __ u_ r* accept4
accept4 __ e_ ** halt_accept

