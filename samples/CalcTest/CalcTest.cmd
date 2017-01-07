@echo off
sc delete CalcTest > nul
sc create CalcTest binPath= "%cd%\CalcTest.sys" type= kernel
sc start CalcTest
sc stop CalcTest
sc delete CalcTest