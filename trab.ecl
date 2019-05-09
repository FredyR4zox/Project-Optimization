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

read_data_base(Tasks) :- findall(T,tarefa(T,_,_,_),Tasks).

/* Determinar o numero minimo de dias (algoritmo do caminho critico) e depois atribuir o numero de trabalhadores necessarios para esses mesmos dias */
project_earliest_start :- read_data_base(Tasks),
						length(Tasks,NTasks), length(OrderedTasks, NTasks),
						max_days(Tasks, MaxD), 
						OrderedTasks#::0..MaxD, Finish#::0..MaxD,
						prec_constrs(Tasks,OrderedTasks,Finish),
						minimize(labeling([Finish|OrderedTasks]),Finish),
						writeln(OrderedTasks).			
						%find_number_workers(1,OrderedTasks,Workers),
						%write_solution(Finish,Workers).

/* Pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores */
project_minimize_workers :- read_data_base(Tasks),
							max_workers(Tasks,MaxW),
							Workers#::0..MaxW,
							%cumulative(OrderedTasks,)
							writeln(Workers).

find_number_workers(_,[],_).
find_number_workers(T,[X|ROrderedTasks],Workers) :- tarefa(T,_,Duration,WorkersN),
												Workers >= WorkersN.

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