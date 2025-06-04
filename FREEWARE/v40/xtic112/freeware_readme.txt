XTIC112, GAMES, X version of a simple but tricky board game

  RULES

    The board is made up of 4x4 squares and 16 pieces. The pieces
    carry 4 properties each, namely:

       o Black or brown
       o Horizontal or vertical
       o Solid or hollow
       o Round or square

    This makes a total of 16 possible pieces and there are exactly
    one piece of each type (so each piece can be represented by
    a binary number of length 4).

    Initially, the board is empty and it is successively filled with
    pieces. The game is over when a row, a column or a diagonal
    has four pieces carrying a common property in it, e.g. four
    black pieces. The player who places the fatal piece loses.

    The game is a two-player game, although in the current release,
    only the human-computer combination is supported.
    Player 1 (the human by default) chooses one of the 16 pieces. Player 2
    (the computer by default) places this piece on one of the 16
    squares of the board and chooses a piece out of the remaining
     15 pieces which he gives to player 1, who places this
     piece on one of the remaining 15 squares on the board, etc.

    As mentioned above, the game is over when a player places a
    piece in such a way that a row, a column or a diagonal
    (but see below) contains four pieces carrying a common property.
    The player who places this piece loses. If there is no empty square
    left, we have a draw (yes, this can happen).

  AUTHOR

    Mattias Jonsson,
    Dept of Mathematics,
    Royal Institute of Technology,
    S-100 44 Stockholm,
    Sweden.

    email: mjo@math.kth.se,
    URL: http://www.math.kth.se/~mjo


