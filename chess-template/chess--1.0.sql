


CREATE OR REPLACE FUNCTION chessgame_in(cstring)
    RETURNS chessgame
    AS 'MODULE_PATHNAME', 'chessgame_in'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'chessgame_out'
    LANGUAGE C STRICT;

-- CREATE OR REPLACE FUNCTION chessgame_recv(internal)
--     RETURNS chessgame
--     AS 'MODULE_PATHNAME', 'chessgame_recv'
--     LANGUAGE C STRICT;

-- CREATE OR REPLACE FUNCTION chessgame_send(chessgame)
--     RETURNS bytea
--     AS 'MODULE_PATHNAME', 'chessgame_send'
--     LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessboard_in(cstring)
    RETURNS chessboard
    AS 'MODULE_PATHNAME', 'chessboard_in'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'chessboard_out'
    LANGUAGE C STRICT;

    CREATE TYPE chessgame (
    internallength = 850,
    input = chessgame_in,
    output = chessgame_out);

CREATE TYPE chessboard (
    internallength = 850,
    input = chessboard_in,
    output = chessboard_out);

-- CREATE OR REPLACE FUNCTION chessboard_recv(internal)
--     RETURNS chessboard
--     AS 'MODULE_PATHNAME', 'chessboard_recv'
--     LANGUAGE C STRICT;

-- CREATE OR REPLACE FUNCTION chessboard_send(chessboard)
--     RETURNS bytea
--     AS 'MODULE_PATHNAME', 'chessboard_send'
--     LANGUAGE C STRICT;

    

CREATE OR REPLACE FUNCTION get_board_state(game chessgame, half_moves integer)
RETURNS chessboard
AS 'MODULE_PATHNAME', 'getBoard'
LANGUAGE C STRICT;

-- CREATE OR REPLACE FUNCTION check_board_state(game_moves text, board_state text, half_moves integer)
-- RETURNS boolean AS $$
-- BEGIN
--     RETURN hasBoard(game_moves::chessgame, board_state::chessboard, half_moves);
-- END;
-- $$ LANGUAGE plpgsql;