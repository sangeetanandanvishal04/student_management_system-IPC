#!/bin/bash

# Compile C programs
gcc -o Rec_Gen Record_Generator.c
gcc -o Server Server.c
gcc -o Start_Processor Start_Processor.c

# Run the leading Program
./Start_Processor

# Wait for the background processes to finish
wait

# Clean up compiled files (optional)
rm Rec_Gen Server Start_Processor