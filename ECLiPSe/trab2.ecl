:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).

get_Tasks(Tasks) :- findall(T,tarefa(T,_,_,_,_),Tasks).

get_Workers(Workers) :- findall(W,trabalhador(W,_),Workers).

project2(File) :-
	compile(File),
	get_Tasks(Tasks), sort(Tasks,OTasks),
    build_calendar(Calen),
    length(Tasks,NTasks), 
    length(Days, NTasks), Days#::Calen,
    length(Hours, NTasks), Hours#::8..17,
    hour_constrs(OTasks, Hours),
    get_last_day(Calen,FinishD),
    prec_days_constrs(OTasks,Days,Hours,FinishD),
    workers_constrs.
    %interval_constrs(OTasks,SDates),
    %prazo(FinishDate),
    %writeln(SDates).

max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.

build_calendar(Calen) :-
    calendario(Calend),
    build_first_day(Calen,Calend).

build_first_day([1|CalenL],[d(_,_,D)|Calend]) :- build_list_days(1,CalenL,Calend,D).

build_list_days(_,[],[],_).
build_list_days(T,[T2|CalenL],[d(_,_,S)|L], D) :- S > D, Dif is S - D, T2 is T + Dif, build_list_days(T2,CalenL,L,S), !.
build_list_days(T,[T2|CalenL],[d(_,_,S)|L], D) :- D > S, Dif is D - S, T2 is T + Dif - 1, build_list_days(T2,CalenL,L,S), !.
build_list_days(T,[T2|CalenL],[d(_,_,S)|L], D) :- D == S, Dif is 7, T2 is T + Dif, build_list_days(T2,CalenL,L,S), !.

get_last_day([X], X).
get_last_day([_|L], X) :- get_last_day(L,X), !.

hour_constrs([],_).
hour_constrs([T|RTarefas], Hours) :-
    tarefa(T,_,Hi,_,_),
    selec_elemento(1,T,Hours,HourI),
    ((HourI #>= 8, HourI + Hi #=< 12) or (HourI #>= 13, HourI + Hi #=< 17)),
    hour_constrs(RTarefas,Hours), !.

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

workers_constrs().

interval_constrs([],_).
interval_constrs([T|RTarefas],Datas) :-
    findall((PrevT,Min,Max), intervalo(T,PrevT,Min,Max), IntL),
    selec_elemento(1,T,Datas,DataI),
    interval_constrs_(IntL,Datas,DataI),
    interval_constrs(RTarefas,Datas).

interval_constrs_([],_,_).
interval_constrs_([(J,Min,Max)|RTSegs],Datas,DataI) :-
    selec_elemento(1,J,Datas,DataJ),
    DataI #>= DataJ + Min, DataI #=< DataJ + Max,
    interval_constrs_(RTSegs,Datas,DataI).