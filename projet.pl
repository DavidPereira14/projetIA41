


% Configuration du plateu initial lors du démarrage du jeu  
%|[n,n] [n,n] [n,n] C0 C1 C2
%| []     []   []   C3 C4 C5
%|[b,b] [b,b] [b,b] C6 C7 C8

plateau_initial([
    [n,n],[n,n],[n,n],
    [],[],[],
    [b,b],[b,b],[b,b]
]).

%-------------------------------------------------------------%
%     pile_a(Plateau, Index, Pile)
%-------------------------------------------------------------%

pile_a([Pile|_], 0, Pile).
pile_a([_|R], I, P) :- I > 0, I1 is I - 1, pile_a(R, I1, P).

%-------------------------------------------------------------%
%  remplacer_pile(Plateau, Index, NouvellePile, NouveauPlato)
%-------------------------------------------------------------%

remplacer_pile([_|R], 0, NP, [NP|R]).
remplacer_pile([T|R1], Index, NPile, [T|R2]) :- Index > 0, I1 is Index - 1, remplacer_pile(R1, I1, NPile, R2).


%-------------------------------------------------------------%
%   voisin(Index, VoisinIndex)
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
                                            chemin_de_longueur_sans_repet(Suivant, P1, [Depart], R).

chemin_de_longueur_sans_repet(_, 0, _, []).
chemin_de_longueur_sans_repet(Actuel, Pas, Visitees, [Actuel|R]) :-
    Pas > 0,
    \+ member(Actuel, Visitees),
    voisin(Actuel, Next),
    P1 is Pas - 1,
    chemin_de_longueur_sans_repet(Next, P1, [Actuel|Visitees], R).

%-------------------------------------------------------------------------%
%   coup_legal(Plateau, Joueur, Coup)
%   Coup est une structure du type Coup = coup(Depart, NbPieces, Chemin)
%-------------------------------------------------------------------------%

coup_legal(Plateau, Joueur, coup(Depart, NbPieces, Chemin)) :-
    % 1. La pile de départ appartient au joueur (sommet)
    pile_a(Plateau, Depart, Pile),
    Pile = [Sommet | _],
    Sommet = Joueur,

    % 2. La pile contient assez de pièces
    pieces_deplacables(Pile, Joueur, Max),
    NbPieces =< Max,
    NbPieces >= 1,  % 3. Nombres de pieces entre 1 et 3
    NbPieces =< 3,

    % 4. Chemin valide pour ce nombre de pièces
    length(Chemin, NbPieces),
    chemin_valide(Depart, Chemin),

    % 5. On reste dans le plateau 
    tous_indices_valides(Chemin, Plateau).

%-------------------------------------------------------------%
%   pieces_deplacables(+Pile, +Joueur, -Max)
%   Calcule le nombre maximal de pièces du joueur queon peut déplacer depuis le sommet
%-------------------------------------------------------------%

pieces_deplacables([], _, 0).
pieces_deplacables([Sommet | _], Joueur, 0) :- Sommet \= Joueur, !.
pieces_deplacables([J|R], J, Max) :-
    pieces_deplacables(R, J, MaxR),
    Temp is MaxR + 1,
    Max is min(Temp, 3).


%-------------------------------------------------------------%
%   chemin_valide(+Depart, +Chemin)
%   Vérifie que le chemin est orthogonal et sans répétitions
%-------------------------------------------------------------%

chemin_valide(Depart, Chemin) :-
    chemin_de_longueur(Depart, _, [Depart|Chemin]).

%-------------------------------------------------------------%
%   tous_indices_valides(+Chemin, +Plateau)
%   Vérifie que toutes les cases du chemin sont valides
%-------------------------------------------------------------%

tous_indices_valides([], _).
tous_indices_valides([I|R], Plateau) :-
    length(Plateau, Taille),
    I >= 0, I < Taille,
    tous_indices_valides(R, Plateau).

%-------------------------------------------------------------%
%   prendre_pieces(Pile, Nombre, PiecesPrises, Reste)
%-------------------------------------------------------------%

prendre_pieces(Pile, 0, [], Pile).
prendre_pieces([T|R], N, [T|R2], Reste) :- 
    N > 0,
    N1 is N-1,
    prendre_pieces(R, N1, R2, Reste).

%-------------------------------------------------------------%
%   deposer_pieces(PileDestination, PiecesPrises, NouvellePile)
%-------------------------------------------------------------%

deposer_pieces(PileDestination, PiecesPrises, NouvellePile) :-
    append(PiecesPrises, PileDestination, NouvellePile).


%------------------------------------------------------------------------------------------------------%
%   appliquer_coup(Plateau, Coup, NouveauPlateau) Coup est défini par coup(Depart, NbPieces, Chemin)
%------------------------------------------------------------------------------------------------------%

appliquer_coup(Plateau, coup(Depart, NbPieces, Chemin), NouveauPlateau) :-

    % 1. Récuperer la pile de départ.
    pile_a(Plateau, Depart, Pile),

    % 2. Prendre pièces de la pile
    prendre_pieces(Pile, NbPieces, PiecesPrises, Reste),

    % 3. Récuperer arrivée
    last(Chemin, Arrivee),

    % 4. Récuperer la pile Arrivee
    pile_a(Plateau, Arrivee, PileArrivee),

    % 5. Récuperer la pile de Depart sur la pile Arrivee
    deposer_pieces(PileArrivee, PiecesPrises, NouvellePileArrivee),

    % 6. Mettre a jour la nouvelle pile de depart sur plateau
    remplacer_pile(Plateau, Depart, Reste, PlateauIntermediaire),

    % 7. Mettre a jour la nouvelle pile arrivee sur Plateau
    remplacer_pile(PlateauIntermediaire, Arrivee, NouvellePileArrivee, NouveauPlateau).


%-------------------------------------------------------------%
%   victoire(Plateau, Joueur)
%-------------------------------------------------------------%

victoire(Plateau, Joueur) :-
    verif_victoire(Plateau, Joueur).

verif_victoire([], _).

verif_victoire([[]|R], Joueur) :-
    verif_victoire(R, Joueur).

verif_victoire([[Sommet|_]|R], Joueur) :-
    Sommet = Joueur,
    verif_victoire(R, Joueur).

%-------------------------------------------------------------%
%   jouer(Plateau, Joueur)
%-------------------------------------------------------------%

jouer(Plateau, Joueur) :-
    afficher_plateau(Plateau),
    victoire(Plateau, Joueur),
    format('--- PARTIE TERMINÉE ! Le joueur ~w a gagné ! ---~n', [Joueur]),
    !. % Couper pour ne pas continuer la boucle

jouer(Plateau, Joueur) :-
    format('~n--- Au tour du joueur ~w ---~n', [Joueur]),

    repeat,
        demander_coup(Joueur, Coup),
        (   coup_legal(Plateau, Joueur, Coup)
        -> ! % Coup légal, sortir de la boucle
        ;
            writeln('Coup illégal, Veuillez réessayer.'),
            fail % Recommencer la boucle 
        ),
    
    afficher_coup(Coup),
    appliquer_coup(Plateau, Coup, NouveauPlateau),
    changer_joueur(Joueur, NouveauJoueur),
    jouer(NouveauPlateau, NouveauJoueur).


demander_coup(Joueur, Coup) :-
    % 1. Afficher instruction et le format de saisie attendu
    format('~n--- Saisie du coup pour le joueur ~w ---~n', [Joueur]),
    writeln('Veuillez entrer votre coup sous la forme :'),
    writeln('coup(CaseDepart, NbPieces, Chemin)'),
    writeln(''),
    writeln('Où :'),
    writeln('  - CaseDepart : Indice de la pile à déplacer (0 à 8).'),
    writeln('  - NbPieces : Nombre de pièces à déplacer (1, 2 ou 3).'),
    writeln('  - Chemin : Liste des indices visités, y compris le départ et l\'arrivée.'),
    writeln('    (Ex: [0, 1, 4] pour un déplacement de 2 pas de 0 à 4)'),
    
    % 2. Inviter utilisateur à saisir
    format('Votre coup (terminer par un point .): '),
    read(Coup).

afficher_coup(coup(Depart, NbPieces, Chemin)) :-
    last(Chemin, Arrivee),
    format('  -> Déplace ~w pièce(s) de la case ~w vers la case ~w.~n', 
           [NbPieces, Depart, Arrivee]).
%-------------------------------------------------------------%
%   changer_joueur(Joueur, JoueurSuivant)
%-------------------------------------------------------------%

changer_joueur(n, b).
changer_joueur(b, n).


%-------------------------------------------------------------%
%   afficher_plateau(Plateau)
%-------------------------------------------------------------%

% afficher_plateau(+Plateau)
% Affiche le plateau de 9 cases sous forme de grille 3x3.
afficher_plateau(Plateau) :-
    writeln('\n*** PLATEAU DE JEU POGO (Indices C0 à C8) ***'),
    
    % On coupe la liste des 9 piles en 3 lignes de 3 piles
    Plateau = [P0, P1, P2, P3, P4, P5, P6, P7, P8],
    
    afficher_ligne_simple(0, P0, P1, P2),
    afficher_ligne_simple(3, P3, P4, P5),
    afficher_ligne_simple(6, P6, P7, P8),
    
    writeln('*******************************************\n').

% afficher_ligne_simple(+IndiceDebut, +Pile1, +Pile2, +Pile3)
% Affiche une ligne complète (indices, sommets et hauteurs)
afficher_ligne_simple(IndiceDebut, P1, P2, P3) :-
    I1 is IndiceDebut,
    I2 is IndiceDebut + 1,
    I3 is IndiceDebut + 2,

    % 1. Afficher les indices de case (C0, C1, C2...)
    format('  | C~w   | C~w   | C~w   |~n', [I1, I2, I3]),
    writeln('  +-------+-------+-------+'),
    
    % 2. Afficher le sommet et la hauteur de la pile 1
    afficher_info_case(P1, S1, H1),
    % 3. Afficher le sommet et la hauteur de la pile 2
    afficher_info_case(P2, S2, H2),
    % 4. Afficher le sommet et la hauteur de la pile 3
    afficher_info_case(P3, S3, H3),
    
    % 5. Afficher la ligne de contenu (Sommet(Hauteur))
    format('  | ~w(~w) | ~w(~w) | ~w(~w) |~n', [S1, H1, S2, H2, S3, H3]),
    writeln('  +-------+-------+-------+').

% afficher_info_case(+Pile, -Sommet, -Hauteur)
% Détermine les infos de la case (sommet ou ' ' si vide)
afficher_info_case([], ' ', 0). % Case vide : sommet = ' ' (espace), hauteur = 0
afficher_info_case([Sommet|_], Sommet, Hauteur) :-
    length([Sommet|_], Hauteur).

    
%-------------------------------------------%
%   Lancer le jeu
%-------------------------------------------%

choisir_joueur_initial(Joueur):-
    random(0, 2, R),
    (R = 0
    -> Joueur = b,
        writeln('Pile ou face : Le joueur BLANC(b) commence')
    ;
        Joueur = n,
        writeln('Pile ou face : Le joueur NOIR(n) commence')
    ).

main :-
    writeln('=============================================='),
    writeln('         Lancement du jeu POGO en Prolog      '),
    writeln('=============================================='),
    
    % 1. Initialiser le plateau
    plateau_initial(PlateauInitial),
    
    % 2. Tirer au sort le joueur qui commence
    choisir_joueur_initial(JoueurInitial),
    
    % 3. Lancer la boucle de jeu
    jouer(PlateauInitial, JoueurInitial).