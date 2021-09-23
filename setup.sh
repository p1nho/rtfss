#!/sbin/bash

#Get script dir
DIR=$(dirname "$0" )

#Create cmake dir
#mkdir $DIR/cmake

#Get ANTLR4 jar
ANTLR4_JAR='https://www.antlr.org/download/antlr-4.9.2-complete.jar'
wget $ANTLR4_JAR -O $DIR/cmake/antlr4.jar

#Get ANTLR4 runtime cmake scripts
#Fetch FindANTLR.cmake and ExternalAntlr4Cpp.cmake but for now they are already here.
