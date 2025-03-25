#!/bin/bash
libs= user32.lib gdi32.lib
cl /EHsc /Ox /nologo /std:c++latest main.cpp /Fertweekend_cl /link$libs