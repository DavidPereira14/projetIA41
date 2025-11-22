# projetIA41


🎯 1. Prédicats liés au plateau (structure du jeu)

1️⃣ plateau_initial(Plateau)

Rôle : fournir l’état initial du jeu tel qu’indiqué dans le sujet.
Ce prédicat renvoie la configuration de départ (les piles, leur contenu, leur disposition).
Il est utilisé une seule fois, au début du jeu.

⸻

2️⃣ pile_a(Plateau, Index, Pile)

Rôle : récupérer la pile située à une case donnée.
Exemple : pile d’indice 4 → pile du milieu.

Utile pour :
	•	lire le contenu d’une case
	•	vérifier le sommet d’une pile
	•	manipuler une pile lors d’un déplacement

⸻

3️⃣ remplacer_pile(Plateau, Index, NouvellePile, NouveauPlateau)

Rôle : mettre une nouvelle pile à la place de l’ancienne dans le plateau.
Nécessaire après un déplacement (insertion du reste de la pile, de la nouvelle pile, etc.).

⸻

🎯 2. Prédicats liés aux déplacements autorisés

4️⃣ voisin(Index, VoisinIndex)

Rôle : indiquer quelles cases sont joignables depuis une autre case sans diagonale.
Utilisé pour construire les chemins de déplacement.

⸻

5️⃣ chemin_de_longueur(Depart, Pas, Chemin)

Rôle : générer un chemin valide de longueur Pas cases.
Le chemin doit :

	•	avancer d’une case à la fois
	•	rester orthogonal
	•	ne jamais repasser deux fois par la même case

C’est la règle fondamentale de Pogo :

déplacer n pièces → faire un chemin de n cases.

⸻

6️⃣ coup_legal(Plateau, Joueur, Coup)

Rôle : vérifier si un coup proposé respecte les règles.
Ce prédicat doit vérifier :

	•	que la pile de départ appartient au joueur (même couleur sur le sommet)
	•	qu’elle contient assez de pièces
	•	que le nombre de pièces déplacées est entre 1 et 3
	•	que le chemin est valide pour ce nombre
	•	qu’on reste dans le plateau

C’est le prédicat qui dit :

“Oui, ce coup est autorisé”
ou
“Non, ce coup est interdit”.

⸻

🎯 3. Prédicats liés à l’application d’un coup

7️⃣ prendre_pieces(Pile, Nombre, PiecesPrises, Reste)

Rôle : retirer les Nombre pièces du haut d’une pile.
Donne :

	•	PiecesPrises = les pièces emportées
	•	Reste = la pile restante

⸻

8️⃣ deposer_pieces(PileDestination, PiecesPrises, NouvellePile)

Rôle : déposer les pièces transportées sur la pile d’arrivée.
L’ordre est important :
la première pièce retirée doit être la première posée → elle devient le nouveau sommet.

⸻

9️⃣ appliquer_coup(Plateau, Coup, NouveauPlateau)

Rôle : transformer le plateau actuel en un nouveau plateau après le coup.
Ce prédicat combine :

	•	prendre_pieces
	•	deposer_pieces
	•	remplacer_pile pour mettre à jour les deux cases concernées

C’est lui qui fait “vivre” le jeu.

⸻

🎯 4. Prédicats liés à la victoire

🔟 victoire(Plateau, Joueur)

Rôle : vérifier si un joueur a gagné.
Le joueur gagne si toutes les piles non vides ont sa couleur au sommet.

C’est la condition de fin de partie.

⸻

🎯 5. Boucle de jeu et alternance des tours

1️⃣1️⃣ jouer(Plateau, Joueur)

Rôle : gérer le déroulement complet d’une partie.
Ce prédicat :

	•	affiche la situation actuelle
	•	demande ou calcule un coup
	•	vérifie la légalité du coup
	•	applique le coup
	•	passe le tour au joueur suivant
	•	vérifie la victoire

C’est le cœur du programme → la “boucle principale”.

⸻

1️⃣2️⃣ changer_joueur(Joueur, JoueurSuivant)

Rôle : alterner entre blanc et noir.

Ex. :
	•	blanc → noir
	•	noir → blanc

Simple mais indispensable.

⸻

🎯 6. Interface textuelle

1️⃣3️⃣ afficher_plateau(Plateau)

Rôle : afficher le plateau lisiblement pour l’utilisateur.
Par exemple sous forme de grille 3×3.

⸻

1️⃣4️⃣ demander_coup(Joueur, Coup)

Rôle : demander au joueur humain d’entrer son coup.
Ce prédicat lit :

	•	case de départ
	•	nombre de pièces à déplacer
	•	trajet (ou direction par direction, selon ton choix)

⸻

1️⃣5️⃣ afficher_coup(Coup)

Rôle : écrire sur l’écran ce qu’un joueur ou une IA a joué.
Ex. :
“Le joueur blanc déplace 2 pièces de la case 3 jusqu’à la case 5.”

⸻

🎯 7. Intelligence artificielle (si demandée)

1️⃣6️⃣ evaluer(Plateau, Joueur, Score)

Rôle : donner une valeur numérique au plateau.
Utilisé par l’IA pour comparer des positions.

Exemples de critères :
	•	nombre de piles où le joueur est au sommet
	•	profondeur stratégique
	•	menaces adverses

⸻

1️⃣7️⃣ minimax(Plateau, Joueur, Profondeur, MeilleurCoup, Score)

Rôle : choisir le meilleur coup pour l’IA.
Algo classique :

	•	le joueur maximise
	•	l’adversaire minimise

⸻

1️⃣8️⃣ meilleur_coup(Plateau, Joueur, Coup)

Rôle : obtenir le coup final choisi par l’IA.
C’est une version simplifiée de minimax que tu appelles depuis jouer.