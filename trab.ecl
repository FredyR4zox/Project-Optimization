:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).
:- lib(edge_finder).

:- compile(base_dados).

/* TODO:
project_earliest_start - Calcular o numero de trabalahdores para o numero minimo de dias caclculado : Quando se calcula o numero de trabalhadores apenas analisar os dias em que as tarefas comecam ou acabam, e nao todos 
os dias.
project_minimize_workers - pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores. Usar cumulative(OrderedTasks,Durations,WorkersLimit,1000000).
*/

% tarefa(ID,LPrecs,Dur,NTrabs)
read_data_base(Tasks) :- findall(T,tarefa(T,_,_,_),Tasks).

/* Determinar o numero minimo de dias (algoritmo do caminho critico) e depois atribuir o numero de trabalhadores necessarios para esses mesmos dias */
project_earliest_start :- read_data_base(Tasks),
						length(Tasks,NTasks), length(ESTasksL, NTasks),
						max_days(Tasks, MaxD), ESTasksL#::0..MaxD, Finish#::0..MaxD,
						prec_constrs(Tasks,ESTasksL,Finish),
						minimize(labeling([Finish|ESTasksL]),Finish),
						build_index_list(Tasks,ESTasksL,IndexTasks),
						quicksort(IndexTasks,OrderedTasks),
						build_constraint_list(OrderedTasks,FinishL),
						quicksort(FinishL,OrderedFinish),
						find_nworkers(OrderedTasks,OrderedFinish,0,WorkersN),
						write_solution(Finish,WorkersN).

/* Pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores */
project_minimize_workers :- read_data_base(Tasks),
							max_workers(Tasks,MaxW),
							Workers#::0..MaxW,
							length(Tasks,NTasks), length(OrderedTasks, NTasks),
							max_days(Tasks, MaxD), 
							OrderedTasks#::0..MaxD, Finish#::0..MaxD,
							prec_constrs(Tasks,OrderedTasks,Finish),
							cumulative(OrderedTasks,Duration,WorkersN,Workers),
							writeln(Workers).

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
find_nworkers([(Si,T)|LT],[(Fi,T2)|FL],WorkersA,WorkersN) :- Si == Fi, tarefa(T,_,_,W), tarefa(T2,_,_,W2), WorkersA2 is WorkersA + W, WorkersA3 is WorkersA2 - W2,
														find_nworkers(LT,FL, WorkersA3, WorkersN2),
														WorkersN is max(WorkersN2,WorkersA3), !.  
find_nworkers([(Si,T)|LT],[(Fi,T2)|FL],WorkersA,WorkersN) :- Si > Fi, tarefa(T2,_,_,W), WorkersA2 is WorkersA - W,
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

write_solution(Finish,Workers) :- write("Found a solution in "), write(Finish), write(" days, needing "), write(Workers),
								writeln(" workers").