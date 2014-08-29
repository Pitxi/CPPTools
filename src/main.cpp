/**
 * @file
 * Entry point for the testing program used to test the CPPTools library.
 * @author Javier Campón Pichardo
 * @date 2014/07/30
 * @copyright 2014 Javier Campón Pichardo.
 *
 * Distributeed under the terms of the GNU General Public License version 3 or
 * later.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 */

#include "htmltools.hpp"
#include <iostream>

/**
 * Main function.
 * @param argc Number of arguments passed to the program from the command line.
 * @param argv Arguments passed to the program from the command line.
 * @return
 */
int main(int argc, char** argv)
{
    std::locale::global(std::locale(""));

    std::cout << Tools::html_entities_decode("Brøderbund&nbsp;Software,&nbsp;Inc.") << std::endl;

    return 0;
}
