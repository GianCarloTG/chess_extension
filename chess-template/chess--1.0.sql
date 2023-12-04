CREATE OR REPLACE FUNCTION chessgame_in(cstring)
    RETURNS chessgame
    AS 'MODULE_PATHNAME', 'chessgame_in'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'chessgame_out'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessboard_in(cstring)
    RETURNS chessboard
    AS 'MODULE_PATHNAME', 'chessboard_in'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'chessboard_out'
    LANGUAGE C STRICT;

    CREATE TYPE chessgame (
    internallength = 4096,
    input = chessgame_in,
    output = chessgame_out);

CREATE TYPE chessboard (
    internallength = 850,
    input = chessboard_in,
    output = chessboard_out);

-- FUNCTIONS

CREATE OR REPLACE FUNCTION getBoard(game chessgame, half_moves integer)
RETURNS chessboard
AS 'MODULE_PATHNAME', 'getBoard'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION getFirstMoves(game chessgame, half_moves integer)
RETURNS chessboard
AS 'MODULE_PATHNAME', 'getFirstMoves'
LANGUAGE C STRICT;

-- CREATE OR REPLACE FUNCTION display_chessboard(board chessboard) RETURNS VOID AS $$
-- BEGIN
--     RAISE NOTICE 'FEN representation of chessboard: %', board.fen;
-- END;
-- $$ LANGUAGE plpgsql;