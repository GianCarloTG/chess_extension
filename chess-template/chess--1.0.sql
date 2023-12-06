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
    internallength = 850,
    input = chessgame_in,
    output = chessgame_out);

CREATE TYPE chessboard (
    internallength = 850,
    input = chessboard_in,
    output = chessboard_out);

/******************************************************************************
 * Operators B-Tree
 ******************************************************************************/
CREATE FUNCTION chessgame_equal(game chessgame, opening chessgame)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_equal'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    

CREATE OPERATOR = (
    PROCEDURE = chessgame_equal,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '=',
    NEGATOR = '<>'
);

CREATE FUNCTION chessgame_greater_than(game chessgame, opening chessgame)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_greater_than'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR > (
    PROCEDURE = chessgame_greater_than,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '>'
);

CREATE FUNCTION chessgame_greater_than_or_equal(game chessgame, opening chessgame)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_greater_than_or_equal'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR >= (
    PROCEDURE = chessgame_greater_than_or_equal,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '>='
);
CREATE FUNCTION chessgame_less_than(game chessgame, opening chessgame)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_less_than'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR < (
    PROCEDURE = chessgame_less_than,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '<'
);

CREATE FUNCTION chessgame_less_than_or_eq(game chessgame, opening chessgame)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_less_than_or_eq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <= (
    PROCEDURE = chessgame_less_than_or_eq,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '<='
);

CREATE OPERATOR <> (
    PROCEDURE = chessgame_equal,
    NOT_IMPLEMENTED,
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    COMMUTATOR = '<>',
    NEGATOR = '='
);
CREATE OR REPLACE FUNCTION chess_cmp(game chessgame, opening chessgame)
RETURNS INTEGER
AS 'MODULE_PATHNAME','chess_cmp'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;



/******************************************************************************
 * Functions
 ******************************************************************************/

CREATE OR REPLACE FUNCTION getBoard(game chessgame, half_moves integer)
RETURNS chessboard
AS 'MODULE_PATHNAME', 'getBoard'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION getFirstMoves(game chessgame, half_moves integer)
RETURNS chessgame
AS 'MODULE_PATHNAME', 'getFirstMoves'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION hasBoard(game chessgame, board chessboard, half_moves integer)
RETURNS boolean
AS 'MODULE_PATHNAME', 'hasBoard'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION hasOpening(game chessgame, opening chessgame)
RETURNS boolean
AS 'MODULE_PATHNAME', 'hasOpening'
LANGUAGE C STRICT;

/******************************************************************************
 * B-Tree
 ******************************************************************************/

CREATE OPERATOR CLASS btree_has_opening
    DEFAULT FOR TYPE chessgame USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       chess_cmp(chessgame, chessgame);
