///////////////////////////////////////////////////////////////////////////////
// PROGRAM 3 - RAID
// Title:
// Files:
// Semester: CS537 Spring 2013
//
// PAIR PROGRAMMERS
//
// Pair Partner: (Ted) Tianchu Huang thuang33@wisc.edu
// CS Login: Tianchu
// Lecturer's Name: Michael Swift
//
// Pair Partner: Tyson Williams tjwilliams4@wisc.edu
// CS Login: twilliam
// Lecturer's Name: Michael Swift
//
// Pair Partner: Adam Thorson adthorson@wisc.edu
// CS Login: thorson
// Lecturer's Name: Michael Swift
//
//////////////////////////// 80 columns wide //////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "disk.h"
#include "disk-array.h"


int value = 0;
int lba = 0;
int commandSize = 0;
int commandDisk = 0;

char* error_msg = "Error\n";
int counter     = 0;
int level       = -1;
int strip       = -1;
int disks       = -1;
int size        = -1;
char* trace     = NULL;
int verbose     = 0;
int flag       = 0;
int exit_flag   = 0;
char str[100];
disk_array_t my_disk_array = NULL;
FILE * trace_file = NULL;



/*
 * do RAID 0
 *
 *@param
 *
 */
void doRaid0() {
    int counter;
    
    while(fgets(str, 100, trace_file) != NULL){//for each line
        //parse and detect what command we have
        //for this purpose, "line" is the string on the line from the trace file
        char *command = NULL;
		command = (char*) malloc(512);
		if(str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0'; //remove newline char
        
        command = strtok(str," "); //split string on space delimiter into tokens
        counter = 0;
        while(command != NULL){
            ++counter;
            command = strtok(NULL," ");
        }
        
        if(counter > 4){
            //debugging
            printf("%s\n","Command word counter error");
            
            write(STDERR_FILENO, error_msg, strlen(error_msg));
			exit(-1);
        }
        
        char* commandLine[counter];
        
        command = strtok(str," "); //split string on space delimiter into tokens
        
		int i = 0;
		while( command != NULL ) {
            if (*command != '\0') {
				commandLine[i] = malloc(7*sizeof(char));
                commandLine[i] = command;
            }
			i++;
            command = strtok( NULL, " " );
        }
        
        if(strcmp("READ", commandLine[0]) == 0){
			//debugging
            printf("READ:\t%s\n",commandLine[0]);
		}
		else if(strcmp("WRITE", commandLine[0]) == 0){
						char *data = commandLine[3];
			int j;
			int diskNumber = (commandLine[1]/strip)%disk;
			int blockNumber = ;
			for(j = 0; j < commandLine[1] /*size*/; j++){
				disk_array_write( my_disk_array, diskNumber, /*block*/, data );
				diskNumber++;
			}
            printf("WRITE:\t%s\n",commandLine[0]);
		}
		else if(strcmp("FAIL", commandLine[0]) == 0){
			//debugging
            printf("FAIL:\t%s\n",commandLine[0]);
		}
		else if(strcmp("RECOVER", commandLine[0]) == 0){
			//debugging
            printf("RECOVER:\t%s\n",commandLine[0]);
		}
        else if(strcmp("END", commandLine[0]) == 0){ // END
			//debugging
            printf("END:\t%s\n",commandLine[0]);
            break;
        }
		else{
            //debugging
            printf("%s\n","Command error");
            
			write(STDERR_FILENO, error_msg, strlen(error_msg));
			exit(-1);
		}
        free(command);
    }
    
    
}

/*
 * do RAID 5
 *
 *@param
 *
 */
void doRaid5() {
    
    
    
    
}

/*
 * do RAID 4
 *
 *@param
 *
 */
void doRaid4() {
    
    
    
    
}

/*
 * do RAID 10
 *
 *@param
 *
 */
void doRaid10() {
    
    
    
}



/*
 * Uses a switch to decide between different RAID systems
 *
 *@param choice :must be a single int passed in
 *
 */
void chooseSystem(int choice) {
    
	switch (choice) {
		case 0: doRaid0(); break;
		case 5: doRaid5(); break;
		case 4: doRaid4(); break;
		default: doRaid10(); break; //case 10
	}
    
}





int main(int argc, char * argv[]) {
    
    
    
	//has appropiate amount of arguments?
	if ((argc != 11) && (argc != 13)) {
        //debugging
		printf("%s\n","Arg Error");
        
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		exit(-1);
	}
    
	//set
	for (counter = 0; counter < (argc - 1)/2; ++counter) {
		if (strcmp("-level", argv[2*counter+1]) == 0){
			level = atoi(argv[2*counter+2]);
            
		} else if(strcmp("-strip", argv[2*counter+1]) == 0) {
			strip = atoi(argv[2*counter+2]);
            
		} else if(strcmp("-disks", argv[2*counter+1]) == 0) {
			disks = atoi(argv[2*counter+2]);
            
		} else if(strcmp("-size", argv[2*counter+1]) == 0) {
			size = atoi(argv[2*counter+2]);
            
		} else if(strcmp("-trace", argv[2*counter+1]) == 0) {
			trace = argv[2*counter+2];
            
		} else if(strcmp("-verbose", argv[2*counter+1]) == 0) {
			verbose = atoi(argv[2*counter+2]);
		}
	}
    
	//CHECK ARGUMENT VALUES
	//valid level of RAID?
	if ((level != 0) && (level != 10) && (level != 4) && (level != 5)) {
		flag = 1;
		//debugging
		printf("%s\n","Invalid or Not Provided Level");
	}
    
	/*******************************
     COMPLETE
     CHECK
     ARGUMENT
     VALUES
     *******************************/
    
	//exit if flag was raised
	if (flag == 1) {
        //debugging
		printf("%s\n","Flag error");
        
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		exit(-1);
	}
    
    //open file
    if( (trace_file = fopen(trace, "r")) == NULL){
        //debugging
		printf("%s\n","Unable to Open Trace File");
        
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		exit(-1);
    }
    
    
	//create disk array
	my_disk_array = disk_array_create("MyVirtualDiskArray", disks, size);
	if (!my_disk_array) {
		//debugging
		printf("%s\n","Unable to Create Virtual Disks");
        
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		exit(-1);
	}
    
    chooseSystem(level);
    
    fclose(trace_file);
    
    return 0;
}
