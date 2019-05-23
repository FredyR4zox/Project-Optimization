:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).

get_Tasks(Tasks) :- findall(T,tarefa(T,_,_,_,_),Tasks).

get_Workers(Workers) :- findall(W,trabalhador(W,_),Workers).

project2(File) :-
	compile(File),
	get_Tasks(Tasks), sort(Tasks,OTasks),
    length(Tasks,NTasks), length(Hours, NTasks),
    get_Workers(Workers), sort(Workers, OWorkers), length(Workers,NWorkers),
    specialties(Workers,ListSpecialties), length(ListSpecialties,NSpecialties),
    Hours#::[8..11,13..16], hour_constrs(OTasks,Hours),
    length(Days, NTasks), build_calendar(Calen, DayL),
    Days#::Calen, FinishD#::1..DayL,
    prec_days_constrs(OTasks,Days,Hours,FinishD),
    %interval_constrs(OTasks,Days,Hours),
    build_matrix(NTasks,NWorkers,NSpecialties,M),
    workers_constrs(M, OTasks, OWorkers, ListSpecialties),
    check_sobr(OTasks,OTasks,Days,Hours,M),
    minimize(labeling([FinishD|Days]), FinishD),
    labeling(M),
    writeln(M).

max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.

build_calendar(Calen,DayL) :-
    calendario(Calend),
    build_first_day(Calen,Calend),
    prazo(Term),
    find_term_day(Calen,Calend,Term,DayL).

build_first_day([1|CalenL],[d(_,_,S)|Calend]) :- build_days_list(1,CalenL,Calend,S).

build_days_list(_,[],[],_).
build_days_list(T,[T2|CalenL],[d(_,_,S)|L], D) :- S > D, Dif is S - D, T2 is T + Dif, build_days_list(T2,CalenL,L,S), !.
build_days_list(T,[T2|CalenL],[d(_,_,S)|L], D) :- D > S, Dif is D - S, T2 is T + Dif - 1, build_days_list(T2,CalenL,L,S), !.
build_days_list(T,[T2|CalenL],[d(_,_,S)|L], D) :- D == S, Dif is 7, T2 is T + Dif, build_days_list(T2,CalenL,L,S), !.

find_term_day([Day|_], [d(D,M,S)|_], d(D,M,S), Day).
find_term_day([_|LCalen], [d(D,M,S)|L], d(D2,M2,S2), DayL) :- (D \= D2; M \= M2; S \= S2),
                                            find_term_day(LCalen, L, d(D2,M2,S2), DayL).

hour_constrs([],_).
hour_constrs([T|RTarefas], Hours) :-
    tarefa(T,_,Hi,_,_),
    selec_elemento(1,T,Hours,HourI),
    HourI + Hi #= FinishH,
    FinishH#::[9..12,14..17],
    hour_constrs(RTarefas, Hours). 

prec_days_constrs([],_,_,_).
prec_days_constrs([T|RTarefas],Datas,Hours,Finish) :-
    tarefa(T,LTSegs,Hi,_,_),
    selec_elemento(1,T,Datas,DataI),
    selec_elemento(1,T,Hours,HourI),
    prec_days_constrs_(LTSegs,Datas,Hours,DataI,HourI,Hi),
    DataI #=< Finish,
    prec_days_constrs(RTarefas,Datas,Hours,Finish).

prec_days_constrs_([],_,_,_,_,_).
prec_days_constrs_([J|RTSegs],Datas,Hours,DataI,HourI,Hi) :-
    selec_elemento(1,J,Datas,DataJ),
    selec_elemento(1,J,Hours,HourJ),
    ((DataI #= DataJ, HourI + Hi #=< HourJ) or DataI #< DataJ), 
    prec_days_constrs_(RTSegs,Datas,Hours,DataI, HourI,Hi).

%Selects element in the position T on a list. (CurrentPosition,T,List,ElementAtTPosition)
selec_elemento(T,T,[I|_],I) :- !.
selec_elemento(T0,T,[_|R],I) :-  T0n is T0+1, selec_elemento(T0n,T,R,I).

specialties([],_) :- !.
specialties([W|LW],SpecialtiesL) :- trabalhador(W, LS), specialties(LW,NL), check_membership(LS,NL, NL2),
                        append(NL,NL2,SpecialtiesL), !. 

check_membership([],_,[]).
check_membership([X|LS],SpecialtiesL,[X|NL]) :- not(belongs_to(X,SpecialtiesL)), check_membership(LS,SpecialtiesL, NL).
check_membership([X|LS],SpecialtiesL,NL) :- belongs_to(X,SpecialtiesL), check_membership(LS,SpecialtiesL, NL).

belongs_to(X,[X|_]) :- !.
belongs_to(X,[Y|L]) :- Y \= X, !, belongs_to(X,L).

build_matrix(0,_,_,[]) :- !.
build_matrix(NTasks,NWorkers,NSpecialties,[H|WMatrix]) :- length(H,NWorkers), build_matrix2(NWorkers,NSpecialties,H),
            NTasks2 is NTasks - 1, build_matrix(NTasks2,NWorkers,NSpecialties,WMatrix).

build_matrix2(0,_,[]) :- !.
build_matrix2(NWorkers,NSpecialties,[H|SMatrix]) :- length(H,NSpecialties),
            NWorkers2 is NWorkers - 1, build_matrix2(NWorkers2,NSpecialties,SMatrix).

workers_constrs(_,[],_,_).
workers_constrs([TiL|TWMatrix], [T|OTasksL], OWorkers, ListSpecialties) :- tarefa(T,_,_,WL,_),
                        limit_domain(TiL,WL,OWorkers,ListSpecialties), count_workers(WL, WorkersN), 
                        limit_ocurrences(TiL), fill_domain(TiL), flatten(TiL,TiL2), occurrences(1, TiL2, WorkersN),
                        workers_constrs(TWMatrix,OTasksL,OWorkers,ListSpecialties).

%If a specialist is need in a task and the current worker (Wi) has that specialtie then it can either apply it or not 
%at this thask.
limit_domain([],_,[],_).
limit_domain([Ti|TiL],WL,[Wi|OWorkers],ListSpecialties) :- trabalhador(Wi,SL),
                    limit_domain_(ListSpecialties,Ti,WL,SL),
                    limit_domain(TiL,WL,OWorkers,ListSpecialties).

%S - list of specialties
%SiL - list of specialties of worker i in the current task. (binary)
%WL - list of specialties the worker i has.
%SL - list of specialties current task needs.
limit_domain_([],[],_,_).
limit_domain_([S|SpL],[Si|SiL],WL,SL) :- belongs_to2(S,WL), belongs_to(S,SL), !, Si#::0..1, limit_domain_(SpL,SiL,WL,SL).
limit_domain_([_|SpL],[_|SiL],WL,SL) :- limit_domain_(SpL,SiL,WL,SL).


fill_domain([]).
fill_domain([X|L]) :- fill_domain_(X), fill_domain(L).

fill_domain_([]).
fill_domain_([Si|L]) :- get_max(Si,1), !, fill_domain_(L).
fill_domain_([Si|L]) :- Si #= 0, fill_domain_(L).

belongs_to2(X,[r(X,_)|_]) :- !.
belongs_to2(X,[r(Y,_)|L]) :- Y \= X, !, belongs_to2(X,L).
                                
count_workers([],0).
count_workers([r(_,W)|L],Sum) :- count_workers(L,Sum2), Sum is Sum2 + W.

%Limits the specialty of a worker if assigned to a task to 1, or 0 if not assgined.
limit_ocurrences([]).
limit_ocurrences([X|L]) :- atmost(1,X,1), limit_ocurrences(L).

check_sobr([],_,_,_,_).
check_sobr([T|OTasks],OTasks2,Days,Hours,M) :-
    tarefa(T,_,Hi,WL,_),
    selec_elemento(1,T,Hours,HourI),
    selec_elemento(1,T,Days,DayI),
    selec_elemento(1,T,M,TiL),
    count_workers(WL, WorkersNI),
    check_sobr_(OTasks2,Days,Hours, M,DayI, HourI, Hi, TiL, WorkersNI),
    check_sobr(OTasks,OTasks2,Days,Hours,M).

check_sobr_([],_,_,_,_,_,_,_,_).
check_sobr_([J|OTasks2],Days,Hours, M, DayI, HourI, Hi, TiL, WorkersNI) :-
    tarefa(J,_,_,WL,_),
    selec_elemento(1,J,Hours,HourJ),
    selec_elemento(1,J,Days,DayJ),    
    selec_elemento(1,J,M,TjL),
    count_workers(WL, WorkersNJ),
    SumW is WorkersNJ + WorkersNI,
    DayI #= DayJ, HourJ #>= HourI, HourJ #< HourI + Hi,
    append(TiL,TjL,At), flatten(At,At2), occurrences(1, At2, SumW),
    check_sobr_(OTasks2,Days,Hours, M, DayI, HourI, Hi, TiL, WorkersNI).
check_sobr_([_|OTasks2],Days,Hours, M, DayI, HourI, Hi, TiL, WorkersNI) :- 
    check_sobr_(OTasks2,Days,Hours, M, DayI, HourI, Hi, TiL, WorkersNI).

interval_constrs([],_,_).
interval_constrs([T|RTarefas],Days,Hours) :-
    findall((PrevT,Min,Max), intervalo(T,PrevT,Min,Max), IntL),
    selec_elemento(1,T,Hours,HourI),
    selec_elemento(1,T,Days,DayI),
    interval_constrs_(IntL,Days,Hours,DayI,HourI),
    interval_constrs(RTarefas,Days,Hours).

interval_constrs_([],_,_,_,_).
interval_constrs_([(J,Min,Max)|RTSegs],Days,Hours,DayI,HourI) :-
    tarefa(J,_,Hj,_,_),
    selec_elemento(1,J,Hours,HourJ),
    selec_elemento(1,J,Days,DayJ),
    MinH is floor(Min / 24), MaxH is floor(Max / 24),
    L is DayJ + MinH,
    DayI #>= L, DayI #=< L,
    MinL #= ((HourJ + Hj + Min) mod 24), MaxL #= ((HourJ + Hj + Max) mod 24),
    HourI #>= HourJ + Hj + Min, HourI #=< HourJ + Hj + Max,
    interval_constrs_(RTSegs,Days,Hours,DayI,HourI).