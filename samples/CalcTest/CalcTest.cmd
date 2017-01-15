@echo off
set test=CalcTest
sc delete %test% > nul
sc create %test% binPath= "%cd%\%test%.sys" type= kernel
sc start %test%
sc stop %test%
sc delete %test%