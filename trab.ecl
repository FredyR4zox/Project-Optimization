:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).
:- lib(edge_finder).

:- compile(base_dados).

/* TODO:
project_earliest_start - Calcular o numero de trabalahdores para o numero minimo de dias caclculado : Quando se calcula o numero de trabalhadores apenas analisar os dias em que as tarefas comecam ou acabam, e nao todos 
os dias.
project_minimizae - pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores. Usar cumulative(OrderedTasks,Durations,WorkersLimit,1000000).
*/

read_data_base(Tasks, Durations, WorkersLimit) :- findall(T,tarefa(T,_,_,_),Tasks),
						   						findall(Dur,tarefa(_,_,Dur,_), Durations),
						   						findall(W, tarefa(_,_,_,W), WorkersLimit).

/* Determinar o numero minimo de dias (algoritmo do caminho critico) e depois atribuir o numero de trabalhadores necessarios para esses mesmos dias */
project_earliest_start :- read_data_base(Tasks,Durations,WorkersLimit),
						length(Tasks,NTasks), length(OrderedTasks, NTasks),
						max_days(Tasks, MaxW),
						OrderedTasks#::0..MaxW, Finish#::0..MaxW,
						prec_constrs(Tasks,OrderedTasks,Finish),
						minimize(labeling([Finish]),Finish),
						write(OrderedTasks),
						find_number_workers(Tasks,Durations,WorkersLimit,Finish,Workers).

/* Pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores */
%project_minimize :- read_data_base(Tarefas).

find_number_workers(Tasks,Durations,WorkersLimit,Finish,Workers) :- !.


% precedence restrictions
prec_constrs([],_,_).
prec_constrs([T|RTarefas],Datas,Finish) :-
	tarefa(T,LTSegs,Di,_),
	%element(I,Datas,DataI),
	%seleccionar_vars(LTSegs,Datas,Finish,DataI),
	selec_elemento(1,T,Datas,DataI),
    prec_constrs_(LTSegs,Datas,DataI,Di),
    DataI+Di #=< Finish,
    prec_constrs(RTarefas,Datas,Finish).

prec_constrs_([],_,_,_).
prec_constrs_([J|RTSegs],Datas,DataI,Di) :-
	%element(J,Datas,DataJ), 
    %seleccionar_vars(J,Datas,DataJ),
    selec_elemento(1,J,Datas,DataJ),
    DataI+Di #=< DataJ,
    prec_constrs_(RTSegs,Datas,DataI,Di).

seleccionar_vars([],_,_,[]).
seleccionar_vars([I|RDisc],Disciplinas,X,[Xi|XRDisc]) :-
    selec_elemento(1,T,Disciplinas,I), selec_elemento(1,T,X,Xi),
    seleccionar_vars(RDisc,Disciplinas,X,XRDisc).

%Selects element in the position T on a list. (CurrentPosition,T,List,ElementAtTPosition)
selec_elemento(T,T,[I|_],I) :- !.
selec_elemento(T0,T,[_|R],I) :-  T0n is T0+1, selec_elemento(T0n,T,R,I).

% soma duracao das tarefas
max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.