:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).
:- lib(edge_finder).

get_Tasks(Tasks) :- findall(T,tarefa(T,_,_,_,_),Tasks).

project2(File) :-
	compile(File),
	get_Tasks(Tasks), sort(Tasks,OTasks),
    calendario(Calend),
    build_list_days(1,DaysL,Calend),
    max_days(OTasks, Finish), length(Tasks,NTasks), 
    length(SDates, NTasks), SDates#::0..Finish,
    prec_constrs(OTasks,SDates,FinishDate),
    interval_constrs(OTasks,SDates),
    prazo(FinishDate),
    writeln(SDates).

max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.

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

prec_constrs([],_,_).
prec_constrs([T|RTarefas],Datas,Finish) :-
	tarefa(T,LTSegs,Di,_,_),
	selec_elemento(1,T,Datas,DataI),
    prec_constrs_(LTSegs,Datas,DataI,Di),
    DataI+Di #=< Finish,
    prec_constrs(RTarefas,Datas,Finish).

prec_constrs_([],_,_,_).
prec_constrs_([J|RTSegs],Datas,DataI,Di) :-
    selec_elemento(1,J,Datas,DataJ),
    DataI+Di #=< DataJ,
    prec_constrs_(RTSegs,Datas,DataI,Di).

%Selects element in the position T on a list. (CurrentPosition,T,List,ElementAtTPosition)
selec_elemento(T,T,[I|_],I) :- !.
selec_elemento(T0,T,[_|R],I) :-  T0n is T0+1, selec_elemento(T0n,T,R,I).