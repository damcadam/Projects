/* Checker.cs
 * Author: Rod Howell
 * Modified by: Darius McAdam (I think, maybe not)
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// Instances each represent a single checker.
    /// </summary>
    public class Checker
    {
        /// <summary>
        /// Gets the owner of this checker.
        /// </summary>
        public Player Owner { get; }

        /// <summary>
        /// Gets or sets whether this checker is a king.
        /// </summary>
        public bool IsKing { get; set; }

        /// <summary>
        /// Constructs a checker owned by the given player.
        /// </summary>
        /// <param name="owner">The owner of this checker.</param>
        public Checker(Player owner)
        {
            Owner = owner;
        }
    }
}
