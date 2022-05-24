/* Player.cs
 * Author: Rod Howell
 * Modified by: Darius McAdam
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// Contains information about one of the players.
    /// </summary>
    public class Player
    {
        // ---------------------------- Add fields ------------------------------ //

        // Represents y coordinate of the players back row
        // Directions say to modify the constructor to initialize the field
        private int _backRow;

        /// <summary>
        /// Holds the players pieces, key gives position of a piece, and bool tells us if the piece is a king
        /// </summary>
        public Dictionary<Point, bool> Pieces { get; set; } 

        /// <summary>
        /// Gets the players score as described in Negamax Algorithm
        /// </summary>
        public int PositionValue
        {
            get
            // Code to calculate the score
            {
                // Add up all of the score in pieces for the player
                int score = 0;
                foreach (KeyValuePair<Point, bool> p in Pieces)
                {
                    // King scoring
                    if (p.Value)
                    {
                        score = score + 195;
                        if((p.Key.X + p.Key.Y % 12) == 1 || p.Key.X*p.Key.Y == 12)
                        {
                            score = score + 4;
                        }
                    }
                    // Pawn scoring
                    else
                    {
                        score = score + 80 + (_backRow - p.Key.Y) * (_backRow - p.Key.Y);
                    }
                }
                return score;
            }
        }

        /// <summary>
        /// The directions a pawn can move. This will be a 2-element
        /// array, both of whose coordinates each have an absolute value
        /// of 1. The y-coordinate will be the same in both, and will
        /// depend on which player is being represented.
        /// </summary>
        public Point[] PawnDirections { get; }

        /// <summary>
        /// The y-value at which a pawn becomes a king, either 0 or 7 depending
        /// on which player is being represented.
        /// </summary>
        public int KingAt { get; }

        /// <summary>
        /// The name of this player.
        /// </summary>
        public string Name { get; }

        /// <summary>
        /// Concstructs a player who moves in the given direction (1 for White,
        /// -1 for Black) and has the given name ("White" or "Black").
        /// </summary>
        /// <param name="dir">The direction the player normally moves (1 for White,
        /// -1 for Black).</param>
        /// <param name="name">The name of this player ("White" or "Black").</param>
        /// <param name="backRow">Int representing the y coordiante of the player's back row.</param>
        public Player(int dir, string name, int backRow)
        {
            PawnDirections = new Point[] { new Point(-1, dir), new Point(1, dir) };
            if (dir == -1)
            {
                KingAt = 0;
            }
            else
            {
                KingAt = 7;
            }
            Name = name;
            // Initialize back row
            _backRow = backRow;
        }
    }
}
