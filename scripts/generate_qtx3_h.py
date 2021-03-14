# -*- coding: utf-8 -*-
"""
Created on 14.03.2021 22:03 03
 
@author: Piotr Gradkowski <grotsztaksel@o2.pl>


@brief Script collecting all header files relevant to external use of the library and put them in a single header file

"""

import argparse
import os

__date__ = '2021-03-14'
__authors__ = ["Piotr Gradkowski <grotsztaksel@o2.pl>"]


def main():
    parser = argparse.ArgumentParser(
        "Collect selected header files relevant to external use of the library and put them in a single header file")

    parser.add_argument('i', help='List of semicolon-separated header file names to be merged')
    parser.add_argument('-o', '-output', default="qtx3.h", help='Output header file name')

    args = parser.parse_args()
    writer = HeaderWriter(args.o)
    for file in args.i.split(";"):
        writer.scan(file)


class HeaderWriter():
    def __init__(self, output: str):
        self.output = output
        self.header_guard = os.path.split(output)[1].replace('.', '_').upper()
        self.h = open(output, "w")
        self.h.write("#ifndef " + self.header_quard)
        self.h.write("#define " + self.header_quard)

        self.includes = dict()

    def __del__(self):
        self.h.write("#endif  // " + self.header_guard)
        self.h.close()

    def scan(self, file):
        """Scan the input header file. Collect the following information:
        - used includes (to sort and remove duplicates)
        - used namespaces (to declare each namespace only once)
        - defined classes (to write forward declarations)
        """
        with open(file, "r") as h:
            header_original = h.read()

        header_working = self.preprocess(header_original)

    @staticmethod
    def preprocess(input):
        """Modify input file for simpler processing:
        - remove header quards
        - remove comments
        - remove empty lines
        """


if __name__ == '__main__':
    main()
