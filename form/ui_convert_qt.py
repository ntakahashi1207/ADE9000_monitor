# -*- coding: utf-8 -*-
from PyQt5 import uic
 
fin = open('ADE9000_form.ui', 'r', encoding='utf8')
fout = open('ADE9000_form.py', 'w', encoding='utf8')
uic.compileUi(fin,fout,execute=False)
fin.close()
fout.close()
