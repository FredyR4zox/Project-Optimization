:- lib(ic).
:- lib(ic_global).
:- lib(branch_and_bound).

:- compile(base_dados).

read_data_base(Tarefas) :- findall(T,tarefa(T,_,_,_),Tarefas).

/* Determinar o numero minimo de dias (algoritmo do caminho critico) e depois atribuir o numero de trabalhadores necessarios para esses mesmos dias */
project_earliest_start :- read_data_base(Tasks),
						length(Tasks,NTasks), length(OrderedTasks, NTasks),
						max_days(Tasks, MaxW),
						OrderedTasks::0..MaxW, Finish::0..MaxW,
						prec_constrs(Tasks,OrderedTasks,Finish),
						minimize(labeling([Finish],Finish)).

/* Pesquisa para as atividades nao criticas de modo a minimizar o numero de trabalhadores */
project_minimize :- read_data_base(Tarefas),
					verifica_prec(Tarefas, OrganizedSet).

% restricoes de precedencia
prec_constrs([],_,_).
prec_constrs([T|RTarefas],Datas,Concl) :-
	tarefa(T,Di,LTSegs,_),
	element(T,Datas,DataI),
    prec_constrs_(LTSegs,Datas,DataI,Di),
    DataI+Di #=< Concl,
    prec_constrs(RTarefas,Datas,Concl).

prec_constrs_([],_,_,_).
prec_constrs_([J|RTSegs],Datas,DataI,Di) :- 
     element(J,Datas,DataJ),
     DataI+Di #=< DataJ,
     prec_constrs_(RTSegs,Datas,DataI,Di).

% soma duracao das tarefas
max_days([],0).
max_days([T|LTasks],Sum) :- tarefa(T,_,D,_), max_days(LTasks,Sum2), Sum is Sum2+D.


/* Quando se calcula o numero de trabalhadores apenas analisar os dias em que as tarefas comecam ou acabam, e nao todos 
os dias */