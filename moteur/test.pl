% =======================================================
% REGLES DU JEU POGO + IA (VERSION FINALE)
% =======================================================

% --- 1. CONFIGURATION ---

plateau_initial([
    [n,n],[n,n],[n,n],
    [],[],[],
    [b,b],[b,b],[b,b]
]).

pile_a([Pile|_], 0, Pile).
pile_a([_|R], I, P) :- I > 0, I1 is I - 1, pile_a(R, I1, P).

remplacer_pile([_|R], 0, NP, [NP|R]).
remplacer_pile([T|R1], Index, NPile, [T|R2]) :- Index > 0, I1 is Index - 1, remplacer_pile(R1, I1, NPile, R2).

% --- 2. TOPOLOGIE ---
voisin(0, 1). voisin(0, 3).
voisin(1, 0). voisin(1, 2). voisin(1, 4).
voisin(2, 1). voisin(2, 5).
voisin(3, 0). voisin(3, 4). voisin(3, 6).
voisin(4, 1). voisin(4, 3). voisin(4, 5). voisin(4, 7).
voisin(5, 2). voisin(5, 4). voisin(5, 8).
voisin(6, 3). voisin(6, 7).
voisin(7, 4). voisin(7, 6). voisin(7, 8).
voisin(8, 5). voisin(8, 7).

% --- 3. LOGIQUE DEPLACEMENT ---

chemin_de_longueur(Depart, 0, [Depart]).
chemin_de_longueur(Depart, Pas, [Depart|R]) :-
    Pas > 0,
    voisin(Depart, Suivant),
    P1 is Pas - 1,
    chemin_interne(Suivant, P1, [Depart], R).

chemin_interne(Final, 0, Visitees, [Final]) :- \+ member(Final, Visitees).
chemin_interne(Actuel, Pas, Visitees, [Actuel|R]) :-
    Pas > 0, \+ member(Actuel, Visitees), voisin(Actuel, Suivant), P1 is Pas - 1,
    chemin_interne(Suivant, P1, [Actuel|Visitees], R).

pieces_deplacables([], _, 0).
pieces_deplacables(Pile, Joueur, 0) :- last(Pile, Sommet), Sommet \= Joueur, !.
pieces_deplacables(Pile, Joueur, Max) :-
    last(Pile, Sommet), Sommet = Joueur,
    length(Pile, Taille),
    Max is min(Taille, 3).

coup_legal(Plateau, Joueur, coup(Depart, NbPieces, Chemin)) :-
    pile_a(Plateau, Depart, Pile),
    pieces_deplacables(Pile, Joueur, Max),
    Max >= 1, NbPieces =< Max, NbPieces >= 1, NbPieces =< 3,
    length(Chemin, Len), Len =:= NbPieces + 1,
    true.

% --- 4. EXECUTION ---

prendre_fin(Pile, N, Reste, Prises) :- append(Reste, Prises, Pile), length(Prises, N).

jouer_coup(Plateau, coup(Dep, Nb, _), Arrivee, NouveauPlateau) :-
    pile_a(Plateau, Dep, PileDep),
    pile_a(Plateau, Arrivee, PileArr),
    prendre_fin(PileDep, Nb, ResteDep, Prises),
    append(PileArr, Prises, NouvPileArr),
    remplacer_pile(Plateau, Dep, ResteDep, PTemp),
    remplacer_pile(PTemp, Arrivee, NouvPileArr, NouveauPlateau).

% --- 5. PONT C++ (INTERFACE) ---

coup_possible(Plateau, Joueur, Depart, Arrivee, NbPieces) :-
    chemin_de_longueur(Depart, NbPieces, Chemin),
    last(Chemin, Arrivee),
    coup_legal(Plateau, Joueur, coup(Depart, NbPieces, Chemin)).

jouer_coup_wrapper(Plateau, Joueur, Dep, Arr, Nb, NouveauPlateau) :-
    coup_possible(Plateau, Joueur, Dep, Arr, Nb),
    jouer_coup(Plateau, coup(Dep, Nb, _), Arr, NouveauPlateau).

% --- 6. INTELLIGENCE ARTIFICIELLE (MINIMAX) ---

% Changer de joueur
changer_joueur(b, n).
changer_joueur(n, b).

% Générateur de tous les coups possibles pour un joueur
generer_coups(Plateau, Joueur, ListeCoups) :-
    findall(coup(Dep, Nb, Chemin), (
        between(0, 8, Dep),                 % Pour chaque case
        pile_a(Plateau, Dep, Pile),
        pieces_deplacables(Pile, Joueur, Max),
        Max >= 1,                           % Si on peut bouger
        between(1, Max, Nb),                % Pour chaque nombre de pièces (1..Max)
        chemin_de_longueur(Dep, Nb, Chemin),% Trouver un chemin
        % Vérif rapide : ne pas boucler
        true
    ), ListeCoups).

% Evaluation simple : Nombre de piles contrôlées
evaluer(Plateau, Joueur, Score) :-
    findall(1, (member(Pile, Plateau), last(Pile, Sommet), Sommet == Joueur), PilesJ),
    length(PilesJ, ScoreJ),
    changer_joueur(Joueur, Adv),
    findall(1, (member(Pile, Plateau), last(Pile, Sommet), Sommet == Adv), PilesA),
    length(PilesA, ScoreA),
    Score is ScoreJ - ScoreA.

% Algorithme Minimax
minimax(Plateau, Joueur, 0, _, Score) :-
    evaluer(Plateau, Joueur, Score), !.

minimax(Plateau, Joueur, Profondeur, MeilleurCoup, Score) :-
    generer_coups(Plateau, Joueur, Coups),
    ( Coups = [] ->
        % Pas de coup = Défaite (ou score très bas)
        Score = -1000, MeilleurCoup = none
    ;
        changer_joueur(Joueur, Adversaire),
        P1 is Profondeur - 1,
        meilleur_parmi(Coups, Plateau, Adversaire, P1, -10000, none, MeilleurCoup, Score)
    ).

% Boucle pour trouver le max
meilleur_parmi([], _, _, _, ScoreMax, CoupMax, CoupMax, ScoreMax).
meilleur_parmi([Coup|R], Plateau, Adversaire, Prof, ScoreCourant, CoupCourant, MeilleurCoup, ScoreFinal) :-
    Coup = coup(_, _, Chemin), last(Chemin, Arr),
    jouer_coup(Plateau, Coup, Arr, NouvPlateau),

    % Appel récursif (Minimax de l'adversaire)
    minimax(NouvPlateau, Adversaire, Prof, _, ScoreAdv),
    ScoreCoup is -ScoreAdv, % On inverse le score car c'est à nous

    (ScoreCoup > ScoreCourant ->
        meilleur_parmi(R, Plateau, Adversaire, Prof, ScoreCoup, Coup, MeilleurCoup, ScoreFinal)
    ;
        meilleur_parmi(R, Plateau, Adversaire, Prof, ScoreCourant, CoupCourant, MeilleurCoup, ScoreFinal)
    ).

% --- WRAPPER IA POUR C++ ---
get_best_move_wrapper(Plateau, Joueur, Dep, Arr, Nb) :-
    % Profondeur 2 pour une réponse rapide
    minimax(Plateau, Joueur, 2, MeilleurCoup, _),
    MeilleurCoup = coup(Dep, Nb, Chemin),
    last(Chemin, Arr).

% Un joueur gagne si, pour toutes les piles du plateau :
% Soit la pile est vide, Soit le sommet est à lui.
victoire(Plateau, Joueur) :-
    forall(member(Pile, Plateau),
           (Pile == [] ; (last(Pile, Sommet), Sommet == Joueur))).