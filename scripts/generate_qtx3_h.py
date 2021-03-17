# -*- coding: utf-8 -*-
"""
Created on 14.03.2021 22:03 03
 
@author: Piotr Gradkowski <grotsztaksel@o2.pl>


@brief Script collecting all header files relevant to external use of the library and put them in a single header file

"""

import argparse
import os
import re

__date__ = '2021-03-14'
__authors__ = ["Piotr Gradkowski <grotsztaksel@o2.pl>"]

c = r"(\/\*.*?\*\/|\/\/.*?\n)"  # Regex part responsible for catching comments


def main():
    parser = argparse.ArgumentParser(
        "Collect selected header files relevant to external use of the library and put them in a single header file")

    parser.add_argument('-i', help='List of semicolon-separated header file names to be merged', required=True)
    parser.add_argument('-o', '-output', default="qtx3.h", help='Output header file name')

    args = parser.parse_args()

    out = ''

    for file in args.i.split(";"):
        out += removeGuards(file)

    out = removeFriends(out)

    out = collectIncludes(out)
    out = removePrivateParts(out)
    out = writeGuards(args.o, out)
    with open(args.o, 'w') as o:
        o.write(out)


def getIncludeGuard(filename: str) -> str:
    """Convert the filename to a header guard (upper case, underscores)"""
    return os.path.basename(filename).replace('.', '_').upper()


def removeGuards(filename: str) -> str:
    """Return the file content stripped from include guards"""
    with open(filename, 'r') as h:
        # Maybe pointless to split and join over and over, but oh well...
        content_as_list = trimEmptyLines(h.read()).split("\n")

    if re.match(r'^\s*#ifndef\s+' + getIncludeGuard(filename), content_as_list[0]):
        content_as_list.pop(0)
    if re.match(r'^\s*#define\s+' + getIncludeGuard(filename), content_as_list[0]):
        content_as_list.pop(0)
    if re.match(r'^\s*#endif\s+//\s+' + getIncludeGuard(filename), content_as_list[-1]):
        content_as_list.pop(-1)

    return "\n".join(content_as_list) + "\n"


def removeFriends(content: str) -> str:
    """Remove declarations of friend classes and functions/methods"""
    return re.sub(r'^\s*friend\s+.+?;\s*\n', "", content, flags=re.MULTILINE)


def writeGuards(filename: str, content: str) -> str:
    """Embrace the content with include guards derived from filename"""
    guard = getIncludeGuard(filename)
    return "#ifndef {}\n#define {}\n\n{}\n#endif  // {}".format(guard, guard, trimEmptyLines(content), guard)


def trimEmptyLines(content: str) -> str:
    """Remove empty lines from front and back"""
    content_as_list = content.split("\n")

    while not (content_as_list[0].strip()):
        content_as_list.pop(0)
    while not (content_as_list[-1].strip()):
        content_as_list.pop(-1)
    return "\n".join(content_as_list)


def removePrivateParts(content: str) -> str:
    """Remove whatever appears in the private declarations"""
    regex = re.compile(r'^ *private(\s+(signals|slots)\s*)?:.+?(^ *(public|protected|};))',
                       flags=re.MULTILINE | re.DOTALL)

    return regex.sub(r"\3", content)


def collectIncludes(h: str) -> str:
    """In the input string, content of the C++ header file,
    - find all #include statements
    - remove duplicate ones
    - sort alphabetically
    - move them to the top of the file """

    regex = re.compile(r'#include\s+(["<]\S+[">])\s*\n')
    includes = sorted(list(dict.fromkeys(regex.findall(h))))

    out = regex.sub("", h).split("\n")
    out.insert(1, "")
    for inc in reversed(sorted(includes)):
        out.insert(1, "#include {}".format(inc))

    return "\n".join(out)


if __name__ == '__main__':
    main()
