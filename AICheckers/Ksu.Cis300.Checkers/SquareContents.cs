/* SquareContents.cs
 * Authors: Josh Weese and Rod Howell
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// The possible contents of a board square.
    /// </summary>
    public enum SquareContents
    {
        /// <summary>
        /// A white pawn.
        /// </summary>
        WhitePawn,

        /// <summary>
        /// A white king.
        /// </summary>
        WhiteKing,

        /// <summary>
        /// A black pawn.
        /// </summary>
        BlackPawn,

        /// <summary>
        /// A black king.
        /// </summary>
        BlackKing,

        /// <summary>
        /// An empty square on the board.
        /// </summary>
        None,

        /// <summary>
        /// Indicates the square is not a valid square on the board.
        /// </summary>
        Invalid
    }

}
