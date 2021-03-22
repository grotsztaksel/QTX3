# -*- coding: utf-8 -*-
"""
Created on 14.03.2021 23:18 03
 
@author: Piotr Gradkowski <grotsztaksel@o2.pl>
"""

__date__ = '2021-03-14'
__authors__ = ["Piotr Gradkowski <grotsztaksel@o2.pl>"]

import unittest
import os
import shutil
from generate_qtx3_h import *


class TestGenerateQtx3(unittest.TestCase):
    def setUp(self):
        self.test_dir = os.path.join(os.getcwd(), "test_files")
        if not os.path.isdir(self.test_dir):
            os.makedirs(self.test_dir)

    def tearDown(self):
        """Delete all created files"""
        shutil.rmtree(self.test_dir)

    def test_getIncludeGuard(self):
        self.assertEqual("MYGUARD_H", getIncludeGuard(
            os.path.join("root", "dir", "subdir", "project", "src", "include", "myguard.h")))

    def test_removeGuards(self):
        content = "#ifndef HEADERMATCH_H\n" \
                  "#define HEADERMATCH_H\n" \
                  "some nice content\n" \
                  "even more content\n" \
                  "#endif //  HEADERMATCH_H\n"

        hfile_match = os.path.join(self.test_dir, "headermatch.h")
        with open(hfile_match, 'w') as h:
            h.write(content)
        hfile_nomatch = os.path.join(self.test_dir, "headernomatch.h")
        with open(hfile_nomatch, 'w') as h:
            h.write(content)
        self.assertEqual("some nice content\n"
                         "even more content\n", removeGuards(hfile_match))
        self.assertEqual(content, removeGuards(hfile_nomatch))


if __name__ == '__main__':
    unittest.main()
