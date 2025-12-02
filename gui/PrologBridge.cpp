#include "PrologBridge.h"
#include <SWI-cpp.h>
#include <iostream>

PrologBridge::PrologBridge()
{
    // Initialisation SWI-Prolog
    char *argv[] = { (char*)"swipl", (char*)"-q", nullptr };
    PL_initialise(2, argv);

    try {
        // Charger le fichier Prolog
        PlCall("consult('../../moteur/test.pl')");
    } catch(const PlException &ex) {
        std::cerr << "Erreur Prolog: " << (char*)ex << std::endl;
    }
}

PrologBridge::~PrologBridge()
{
    PL_halt(0);
}

bool PrologBridge::isValidMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player) {
    try {
        // 1. Convertir le plateau C++ en liste Prolog
        PlTerm plBoard;
        PlTail listTail(plBoard);

        for (const auto& cell : board) {
            PlTerm pileTerm;
            PlTail pileTail(pileTerm);

            // On inverse la pile car Prolog lit [Haut|Bas] ou [Bas|Haut] ?
            // Dans projet.pl, pile_a retourne la liste telle quelle.
            // Assumons que C++ stack[0] est le bas.
            for (const auto& p : cell.stack) {
                const char* atom = (p == WHITE) ? "b" : "n";
                pileTail.append(PlTerm(atom));
            }
            pileTail.close();
            listTail.append(pileTerm);
        }
        listTail.close();

        // 2. Préparer les autres arguments
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTerm plJoueur(playerChar);
        PlTerm plStart((long)start);
        PlTerm plEnd((long)end);
        PlTerm plNb((long)numPieces);

        // 3. Appeler coup_possible(Plateau, Joueur, Depart, Arrivee, NbPieces)
        PlTermv args(5);
        args[0] = plBoard;
        args[1] = plJoueur;
        args[2] = plStart;
        args[3] = plEnd;
        args[4] = plNb;

        PlQuery q("coup_possible", args);
        return q.next_solution();

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog isValidMove: " << (char*)ex << std::endl;
        return false;
    }
}

std::vector<Cell> PrologBridge::executeMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player) {
    std::vector<Cell> newBoard; // Le futur plateau vide

    try {
        // 1. CONVERSION C++ -> PROLOG (Comme avant)
        PlTerm plBoard;
        PlTail listTail(plBoard);
        for (const auto& cell : board) {
            PlTerm pileTerm;
            PlTail pileTail(pileTerm);
            for (const auto& p : cell.stack) {
                const char* atom = (p == WHITE) ? "b" : "n";
                pileTail.append(PlTerm(atom));
            }
            pileTail.close();
            listTail.append(pileTerm);
        }
        listTail.close();

        // 2. PRÉPARATION ARGUMENTS
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTerm plResultBoard; // Variable qui recevra le résultat (NouveauPlateau)

        PlTermv args(6);
        args[0] = plBoard;
        args[1] = PlTerm(playerChar);
        args[2] = PlTerm((long)start);
        args[3] = PlTerm((long)end);
        args[4] = PlTerm((long)numPieces);
        args[5] = plResultBoard;

        // --- VERIFIEZ CETTE LIGNE ---
        // Le nom entre guillemets DOIT être "jouer_coup_wrapper"
        PlQuery q("jouer_coup_wrapper", args);

        if (q.next_solution()) {
            // CA A MARCHÉ ! On doit maintenant lire plResultBoard pour remplir newBoard

            // On parcourt la liste principale (les 9 cases)
            PlTail boardList(plResultBoard);
            PlTerm pileTerm;

            while(boardList.next(pileTerm)) {
                Cell newCell;
                // On parcourt la sous-liste (la pile de pions)
                PlTail stackList(pileTerm);
                PlTerm pieceTerm;
                while(stackList.next(pieceTerm)) {
                    // On lit l'atome 'b' ou 'n'
                    std::string atom = (char*)pieceTerm;
                    if (atom == "b") newCell.stack.push_back(WHITE);
                    else if (atom == "n") newCell.stack.push_back(BLACK);
                }
                newBoard.push_back(newCell);
            }
        } else {
            std::cerr << "Prolog a echoue a jouer le coup (Coup invalide ?)" << std::endl;
            return board; // On renvoie l'ancien plateau si échec
        }

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog executeMove: " << (char*)ex << std::endl;
        return board;
    }

    return newBoard;
}


MoveInfo PrologBridge::getAIMove(const std::vector<Cell>& board, PlayerColor player) {
    MoveInfo move = {-1, -1, 0}; // Valeur par défaut (erreur)

    try {
        // 1. Convertir le plateau C++ -> Prolog
        PlTerm plBoard;
        PlTail listTail(plBoard);
        for (const auto& cell : board) {
            PlTerm pileTerm;
            PlTail pileTail(pileTerm);
            for (const auto& p : cell.stack) {
                const char* atom = (p == WHITE) ? "b" : "n";
                pileTail.append(PlTerm(atom));
            }
            pileTail.close();
            listTail.append(pileTerm);
        }
        listTail.close();

        // 2. Préparer les variables de sortie
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTerm plJoueur(playerChar);
        PlTerm plDep, plArr, plNb; // Variables qui recevront la réponse

        PlTermv args(5);
        args[0] = plBoard;
        args[1] = plJoueur;
        args[2] = plDep;
        args[3] = plArr;
        args[4] = plNb;

        // 3. Appel de get_best_move_wrapper
        PlQuery q("get_best_move_wrapper", args);

        if (q.next_solution()) {
            move.start = (int)plDep;
            move.end = (int)plArr;
            move.numPieces = (int)plNb;
            std::cout << "IA a choisi : De " << move.start << " vers " << move.end
                      << " (" << move.numPieces << " pieces)" << std::endl;
        } else {
            std::cerr << "L'IA n'a pas trouve de coup (Pas de solution Prolog)." << std::endl;
        }

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog IA: " << (char*)ex << std::endl;
    }

    return move;
}

PlayerColor PrologBridge::getWinner(const std::vector<Cell>& board) {
    // 1. Convertir le plateau (code habituel)
    PlTerm plBoard;
    PlTail listTail(plBoard);
    for (const auto& cell : board) {
        PlTerm pileTerm;
        PlTail pileTail(pileTerm);
        for (const auto& p : cell.stack) {
            const char* atom = (p == WHITE) ? "b" : "n";
            pileTail.append(PlTerm(atom));
        }
        pileTail.close();
        listTail.append(pileTerm);
    }
    listTail.close();

    // 2. Vérifier si BLANC gagne
    {
        PlTermv args(2);
        args[0] = plBoard;
        args[1] = PlTerm("b"); // Blanc
        PlQuery q("victoire", args);
        if (q.next_solution()) return WHITE;
    }

    // 3. Vérifier si NOIR gagne
    {
        PlTermv args(2);
        args[0] = plBoard;
        args[1] = PlTerm("n"); // Noir
        PlQuery q("victoire", args);
        if (q.next_solution()) return BLACK;
    }

    return EMPTY; // Partie en cours
}