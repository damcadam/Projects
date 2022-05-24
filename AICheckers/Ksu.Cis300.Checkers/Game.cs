/*
 * Game.cs
 * Authors: Josh Weese and Rod Howell
 * Modified By: Darius McAdam
 */
using System;
using System.Collections.Generic;
using System.Drawing;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// This class contains the logic for the game of Checkers
    /// </summary>
    public class Game
    {
        /// <summary>
        /// Gets whether there is a selected square.
        /// </summary>
        private bool _squareIsSelected;

        /// <summary>
        /// The currently-selected square. 
        /// </summary>
        private Point _selectedSquare;

        // Note: white's back row starts at y coordinate 0 and black's back row starts at 7

        /// <summary>
        /// The player moving the white pieces.
        /// </summary>
        private Player _whitePlayer = new Player(1, "White", 0);

        /// <summary>
        /// The player moving the black pieces.
        /// </summary>
        private Player _blackPlayer = new Player(-1, "Black", 7);

        /// <summary>
        /// The player whose turn it currently is.
        /// </summary>
        private Player _currentPlayer;

        /// <summary>
        /// The player whose turn it is not.
        /// </summary>
        private Player _otherPlayer;

        /// <summary>
        /// The moves that are currently legal.
        /// </summary>
        private Stack<Move> _legalMoves = new Stack<Move>();

        /// <summary>
        /// The directions a king can move.
        /// </summary>
        private Point[] _allDirections = { new Point(-1, -1), new Point(-1, 1), new Point(1, -1), new Point(1, 1) };

        /// <summary>
        /// The history of moves.
        /// </summary>
        private Stack<Move> _moveHistory = new Stack<Move>();

        /// <summary>
        /// base value of a loss 
        /// </summary>
        private const int _loss = -200;

        /// <summary>
        /// Used for generating random numbers, seed set to 0
        /// </summary>
        private Random _random = new Random(0);

        /// <summary>
        /// Whose turn it is.
        /// </summary>
        public string CurrentName => _currentPlayer.Name;

        /// <summary>
        /// Whose turn it is not.
        /// </summary>
        public string OtherName => _otherPlayer.Name;

        /// <summary>
        /// Gets whether the game is over.
        /// </summary>
        public bool IsOver => _legalMoves.Count == 0;

        /// <summary>
        /// Gets the contents of the given square.
        /// </summary>
        /// <param name="square">The square to check.</param>
        /// <returns>The contents of square.</returns>
        public SquareContents GetContents(Point square)
        {
            if (square.X < 0 || square.X >= 8 || square.Y < 0 || square.Y >= 8)
            {
                return SquareContents.Invalid;
            }

            // Determine who has the checker and create an object holding the right checker because we removed _board

            // Check if the white player has the checker
            if (_whitePlayer.Pieces.ContainsKey(square))
            {
                // Check if it's a pawn or a king
                bool isWhiteKing = _whitePlayer.Pieces[square];
                if (isWhiteKing)
                {
                    return SquareContents.WhiteKing;
                }
                else
                {
                    return SquareContents.WhitePawn;
                }
            }
            // Check if the black player has the checker
            else if (_blackPlayer.Pieces.ContainsKey(square))
            {
                bool isBlackKing = _blackPlayer.Pieces[square];
                if (isBlackKing)
                {
                    return SquareContents.BlackKing;
                }
                else
                {
                    return SquareContents.BlackPawn;
                }
            }
            // Case where no one has the checker
            else
            {
                return SquareContents.None;
            }
        }

        /// <summary>
        /// Gets the valid directions for the current player assuming the
        /// given type of piece.
        /// </summary>
        /// <param name="isKing">Whether the piece to be moved is a king.</param>
        /// <returns>The valid directions the piece can move.</returns>
        private Point[] GetValidDirections(bool isKing)
        {
            if (isKing)
            {
                return _allDirections;
            }
            else
            {
                return _currentPlayer.PawnDirections;
            }
        }

        /// <summary>
        /// Adds to _legalMoves the non-jumps from the given location.
        /// </summary>
        /// <param name="loc">The location of the checker to move.</param>
        /// <param name="isCheckerKing">True is the checker at a location is a king</param>
        private void GetNonJumpsFrom(Point loc, bool isCheckerKing)
        {
            // Replace code that used _board by subbing new parameter for checker.isKing
            Point[] dirs = GetValidDirections(isCheckerKing);
            foreach (Point d in dirs)
            {
                Point target = new Point(loc.X + d.X, loc.Y + d.Y);
                if (GetContents(target) == SquareContents.None)
                {
                    _legalMoves.Push(new Move(loc, target, isCheckerKing, _currentPlayer, _otherPlayer, _legalMoves));
                }
            }
        }

        /// <summary>
        /// Checks whether a jump can be made from the given start square in the given
        /// direction. The only checks made are whether the first square contains an enemy
        /// piece and the second square is a valid empty square.
        /// </summary>
        /// <param name="startSquare">Where the player's piece starts</param>
        /// <param name="dir">The direction the player's piece is moving</param>
        /// <param name="isKingCaptured">out parameter that is true if a king is captured</param>
        /// <returns>Whether or not the jump can be made</returns>
        private bool CanJump(Point startSquare, Point dir, out bool isKingCaptured)
        {
            if (GetContents(new Point(startSquare.X + 2 * dir.X, startSquare.Y + 2 * dir.Y)) ==
                SquareContents.None)
            {
                Point enemySquare = new Point(startSquare.X + dir.X, startSquare.Y + dir.Y);

                // Check if the next square contains an enemy checker
                if (_otherPlayer.Pieces.ContainsKey(enemySquare))
                {
                    // Set the out parameter depending if the enemey square is a king
                    isKingCaptured = _otherPlayer.Pieces[enemySquare];
                    // If it's an enemy we can jump!
                    return true;
                }
            }
            // No capture happens so our out parameter is false
            isKingCaptured = false;
            return false;
        }

        /// <summary>
        /// Adds to _legalMoves the jump moves from the given location.
        /// </summary>
        /// <param name="loc">The location of the piece making the move.</param>
        /// <param name="isCheckerKing">True if the checker at a location is a king.</param>
        private void GetJumpsFrom(Point loc, bool isCheckerKing)
        {
            Point[] dirs = GetValidDirections(isCheckerKing);
            foreach (Point d in dirs)
            {
                // Create bool for the new out parameter
                bool isKingCaptured = false;
                if (CanJump(loc, d, out isKingCaptured))
                {
                    Point capLoc = new Point(loc.X + d.X, loc.Y + d.Y);
                    //Checker cap = _board[capLoc.X, capLoc.Y];
                    _legalMoves.Push(new Move(loc, new Point(loc.X + 2 * d.X, loc.Y + 2 * d.Y), isCheckerKing, _currentPlayer,
                        _otherPlayer, _legalMoves, isKingCaptured, capLoc));
                }
            }
        }

        /// <summary>
        /// Adds to _legalMoves the moves of the given type from all of the current player's pieces.
        /// <param name="jumps">Indicates whether jump moves are to be found.</param>
        /// </summary>
        private void GetMoves(bool jumps)
        {
            foreach (KeyValuePair<Point, bool> p in _currentPlayer.Pieces)
            {
                // Do a lookup for each point and check if it's the current player
                if (jumps)
                {
                    GetJumpsFrom(p.Key, p.Value);
                }
                else
                {
                    GetNonJumpsFrom(p.Key, p.Value);
                }
            }
        }

        /// <summary>
        /// Constructor for initializing the game
        /// </summary>
        public Game()
        {
            // Intizalize the dictionaries at the start of the game for the white player and the black player
            _blackPlayer.Pieces = new Dictionary<Point, bool>();
            _whitePlayer.Pieces = new Dictionary<Point, bool>();

            // Start the game as black (these only affect who goes first)
            _currentPlayer = _blackPlayer;
            _otherPlayer = _whitePlayer;

            // Place all of the pieces on the board
            for (int y = 0; y < 8; y++)
            {
                for (int x = 0; x < 8; x++)
                {
                    if ((x + y) % 2 == 1)
                    {
                        if (y < 3)
                        {
                            // Add pieces to the dictionary instead of the board
                            // No checkers start as kings, so the second parameter is false
                            _whitePlayer.Pieces.Add(new Point(x, y), false);
                        }
                        else if (y > 4)
                        {
                            _blackPlayer.Pieces.Add(new Point(x, y), false);
                        }
                    }
                }
            }
            GetMoves(false); // No jumps available for the first move
        }

        /// <summary>
        /// Ends the current player's turn.
        /// </summary>
        private void EndTurn()
        {
            Player temp = _currentPlayer;
            _currentPlayer = _otherPlayer;
            _otherPlayer = temp;
            _legalMoves = new Stack<Move>();
            GetMoves(true);
            if (_legalMoves.Count == 0)
            {
                GetMoves(false);
            }
            _squareIsSelected = false;
        }

        /// <summary>
        /// Determines whether the given square is selected.
        /// </summary>
        /// <param name="square">The square to check.</param>
        /// <returns>Whether square is selected.</returns>
        public bool IsSelected(Point square)
        {
            return _squareIsSelected && _selectedSquare == square;
        }

        /// <summary>
        /// Determines whether the given square is the start square of a
        /// legal move.
        /// </summary>
        /// <param name="square">The square to check.</param>
        /// <returns>Whether square is the start square of a legal move.</returns>
        private bool IsLegalStart(Point square)
        {
            foreach (Move move in _legalMoves)
            {
                if (square == move.From)
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Tries to select the piece at the given location. If the location
        /// is not the start location of a legal move, the selection is
        /// unchanged.
        /// </summary>
        /// <param name="square">The square to select.</param>
        /// <returns>Whether the square was selected.</returns>
        private bool TrySelectSquare(Point square)
        {
            if (IsLegalStart(square))
            {
                _selectedSquare = square;
                _squareIsSelected = true;
                return true;
            }
            return false;
        }

        /// <summary>
        /// Tries to get a legal move.
        /// </summary>
        /// <param name="from">The start square of the move.</param>
        /// <param name="to">The destination square of the move.</param>
        /// <returns>The move described, or null if the move described is
        /// illegal.</returns>
        private Move TryGetMove(Point from, Point to)
        {
            foreach (Move move in _legalMoves)
            {
                if (from == move.From && to == move.To)
                {
                    return move;
                }
            }
            return null;
        }

        /// <summary>
        /// Plays the given move.
        /// </summary>
        /// <param name="move">The move to play.</param>
        private void DoMove(Move move)
        {
            _moveHistory.Push(move);

            // Get the starting and ending point of the move
            Point startingPoint = new Point(move.From.X, move.From.Y);
            Point endingPoint = new Point(move.To.X, move.To.Y);

            // Remove the checker at the old location from our dictionary
            _currentPlayer.Pieces.Remove(startingPoint);

            // Add a location in our dictionary for the new spot
            _currentPlayer.Pieces.Add(endingPoint, move.FromKing); 

            // Determine if the new position makes our checker a king
            // Make it a king if it's on the oppenents back row and is not a king yet
            int backRow;
            if (_otherPlayer.Name == "White")
            {
                backRow = 0;
            }
            else
            {
                backRow = 7;
            }

            if (move.FromKing == false && endingPoint.Y == backRow)
            {
                _currentPlayer.Pieces[endingPoint] = true;
            }

            // If we just jumped over a piece remove the captured checker from our dictionary
            if (move.IsJump)
            {
                // Want to remove for the other player at the point between the starting and ending
                _otherPlayer.Pieces.Remove(new Point((endingPoint.X - startingPoint.X)/2 + startingPoint.X, (endingPoint.Y - startingPoint.Y)/2 + startingPoint.Y));

                // Convert the rest of the code to use the dictionary, I'm not sure what everything does tho
                if (move.FromKing || !(_currentPlayer.Pieces[endingPoint]))
                {
                    _legalMoves = new Stack<Move>();
                    //
                    GetJumpsFrom(move.To, _currentPlayer.Pieces[endingPoint]);
                    if (_legalMoves.Count != 0)
                    {
                        TrySelectSquare(move.To);
                        return;
                    }
                }
            }
            // Still end turn normally
            EndTurn();
        }

        /// <summary>
        /// Tries to select the given square or move the selected piece to it.
        /// </summary>
        /// <param name="target">The target square</param>
        /// <returns>Whether either a selection or a move was successfully made.</returns>
        public bool SelectOrMove(Point target)
        {
            if (TrySelectSquare(target))
            {
                return true;
            }
            else if (_squareIsSelected)
            {
                Move move = TryGetMove(_selectedSquare, target);
                if (move == null)
                {
                    return false;
                }
                else
                {
                    DoMove(move);
                    return true;
                }
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Tries to undo the last move.
        /// </summary>
        /// <returns>Whether a move was undone.</returns>
        public bool Undo()
        {
            if (_moveHistory.Count > 0)
            {
                Move last = _moveHistory.Pop();
                _currentPlayer = last.MovingPlayer;
                _otherPlayer = last.OtherPlayer;
                _legalMoves = last.LegalMoves;

                // Remove the last move
                _currentPlayer.Pieces.Remove(new Point(last.To.X, last.To.Y));
                // Add in the old move
                _currentPlayer.Pieces.Add(new Point(last.From.X, last.From.Y), last.FromKing);

                // If we undo a jump, we want to restore the piece
                if (last.IsJump)
                {
                    _otherPlayer.Pieces.Add(new Point(last.CapturedLocation.X, last.CapturedLocation.Y), last.CapturedIsKing);
                }
                TrySelectSquare(last.From);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Randomly shuffles legal moves
        /// </summary>
        /// <returns>Move[] holding shuffled moves</returns>
        private Move[] ShuffleMoves()
        {
            // Copy stack of legal moves to Move[] using toArray
            Move[] moves = _legalMoves.ToArray();

            // Iterate thru array in reverse
            for (int i = moves.Length - 1; i >= 0; i--)
            {
                // Generate a non negative int EQUAL TO or less than the location
                int rand = _random.Next(i + 1);

                // swap the value of the current location with the random location generated
                Move tempMove = null;
                tempMove = moves[i];
                // old gets rand, rand gets old
                moves[i] = moves[rand];
                moves[rand] = tempMove;
            }
            return moves;
        }

        /// <summary>
        /// Recursivley determine what set of moves are best by generating a tree a certain number of moves ahead
        /// </summary>
        /// <param name="lowerBound">Lower bound on useful game values</param>
        /// <param name="upperBound">Upper bound on useful game values</param>
        /// <param name="lookAhead">Number of turns to look ahead</param>
        /// <param name="legalMoves">Interface of Stack move and Move[]. Gives legal moves</param>
        /// <param name="bestMove">Out parameter where the best move should be returned</param>
        /// <returns>Negamax value as a double representing the best move (or a loss)</returns>
        private double Negamax(double lowerBound, double upperBound, int lookAhead, IEnumerable<Move> legalMoves, out Move bestMove)
        {
            // --------------------------- "Implement algorithm in "Alpha-Beta Pruning" ------------------------------------- //

            // 1. intialize best move to null
            bestMove = null;

            // 2. If the position is at the end of the game, return a value for a loss (using k) and the best move
            if (IsOver)
            {
                // Return the value for a loss (factor in the lookahead to get insight into the AI behavior)
                return _loss - lookAhead;
            }

            // 3. if k == 0, return value computed by evaluation function and the best move
            if (lookAhead == 0)
            {
                // Eval function = current score - other score/ number of pieces on board
                return (_currentPlayer.PositionValue - _otherPlayer.PositionValue)/(double)(_currentPlayer.Pieces.Count + _otherPlayer.Pieces.Count);
            }

            // Get the player before the move was made
            Player playerBeforeMove = _currentPlayer;

            // 4. for each legal move
            foreach (Move move in legalMoves)
            {
                // Play the move
                DoMove(move);

                // Var to hold the adjusted value
                double adjustedValue = 0.0;

                // Pass an unused move because we havent found it yet
                Move unused = null;
                // If the current player is the same as before when the move is made
                if (_currentPlayer.Equals(playerBeforeMove))
                {
                    // Recursivley get a value for the resulting position
                    // Use aplha and beta for the lower bounds (this returns the adjusted value of a child
                    adjustedValue = Negamax(lowerBound, upperBound, lookAhead, _legalMoves, out unused);
                }
                // Otherwise get a value for the resulting position
                else
                {
                    // Change upper bound to -Beta and upperbound, to -alpha, and adjust k to k-1
                    adjustedValue = -Negamax( -upperBound, -lowerBound, lookAhead - 1, _legalMoves, out unused);
                }

                // Undo the last move
                Undo();

                // if aplha > Beta
                if (adjustedValue > lowerBound)
                {
                    // Update aplha to adjusted value (best option so far)
                    lowerBound = adjustedValue;
                    // Update the best move so far
                    bestMove = move;

                    // Return alpha and the best move by out value
                    if (lowerBound >= upperBound)
                    {
                        return lowerBound;
                    }
                }
            }

            // 5. return aplha and the best move (by out value)
            return lowerBound;
        }

        /// <summary>
        /// Finds the best move by looking ahead a certain number of turns
        /// </summary>
        /// <param name="lookAhead">number of turns to look forward</param>
        public void MakeBestMove(int lookAhead)
        {
            // use Negamax method to find the number of turns

            // If the game is over, or lookAhead is less than 1 throw exception
            if (lookAhead < 1 || IsOver)
            {
                throw new InvalidOperationException();
            }

            // If there is only 1 legal move use that as best move instead of calling Negamax
            if (_legalMoves.Count == 1)
            {
                DoMove(_legalMoves.Peek()); 
            }
            else
            {
                // Find the best move!
                Move bestMove;
                // Shuffle legal moves when using Negamax
                Negamax(double.NegativeInfinity, double.PositiveInfinity, lookAhead, ShuffleMoves(), out bestMove);

                // Once we have found the best move, use the right method to actually do the move (doMove)
                DoMove(bestMove);
            }
        }
    }
}
