/* UserInterface.cs
 * Authors: Josh Weese and Rod Howell
 * Modified By: Darius McAdam
 */
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
// Add directive for pausing
using System.Threading;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// A GUI for a checkers-playing program.
    /// </summary>
    public partial class UserInterface : Form
    {
        /// <summary>
        /// Object to control new game dialog value
        /// </summary>
        private NewGameDialog _newGameDialog = new NewGameDialog();

        /// <summary>
        /// The size of an individual board square in pixels.
        /// </summary>
        private const int _squareSize = 60;

        /// <summary>
        /// The current game.
        /// </summary>
        private Game _game = new Game();

        /// <summary>
        /// Image for the white pawn checker piece
        /// </summary>
        private Image _whitePawn = Image.FromFile(@"..\..\pics\white-pawn-modified.png");

        /// <summary>
        /// Image for the white king checker piece
        /// </summary>
        private Image _whiteKing = Image.FromFile(@"..\..\pics\white-king-modified.png");

        /// <summary>
        /// Image for the black pawn checker piece
        /// </summary>
        private Image _blackPawn = Image.FromFile(@"..\..\pics\black-pawn-modified.png");

        /// <summary>
        /// Image for the black king checker piece
        /// </summary>
        private Image _blackKing = Image.FromFile(@"..\..\pics\black-king-modified.png");

        /// <summary>
        /// Initializes the checkers game
        /// </summary>
        public UserInterface()
        {
            InitializeComponent();
            DrawBoard();
        }

        /// <summary>
        /// Draws the initial game board.
        /// </summary>
        private void DrawBoard()
        {
            //setup the panel
            uxBoard.Controls.Clear();
            uxBoard.Width = _squareSize * 8;
            uxBoard.Height = uxBoard.Width;
            Padding p = new Padding();
            p.Left = p.Right = 0;
            for (int y = 0; y < 8; y++)
            {
                for (int x = 0; x < 8; x++)
                {
                    Label square = new Label();
                    square.Width = square.Height = _squareSize;
                    square.Margin = p;
                    square.Text = x + "," + y;
                    square.Click += new EventHandler(BoardSquare_Click);
                    uxBoard.Controls.Add(square);
                }
            }
            RedrawBoard();
        }

        /// <summary>
        /// Gets the checker image to display in the square at the given location,
        /// or null if the square is to be empty.
        /// </summary>
        /// <param name="loc">The location of the square on the board.</param>
        /// <returns>The image to display in that square, or null if the square is to
        /// be empty.</returns>
        private Image GetImage(Point loc)
        {
            switch (_game.GetContents(loc))
            {
                case SquareContents.BlackKing:
                    return _blackKing;
                case SquareContents.BlackPawn:
                    return _blackPawn;
                case SquareContents.WhiteKing:
                    return _whiteKing;
                case SquareContents.WhitePawn:
                    return _whitePawn;
                default:
                    return null;
            }
        }

        /// <summary>
        /// Updates the GUI to match the current game position.
        /// </summary>
        private void RedrawBoard()
        {
            foreach (Label square in uxBoard.Controls)
            {
                Point loc = GetPoint(square.Text);
                square.Image = GetImage(loc);
                if (_game.IsSelected(loc))
                {
                    square.BackColor = Color.Gold;
                }
                else if ((loc.X + loc.Y) % 2 == 0)
                {
                    square.BackColor = Color.Beige;
                }
                else
                {
                    square.BackColor = Color.OliveDrab;
                }
                if (_game.IsOver)
                {
                    uxStatus.Text = "Game over.";
                }
                else
                {
                    uxStatus.Text = _game.CurrentName + "'s turn.";
                }
            }
            // Pause half a second after drawing to slow things down in case the computer is double jumping
            // Redraw now
            Refresh();
            // Pause in ms
            Thread.Sleep(500);
        }

        /// <summary>
        /// Click event handler for when a board square is clicked.  
        /// It will attempt to make a move using the selected square through the game object.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BoardSquare_Click(object sender, EventArgs e)
        {
            if (!_game.IsOver)
            {
                Point loc = GetPoint(((Label)sender).Text);
                if (_game.SelectOrMove(loc))
                {
                    RedrawBoard();
                    // Try to play the computers turn for every turn except the starting turn
                    TryComputerPlay();
                    CheckWin();
                }
                else
                {
                    MessageBox.Show("Invalid move.");
                }
            }
        }

        /// <summary>
        /// If the game is over, displays the appropriate message.
        /// </summary>
        private void CheckWin()
        {
            if (_game.IsOver)
            {
                MessageBox.Show(_game.OtherName + " wins!");
            }
        }

        /// <summary>
        /// Converts the name of a square to a Point.
        /// </summary>
        /// <param name="squareName">The name of a square (i.e., "x,y")</param>
        /// <returns>The equivalent Point</returns>
        private Point GetPoint(string squareName)
        {
            string[] fields = squareName.Split(',');
            int x = Convert.ToInt32(fields[0]);
            int y = Convert.ToInt32(fields[1]);
            return new Point(x, y);
        }

        /// <summary>
        /// Click event handler for the new game menu item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void uxNew_Click(object sender, EventArgs e)
        {
            // Start the new game with our new method
            NewGame();
        }

        /// <summary>
        /// Click event handler for the undo menu item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void uxUndo_Click(object sender, EventArgs e)
        {
            if (_game.Undo())
                RedrawBoard();
        }

        /// <summary>
        /// Play computer's turn
        /// </summary>
        private void TryComputerPlay()
        {
            // Keep doing this as long as the game isn't over and the name of _current player matches the compuer in new dialog object
            if (_game.IsOver == false)
            {
                // Is current name not updating right here??
                // It's from the game obj so it should be the same
                while (_game.CurrentName == _newGameDialog.ComputerPlayer)
                {
                    // Make the best move using the level in new dialog object
                    _game.MakeBestMove(_newGameDialog.Level);

                    // Redraw the board
                    RedrawBoard();
                }
            }
        }

        /// <summary>
        /// Starts a new game
        /// </summary>
        /// <returns>return true if the user started a new game</returns>
        private bool NewGame()
        {
            if (_newGameDialog.ShowDialog() == DialogResult.OK)
            {
                // Construct new game 
                _game = new Game();

                // Use show to make sure the checkers game is visible
                Show();

                // Redraw the board
                RedrawBoard();

                // If there is no computer enable the undo
                if (_newGameDialog.ComputerPlayer == "")
                {
                    uxUndo.Enabled = true;
                }
                // If we have a computer disable it
                else
                {
                    uxUndo.Enabled = false;
                }

                // Try to play the computer's turn for the starting turn
                TryComputerPlay();
                return true;    
            }

            // Return true if the user clicked OK
            return false;
        }

        /// <summary>
        /// Do this when the GUI loads in as the program is starting
        /// </summary>
        /// <param name="e"></param>
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            // After loading start the new game!
            // Close it if the user canceled
            if (NewGame() == false)
            {
                Application.Exit();
            }
        }
    }
}
