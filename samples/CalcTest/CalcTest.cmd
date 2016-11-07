@echo off
sc delete CalcTest
sc create CalcTest binPath= "%cd%\CalcTest.sys" type= kernel
sc start CalcTest
sc stop CalcTest