c:-[pl_to_nl].

% progs has sample code w/.pl suffix, arg to go.sh has no .pl, so add
% these to get path to source code
addprogpl(F,PL) :- concat_atom(['progs/',F,'.pl'],PL).


% entry point:

pl(F):-
 addprogpl(F,PL),     % PL <- proper source file path
 pl_to_nl(PL),        % make .nl source ("natural language assembly code")
 % jpl_call('iProlog.Main',run,[PL],_R),
 writeln(done).

pl_to_nl(PL):-
  atom_concat(PL,'.nl',NL),   % <PL path> - add .nl for output
  pl_to_nl(PL,NL).               % translate .pl to .nl


% pl_to_nl - translate contents of PL.pl to equational English-like
% "sentence" form, writing to file named PL.pl.nl (=NL)

pl_to_nl(PL,NL):-
  see(PL),            % Open PL for reading, make it current input
  tell(NL),           % Open NL for writing, make it current output
  repeat,             % for infinite choice points
   clause_to_sentence(EOF),  %   send translation of clause to sentence to NL
   EOF==yes,          %   if EOF hit instead of clause
   !,                 %     cut
  seen,               % Close PL. New input stream becomes user_input
  told.               % Close NL

% ------------------------------------------
% Pretty good explanation here:
%

read_with_names(T,T0):-
  write(user_error, '%---- read_with_names: '),nl(user_error),

  read_term(T0,[variable_names(Es)]), % Read term from <PL>.pl, unify it with T0.
                                      % "Unify Es with list of Name = Var,
                                      % where Name is an atom describing
                                      % the variable name and Var is a variable
                                      % that shares with the corresponding
                                      % variable in T0."???
                                      % The variables appear in read-order.

% https://stackoverflow.com/questions/7947910/converting-terms-to-atoms-preserving-variable-names-in-yap-prolog/7948525#7948525
%
%    ?- read_term(T,[variable_names(Eqs)]).
%    |: X+3*Y+X.
%  Eqs = ['X'=_A,'Y'=_B], % mapping of var-as-atom to gen'ed vars in T:
%  T = _A+3*_B+_A

  write(user_error,'   T0 = '),write(user_error,T0),nl(user_error),
  write(user_error,'   Es = '),write(user_error,Es),nl(user_error),

  copy_term(T0,T),                    % "Create a version of T0 with renamed
                                      % (fresh) variables and unify it to T.
                                      % Attributed variables have their
                                      % attributes copied"

  write(user_error,'After copy_term(T0,T) T = '),
  write(user_error,T),nl(user_error),

  maplist(call,Es).                   % Try to unify all Es?
                                      % ????????????????????
                                      % ????????????????????


write_sym( if  ) :- !,  nl, write( if  ), nl, write('  ').
write_sym( and ) :- !,      write( and ), nl, write('  ').
write_sym( (.) ) :- !,      write( (.) ), nl.
write_sym( W   ) :-         write( W   ),     write(' ').

% clause_to_sentence - translate a clause to a "sentence"
%
clause_to_sentence(EOF):-
   read_with_names(T,T0),             % get some T0 from input,
                                      % T is copy with renamed variables,
                                      % T0 unified to T
   (
     T==end_of_file->EOF=yes          % like a possible result from read/1
   ;                                  % ;/2 -- like ||, and with -> above
                                      % "transparent to cuts"
     EOF=no,
     clause_to_nat(T,Ns),             % Ns <- "naturalized" T
     T=T0,                            % unify T0 to T
     clean_up_nats(Ns,Xs),            %
     maplist(write_sym,Xs),nl
  ).

% Not sure what the apparent two final implicit params in DCG rules are
% about.
%
clean_up_nats(Ns,Xs):- clean_up_nats(Ns,Xs,0,_),
                       write(user_error,'---- clean_up_nats:'), nl(user_error),
                       write(user_error,'   Ns='),
                       write(user_error,    Ns),                nl(user_error),
                       write(user_error,'   Xs='),
                       write(user_error,    Xs),                nl(user_error).

% Use of "-->" Definite Clause Grammar (DCG) operator
% https://stackoverflow.com/questions/32579266/what-does-the-operator-in-prolog-do

clean_up_nats( [],     []     ) --> [].
clean_up_nats( [N|Ns], [X|Xs] ) --> clean_up_nat(N,X), clean_up_nats(Ns,Xs).

% var(X) if X free var
% succ(K1,K2) if K2=K1+1 and K1>=0
% atom_concat('_',K1,X) if X <- K1 with _ prepended

clean_up_nat( N,       X, K1, K2 ) :-  var(N),
                                       !, succ(K1,K2), atom_concat('_',K1,X), N=X.
clean_up_nat( ('[|]'), X, K,  K  ) :-  !, X=list.
clean_up_nat( ([]),    X, K,  K  ) :-  !, X=nil.
clean_up_nat( X,       X, K,  K  ) .


% clause_to_nat -
%
clause_to_nat(C,Ns):- clause_to_eqs(C,_,Es)
                   ,  eqss_to_nat(Es,Ns)
                   .

clause_to_eqs(C,Xs,Es):- (C=(H:-Bs)->true;C=H,Bs=true)
                      ,  clause_to_list(H,Bs,Ts)
                      ,  maplist(term_to_eqs,Xs,Ts,Es)
                      .

clause_to_list(H,Bs,Cs) :-  body_to_list((H,Bs),Cs)
                        .


body_to_list( B,      R      ) :- var(B), !, R=[B].
body_to_list( (B,Cs), [B|Bs] ) :-         !, body_to_list(Cs,Bs).
body_to_list( true,   []     ) :-         !.
body_to_list( C,      [C]    ) .


eqss_to_nat(Xs,Ns) :- eqss_to_nat(Xs,Ns,[]).

eqss_to_nat( [H]    ) --> !, pred_to_nat(H), [(.)].
eqss_to_nat( [H|Bs] ) -->    pred_to_nat(H), [(if)], body_to_nat(Bs), [(.)].


body_to_nat( []     ) --> !, [].
body_to_nat( [B]    ) --> !, pred_to_nat(B).
body_to_nat( [B|Bs] ) -->    pred_to_nat(B), [and],  body_to_nat(Bs).


pred_to_nat([_=P|Ts])-->
  {P=..Xs,trim_call(Xs,Ys)},
  eq_to_words(Ys),
  eqs_to_nat(Ts).


trim_call( [call|Xs], R  ) :- !, R=Xs.
trim_call( Xs,        Xs ) .

eqs_to_nat( []       ) --> !, [].
eqs_to_nat( [X=T|Es] ) --> [and], {T=..Xs}, [X], holds_lists_eqs(Xs), eqs_to_nat(Es).

holds_lists_eqs( [lists|Xs] ) --> !, [lists], eq_to_words(Xs).
holds_lists_eqs( Xs         ) -->    [holds], eq_to_words(Xs).

eq_to_words( []     ) --> [].
eq_to_words( [X|Xs] ) --> [X], eq_to_words(Xs).


% terms to equations

term_to_eqs( X, T, [X=T] ) :-   var(T),           !.
term_to_eqs( X, T, [X=T] ) :-   atomic(T),        !.
term_to_eqs( X, T, Es    ) :-   compound(T),      term_to_eqs(X,T,Es,[]).

term_to_eqs( X, T  ) --> {var(T)},          !, {X=T}.
term_to_eqs( X, T  ) --> {atomic(T)},       !, {X=T}.
term_to_eqs( X, Xs ) --> {is_list(Xs)},     !, {T=..[lists|Xs]}, term_to_eqs(X,T).
term_to_eqs( X, T  ) --> {compound(T),
		          functor(T,F,N),
			  functor(TT,F,N)},    [X=TT], term_to_arg_eqs(0,N,TT,T).

term_to_arg_eqs( N, N, _, _ ) --> !, [].
term_to_arg_eqs( I, N, X, T ) -->
  {I1 is I+1},
  {arg(I1,X,V)},
  {arg(I1,T,A)},
  term_to_eqs(V,A),
  term_to_arg_eqs(I1,N,X,T).


go:-
 pl_to_nl('progs/perms.pl'),
 shell('cat progs/perms.pl.nl').
go1:-
 pl_to_nl('progs/queens.pl'),
 shell('cat progs/queens.pl.nl').
go2:-
 pl_to_nl('progs/sud4x.pl'),
 shell('cat progs/sud4x.pl.nl').
