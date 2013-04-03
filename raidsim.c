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

#define TRUE 1
#define FALSE 0

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
int * working_disks = NULL;


/*
 * do RAID 0
 *
 *@param
 *
 */
void doRaid0() {
    int counter;
    char *data;
    data = malloc(1024);
    
    while (fgets(str, 100, trace_file) != NULL) {//for each line
        //parse and detect what command we have
        //for this purpose, "line" is the string on the line from the trace file
        char *command = NULL;
		command = (char*) malloc(512);
		if(str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0'; //remove newline char
                
        char * commandLine[5];
        
        command = strtok(str, " "); //split string on space delimiter into tokens
        
		int i = 0;
		while( command != NULL ) {
            //commandLine[i] = malloc(8 * sizeof(char));
            //printf("%s\n",command);
            commandLine[i] = command;
			i++;
            command = strtok( NULL, " " );
        }
        int j;
        for (j=0; j<i; j++) {
            printf("%s ",commandLine[j]);
        }
        printf("\n");
        
        if (strcmp("READ", commandLine[0]) == 0){ //READ LBA SIZE
			int numberOfReads = atoi(commandLine[2]);
			int currentLBA = atoi(commandLine[1]);
			int j;
			int blockNumber; //starting LBA
			int diskNumber;
			int stripLayer;
			int blockOfStrip;
			int temp;
			
			for (j = 0; j < numberOfReads /*size*/; j++){ // number of blocks we have to write to
				temp = currentLBA/strip;
				stripLayer = temp/disks;
				blockOfStrip = currentLBA % strip;
				blockNumber = stripLayer*strip + blockOfStrip;
				diskNumber = temp%disks; //algorithm to calculate the disk we read from
                printf("READ disk: %d block: %d\n",diskNumber,blockNumber);
				disk_array_read( my_disk_array, diskNumber, blockNumber, data );
				
				printf("%s ", data);
			}
			//printf("\n");
		}
		
		else if(strcmp("WRITE", commandLine[0]) == 0){ //WRITE LBA SIZE VALUE
			char *data = commandLine[3];
			int numberOfWrites = atoi(commandLine[2]);
			int currentLBA = atoi(commandLine[1]);
			int j;
			int blockNumber; //starting LBA
			int diskNumber;
			int stripLayer;
			int blockOfStrip;
			int temp;
			
			for(j = 0; j < numberOfWrites /*size*/; j++){ // number of blocks we have to write to
				temp = currentLBA/strip;
				stripLayer = temp/disks;
				blockOfStrip = currentLBA % strip;
				blockNumber = stripLayer*strip + blockOfStrip;
				diskNumber = temp%disks; //algorithm to calculate the disk we write to
                printf("WRITE disk: %d block: %d\n",diskNumber,blockNumber);
				disk_array_write( my_disk_array, diskNumber, blockNumber, data );
			}
		}
		
		else if(strcmp("FAIL", commandLine[0]) == 0) { //FAIL DISK
            disk_array_fail_disk( my_disk_array, atoi(commandLine[1]));
		}
		
		else if(strcmp("RECOVER", commandLine[0]) == 0) { //RECOVER DISK
            disk_array_recover_disk( my_disk_array, atoi(commandLine[1]));
		}
		
        else if(strcmp("END", commandLine[0]) == 0){ // END
            break;
        }
		
		else{
			//debugging
			printf("%s\n","Trace File Error");
            
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

    int counter;
    char *data;
    data = malloc(1024);
    
    if ((disks % 2) == 1) {
        printf("Error: odd number of disks");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        exit(-1);
    }
    
    while (fgets(str, 100, trace_file) != NULL) {//for each line
        //parse and detect what command we have
        //for this purpose, "line" is the string on the line from the trace file
        char *command = NULL;
        command = (char*) malloc(512);
        if(str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = '\0'; //remove newline char
        
        char * commandLine[5];
        
        command = strtok(str, " "); //split string on space delimiter into tokens
        
        int i = 0;
        while( command != NULL ) {
            //commandLine[i] = malloc(8 * sizeof(char));
            //printf("%s\n",command);
            commandLine[i] = command;
            i++;
            command = strtok( NULL, " " );
        }
        int j;
        for (j=0; j<i; j++) {
            printf("%s ",commandLine[j]);
        }
        printf("\n");
        
        if (strcmp("READ", commandLine[0]) == 0){ //READ LBA SIZE
            int numberOfReads = atoi(commandLine[2]);
            int currentLBA = atoi(commandLine[1]);
            int j;
            int blockNumber; //starting LBA
            int diskNumber;
            int stripLayer;
            int blockOfStrip;
            int temp;
            
            for(j = 0; j < numberOfReads /*size*/; j++){ // number of blocks we have to write to
                temp = currentLBA/strip;
                
                stripLayer = temp / (disks / 2);
                blockOfStrip = currentLBA % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                
                diskNumber = 2 * (temp % (disks / 2)); //algorithm to calculate the disk we write to
                
                //has disk failed?
                if(!working_disks[diskNumber]){
                    ++diskNumber;
                    
                    
                }
                
                printf("READ disk: %d block: %d\n",diskNumber,blockNumber);
                disk_array_read( my_disk_array, diskNumber, blockNumber, data );
                
                printf("%s ", data);
            }
        }
        
        else if (strcmp("WRITE", commandLine[0]) == 0){ //WRITE LBA SIZE VALUE
            char *data = commandLine[3];
            int numberOfWrites = atoi(commandLine[2]);
            int currentLBA = atoi(commandLine[1]);
            int j;
            int blockNumber; //starting LBA
            int diskNumber;
            int stripLayer;
            int blockOfStrip;
            int temp;
            
            for (j = 0; j < numberOfWrites /*size*/; j++){ // number of blocks we have to write to
                temp = currentLBA/strip;
                
                stripLayer = temp / (disks / 2);
                blockOfStrip = currentLBA % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                
                diskNumber = 2 * (temp % (disks / 2)); //algorithm to calculate the disk we write to
                
                printf("WRITE disk: %d block: %d\n", diskNumber,blockNumber);
                disk_array_write( my_disk_array, diskNumber, blockNumber, data );
                printf("WRITE disk: %d block: %d\n", (diskNumber + 1) ,blockNumber);
                disk_array_write( my_disk_array, (diskNumber + 1), blockNumber, data );
            }
        }
        
        else if(strcmp("FAIL", commandLine[0]) == 0) { //FAIL DISK
            disk_array_fail_disk( my_disk_array, atoi(commandLine[1]));
            working_disks[atoi(commandLine[1])] = FALSE;
        }
        
        else if(strcmp("RECOVER", commandLine[0]) == 0) { //RECOVER DISK
            disk_array_recover_disk( my_disk_array, atoi(commandLine[1]));
            working_disks[atoi(commandLine[1])] = TRUE;
        }
        
        else if(strcmp("END", commandLine[0]) == 0){ // END
            break;
        }
        
        else{
            //debugging
            printf("%s\n","Trace File Error");
            
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(-1);
        }
        free(command);
    }
    
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
	//start all disks at working
	int i = 0;
	working_disks = malloc(disks * sizeof(int));
	for(i = 0; i < disks; ++i)
		working_disks[i] = TRUE;
    
    chooseSystem(level);
    
    fclose(trace_file);
    
    return 0;
}
