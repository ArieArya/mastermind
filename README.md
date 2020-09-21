# Mastermind Codebreaking Solver
Mastermind is a two player game where one player acts as a code maker, and the other acts as a code breaker. The code maker creates a hidden code consisting of
different coloured pegs, and the code breaker attempts to break the code by guessing using different coloured pegs. For more information about the game, please visit
https://en.wikipedia.org/wiki/Mastermind_(board_game).

This script will generate a randomized code based on the user inputs, and will attempt to break it with the minimum number of guesses possible. The algorithm utilizes
a mixture of the Knuth and Swaszek algorithms, combined with an efficient indexing system to reduce memory complexity.
