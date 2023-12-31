#include <stdio.h>
#include <postgres.h>
#include <stdlib.h>
#include <fmgr.h>

#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "smallchesslib.h"
#include <stdbool.h>
#include <string.h>

#define ChessgameExistsStrategyNumber	1
typedef uint16 StrategyNumber;

PG_MODULE_MAGIC;

typedef struct {
    char fen[128]; 
} chessboard;

typedef struct {
    char san[1024]; 
} chessgame;

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
PG_FUNCTION_INFO_V1(chessboard_in);
PG_FUNCTION_INFO_V1(chessboard_out);

// Function to convert external representation to chessgame type
Datum chessgame_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    chessgame *result;
    char sanitized_moves[4096] = ""; 
    int index = 0;

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
    PG_RETURN_POINTER(result);
}

Datum chessgame_out(PG_FUNCTION_ARGS) {
    chessgame *chess = PG_GETARG_CHESSGAME_P(0);
    char *result;

    result = psprintf("%s", chess->san);
    PG_RETURN_CSTRING(result);
}

Datum chessboard_in(PG_FUNCTION_ARGS) {
    char *fen_str = PG_GETARG_CSTRING(0);

    chessboard *result = (chessboard *) palloc(sizeof(chessboard));
    strncpy(result->fen, fen_str, sizeof(result->fen));

    PG_RETURN_POINTER(result);
}

Datum chessboard_out(PG_FUNCTION_ARGS) {
    chessboard *board = (chessboard *) PG_GETARG_POINTER(0);
    char *result = pstrdup(board->fen);

    PG_RETURN_CSTRING(result);
}

// Chess functions

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
	char *game_str = pstrdup(game->san);
	SCL_Record chesslib_record;
	SCL_recordFromPGN(chesslib_record, game_str);
	SCL_Board chesslib_board;
	SCL_boardInit(chesslib_board);
	if (halfMoves > 0) {
        SCL_recordApply(chesslib_record, chesslib_board, halfMoves);
    }
	char board_str[128];
    SCL_boardToFEN(chesslib_board, board_str);
    chessboard *tempBoard = (chessboard *)palloc(sizeof(chessboard));
    strncpy(tempBoard->fen, board_str, sizeof(tempBoard->fen));
    PG_RETURN_POINTER(tempBoard);
}

int cGetFirstMoves(SCL_Record record, int moves) {

    uint16_t l = SCL_recordLength(record);

    if (moves == l) {
        return 0;
    }
    else if (moves == 0) {
        SCL_recordInit(record);
    }
    else {
        moves = (moves - 1) * 2;
        record[moves] = (record[moves] & 0x3f) | SCL_RECORD_END;
    }

    return 0;
}

PG_FUNCTION_INFO_V1(getFirstMoves);

Datum getFirstMoves(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid arguments for getFirstMoves")));
        PG_RETURN_NULL();
    }
	chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    int halfMoveCount = PG_GETARG_INT32(1);

    if (halfMoveCount < 0) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Half-move count should be non-negative")));
    }
    char *game_str = pstrdup(game->san);
	SCL_Record chesslib_record;
	SCL_recordFromPGN(chesslib_record, game_str);
    cGetFirstMoves(chesslib_record, halfMoveCount);
	char result_str[4096];
	SCL_printPGN(chesslib_record, 0, result_str);
	chessgame *result = (chessgame *) palloc(sizeof(chessgame));
    strncpy(result->san, result_str, sizeof(result->san));
    PG_RETURN_POINTER(result);
}

int cHasBoard(SCL_Record record, SCL_Board board, int moves) {

    SCL_Board gameboard;
    SCL_boardInit(gameboard);
    if (SCL_boardsDiffer(gameboard, board) == 0) {
        return 1; //TRUE
    }

    for (int i = 0; i < moves; ++i)
    {
        uint8_t s0, s1;
        char p;

        SCL_recordGetMove(record, i, &s0, &s1, &p);
        SCL_boardMakeMove(gameboard, s0, s1, p);
        if (SCL_boardsDiffer(gameboard, board) == 0) {
            return 1; //TRUE
        }
    }
    return 0; //FALSE
}

PG_FUNCTION_INFO_V1(hasBoard);

Datum hasBoard(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid arguments for hasBoard")));
        PG_RETURN_NULL();
    }
    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
	chessboard *board = (chessboard *)PG_GETARG_POINTER(1);
    int halfMoves = PG_GETARG_INT32(2);
    char *game_str = pstrdup(game->san);
    char *board_str = pstrdup(board->fen);
	SCL_Record chesslib_record;
	SCL_recordFromPGN(chesslib_record, game_str);
	SCL_Board chesslib_board;
	SCL_boardInit(chesslib_board);
    SCL_boardFromFEN(chesslib_board, board_str);
    int res;
	res = cHasBoard(chesslib_record, chesslib_board, halfMoves);
    PG_RETURN_BOOL(res == 1);
}

int cHasOpening(SCL_Record record, SCL_Record opening) {

    int openingLength = SCL_recordLength(opening);
    int recordLength = SCL_recordLength(record);

    if (openingLength > recordLength) {
        return 0; //FALSE
    }

    cGetFirstMoves(record, openingLength);
	
    for (int i = 0; i < openingLength * 2; i++) {
        if (record[i] != opening[i]) {
            return 0; //FALSE
        }
    }
    return 1; 
}

PG_FUNCTION_INFO_V1(hasOpening);

Datum hasOpening(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid arguments for hasOpening")));
        PG_RETURN_NULL();
    }
    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
	chessgame *opening = (chessgame *)PG_GETARG_POINTER(1);
    char *game_str = pstrdup(game->san);
    char *opening_str = pstrdup(opening->san);
	SCL_Record chesslib_record;
	SCL_recordFromPGN(chesslib_record, game_str);
	SCL_Record chesslib_opening;
	SCL_recordFromPGN(chesslib_opening, opening_str);
    int res;
	res = cHasOpening(chesslib_record, chesslib_opening);
    PG_RETURN_BOOL(res == 1);
}

/*___________________________________________________________________________
 ########### B-Tree ###########
___________________________________________________________________________
*/

static bool chessgame_eq(chessgame *a, chessgame *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    int length_a = strlen(a->san);
    int length_b = strlen(b->san);

    if (length_a != length_b) {
        return false;
    }
    for (int i = 0; i < length_a; i++) {
        if (a->san[i] != b->san[i]) {
            return false;
        }
    }
    return true;
}

PG_FUNCTION_INFO_V1(chessgame_equal);

Datum chessgame_equal(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(chessgame_eq(a, b));
}

PG_FUNCTION_INFO_V1(chessgame_dif);

Datum chessgame_dif(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(!chessgame_eq(a, b));
}

static bool chessgame_gt(chessgame *a, chessgame *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    int length_a = strlen(a->san);
    int length_b = strlen(b->san);
    int min_length = (length_a < length_b) ? length_a : length_b;

    for (int i = 0; i < min_length; i++) {
        if (a->san[i] > b->san[i]) {
            return true;
        } else if (a->san[i] < b->san[i]) {
            return false;
        }
    }
    return (length_a > length_b);
}

PG_FUNCTION_INFO_V1(chessgame_greater_than);

Datum chessgame_greater_than(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(chessgame_gt(a, b));
}

static bool chessgame_gte(chessgame *a, chessgame *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    int length_a = strlen(a->san);
    int length_b = strlen(b->san);
    int min_length = (length_a < length_b) ? length_a : length_b;

    for (int i = 0; i < min_length; i++) {
        if (a->san[i] > b->san[i]) {
            return true;
        } else if (a->san[i] < b->san[i]) {
            return false;
        }
    }

    return (length_a >= length_b);
}

PG_FUNCTION_INFO_V1(chessgame_greater_than_or_equal);

Datum chessgame_greater_than_or_equal(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(chessgame_gte(a, b));
}

static bool chessgame_lt(chessgame *a, chessgame *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    int length_a = strlen(a->san);
    int length_b = strlen(b->san);
    int min_length = (length_a < length_b) ? length_a : length_b;

    for (int i = 0; i < min_length; i++) {
        if (a->san[i] < b->san[i]) {
            return true;
        } else if (a->san[i] > b->san[i]) {
            return false;
        }
    }

    return (length_a < length_b);
}

PG_FUNCTION_INFO_V1(chessgame_less_than);
Datum chessgame_less_than(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(chessgame_lt(a, b));
}

static bool chessgame_lte(chessgame *a, chessgame *b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    int length_a = strlen(a->san);
    int length_b = strlen(b->san);
    int min_length = (length_a < length_b) ? length_a : length_b;

    for (int i = 0; i < min_length; i++) {
        if (a->san[i] < b->san[i]) {
            return true;
        } else if (a->san[i] > b->san[i]) {
            return false;
        }
    }

    return (length_a <= length_b);
}

PG_FUNCTION_INFO_V1(chessgame_less_than_or_eq);
Datum chessgame_less_than_or_eq(PG_FUNCTION_ARGS) {
    chessgame *a = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *b = (chessgame *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(chessgame_lte(a, b));
}

PG_FUNCTION_INFO_V1(chess_cmp);

Datum chess_cmp(PG_FUNCTION_ARGS) {
    text *arg1 = PG_GETARG_TEXT_PP(0);
    text *arg2 = PG_GETARG_TEXT_PP(1);

    char *str1 = text_to_cstring(arg1);
    char *str2 = text_to_cstring(arg2);

    int result = strcmp(str1, str2);
    pfree(str1);
    pfree(str2);

    PG_RETURN_INT32(result);
}

/*___________________________________________________________________________
 ########### GIN ###########
___________________________________________________________________________
*/

PG_FUNCTION_INFO_V1(chessgame_extract_value);

Datum chessgame_extract_value(PG_FUNCTION_ARGS) {
	chessgame *game = (chessgame *) PG_GETARG_POINTER(0);
	int32 *nentries = (int32 *) PG_GETARG_POINTER(1);
	
	Datum *entries; // return value
	
	// nentries <- number of chessboards
	// entries <- array of chessboards
	SCL_Record record;
	SCL_recordFromPGN(record, game->san);
	int count = SCL_recordLength(record);
	*nentries = count + 1;
	
	entries = (Datum *) palloc(sizeof(Datum) * (count + 1));
	
	SCL_Board gameboard;
	SCL_boardInit(gameboard);
	char fen_str[128];
    SCL_boardToFEN(gameboard, fen_str);
	
	chessboard *result = (chessboard *) palloc(sizeof(chessboard));
    strncpy(result->fen, fen_str, sizeof(result->fen));
	
	entries[0] = PointerGetDatum(result);
	
	for (int i = 0; i < count; ++i)
	{
		uint8_t s0, s1;
        char p;

        SCL_recordGetMove(record, i, &s0, &s1, &p);
        SCL_boardMakeMove(gameboard, s0, s1, p);
		SCL_boardToFEN(gameboard, fen_str);
		
		strncpy(result->fen, fen_str, sizeof(result->fen));
		
		entries[i + 1] = PointerGetDatum(result);
	}
	PG_RETURN_POINTER(entries);
}

PG_FUNCTION_INFO_V1(chessgame_extract_query);

Datum chessgame_extract_query(PG_FUNCTION_ARGS) {
	int32			*nentries = (int32 *) PG_GETARG_POINTER(1);
	StrategyNumber	strategy = PG_GETARG_UINT16(2);
	int32			*searchMode = (int32 *) PG_GETARG_POINTER(6);
	Datum			*entries;
	if (strategy == ChessgameExistsStrategyNumber){
		// argument is chessboard, operation chessgame ? chessboard
		chessboard	*b = (chessboard *) PG_GETARG_POINTER(0);
		chessboard	*item = (chessboard *) palloc(sizeof(chessboard));

		*nentries = 1;
		entries = (Datum *) palloc(sizeof(Datum));
		strncpy(item->fen, b->fen, sizeof(item->fen));
		entries[0] = PointerGetDatum(item);
	}
	PG_RETURN_POINTER(entries);
}

PG_FUNCTION_INFO_V1(chessgame_compare_entry);

Datum chessgame_compare_entry(PG_FUNCTION_ARGS) {
	// compare two chessboards
    chessboard	*arg1 = (chessboard *) PG_GETARG_POINTER(0);
    chessboard  *arg2 = (chessboard *) PG_GETARG_POINTER(1);
    int32       result;
	
	SCL_Board gameboard;
	SCL_boardInit(gameboard);
    SCL_boardFromFEN(gameboard, arg1->fen);
	
	SCL_Board board;
	SCL_boardInit(board);
    SCL_boardFromFEN(board, arg2->fen);
	
    result = SCL_boardsDiffer(gameboard, board);	// 0 if equal, 1 if not
  
    PG_FREE_IF_COPY(arg1, 0);
    PG_FREE_IF_COPY(arg2, 1);

    PG_RETURN_INT32(result);
}

PG_FUNCTION_INFO_V1(chessgame_consistent);

Datum chessgame_consistent(PG_FUNCTION_ARGS) {
	bool	*check = (bool *) PG_GETARG_POINTER(0);
	StrategyNumber strategy = PG_GETARG_UINT16(1);
	bool res;
	if ( strategy == ChessgameExistsStrategyNumber ) {
		// chessgame ? chessboard operation
		res = check[0] == true;
	}
	PG_RETURN_BOOL(res);
} 


PG_FUNCTION_INFO_V1(hasBoardGeneral);

Datum hasBoardGeneral(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid arguments for hasBoard")));
        PG_RETURN_NULL();
    }

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
	chessboard *board = (chessboard *)PG_GETARG_POINTER(1);

    char *game_str = pstrdup(game->san);
    char *board_str = pstrdup(board->fen);
	
	SCL_Record chesslib_record;
	SCL_recordFromPGN(chesslib_record, game_str);
	
	SCL_Board chesslib_board;
	SCL_boardInit(chesslib_board);
    SCL_boardFromFEN(chesslib_board, board_str);
	
	int halfMoves = SCL_recordLength(chesslib_record);
    int res;
	res = cHasBoard(chesslib_record, chesslib_board, halfMoves);

    PG_RETURN_BOOL(res == 1);
}