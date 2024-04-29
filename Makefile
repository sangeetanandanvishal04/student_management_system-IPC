all:Rec_Gen Server Start_Processor

Rec_Gen: Record_Generator.c
    gcc -o Rec_Gen Record_Generator.c

Server: Server.c
    gcc -o Server Server.c

OS_Project: Start_Processor.c
    gcc -o Start_Processor Start_Processor.c

clean:
    rm -f Rec_Gen Server Start_Processor