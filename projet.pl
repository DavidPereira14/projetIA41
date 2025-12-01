% ==================================================================
%                       CONFIGURATION DU JEU
% ==================================================================

% Configuration du plateau initial (Indices 0 à 8)
% Piles de 2 pions sur C0, C1, C2 (n) et C6, C7, C8 (b).
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
%   voisin(Index, VoisinIndex) (Connexions orthogonales)
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

% ==================================================================
%                       LOGIQUE DES DÉPLACEMENTS
% ==================================================================

% --- Génération/Vérification du Chemin (CORRIGÉ) ---

% chemin_de_longueur(+Depart, +Pas, -Chemin)
chemin_de_longueur(Depart, Pas, Chemin) :-
    length(Chemin, Len),
    Len is Pas + 1,
    Chemin = [Depart | R],
    chemin_sans_repet(Depart, Pas, [Depart], R).

chemin_sans_repet(_, 0, _, []).
chemin_sans_repet(Actuel, Pas, Visitees, [Next|R]) :-
    Pas > 0,
    voisin(Actuel, Next),
    \+ member(Next, Visitees),
    P1 is Pas - 1,
    chemin_sans_repet(Next, P1, [Next|Visitees], R).


% --- Vérification de la Légalité ---

% coups_legaux(Plateau, Joueur, Coups)
coups_legaux(Plateau, Joueur, Coups) :-
    findall(Coup,
            ( (Coup = coup(Depart, 1, [Depart, Arrivee]),
               between(0, 8, Depart),
               pile_a(Plateau, Depart, Pile),
               Pile = [Sommet | _],
               Sommet = Joueur,
               pieces_deplacables(Pile, Joueur, Max),
               1 =< Max,
               voisin(Depart, Arrivee),
               tous_indices_valides([Depart, Arrivee], Plateau))
            ;
              (Coup = coup(Depart, 2, [Depart, Inter, Arrivee]),
               between(0, 8, Depart),
               pile_a(Plateau, Depart, Pile),
               Pile = [Sommet | _],
               Sommet = Joueur,
               pieces_deplacables(Pile, Joueur, Max),
               2 =< Max,
               voisin(Depart, Inter),
               voisin(Inter, Arrivee),
               Arrivee \= Depart,
               Inter \= Arrivee,
               \+ member(Arrivee, [Depart]),
               tous_indices_valides([Depart, Inter, Arrivee], Plateau))
            ),
            Coups).


% coup_legal(Plateau, Joueur, Coup)
coup_legal(Plateau, Joueur, coup(Depart, NbPieces, Chemin)) :-
    % 1. Sommet du joueur
    pile_a(Plateau, Depart, Pile),
    Pile = [Sommet | _],
    Sommet = Joueur,

    % 2. Pièces max 3 et disponibles
    pieces_deplacables(Pile, Joueur, Max),
    NbPieces >= 1,
    NbPieces =< 3,
    NbPieces =< Max,

    % 3. Chemin valide (Pas = NbPieces)
    Pas is NbPieces,
    chemin_de_longueur(Depart, Pas, Chemin),

    % 4. Indices valides
    tous_indices_valides(Chemin, Plateau).

% pieces_deplacables(+Pile, +Joueur, -Max)
pieces_deplacables([], _, 0).
pieces_deplacables([Sommet | _], Joueur, 0) :- Sommet \= Joueur, !.
pieces_deplacables([J|R], J, Max) :-
    pieces_deplacables(R, J, MaxR),
    Temp is MaxR + 1,
    Max is min(Temp, 3).

% tous_indices_valides(+Chemin, +Plateau)
tous_indices_valides([], _).
tous_indices_valides([I|R], Plateau) :-
    length(Plateau, Taille),
    I >= 0, I < Taille,
    tous_indices_valides(R, Plateau).

% ==================================================================
%                       APPLICATION DU COUP
% ==================================================================

% prendre_pieces(Pile, Nombre, PiecesPrises, Reste)
prendre_pieces(Pile, 0, [], Pile).
prendre_pieces([T|R], N, [T|R2], Reste) :- 
    N > 0,
    N1 is N-1,
    prendre_pieces(R, N1, R2, Reste).

% deposer_pieces(PileDestination, PiecesPrises, NouvellePile)
deposer_pieces(PileDestination, PiecesPrises, NouvellePile) :-
    append(PiecesPrises, PileDestination, NouvellePile).

% appliquer_coup(Plateau, Coup, NouveauPlateau)
appliquer_coup(Plateau, coup(Depart, NbPieces, Chemin), NouveauPlateau) :-
    
    pile_a(Plateau, Depart, PileDepart),
    last(Chemin, Arrivee),
    pile_a(Plateau, Arrivee, PileArrivee), 

    prendre_pieces(PileDepart, NbPieces, PiecesPrises, ResteDepart),
    deposer_pieces(PileArrivee, PiecesPrises, NouvellePileArrivee),
    
    remplacer_pile(Plateau, Depart, ResteDepart, PlateauIntermediaire),
    remplacer_pile(PlateauIntermediaire, Arrivee, NouvellePileArrivee, NouveauPlateau).

% ==================================================================
%                       INTELLIGENCE ARTIFICIELLE
% ==================================================================

% evaluer(Plateau, Joueur, Score)
evaluer(Plateau, Joueur, Score) :-
    changer_joueur(Joueur, Adversaire),
    score_joueur(Plateau, Joueur, ScoreJoueur),
    score_joueur(Plateau, Adversaire, ScoreAdversaire),
    Score is ScoreJoueur - ScoreAdversaire.

% score_joueur(Plateau, Joueur, Score)
score_joueur(Plateau, Joueur, Score) :-
    findall(H, (member(Pile, Plateau), Pile = [Joueur|_], length(Pile, H)), Heights),
    sum_list(Heights, Score).

% minimax(Plateau, Joueur, Profondeur, MeilleurCoup, Score)
minimax(Plateau, Joueur, 0, _, Score) :-
    evaluer(Plateau, Joueur, Score), !.

minimax(Plateau, Joueur, Profondeur, MeilleurCoup, Score) :-
    coups_legaux(Plateau, Joueur, Coups),
    (   Coups = []
    ->  Score = -1000, MeilleurCoup = none  % Pas de coup possible, défaite
    ;   P1 is Profondeur - 1,
        changer_joueur(Joueur, Adversaire),
        evaluer_coups(Coups, Plateau, Adversaire, P1, MeilleurCoup, Score)
    ).

% evaluer_coups(Coups, Plateau, Joueur, Adversaire, Profondeur, MeilleurCoup, Score)
evaluer_coups([Coup], Plateau, Adversaire, Profondeur, Coup, Score) :-
    appliquer_coup(Plateau, Coup, NouveauPlateau),
    minimax(NouveauPlateau, Adversaire, Profondeur, _, ScoreOpp),
    Score is -ScoreOpp.

evaluer_coups([Coup|R], Plateau, Adversaire, Profondeur, MeilleurCoup, Score) :-
    appliquer_coup(Plateau, Coup, NouveauPlateau),
    minimax(NouveauPlateau, Adversaire, Profondeur, _, ScoreOpp),
    ScoreCoup is -ScoreOpp,
    evaluer_coups(R, Plateau, Adversaire, Profondeur, AutreCoup, ScoreAutre),
    (   ScoreCoup > ScoreAutre
    ->  MeilleurCoup = Coup, Score = ScoreCoup
    ;   MeilleurCoup = AutreCoup, Score = ScoreAutre
    ).

% meilleur_coup(Plateau, Joueur, Coup)
meilleur_coup(Plateau, Joueur, Coup) :-
    minimax(Plateau, Joueur, 2, Coup, _).  % Profondeur 1 pour éviter le dépassement de pile

% ==================================================================
%                           VICTOIRE
% ==================================================================

% victoire(Plateau, Joueur)
victoire(Plateau, Joueur) :-
    verif_victoire(Plateau, Joueur).

verif_victoire([], _). 
verif_victoire([[]|R], Joueur) :-
    verif_victoire(R, Joueur).
verif_victoire([[Joueur|_]|R], Joueur) :-
    verif_victoire(R, Joueur).

% ==================================================================
%                       BOUCLE DE JEU & I/O
% ==================================================================

% changer_joueur(Joueur, JoueurSuivant)
changer_joueur(n, b).
changer_joueur(b, n).

% changer_joueur_joueur(Joueur, JoueurSuivant)
changer_joueur_joueur(joueur(_, C1), joueur(Type2, C2)) :- changer_joueur(C1, C2), type_joueur(C2, Type2).

% --- Boucle Principale  ---

% Clause 1 : Le joueur PRÉCÉDENT a gagné (Arrêt du jeu)
jouer(Plateau, joueur(_, Couleur)) :-
    changer_joueur(Couleur, CouleurPrecedente),
    victoire(Plateau, CouleurPrecedente),
    afficher_plateau(Plateau),
    format('--- PARTIE TERMINEE ! Le joueur ~w a gagne ! ---~n', [CouleurPrecedente]),
    !.

% Clause 2 : Le tour se déroule normalement
jouer(Plateau, joueur(Type, Couleur)) :-
    afficher_plateau(Plateau),
    format('~n--- Au tour du joueur ~w ---~n', [Couleur]),

    % 1. Obtenir un coup légal
    repeat,
        demander_coup(Plateau, joueur(Type, Couleur), Coup),
        (   coup_legal(Plateau, Couleur, Coup)
        ->  !
        ;
            writeln('Coup illégal. Vérifiez la case de départ, le nombre de pièces, et le chemin. Veuillez réessayer.'),
            fail
        ),

    % 2. Appliquer le coup
    (   Type = ai
    ->  format('L\'IA joue : ', [])
    ;   true
    ),
    afficher_coup(Coup),
    appliquer_coup(Plateau, Coup, NouveauPlateau),

    % 3. Tour suivant
    changer_joueur_joueur(joueur(Type, Couleur), NouveauJoueur),
    jouer(NouveauPlateau, NouveauJoueur).

% --- Interface Utilisateur ---

demander_coup(_, joueur(human, Couleur), Coup) :-
    format('~n--- Saisie du coup pour le joueur ~w ---~n', [Couleur]),
    writeln('Veuillez entrer votre coup sous la forme :'),
    writeln('coup(CaseDepart, NbPieces, Chemin)'),
    writeln(''),
    writeln('Où :'),
    writeln('  - CaseDepart : Indice de la pile à déplacer (0 à 8).'),
    writeln('  - NbPieces : Nombre de pièces à déplacer (1, 2 ou 3).'),
    writeln('  - Chemin : Liste des indices visités, y compris le départ et l\'arrivée.'),
    writeln('    (Ex: [6, 7, 4] pour un déplacement de 2 pas de 6 à 4)'),

    format('Votre coup (terminer par un point .): '),
    read(Coup).

demander_coup(Plateau, joueur(ai, Couleur), Coup) :-
    format('~n--- L\'IA (~w) réfléchit... ---~n', [Couleur]),
    meilleur_coup(Plateau, Couleur, Coup),
    sleep(1).  % Attendre 1 seconde entre les coups IA

afficher_coup(coup(Depart, NbPieces, Chemin)) :-
    last(Chemin, Arrivee),
    format('  -> Le joueur déplace ~w pièce(s) de C~w vers C~w.~n', 
           [NbPieces, Depart, Arrivee]).

% --- Affichage du Plateau (Détaillé et Agrandi) ---

afficher_plateau(Plateau) :-
    writeln('\n-------------------------------------------------------------------------------'),
    writeln('                     PLATEAU DE JEU POGO (C0-C8) '),
    writeln('-------------------------------------------------------------------------------'),
    writeln(''),

    Plateau = [P0, P1, P2, P3, P4, P5, P6, P7, P8],

    afficher_ligne_ludique(0, P0, P1, P2),
    writeln(''),
    afficher_ligne_ludique(3, P3, P4, P5),
    writeln(''),
    afficher_ligne_ludique(6, P6, P7, P8),

    writeln('-------------------------------------------------------------------------------\n'),
    writeln('-------------------------------------------------------------------------------\n').


afficher_ligne_ludique(IndiceDebut, P1, P2, P3) :-
    I1 is IndiceDebut, I2 is IndiceDebut + 1, I3 is IndiceDebut + 2,

    % Ligne 1: Indices de case
    format('  |     C~w      |     C~w      |     C~w      |~n', [I1, I2, I3]),
    writeln('  +---------------+---------------+---------------+'),

    % Ligne 2: Affichage de la composition détaillée de la pile
    write('  |'), afficher_composition_pile_ligne(P1),
    write('|'), afficher_composition_pile_ligne(P2),
    write('|'), afficher_composition_pile_ligne(P3), writeln('|'),

    % Ligne 3: Première lettre du sommet
    top_letter(P1, L1),
    top_letter(P2, L2),
    top_letter(P3, L3),
    format('  |      ~w      |      ~w      |      ~w      |~n',
           [L1, L2, L3]),
    writeln('  +---------------+---------------+---------------+').


afficher_composition_pile_ligne(Pile) :-
    (   Pile = []
    ->  String = 'VIDE'
    ;   term_string(Pile, StringP),
        (   atom_length(StringP, LenP), LenP > 13
        ->  sub_atom(StringP, 0, 12, _, Short),
            atom_concat(Short, '..]', String)
        ;   String = StringP
        )
    ),

    atom_length(String, Len),
    Padding is (15 - Len) // 2,
    Extra is 15 - Len - Padding, % Ajuster la taille de la colonne à 15 (incluant les |)

    tab(Padding),
    write(String),
    tab(Extra).


% top_letter(Pile, Letter)
top_letter([], ' ').
top_letter([Top|_], Top).
    
% --- Lancement du Jeu ---

choisir_type_joueur(Couleur, Type) :-
    format('Joueur ~w : humain (h) ou IA (i) ? ', [Couleur]),
    read(Input),
    (   Input = h
    ->  Type = human,
        format('Joueur ~w : Humain~n', [Couleur])
    ;   Input = i
    ->  Type = ai,
        format('Joueur ~w : IA~n', [Couleur])
    ;   writeln('Entrée invalide. Réessayez.'),
        choisir_type_joueur(Couleur, Type)
    ).

choisir_joueur_initial(Couleur):-
    random(0, 2, R),
    (R = 0
    -> Couleur = b,
        writeln('Pile ou face : Le joueur BLANC (b) commence.')
    ;
        Couleur = n,
        writeln('Pile ou face : Le joueur NOIR (n) commence.')
    ).

main :-
    writeln('----------------------------------------------'),
    writeln('         Lancement du jeu POGO en Prolog      '),
    writeln('----------------------------------------------'),

    % Choisir les types de joueurs
    choisir_type_joueur(n, TypeN),
    choisir_type_joueur(b, TypeB),

    % Enregistrer les types
    assert(type_joueur(n, TypeN)),
    assert(type_joueur(b, TypeB)),

    plateau_initial(PlateauInitial),
    choisir_joueur_initial(CouleurInitial),

    type_joueur(CouleurInitial, TypeInitial),
    JoueurInitial = joueur(TypeInitial, CouleurInitial),

    jouer(PlateauInitial, JoueurInitial).