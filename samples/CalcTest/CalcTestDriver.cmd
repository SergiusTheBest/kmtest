@echo off
set test=CalcTestDriver
sc delete %test% > nul
sc create %test% binPath= "%cd%\%test%.sys" type= kernel
sc start %test%
sc stop %test%
sc delete %test%