#include <stdio.h>
#include <postgres.h>
#include <stdlib.h>
#include <fmgr.h>

#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "smallchesslib.h"
#include <stdbool.h>
#include <string.h>

#define ERR_CODE1    400
#define ERR_CODE2    401
// #ifndef CHESS_FUNCTIONS_H
// #define CHESS_FUNCTIONS_H


PG_MODULE_MAGIC;

typedef struct {
    char fen[128]; // FEN representation of chessboard
} chessboard;

typedef struct {
    char san[128]; // SAN representation of chess game
} chessgame;


bool smallchesslib_is_valid_fen(const char *fen) {
    // Implementation to validate FEN notation
    int rank = 0;
    int file = 0;
    int piecesCount = 0;

    while (*fen && rank < 8) {
        if (*fen == '/') {
            if (file != 8) {
                // Invalid rank length
                return false;
            }
            file = 0;
            rank++;
        } else if (isdigit(*fen)) {
            file += *fen - '0';
            if (file > 8) {
                // Invalid file length
                return false;
            }
        } else if (strchr("KQRBNPkqrbnp", *fen)) {
            file++;
            piecesCount++;
            if (file > 8) {
                // Invalid file length
                return false;
            }
        } else {
            // Invalid character
            return false;
        }
        fen++;
    }

    if (rank != 8 || piecesCount < 2) {
        // FEN should have 8 ranks and at least two pieces (both sides)
        return false;
    }

    // Check additional FEN conditions if required

    return true;
}

#define DatumGetChessgameP(X)  ((chessgame *) DatumGetPointer(X))
#define ChessgamePGetDatum(X)  PointerGetDatum(X)
#define PG_GETARG_CHESSGAME_P(n) DatumGetChessgameP(PG_GETARG_DATUM(n))
#define PG_RETURN_CHESSGAME_P(x) return ChessgamePGetDatum(x)

#define DatumGetChessboardP(X)  ((chessboard *) DatumGetPointer(X))
#define ChessboardPGetDatum(X)  PointerGetDatum(X)
#define PG_GETARG_CHESSBOARD_P(n) DatumGetChessboardP(PG_GETARG_DATUM(n))

/*___________________________________________________________________________
* Function declarations */

PG_FUNCTION_INFO_V1(chessgame_in);
PG_FUNCTION_INFO_V1(chessgame_out);
PG_FUNCTION_INFO_V1(chessgame_recv);
PG_FUNCTION_INFO_V1(chessgame_send);

PG_FUNCTION_INFO_V1(chessboard_in);
PG_FUNCTION_INFO_V1(chessboard_out);
PG_FUNCTION_INFO_V1(chessboard_recv);
PG_FUNCTION_INFO_V1(chessboard_send);



// Function to convert external representation to chessgame type
Datum chessgame_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    chessgame *result;

    // Create a function to parse the input string into the expected format
    // Example logic (modify as needed) - assuming moves separated by space
    char sanitized_moves[128] = ""; // Adjust the size as needed
    char move[16]; // Adjust the size as needed
    int index = 0;

    // Iterate through the input string to format it correctly for chessgame
    while (*str != '\0') {
        if (*str == ' ' && index > 0) {
            sanitized_moves[index++] = ' ';
        } else if (isalnum(*str) || *str == '-') {
            sanitized_moves[index++] = *str;
        }
        str++;
    }
    sanitized_moves[index] = '\0';

    result = (chessgame *) palloc(sizeof(chessgame));
    strncpy(result->san, sanitized_moves, sizeof(result->san));
    PG_RETURN_CHESSGAME_P(result);
}


// Function to convert chessgame type to external representation
Datum chessgame_out(PG_FUNCTION_ARGS) {
    chessgame *chess = PG_GETARG_CHESSGAME_P(0);
    char *result;

    result = psprintf("%s", chess->san);
    PG_RETURN_CSTRING(result);
}

// Function to receive chessgame type from binary representation
Datum chessgame_recv(PG_FUNCTION_ARGS) {
    ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("chessgame_recv not implemented")));
    PG_RETURN_NULL();
}

// Function to send chessgame type as binary representation
Datum chessgame_send(PG_FUNCTION_ARGS) {
    ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("chessgame_send not implemented")));
    PG_RETURN_NULL();
}

// Function to convert external representation to chessboard type
Datum chessboard_in(PG_FUNCTION_ARGS) {
    char *fen_str = PG_GETARG_CSTRING(0);
    chessboard *result = (chessboard *) palloc(sizeof(chessboard));

    // Use functions from smallchesslib to validate and handle FEN string
    if (!smallchesslib_is_valid_fen(fen_str)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid FEN string")));
        PG_RETURN_NULL();
    }

    // Copy the validated FEN string
    strncpy(result->fen, fen_str, sizeof(result->fen));
    PG_RETURN_POINTER(result);
}

// Function to convert chessboard type to external representation
Datum chessboard_out(PG_FUNCTION_ARGS) {
    chessboard *board = (chessboard *) PG_GETARG_POINTER(0);
    char *result;

    result = psprintf("%s", board->fen);
    PG_RETURN_CSTRING(result);
}

// Function to receive chessboard type from binary representation
Datum chessboard_recv(PG_FUNCTION_ARGS) {
    ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("chessboard_recv not implemented")));
    PG_RETURN_NULL();
}
Datum chessboard_send(PG_FUNCTION_ARGS) {
    ereport(ERROR,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("chessboard_send not implemented")));
    PG_RETURN_NULL();
}

/*___________________________________________________________________________
* Function aditionals */

static void simulateMove(chessboard *board, int moveNumber) {
    // Define an initial FEN for the starting board state
    const char* initialFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Use a library or custom code to manage moves and update the board state
    // For illustration, we'll simulate a couple of moves
    switch (moveNumber) {
        case 1:
            // Apply move 1. e4
            // Parse the move, update the board accordingly
            // Example: Update board after 1. e4
            snprintf(board->fen, sizeof(board->fen), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
            break;
        case 2:
            // Apply move 1... e5
            // Parse the move, update the board accordingly
            // Example: Update board after 1... e5
            snprintf(board->fen, sizeof(board->fen), "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
            break;
        // Implement more cases for each move to update the board state
        // You'll need proper move parsing, piece placement, and board updating logic here
        default:
            // For unhandled moves, return the current board state
            snprintf(board->fen, sizeof(board->fen), "%s", initialFEN);
            break;
    }
}

PG_FUNCTION_INFO_V1(getBoard);

Datum getBoard(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid arguments for getBoard")));
        PG_RETURN_NULL();
    }

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    int halfMoves = PG_GETARG_INT32(1);

    // Assuming initial board state
    chessboard *initialBoard = (chessboard *)palloc(sizeof(chessboard));
    snprintf(initialBoard->fen, sizeof(initialBoard->fen), "Initial FEN");

    if (halfMoves <= 0) {
        // Return the initial board state for halfMoves = 0
        PG_RETURN_POINTER(initialBoard);
    }

    // Create a temporary board and initialize it with the initial board state
    chessboard *tempBoard = (chessboard *)palloc(sizeof(chessboard));
    strncpy(tempBoard->fen, initialBoard->fen, sizeof(tempBoard->fen));

    // Simulate applying moves up to the given half-move
    for (int move = 1; move <= halfMoves; move++) {
        simulateMove(tempBoard, move);
    }

    // Return the board state at the given half-move
    PG_RETURN_POINTER(tempBoard);
}









// PG_FUNCTION_INFO_V1(hasBoard);
// Datum hasBoard(PG_FUNCTION_ARGS) {
//     if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2)) {
//         ereport(ERROR,
//                 (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
//                  errmsg("Invalid arguments for hasBoard")));
//         PG_RETURN_NULL();
//     }

//     chessgame *game = PG_GETARG_CHESSGAME_P(0);
//     text *board_text = PG_GETARG_TEXT_P(1);
//     int halfMoves = PG_GETARG_INT32(2);

//     // Convert text to char * for chessboard
//     char *board_str = text_to_cstring(board_text);

//     // Create a chessboard structure and set the FEN
//     chessboard *board = (chessboard *) palloc(sizeof(chessboard));
//     strncpy(board->fen, board_str, sizeof(board->fen));

//     // Free the memory allocated by text_to_cstring
//     pfree(board_str);

//     // Create a temporary chessboard to apply moves and compare states
//     chessboard tempBoard;
//     strncpy(tempBoard.fen, board->fen, sizeof(tempBoard.fen));

//     // Parse the moves from the chessgame
//     char *moves = strtok(game->san, " ");
//     int moveCount = 0;

//     while (moves != NULL && moveCount < halfMoves * 2) {
//         // Apply each move to the temporary board
//         char tempFEN[128]; // Temporary FEN to update the board state
//         strncpy(tempFEN, tempBoard.fen, sizeof(tempFEN)); // Copy the current board state

//         // Extract the source and destination squares from the move
//         char from[3], to[3];
//         strncpy(from, moves, 2);
//         from[2] = '\0';
//         strncpy(to, moves + 2, 2);
//         to[2] = '\0';

//         // Find the source and destination indexes in FEN notation
//         int sourceIndex = 56 - ((from[1] - '1') * 8) + (from[0] - 'a');
//         int destIndex = 56 - ((to[1] - '1') * 8) + (to[0] - 'a');

//         // Move the piece in the FEN notation
//         tempFEN[destIndex] = tempFEN[sourceIndex];
//         tempFEN[sourceIndex] = '-';

//         // Update the temporary board state
//         strncpy(tempBoard.fen, tempFEN, sizeof(tempBoard.fen));

//         // Check if the temporary board matches the given board
//         if (strncmp(tempBoard.fen, board->fen, sizeof(tempBoard.fen)) == 0) {
//             PG_RETURN_BOOL(true); // Board state found in the chessgame
//         }

//         moves = strtok(NULL, " ");
//         moveCount++;
//     }

//     PG_RETURN_BOOL(false); // Board state not found in the specified half-moves
// }