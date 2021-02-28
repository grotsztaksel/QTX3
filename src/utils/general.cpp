#include "general.h"
template <typename Out>
/* by Evan Teran https://stackoverflow.com/users/13430/evan-teran
   copied from
   https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
*/
void split(const std::string& s, char delim, Out result) {
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim)) {
    *result++ = item;
  }
}
std::vector<std::string> split(const std::string& s, char delim) {
  /* by Evan Teran https://stackoverflow.com/users/13430/evan-teran
     copied from
     https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
  */
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}
