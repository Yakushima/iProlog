something(b).
bad(a).
% not(something(c)).
not(something(a)).

goal(P) :-
  bad(P),
  not(something(P)).

% Problem:
%	P <- a
%       not _0
%	_0 to hold something(P)
%		fails with P=a
