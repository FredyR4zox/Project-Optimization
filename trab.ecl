:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).
:- lib(edge_finder).

:- compile(base_dados).

% tarefa(ID,LPrecs,Dur,NTrabs)
read_data_base(Tasks) :- findall(T,tarefa(T,_,_,_),Tasks).

get_constraints(DurationL, WorkersL) :- findall(D,tarefa(_,_,D,_),DurationL),
							findall(W,tarefa(_,_,_,W),WorkersL).


% Determinar o numero minimo de dias (algoritmo do caminho critico) e depois atribuir o numero de trabalhadores necessarios para esses mesmos dias
project :- read_data_base(Tasks),
	length(Tasks,NTasks), length(ESTasksL, NTasks),
	max_days(Tasks, MaxD), ESTasksL#::0..MaxD, Finish#::0..MaxD,
	prec_constrs(Tasks,ESTasksL,Finish),
	minimize(labeling([Finish|ESTasksL]),Finish),
	build_index_list(Tasks,ESTasksL,IndexTasks),
	quicksort(IndexTasks,OrderedTasks),
	build_constraint_list(OrderedTasks,FinishL),
	quicksort(FinishL,OrderedFinish),
	find_nworkers(OrderedTasks,OrderedFinish,0,WorkersN),
	writeln("Solution for earliest start:"),
	writeln(ESTasksL),
	write_solution(Finish,WorkersN),
	minimize_workers(WorkersN, MaxD).

% Pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores
minimize_workers(WorkersN, MaxD) :- writeln(""),
						read_data_base(Tasks), get_constraints(DurationL,WorkersL),
						length(Tasks,NTasks), length(ESTasksL, NTasks),
						Workers#::0..WorkersN, ESTasksL#::0..MaxD, Finish#::0..MaxD,
						prec_constrs(Tasks,ESTasksL,Finish),
						minimize(labeling([Finish]), Finish),
						cumulative(ESTasksL,DurationL,WorkersL,Workers),
						minimize(labeling([Workers|ESTasksL]), Workers),
						writeln("Solution to minimize workers: "),
						writeln(ESTasksL),
						write_solution(Finish,Workers).

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

partition2([],[],_,[]).
partition2([(Y,T2)|L2], L3, (X,T), [(Y,T2)|L]) :- Y =< X, partition2(L2,L3,(X,T),L), !.
partition2(L2, [(Y,T2)|L3], (X,T), [(Y,T2)|L]) :- Y > X,  partition2(L2,L3,(X,T),L), !.

quicksort([],[]).
quicksort([(X,T)|L],LR) :- partition2(LL,LH,(X,T),L), quicksort(LL,LR2), quicksort(LH,LR3),
					  append(LR2,[(X,T)|LR3], LR), !.

build_index_list([],[],[]).
build_index_list([T|LT],[Di|L],[(Di,T)|LR]) :- build_index_list(LT,L,LR).

build_constraint_list([],[]).
build_constraint_list([(Di,T)|LR],[(F,T)|LD]) :- tarefa(T,_,D,_), F is Di+D, build_constraint_list(LR,LD).

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

%finds the days upper bound
max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.

%finds the workers upper bound
max_workers([],0).
max_workers([T|LTasks],Sum) :- tarefa(T,_,_,W), max_workers(LTasks,Sum2), Sum is Sum2+W.

write_solution(Finish,Workers) :- write("Finishes in "), write(Finish), write(" days, and needs "), write(Workers),
						writeln(" workers").