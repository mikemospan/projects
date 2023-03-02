SIZE = 8

pieces = {
    "EMPTY": {"letter": '-', "value": 0},
    "PAWN": {"letter": 'P', "value": 1, "player": 1, "hasMoved": False},
    "KNIGHT": {"letter": 'N', "value": 3, "player": 1},
    "BISHOP": {"letter": 'B', "value": 3, "player": 1},
    "ROOK": {"letter": 'R', "value": 5, "player": 1},
    "QUEEN": {"letter": 'Q', "value": 9, "player": 1},
    "KING": {"letter": 'K', "value": 100, "player": 1},
}

def initialiseBoard():
    board = []

    for row in range(SIZE):
        board.append([])
        for col in range(SIZE):
            if row == 1 or row == 6:
                board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["PAWN"])})
            elif row == 0 or row == 7:
                if col == 0 or col == 7:
                    board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["ROOK"])})
                elif col == 1 or col == 6:
                    board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["KNIGHT"])})
                elif col == 2 or col == 5:
                    board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["BISHOP"])})
                elif col == 3:
                    board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["QUEEN"])})
                elif col == 4:
                    board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["KING"])})
            else:
                board[row].append({"coordinate": str(row) + str(col), "piece": dict(pieces["EMPTY"])})

            if row == 0 or row == 1:
                board[row][col]["piece"]["player"] = 2
    printBoard(board)
    return board

def printBoard(board):
    print()
    nums = SIZE
    letters = 65

    for row in range(SIZE + 1):
        if row == SIZE:
            print()
        for col in range(SIZE + 1):
            if col == 0:
                if row == SIZE:
                    print(end = "     ")
                else:
                    print(nums, end = "    ")
                    nums = nums - 1
            elif row == SIZE and col > 0:
                print(chr(letters), end = " ")
                letters = letters + 1
            else:
                print(board[row][col - 1]["piece"]["letter"], end = " ")
        print()

def validCoordinate(origin, dest):
    if not len(origin) == 2 or not len(dest) == 2:
        print("Enter a 2 digit coordinate!")
        return False
    if not 0 <= int(origin[0]) <= 7 or not 0 <= int(origin[1]) <= 7:
        print("Enter a valid current position!")
        return False
    if not 0 <= int(dest[0]) <= 7 or not 0 <= int(dest[1]) <= 7:
        print("Enter a valid destination!")
        return False
    return True

def move(board, origin, dest):
    originPiece = board[int(origin[0])][int(origin[1])]["piece"]

    board[int(dest[0])][int(dest[1])]["piece"] = originPiece
    board[int(origin[0])][int(origin[1])]["piece"] = pieces["EMPTY"]
    printBoard(board)

def playerTurn(board, origin, dest):
    if not validCoordinate(origin, dest):
        return
    
    piece = board[int(origin[0])][int(origin[1])]["piece"]

    pawnDirection = -1
    if (piece["player"] == 2):
        pawnDirection = 1
    
    canPawnMove = int(dest[0]) - int(origin[0]) == pawnDirection and int(dest[1]) == int(origin[1])
    canKnightMove = abs(int(dest[0]) - int(origin[0])) == 2 and abs(int(dest[1]) - int(origin[1])) == 1
    canBishopMove = abs(int(dest[0]) - int(origin[0])) == abs(int(dest[1]) - int(origin[1]))
    canRookMove = (abs(int(dest[0]) - int(origin[0])) > 0 and int(dest[1]) == int(origin[1])) or (abs(int(dest[1]) - int(origin[1])) > 0 and int(dest[0]) == int(origin[0]))
    canQueenMove = canBishopMove or canRookMove
    canKingMove = abs(int(dest[0]) - int(origin[0])) == 1 or abs(int(dest[1]) - int(origin[1])) == 1

    destEmpty = board[int(dest[0])][int(dest[1])]["piece"] == pieces["EMPTY"]
    destMyPiece = board[int(dest[0])][int(dest[1])]["piece"]["player"] == piece["player"]
    
    if piece["letter"] == 'P' and destEmpty:
        if (not piece["hasMoved"] and int(dest[1]) == int(origin[1])):
            if (pawnDirection == -1 and -2 <= int(dest[0]) - int(origin[0]) <= -1) or (pawnDirection == 1 and 1 <= int(dest[0]) - int(origin[0]) <= 2):
                board[int(origin[0])][int(origin[1])]["piece"]["hasMoved"] = True
                move(board, origin, dest)
        elif canPawnMove:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    elif piece["letter"] == 'B':
        if canBishopMove and not destMyPiece:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    elif piece["letter"] == 'N':
        if canKnightMove and not destMyPiece:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    elif piece["letter"] == 'R':
        if canRookMove and not destMyPiece:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    elif piece["letter"] == 'Q':
        if canQueenMove and not destMyPiece:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    elif piece["letter"] == 'K':
        if canKingMove and not destMyPiece:
            move(board, origin, dest)
        else:
            print("That move is invalid, please try again.")
    else:
        print("That move is invalid, please try again.")

def changeCoord(coord):
    newLetter = str(ord(coord[0].upper()) - 65)
    newNumber = str(-int(coord[1]) + 8)
    return newNumber + newLetter

def main():
    board = initialiseBoard()
    while True:
        origin = input("Enter the current position of the piece you want to move: ")
        dest = input("Enter the destination of where you want the piece to move: ")
        playerTurn(board, changeCoord(origin), changeCoord(dest))

main()