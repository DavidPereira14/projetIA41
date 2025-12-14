#include "PrologBridge.h"
#include <SWI-cpp.h>
#include <iostream>

PrologBridge::PrologBridge()
{
    char *argv[] = { (char*)"swipl", (char*)"-q", nullptr };
    PL_initialise(2, argv);

    try {

        // On demande 256 Mo (au lieu de 2 Go qui fait crasher).
         PlCall("set_prolog_flag(stack_limit, 256000000)");

        // Chargement du fichier
        PlCall("consult('../../moteur/projet.pl')");

        std::cout << "[PrologBridge] Memoire (256Mo) ok, projet.pl charge." << std::endl;

    } catch(const PlException &ex) {
        std::cerr << "Erreur Prolog Init: " << (char*)ex << std::endl;
    }
}

PrologBridge::~PrologBridge()
{
    PL_halt(0);
}

// ---------------------------------------------------------
// VERIFICATION DE COUP (Humain)
// ---------------------------------------------------------
bool PrologBridge::isValidMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player) {
    try {
        // A. Conversion C++ -> Prolog
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

        // B. Préparation des arguments pour coup_possible/5
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTermv args(5);
        args[0] = plBoard;
        args[1] = PlTerm(playerChar);
        args[2] = PlTerm((long)start);
        args[3] = PlTerm((long)end);
        args[4] = PlTerm((long)numPieces);

        // C. Appel du prédicat
        PlQuery q("coup_possible", args);
        return q.next_solution();

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog isValidMove: " << (char*)ex << std::endl;
        return false;
    }
}

// ---------------------------------------------------------
// EXECUTION DE COUP (Mise à jour du plateau)
// ---------------------------------------------------------
std::vector<Cell> PrologBridge::executeMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player) {
    std::vector<Cell> newBoard; 

    try {
        // A. Conversion C++ -> Prolog
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

        // B. Appel de jouer_coup_wrapper/6
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTerm plResultBoard; // Variable de sortie

        PlTermv args(6);
        args[0] = plBoard;
        args[1] = PlTerm(playerChar);
        args[2] = PlTerm((long)start);
        args[3] = PlTerm((long)end);
        args[4] = PlTerm((long)numPieces);
        args[5] = plResultBoard; // Prolog va écrire le nouveau plateau ici

        PlQuery q("jouer_coup_wrapper", args);

        if (q.next_solution()) {
            // C. Reconstruction du plateau C++ depuis la réponse Prolog
            PlTail boardList(plResultBoard);
            PlTerm pileTerm;

            while(boardList.next(pileTerm)) {
                Cell newCell;
                PlTail stackList(pileTerm);
                PlTerm pieceTerm;
                while(stackList.next(pieceTerm)) {
                    // Lecture des atomes 'b' ou 'n'
                    std::string atom = (char*)pieceTerm;
                    if (atom == "b") newCell.stack.push_back(WHITE);
                    else if (atom == "n") newCell.stack.push_back(BLACK);
                }
                newBoard.push_back(newCell);
            }
        } else {
            std::cerr << "Erreur: Prolog n'a pas renvoye de nouveau plateau." << std::endl;
            return board; // En cas d'échec, on garde l'ancien plateau
        }

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog executeMove: " << (char*)ex << std::endl;
        return board;
    }

    return newBoard;
}

// ---------------------------------------------------------
// INTELLIGENCE ARTIFICIELLE
// ---------------------------------------------------------
MoveInfo PrologBridge::getAIMove(const std::vector<Cell>& board, PlayerColor player) {
    MoveInfo move = {-1, -1, 0}; 

    try {
        // A. Conversion C++ -> Prolog
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

        // B. Appel de get_best_move_wrapper/5
        const char* playerChar = (player == WHITE) ? "b" : "n";
        PlTerm plDep, plArr, plNb; 

        PlTermv args(5);
        args[0] = plBoard;
        args[1] = PlTerm(playerChar);
        args[2] = plDep;
        args[3] = plArr;
        args[4] = plNb;

        PlQuery q("get_best_move_wrapper", args);

        if (q.next_solution()) {
            move.start = (long)plDep;
            move.end = (long)plArr;
            move.numPieces = (long)plNb;
            std::cout << "IA (Prolog) joue : " << move.start << " -> " << move.end 
                      << " (" << move.numPieces << " pieces)" << std::endl;
        } else {
            std::cerr << "L'IA n'a trouve aucun coup valide." << std::endl;
        }

    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog IA: " << (char*)ex << std::endl;
    }

    return move;
}

// ---------------------------------------------------------
// VICTOIRE
// ---------------------------------------------------------
PlayerColor PrologBridge::getWinner(const std::vector<Cell>& board) {
    try {
        // A. Conversion
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

        // B. Test Victoire Blanc
        {
            PlTermv args(2);
            args[0] = plBoard;
            args[1] = PlTerm("b");
            PlQuery q("victoire", args);
            if (q.next_solution()) return WHITE;
        }

        // C. Test Victoire Noir
        {
            PlTermv args(2);
            args[0] = plBoard;
            args[1] = PlTerm("n");
            PlQuery q("victoire", args);
            if (q.next_solution()) return BLACK;
        }
    } catch (PlException &ex) {
        std::cerr << "Erreur Prolog Victoire: " << (char*)ex << std::endl;
    }

    return EMPTY;
}