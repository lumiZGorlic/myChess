# https://chess.stackexchange.com/questions/19633/chess-problem-database-with-pgn-or-fen
# wtharvey links from the above link pasted into the puzzle.txt
filename = 'puzzles.txt'

with open(filename) as file:
    lines = [line.rstrip() for line in file]

i = 0
fens, bestmvs = [], []

while i < 30:   # just get first 30
    fens.append(lines[i][:-4])

    moves = lines[i+1].split()
    bestmvs.append(moves[1])

    i += 2


for f in fens: print(f)

print('-------------------------')

for m in bestmvs: print(m)


