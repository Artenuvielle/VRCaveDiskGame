@echo off

for /d /r ".\proto\" %%a in (out\) do if exist "%%a" rmdir /s /q "%%a"
cd .\proto
mkdir .\out
for %%f in (.\*.proto) do (
	protoc --cpp_out=./out %%f
	move .\out\%%~nf.pb.h ..\includes\
	move .\out\%%~nf.pb.cc ..\sources\
	)
cd ..