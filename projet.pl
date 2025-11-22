
‰ Configuration du plateu initial lors du démarrage du jeu  
‰|[n,n] [n,n] [n,n] C0 C1 C2
%| []     []   []   C3 C4 C5
%|[b,b] [b,b] [b,b] C6 C7 C8

plateau_initial([
    [n,n],[n,n],[n,n],
    [],[],[],
    [b,b],[b,b],[b,b]
]).

% plateau_a(Plateau, Index, Pile)

plateau_a(P, 0, P).
plateau_a([T|R], I, P) :- T /= P, I1 is I - 1, plateau_a(R, I1, P).
