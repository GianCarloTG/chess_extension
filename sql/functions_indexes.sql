-- GET BOARD --
SELECT getboard(game_data, 1) FROM chess_games limit(3);
SELECT getboard(game_data, 10) FROM chess_games limit(3);
SELECT getboard(game_data, 20) FROM chess_games limit(3);

-- GET FIRST MOVE --
SELECT getfirstmoves(game_data, 4) FROM chess_games limit(3);
SELECT getfirstmoves(game_data, 6) FROM chess_games limit(3);
SELECT getfirstmoves(game_data, 10) FROM chess_games limit(3);


-- HAS OPENING --
SELECT count(*) hasopening
FROM chess_games
WHERE hasopening(game_data, '1. e4 e5 2. Nf3 Nf6 3. d3');

SELECT count(*) hasopening
FROM chess_games
WHERE hasopening(game_data, '1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7 ');

-- HAS BOARD --
SELECT count(*) hasboard
FROM chess_games
WHERE hasboard(game_data,
'r1b1k2r/pppp1pp1/2n2q1p/4p3/1b2P3/2NP1N2/PPP2PPP/R2QKB1R w KQkq - 2 7', 10);

SELECT count(*) hasboard
FROM chess_games
WHERE hasboard(game_data,
'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 10);

-- CHECK INDEX -- 

-- CREATE INDEX idx_has_opening_btree on chess_games(game_data) using btree_has_opening;
-- CREATE INDEX idx_has_opening_btree ON chess_games USING btree(game_data);
-- CREATE INDEX idx_chessgame ON chess_games USING btree (game_data btree_has_opening);
CREATE INDEX idx_chessgame ON chess_games USING btree (game_data btree_has_opening);
SET ENABLE_SEQSCAN TO OFF;

EXPLAIN ANALYZE 
SELECT count(*)
FROM chess_games
WHERE hasopening(game_data, '1. Nf3 Nf6 2. c4 g6 3. Nc3 Bg7 ');