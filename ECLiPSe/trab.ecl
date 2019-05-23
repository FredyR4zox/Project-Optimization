:- lib(ic).
:- lib(ic_cumulative).
:- lib(branch_and_bound).

:- dynamic(getGoalCount/1).

project(File) :- 
	compile(File), 
	read_data_base(Tasks), sort(Tasks, OTasks), get_constraints(DurationL,WorkersL),
	get_min_days(OTasks,ESTasksL,Finish),	
	write("Number of days: "), writeln(Finish),
	order_information(OTasks,ESTasksL,OrderedTasks,OrderedFinish),
	find_nworkers(OrderedTasks,OrderedFinish,0,WorkersN),
	write("Number of Workers for ES: "), writeln(WorkersN),
	writeln("Start Times for ES: "), print_times(ESTasksL), nl,
	length(OTasks,NTasks), length(STasksL, NTasks),
	STasksL#::0..Finish,
	prec_constrs(OTasks,STasksL,Finish),
	get_critical_tasks(1, STasksL, CritTasks),
	get_critical_workers(CritTasks, WorkersCrit),
	write("Number of Workers for crit: "), writeln(WorkersCrit),
	minimize_workers(STasksL, DurationL, WorkersL, WorkersN, WorkersCrit, WorkersMin),
	write("Min number of Workers: "), writeln(WorkersMin), nl,
	writeln("Start Times for Min Workers: "), print_times(STasksL),
	count(find_another_sol(OTasks,DurationL,WorkersL, Finish, WorkersMin), Count),
	(Count > 1, !, writeln("Existem solucoes alternativas"); writeln("Solucao unica")).

% Gets the minimum number of workers for the given finish date.
minimize_workers(STasksL, DurationL, WorkersL, WorkersN, WorkersCrit, WorkersMin) :-
	WorkersMin#::WorkersCrit..WorkersN,
	cumulative(STasksL,DurationL,WorkersL,WorkersMin),
	minimize(search([WorkersMin|STasksL],0,first_fail,indomain_min,complete,[]), WorkersMin).

% tarefa(ID,LPrecs,Dur,NTrabs)
read_data_base(Tasks) :- findall(T,tarefa(T,_,_,_),Tasks).

%Gets the list of Durations and Workers needed for each Task
get_constraints(DurationL, WorkersL) :- findall(D,tarefa(_,_,D,_),DurationL),
							findall(W,tarefa(_,_,_,W),WorkersL).

%Prints start times of each task
print_times([]) :- nl.
print_times([Xi|L]) :- write(Xi), write(" "), print_times(L).

%Minimizes the project's number of days
get_min_days(Tasks,ESTasksL,Finish) :- 
	length(Tasks,NTasks), length(ESTasksL, NTasks),
	max_days(Tasks, MaxD), ESTasksL#::0..MaxD, Finish#::0..MaxD,
	prec_constrs(Tasks,ESTasksL,Finish),
	minimize(labeling([Finish|ESTasksL]),Finish).

%finds the days upper bound
max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.

% precedence restrictions
prec_constrs([],_,_).
prec_constrs([T|RTarefas],Datas,Finish) :-
	tarefa(T,LTSegs,Di,_),
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

%Builds the ES and LS lists.
order_information(Tasks,ESTasksL,OrderedTasks,OrderedFinish) :-
	build_index_list(Tasks,ESTasksL,IndexTasks),
	quicksort(IndexTasks,OrderedTasks),
	build_finish_list(OrderedTasks,FinishL),
	quicksort(FinishL,OrderedFinish).

%Builds a list of pairs (Di,T), where Di is the start time of task T
build_index_list([],[],[]).
build_index_list([T|LT],[Di|L],[(Di,T)|LR]) :- build_index_list(LT,L,LR).

%Builds a list of pairs (F,T), where F is the finish time of Task T.
build_finish_list([],[]).
build_finish_list([(Di,T)|LR],[(F,T)|LD]) :- tarefa(T,_,D,_), F is Di+D, build_finish_list(LR,LD).

%Iterates throw the ordered lists of start and finish times of each task, and each time a task finishs just remove the number of workers needed of
%that, and when a task starts add the number of workers.
find_nworkers([],[],0,0).
find_nworkers([(_,T)|LT],[],WorkersA,WorkersN) :- tarefa(T,_,_,W), WorkersA2 is WorkersA + W,
										find_nworkers(LT,[], WorkersA2, WorkersN2),
										WorkersN is max(WorkersN2,WorkersA2), !.
find_nworkers([],[(_,T)|FL],WorkersA,WorkersN) :- tarefa(T,_,_,W), WorkersA2 is WorkersA - W,
										find_nworkers([],FL, WorkersA2, WorkersN2),
										WorkersN is max(WorkersN2,WorkersA2), !.
find_nworkers([(Si,T)|LT],[(Fi,T2)|FL],WorkersA,WorkersN) :- Si < Fi, tarefa(T,_,_,W), WorkersA2 is WorkersA + W,
										find_nworkers(LT,[(Fi,T2)|FL], WorkersA2, WorkersN2),
										WorkersN is max(WorkersN2,WorkersA2), !.
find_nworkers([(Si,T)|LT],[(Fi,T2)|FL],WorkersA,WorkersN) :- Si >= Fi, tarefa(T2,_,_,W), WorkersA2 is WorkersA - W,
										find_nworkers([(Si,T)|LT],FL, WorkersA2, WorkersN2),
										WorkersN is max(WorkersN2,WorkersA2), !.

%Gets the critical tasks
get_critical_tasks(_,[],[]).
get_critical_tasks(T, [Xi|L], [(Xi,T)|CritTasksL]) :- get_bounds(Xi,LB,UB), LB=UB, T2 is T + 1, get_critical_tasks(T2,L,CritTasksL), !.
get_critical_tasks(T, [Xi|L], CritTasksL) :- get_bounds(Xi,LB,UB), not(LB=UB), T2 is T + 1, get_critical_tasks(T2,L,CritTasksL), !.

%Gets the number of critical workers
get_critical_workers(CritTasks, WorkersCrit) :-
	quicksort(CritTasks,OrderedTasks),
	build_finish_list(OrderedTasks,FinishL),
	quicksort(FinishL,OrderedFinish),
	find_nworkers(OrderedTasks,OrderedFinish,0,WorkersCrit).

count(Goal,Count) :-
    setGoalCount(0),
    call(Goal),
    getGoalCount(Count), 
    ((Count >= 2, !, true); (incGoalCount(1), fail)).
count(_,Count) :-
    getGoalCount(Count).

setGoalCount(_) :-
    retract(getGoalCount(_)),
    fail.
setGoalCount(X) :-
    assert(getGoalCount(X)).

incGoalCount(Y) :-
    retract(getGoalCount(X)),
    !,
    Z is X + Y, assert(getGoalCount(Z)).

%Used to find different start times for each Task given a finish date and the number of workers.
find_another_sol(Tasks,DurationL,WorkersL, Finish, WorkersMin) :-
	length(Tasks,NTasks), length(CTasks, NTasks),
	CTasks#::0..Finish,
	prec_constrs(Tasks,CTasks,Finish),
	cumulative(CTasks,DurationL, WorkersL, WorkersMin),
	search(CTasks,0,first_fail,indomain_min,complete,[]).

partition2([],[],_,[]).
partition2([(Y,T2)|L2], L3, (X,T), [(Y,T2)|L]) :- Y =< X, partition2(L2,L3,(X,T),L), !.
partition2(L2, [(Y,T2)|L3], (X,T), [(Y,T2)|L]) :- Y > X,  partition2(L2,L3,(X,T),L), !.

quicksort([],[]).
quicksort([(X,T)|L],LR) :- partition2(LL,LH,(X,T),L), quicksort(LL,LR2), quicksort(LH,LR3),
					  append(LR2,[(X,T)|LR3], LR), !.