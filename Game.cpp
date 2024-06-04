#include "Game.h"

// Main function of the game. It initializes the game board and game pieces,
// asks the user if they want to load a saved game, and if not, sets the starting player to white.
// It then enters the main game loop, where it displays the game board and game stats,
// prompts the user for a move, validates the move, and performs the move.
// If the user chooses to exit, it asks if they want to save the game.

int main()
{
    static char board[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
    };

    static char takenPiecesBlack[16];
    int lBlack = -1;
    static char takenPiecesWhite[16];
    int lWhite = -1;

    Color color = Color::Undefined;

    askLoadGame(board, takenPiecesBlack, takenPiecesWhite, &color);

    if (color == Color::Undefined) {
        color = Color::White;
    }

    string errMessage = "";
    string winner = "";
    bool patt = false;
    bool running = true;

    while (running) {
        displayBoard(board, 8, 8, color, errMessage);
        displayStats(takenPiecesBlack, takenPiecesWhite);
        errMessage = "";

        bool exit = false;
        Move move = promptForMove(8, &exit);
        if (exit) {
            askSaveGame(board, takenPiecesBlack, takenPiecesWhite, color);
            return 0;
        }

        if (!isMoveValid(board, color, move, &errMessage)) {
            continue;
        }

        if (isBeat(board, move) && isValidBeat(board, move, color, &errMessage)) {
            string colorAsString = color == Color::Black ? "Black" : "White";
            char piece = board[move.to.y - 1][move.to.x - 1];

            if (piece == 'k' || piece == 'K') {
                winner = colorAsString;
                break;
            }

            if (color == Color::Black) {
                takenPiecesWhite[lWhite + 1] = piece;
                lWhite++;
            }
            else {
                takenPiecesBlack[lBlack + 1] = piece;
                lBlack++;
            }

            if (lBlack >= 14 && lWhite >= 14) {
                patt = true;
                break;
            }
        }

        if (errMessage == "") {
            doMove(board, move);
            checkConvertPawn(board, move, color);
            color = color == Color::Black ? Color::White : Color::Black;
        }
    }

    system("cls");

    if (patt) {
        cout << "Patt!\n";
    }
    else {
        cout << winner << " Wins!\n";
    }
}

// Displays the pieces taken by each player.
// The pieces are stored in two arrays, one for black and one for white.

void displayStats(char takenPiecesBlack[16], char takenPiecesWhite[16]) {
    cout << "Taken pieces: (Black) " << takenPiecesBlack << ", (White) " << takenPiecesWhite << endl;
    cout << endl;
}

// Displays the current state of the game board, the current player's turn, and any error messages.
// The board is displayed with different colors for different squares to resemble a real chess board.

void displayBoard(char board[][8], int sizeX, int sizeY, Color currentColor, string message) {
    system("cls");

    string colorAsString = currentColor == Color::Black ? "Black" : "White";
    cout << "\nIt is " << colorAsString << "'s turn\n";
    cout << endl;

    if (message.length() > 0) {
        cout << message << endl;
        cout << endl;
    }

    cout << "   ";
    for (int x = 0; x < sizeX; x++) {
        cout << " " << x + 1 << " ";
    }
    cout << endl;
    for (int y = 0; y < sizeY; y++) {
        cout << "\33[0m " << y + 1 << " ";
        for (int x = 0; x < sizeX; x++) {
            if (y % 2 == 0) {
                if (x % 2 == 0) {
                    cout << "\33[30m\33[47m";
                }
                else {
                    cout << "\33[97m\33[40m";
                }
            }
            else {
                if (x % 2 == 0) {
                    cout << "\33[97m\33[40m";
                }
                else {
                    cout << "\33[30m\33[47m";
                }
            }

            cout << " " << board[y][x] << " ";
        }
        cout << endl;
    }
    cout << "\33[0m\n";
}

// Prompts the user to select a color to play as.
// The user can choose between black and white.
// The function will keep prompting until a valid color is selected.
//Now unused bcs the AI opponent isn't implemented yet

Color selectColor() {
    Color color;
    bool colorSelected = false;
    while (!colorSelected) {
        color = promptForColor();
        if (color != Color::White && color != Color::Black) {
            system("cls");
            continue;
        }
        colorSelected = true;
    }
    system("cls");
    return color;
}

// Prompts the user to enter a color.
// The user can enter 1 for black or 2 for white.
// The function returns the corresponding Color enum value.
// Again unused bcs the AI opponent isn't implemented yet

Color promptForColor() {
    Color color;
    int tempNumber;
    cout << "Which color do you want to play as? (black - 1, white - 2): ";
    cin >> tempNumber;
    color = (Color)tempNumber;
    return color;
}

// Prompts the user to enter a piece type for pawn promotion.
// The user can enter 'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight.
// The function returns the entered character.

char promptForPiece() {
    char piece;
    string input;
    cout << "Which piece do you want to promote your pawn to? (q,r,b,n)";
    cin >> input;
    piece = input[0];
    return piece;
}

// Prompts the user to enter a move or to exit the game.
// A move is entered as four coordinates: fromX,fromY,toX,toY.
// If the user enters "exit", the function sets the exit flag to true and returns an empty Move.


Move promptForMove(int maxCoord, bool *exit) {
    Vector2D fromPos;
    Vector2D toPos;
    bool moveValid = false;
    while (!moveValid) {
        string input;
        cout << "Where do you want to move? fromX,fromY,toX,toY or exit: ";
        cin >> input;

        if (input == "exit") {
            *exit = true;
            return {};
        }

        fromPos.x = stoi(input.substr(0, input.find(",", 0)));
        input = input.substr(input.find(",", 0) + 1, input.length());

        fromPos.y = stoi(input.substr(0, input.find(",", 0)));
        input = input.substr(input.find(",", 0) + 1, input.length());

        toPos.x = stoi(input.substr(0, input.find(",", 0)));
        input = input.substr(input.find(",", 0) + 1, input.length());

        toPos.y = stoi(input.substr(0, input.find(",", 0)));
        input = input.substr(input.find(",", 0) + 1, input.length());

        if (fromPos.x < 1 || fromPos.x > maxCoord || fromPos.y < 1 || fromPos.y > maxCoord) {
            system("cls");
            cout << "From value must be in the range (1," << maxCoord << ")\n";
            continue;
        }
        if (toPos.x < 1 || toPos.x > maxCoord || toPos.y < 1 || toPos.y > maxCoord) {
            system("cls");
            cout << "To value must be in the range (1," << maxCoord << ")\n";
            continue;
        }

        // TODO: Add checks

        moveValid = true;
    }

    return { fromPos, toPos };
}

// Performs a move on the game board.
// It moves the piece from the 'from' position to the 'to' position on the board.

void doMove(char board[][8], Move move) {
    char piece = board[move.from.y - 1][move.from.x - 1];
    board[move.from.y - 1][move.from.x - 1] = ' ';
    board[move.to.y - 1][move.to.x - 1] = piece;
}

// Checks if a pawn should be promoted.
// If a pawn reaches the opposite end of the board, it prompts the user to select a piece for promotion.

void checkConvertPawn(char board[][8], Move move, Color color) {
    if (color == Color::Black && move.to.y == 8) {
        convertPawn(board, move.to, color);
    }
    else if (color == Color::White && move.to.y == 1) {
        convertPawn(board, move.to, color);
    }
}

// Promotes a pawn to a different piece.
// The user can choose to promote the pawn to a queen, rook, bishop, or knight.

void convertPawn(char board[][8], Vector2D pos, Color color) {
    if (pos.y != 1 && pos.y != 8) return;

    system("cls");
    char piece;
    bool pieceSelected = false;
    while (!pieceSelected) {
        piece = promptForPiece();
        if (piece != 'q' && piece != 'r' && piece != 'b' && piece != 'n') {
            system("cls");
            continue;
        }
        pieceSelected = true;
    }
    system("cls");

    piece = color == Color::Black ? piece : toupper(piece);

    board[pos.y - 1][pos.x - 1] = piece;
}

// Checks if a piece at a given position on the board belongs to a given player.
// It checks the case of the piece character to determine the owner.

bool ownsPiece(char board[][8], Vector2D pos, Color color) {
    if (islower(board[pos.y - 1][pos.x - 1]) && color == Color::Black) {
        return true;
    }
    else if (isupper(board[pos.y - 1][pos.x - 1]) && color == Color::White) {
        return true;
    }
    else {
        return false;
    }
}

// Validates a move according to the rules of chess.
// It checks if there is a piece at the 'from' position, if the piece belongs to the current player,
// if the 'from' and 'to' positions are different, and if the move is valid for the piece type.
// If the move is invalid, it sets an error message.

bool isMoveValid(char board[][8], Color color, Move move, string *errMessage) {
    if (board[move.from.y - 1][move.from.x - 1] == ' ') {
        *errMessage = "There's no piece at that location.";
        return false;
    }

    if (!ownsPiece(board, move.from, color)) {
        *errMessage = "You don't own that piece.";
        return false;
    }

    if (move.from.x == move.to.x && move.from.y == move.to.y) {
        *errMessage = "Come on, I can't see the difference there.";
        return false;
    }

    switch (tolower(board[move.from.y - 1][move.from.x - 1])) {
        case 'p':
            if (isPawnMove(move, color)) {
                if (isPieceOnSquare(board, move.to)) {
                    *errMessage = "Target pos is already occupied";
                    return false;
                }
            } else if (!(isDiagonalPawn(move, color) && isPieceOnSquare(board, move.to) && !ownsPiece(board, move.to, color))) {
                // TODO: ugly if statement
                *errMessage = "Move is not permitted";
                return false;
            }
            break;
        case 'r':
            if (!isStraight(move)) {
                *errMessage = "Move is not straight";
                return false;
            }
            if (isRookPathBlocked(board, move)) {
                *errMessage = "Move is blocked";
                return false;
            }
            break;
        case 'n':
            if (!isLShape(move)) {
                *errMessage = "Move is not L shape";
                return false;
            }
            break;
        case 'b':
            if (!isDiagonal(move)) {
                *errMessage = "Move is not diagonal";
                return false;
            }
            if (isBishopPathBlocked(board, move)) {
                *errMessage = "Move is blocked";
                return false;
            }
            break;
        case 'q':
            if (!(isDiagonal(move) || isStraight(move))) {
                *errMessage = "Move is not diagonal or straight";
                return false;
            }
            if (isQueenPathBlocked(board, move)) {
                *errMessage = "Move is blocked";
                return false;
            }
            break;
        case 'k':
            if (abs(move.to.x - move.from.x) > 1 || abs(move.to.y - move.from.y) > 1) {
                *errMessage = "Move is to far";
                return false;
            }
            if (isKingNearby(board, move, color)) {
                *errMessage = "Opponents king is nearby";
                return false;
            }
            break;
        default:
            *errMessage = "Not a valid piece type.";
            return false;
    }




    return true;
}

// Checks if the king of the current player is in a position where it can be captured by the opponent in the next move.
// It checks the squares around the king for opponent's pieces that can capture the king.

bool isKingNearby(char board[][8], Move move, Color color) {
    Vector2D nearbySquares[8] = {
            { move.to.x - 1, move.to.y },
            { move.to.x + 1, move.to.y },
            { move.to.x, move.to.y - 1 },
            { move.to.x, move.to.y + 1 },
            { move.to.x - 1, move.to.y - 1 },
            { move.to.x - 1, move.to.y + 1 },
            { move.to.x + 1, move.to.y - 1 },
            { move.to.x + 1, move.to.y + 1 }
    };

    char kingPiece = color == Color::Black ? 'K' : 'k';

    for (int i = 0; i < 8; i++) {
        if (nearbySquares[i].x > -1 && nearbySquares[i].x < 8 && nearbySquares[i].y > -1 && nearbySquares[i].y < 8) {
            cout << board[nearbySquares[i].y - 1][nearbySquares[i].x - 1];
            if (board[nearbySquares[i].y - 1][nearbySquares[i].x - 1] == kingPiece) {
                return true;
            }
        }
    }
    return false;
}

// Checks if a move results in a capture of an opponent's piece.
// It checks if there is a piece at the 'to' position of the move.

bool isBeat(char board[][8], Move move) {
    if (board[move.to.y - 1][move.to.x - 1] != ' ') {
        return true;
    }
    return false;
}

// Validates a capture move.
// It checks if the piece at the 'to' position belongs to the opponent.
// If the piece belongs to the current player, it sets an error message.

bool isValidBeat(char board[][8], Move move, Color color, string *errMessage) {
    if (ownsPiece(board, move.to, color)) {
        *errMessage = "You can't beat your own piece";
        return false;
    }
    else {
        return true;
    }
}

// Checks if there is a piece on a given square of the board.

bool isPieceOnSquare(char board[][8], Vector2D square) {
    if (board[square.y - 1][square.x - 1] != ' ') {
        return true;
    }
    return false;
}

// Checks if a move is straight, i.e., in a horizontal or vertical direction.

bool isStraight(Move move) {
    if (move.from.x == move.to.x) {
        return true;
    }
    if (move.from.y == move.to.y) {
        return true;
    }
    return false;
}

// Checks if a move is diagonal, i.e., the absolute difference between the x and y coordinates of the 'from' and 'to' positions is the same.

bool isDiagonal(Move move) {
    if (abs(move.to.x - move.from.x) == abs(move.to.y - move.from.y)){
        return true;
    }
    return false;
}

// Checks if a move is a valid diagonal move for a pawn.
// A pawn can move diagonally only when capturing an opponent's piece.

bool isDiagonalPawn(Move move, Color color) {
    if (abs(move.from.x - move.to.x) != 1) {
        return false;
    }
    if (color == Color::Black && move.from.y - move.to.y == -1) {
        return true;
    }
    if (color == Color::White && move.from.y - move.to.y == 1) {
        return true;
    }
    return false;
}

// Checks if a move is in an L shape, i.e., two squares in one direction and one square in a perpendicular direction.
// This is the valid move for a knight.

bool isLShape(Move move) {
    if (abs(move.from.x - move.to.x) == 2 && abs(move.from.y - move.to.y) == 1) {
        return true;
    }
    if (abs(move.from.x - move.to.x) == 1 && abs(move.from.y - move.to.y) == 2) {
        return true;
    }
    return false;
}

// Checks if a move is a valid move for a pawn.
// A pawn can move one square forward, or two squares forward from its initial position, or one square diagonally forward when capturing an opponent's piece.

bool isPawnMove(Move move, Color color) {
    if (move.from.x != move.to.x) {
        return false;
    }
    if (color == Color::Black && move.from.y == 2 && (move.from.y - move.to.y == -1 || move.from.y - move.to.y == -2)) {
        return true;
    }
    if (color == Color::White && move.from.y == 7 && (move.from.y - move.to.y == 1 || move.from.y - move.to.y == 2)) {
        return true;
    }
    if (color == Color::Black && move.from.y - move.to.y == -1) {
        return true;
    }
    if (color == Color::White && move.from.y - move.to.y == 1) {
        return true;
    }

    return false;
}

// Determines the 2D direction of a move, i.e., the direction in the x and y coordinates.
// It returns a Direction2D struct with the x and y directions.

Direction2D getDirection2D(Move move) {
    int dirX = move.to.x > move.from.x ? 1 : -1;
    int dirY = move.to.y > move.from.y ? 1 : -1;

    return { dirX, dirY };
}

// Determines the direction of a move.
// It returns a Direction enum value: Up, Down, Left, or Right.

Direction getDirection(Move move) {
    if (move.to.x > move.from.x && move.to.y == move.from.y) {
        return Direction::Right;
    }
    else if (move.to.x < move.from.x && move.to.y == move.from.y) {
        return Direction::Left;
    }
    else if (move.to.y > move.from.y && move.to.x == move.from.x) {
        return Direction::Down;
    }
    else if (move.to.y < move.from.y && move.to.x == move.from.x) {
        return Direction::Up;
    }
    else {
        return Direction::Undefined;
    }
}

// Checks if the path of a rook move is blocked by other pieces.
// A rook can move any number of squares along a rank or file, but cannot leap over other pieces.

bool isRookPathBlocked(char board[][8], Move move) {
    switch (getDirection(move))
    {
        case Direction::Right:
            for (int i = 1; i < (move.to.x - move.from.x); i++) {
                cout << i;
                if (isPieceOnSquare(board, { move.from.x + i, move.from.y })) {
                    return true;
                }
            }
            break;
        case Direction::Left:
            for (int i = 1; i < (move.from.x - move.to.x); i++) {
                cout << i;
                if (isPieceOnSquare(board, { move.from.x - i, move.from.y })) {
                    return true;
                }
            }
            break;
        case Direction::Down:
            for (int i = 1; i < (move.to.y - move.from.y); i++) {
                cout << i;
                if (isPieceOnSquare(board, { move.from.x, move.from.y + i })) {
                    return true;
                }
            }
            break;
        case Direction::Up:
            for (int i = 1; i < (move.from.y - move.to.y); i++) {
                cout << i;
                if (isPieceOnSquare(board, { move.from.x, move.from.y - i })) {
                    return true;
                }
            }
            break;
        default:
            break;
    }

    return false;
}

// Checks if the path of a bishop move is blocked by other pieces.
// A bishop can move any number of squares diagonally, but cannot leap over other pieces.

bool isBishopPathBlocked(char board[][8], Move move) {
    Direction2D dir2d = getDirection2D(move);

    for (int i = 1; i < abs(move.to.x - move.from.x); i++) {
        if (isPieceOnSquare(board, { move.from.x + i * dir2d.x, move.from.y + i * dir2d.y })) {
            return true;
        }
    }
    return false;
}

// Checks if the path of a queen move is blocked by other pieces.
// A queen can move any number of squares along a rank, file, or diagonal, but cannot leap over other pieces.

bool isQueenPathBlocked(char board[][8], Move move) {
    if (isStraight(move)) {
        return isRookPathBlocked(board, move);
    }
    else {
        return isBishopPathBlocked(board, move);
    }
}

// Checks if a save file exists.
// It tries to open a file named "chesssave.txt" and returns true if the file exists.

bool saveFileExists() {
    ifstream file("chesssave.txt");
    return file.good();
}

// Saves the current state of the game to a file.
// It saves the game board, the pieces taken by each player, and the current player's color.

void saveGame(char board[][8], char takenPiecesBlack[16], char takenPiecesWhite[16], Color color) {
    fstream file;
    file.open("chesssave.txt", ios::out | ios::trunc);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            file << board[i][j];
        }
    }
    file << endl;

    file << takenPiecesBlack << endl;
    file << takenPiecesWhite << endl;

    file << int(color);

    file.close();
}

// Asks the user if they want to save the game.
// If the user enters 'y', it calls the saveGame function to save the game.

void askSaveGame(char board[][8], char takenPiecesBlack[16], char beatenPiecesWhite[16], Color color) {
    while (true) {
        system("cls");
        string input;
        cout << "Do you want to save the game? (y/n): ";
        cin >> input;

        if (input == "y") {
            saveGame(board, takenPiecesBlack, beatenPiecesWhite, color);
            break;
        }
        else if (input == "n") {
            break;
        }
    }
}

// Loads a saved game from a file.
// It loads the game board, the pieces taken by each player, and the current player's color from the file.

void loadGame(char board[][8], char takenPiecesBlack[16], char takenPiecesWhite[16], Color *color) {
    fstream file;
    file.open("chesssave.txt", ios::in);

    if (!file) {
        return;
    }

    string boardLine, takenPiecesBlackLine, takenPiecesWhiteLine, colorLine;
    Vector2D pos = { 0, 0 };
    while (getline(file, boardLine) &&
           getline(file, takenPiecesBlackLine) &&
           getline(file, takenPiecesWhiteLine) &&
           getline(file, colorLine)) {
        for (char temp : boardLine) {
            cout << temp;
            board[pos.y][pos.x] = temp;
            pos.x++;
            if (pos.x >= 8) {
                pos.y++;
                pos.x = 0;
            }
        }
        for (int i = 0; i < takenPiecesBlackLine.length(); i++) {
            takenPiecesBlack[i] = takenPiecesBlackLine[i];
        }
        for (int i = 0; i < takenPiecesWhiteLine.length(); i++) {
            takenPiecesWhite[i] = takenPiecesWhiteLine[i];
        }
        *color = (Color)stoi(colorLine);
    }

    file.close();
}

// Asks the user if they want to load a saved game.
// If the user enters 'y', it calls the loadGame function to load the game.

void askLoadGame(char board[][8], char takenPiecesBlack[16], char takenPiecesWhite[16], Color* color) {
    if (!saveFileExists()) return;

    while (true) {
        system("cls");
        string input;
        cout << "Do you want to load the last game? (y/n): ";
        cin >> input;

        if (input == "y") {
            loadGame(board, takenPiecesBlack, takenPiecesWhite, color);
            break;
        }
        else if (input == "n") {
            break;
        }
    }
}






