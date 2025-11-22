


‰ Configuration du plateu initial lors du démarrage du jeu  
‰|[n,n] [n,n] [n,n] C0 C1 C2
%| []     []   []   C3 C4 C5
%|[b,b] [b,b] [b,b] C6 C7 C8

plateau_initial([
    [n,n],[n,n],[n,n],
    [],[],[],
    [b,b],[b,b],[b,b]
]).

%-------------------------------------------------------------%
%     plateau_a(Plateau, Index, Pile)
%-------------------------------------------------------------%

plateau_a([Pile|_], 0, Pile).
plateau_a([_|R], I, P) :- I > 0, I1 is I - 1, plateau_a(R, I1, P).

%-------------------------------------------------------------%
%  remplacer_pile(Plateau, Index, NouvellePile, NouveauPlato)
%-------------------------------------------------------------%

remplacer_pile([_|R], 0, NP, [NP|R]).
remplacer_pile([T|R1], Index, NPile, [T|R2]) :- Index > 0, I1 is Index - 1, remplacer_pile(R1, I1, NPile, R2).


%-------------------------------------------------------------%
%   voisin(Index, VosinIndex)
%-------------------------------------------------------------%

voisin(0, 1). voisin(0, 3).
voisin(1, 0). voisin(1, 2). voisin(1, 4).
voisin(2, 1). voisin(2, 5).
voisin(3, 0). voisin(3, 4). voisin(3, 6).
voisin(4, 1). voisin(4, 3). voisin(4, 5). voisin(4, 7).
voisin(5, 2). voisin(5, 4). voisin(5, 8).
voisin(6, 3). voisin(6, 7).
voisin(7, 4). voisin(7, 6). voisin(7, 8).
voisin(8, 5). voisin(8, 7).

%-------------------------------------------------------------%
%   chemin_de_longueur(Depart, Pas, Chemin)
%-------------------------------------------------------------%

chemin_de_longueur(Depart, 0, [Depart]).
chemin_de_longueur(Depart, Pas, [Depart|R]) :- Pas > 0, 
                                            voisin(Depart, Suivant), 
                                            P1 is Pas - 1, 
                                            chemin_de_longueur_sans_repet(Next, P1 [Depart], R).

chemin_de_longueur_sans_repet(_, 0, _, []).
chemin_de_longueur_sans_repet(Actuel, Pas, Visitees, [Actuel|R]) :-
    Pas > 0,
    \+ member(Actuel, Visitees),
    voisin(Actuel, Next),
    P1 is Pas - 1,
    chemin_de_longueur_sans_repet(Next, P1, [Actuel|Visitees], R).

