create extension chess;

CREATE TABLE chess_games (
    id SERIAL PRIMARY KEY,
    game_name VARCHAR(100),
    game_data chessgame
);

CREATE TABLE chess_board (
    id SERIAL PRIMARY KEY,
    bard_name VARCHAR(100),
    board_data chessboard
);

COPY chess_games(game_name, game_data)
FROM '/home/sql/chess_games.csv' DELIMITER ',' CSV HEADER;

COPY chess_board(bard_name, board_data)
FROM '/home/sql/chess_board.csv' DELIMITER ',' CSV HEADER;