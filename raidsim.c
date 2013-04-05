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

void toParity(int parityDisk, int blockNumber, int dataDisk, char * dataBlock);
char fromParity(int blockNumber, int diskNumber);

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

// used to load respective values in blocks of stripe
char result; //the bitwise result of XOR's


/*
 * do RAID 0
 *
 *@param
 *
 */
void doRaid0() {
    char *data;
    data = malloc(1024);
    
    while (fgets(str, 100, trace_file) != NULL) {
        printf("%s", str);
        
        //for each line parse and detect what command we have
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
        
        if (strcmp("READ", commandLine[0]) == 0) { //READ LBA SIZE
			int numberOfReads = atoi(commandLine[2]);
			int currentLBA = atoi(commandLine[1]);
			int j;
			int blockNumber; //starting LBA
			int diskNumber;
			int stripLayer;
			int blockOfStrip;
			int temp;
            int printedData;
			
			for (j = currentLBA; j < (currentLBA + numberOfReads) /*size*/; j++) { // number of blocks we have to write to
				temp = j / strip;
				stripLayer = temp / disks;
				blockOfStrip = j % strip;
				blockNumber = stripLayer * strip + blockOfStrip;
				diskNumber = temp % disks; //algorithm to calculate the disk we read from
                
                int readCheck = disk_array_read( my_disk_array, diskNumber, blockNumber, data );
				if (readCheck == -1) {
                    printf("ERROR ");
                }
                else {
                    printedData = atoi(data);
                    printf("%d ", printedData);
                }
			}
		}
		
		else if (strcmp("WRITE", commandLine[0]) == 0) { //WRITE LBA SIZE VALUE
			char *data = commandLine[3];
			int numberOfWrites = atoi(commandLine[2]);
			int currentLBA = atoi(commandLine[1]);
			int j;
			int blockNumber; //starting LBA
			int diskNumber;
			int stripLayer;
			int blockOfStrip;
			int temp;
            int writeCheck;
			
			for (j = currentLBA; j < (currentLBA + numberOfWrites) /*size*/; j++) { // number of blocks we have to write to
				temp = j / strip;
				stripLayer = temp/disks;
				blockOfStrip = j % strip;
				blockNumber = stripLayer * strip + blockOfStrip;
				diskNumber = temp % disks; //algorithm to calculate the disk we write to
                
				writeCheck = disk_array_write( my_disk_array, diskNumber, blockNumber, data );
			}
            if (writeCheck == -1) {
                printf("ERROR ");
            }
		}
		
		else if (strcmp("FAIL", commandLine[0]) == 0) { //FAIL DISK
            disk_array_fail_disk( my_disk_array, atoi(commandLine[1]));
            working_disks[atoi(commandLine[1])] = FALSE;
		}
		
		else if (strcmp("RECOVER", commandLine[0]) == 0) { //RECOVER DISK
            disk_array_recover_disk( my_disk_array, atoi(commandLine[1]));
		}
		
        else if (strcmp("END", commandLine[0]) == 0){ // END
            disk_array_print_stats(my_disk_array);
            break;
        }
		
		else {
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


/*
 * do RAID 4
 *
 *@param
 *
 */
void doRaid4() {
    char *data;
    int blockNumber; //starting LBA
    int diskNumber;
    int stripLayer;
    int blockOfStrip;
    int temp;
    int numberOfReads;
    data = malloc(1024);
    
    
    while (fgets(str, 100, trace_file) != NULL) {
        printf("%s", str);
        
        //for each line parse and detect what command we have
        //for this purpose, "line" is the string on the line from the trace file
        char *command = NULL;
        command = (char*) malloc(512);
        if(str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = '\0'; //remove newline char
        
        char * commandLine[5];
        
        command = strtok(str, " "); //split string on space delimiter into tokens
        
        int i = 0;
        while ( command != NULL ) {
            //commandLine[i] = malloc(8 * sizeof(char));
            //printf("%s\n",command);
            commandLine[i] = command;
            i++;
            command = strtok( NULL, " " );
        }
        
        if (strcmp("READ", commandLine[0]) == 0) { //READ LBA SIZE
            numberOfReads = atoi(commandLine[2]);
            int currentLBA = atoi(commandLine[1]);
            int j, printedData;
            int threshold = currentLBA + numberOfReads;
            if (threshold > size * (disks-1 /*excluding parityDisk*/ ))
                //we need this so we do not write more than the total amount of blocks in all disks
                threshold = size * (disks-1); //ex if there are only 10 blocks, we can't write >10 times
            
            
            for (j = currentLBA; j < threshold; j++) { // number of blocks we have to write to
                temp = j / strip;
                stripLayer = temp / (disks-1); //make it seem like we do not have the last disk
                blockOfStrip = j % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                diskNumber = temp % (disks-1); //algorithm to calculate the disk we read from
                
                int readCheck = disk_array_read( my_disk_array, diskNumber, blockNumber, data );
                
                
                
                
                //if disk_array_read returns -1, we tried to read froma failed disk
                //if so, we need to try and recover the old info
                if (readCheck == -1) {
                    
                    //fromParity(j (blockNumber), currentLBA (dataDisk) );
                    
                    char blocks[disks][1024];
                    int i, k;
                    for (i = 0; i < disks; i++) {
                        if (i != diskNumber) { // we don't want to read old datadisk data
                            disk_array_read(my_disk_array, i, blockNumber, &blocks[i][0]); //if this doesnt work change "block[i]" into "&block[i][0]"
                        }
                    }
                    for (i = 0; i < 1024; i++) {
                        char parity = 0;
                        for (k = 0; k < disks; k++) {
                            if (k != diskNumber)
                                parity = parity ^ blocks[k][i];
                        }
                        blocks[diskNumber][i] = parity;
                    }
                    //disk_array_write(my_disk_array, currentLBA, j, blocks[currentLBA]);
                    //if this doesnt work change "block[parityDisk]" into "&block[parityDisk][0]"
                    
                    printf("Recovered data: %s\n", blocks[diskNumber]);
                    
                }
                
                else {
                    printedData = atoi(data);
                    printf("%d ", printedData);
                }
            }
        }
        
        else if ( strcmp("WRITE", commandLine[0]) == 0 ) { //WRITE LBA SIZE VALUE
            char *data = commandLine[3];
            int numberOfWrites = atoi(commandLine[2]);
            int currentLBA = atoi(commandLine[1]);
            int j;
            int threshold = currentLBA + numberOfWrites;
            if (threshold > size * (disks-1)) { //we need this so we do not write more than the total amount of blocks in all disks
                threshold = size * (disks-1); //ex if there are only 10 blocks, we can't write >10 times
            }
            
            for (j = currentLBA; j < threshold; j++) { // number of blocks we have to write to
                temp = j / strip;
                stripLayer = temp / (disks-1);
                blockOfStrip = j % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                diskNumber = temp % (disks-1); //algorithm to calculate the disk we write to
                
                //writeCheck = disk_array_write( my_disk_array, diskNumber, blockNumber, data );
                
                //write to parity disk (last disk)
                int parityDisk = disks - 1;
                
                
                //toParity(parityDisk, blockNumber, diskNumber (datadisk), data); //possible? error in last parameter (should be pointer maybe? idk)
                
                
                // TO PARITY: computing parity to store back in parity disk
                
                
                
                char blocks[disks][1024];
                int i, j;
                for (i = 0; i < disks; i++) {
                    if ((i != parityDisk) && (i != diskNumber)) // we don't want to read either parityDisk or old datadisk
                        disk_array_read(my_disk_array, i, blockNumber, &blocks[i][0]); //if this doesnt work change "block[i]" into "&block[i][0]"
                }
                memcpy(&blocks[diskNumber][0], &data, 1024);
                
                for (i = 0; i < 1024; i++) {
                    int parity = 0;
                    for (j = 0; j < disks; j++) {
                        if (j == parityDisk)
                            continue; //do nothing
                        parity = parity ^ blocks[j][i];
                    }
                    blocks[parityDisk][i] = parity;
                }
                disk_array_write(my_disk_array, parityDisk, blockNumber, &blocks[parityDisk][0]);
                
               //printf("new parity is: %s\n", blocks[parityDisk]);
                //if this doesnt work change "block[parityDisk]" into "&block[parityDisk][0]"
                

                
                
                //write to updating disk
                int writeCheck = disk_array_write( my_disk_array, diskNumber, blockNumber, data);
                
                //if we detect that we are writing to a failed disk, rewrite 0 and print ERROR
                if (writeCheck == -1) {
                    disk_array_write( my_disk_array, diskNumber, blockNumber, 0);
                    printf("ERROR: writing into failed disk\n");
                    return;
                }
                
            }
        }
        
        else if (strcmp("FAIL", commandLine[0]) == 0) { //FAIL DISK
            disk_array_fail_disk( my_disk_array, atoi(commandLine[1]));
            working_disks[atoi(commandLine[1])] = FALSE;
            
        }
        
        else if (strcmp("RECOVER", commandLine[0]) == 0) { //RECOVER DISK
            
            int recoveredDisk = atoi(commandLine[1]);
            disk_array_recover_disk( my_disk_array, recoveredDisk); //we clear the disk
            working_disks[recoveredDisk] = TRUE;
           
            /*
            int j;
            for (j = 0; j < size; j++) { //for every block within a disk
                fromParity(j, atoi(commandLine[1]));
            }
             */
            
            char blocks[disks][1024];
            int i, k;
            for (i = 0; i < disks; i++) {
                if (i != recoveredDisk) { // we don't want to read old datadisk data
                    disk_array_read(my_disk_array, i, blockNumber, &blocks[i][0]); //if this doesnt work change "block[i]" into "&block[i][0]"
                }
            }
            for (i = 0; i < 1024; i++) {
                char parity = 0;
                for (k = 0; k < disks; k++) {
                    if (k != recoveredDisk)
                        parity = parity ^ blocks[k][i];
                }
                blocks[recoveredDisk][i] = parity;
            }
            //disk_array_write(my_disk_array, currentLBA, j, blocks[currentLBA]);
            //if this doesnt work change "block[parityDisk]" into "&block[parityDisk][0]"
            
            disk_array_write(my_disk_array, recoveredDisk, blockNumber, blocks[recoveredDisk]);
            
            printf("Recovered data: %s\n", blocks[diskNumber]);
            
        }
        
        else if (strcmp("END", commandLine[0]) == 0) { // END
            disk_array_print_stats(my_disk_array);
            break;
        }
        
        else {
            //debugging
            printf("%s\n","Trace File Error");
            
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(-1);
        }
        free(command);
    }
}

/*
 * do RAID 10
 *
 *@param
 *
 */
void doRaid10() {
    char *data;
    data = malloc(1024);
    
    if ((disks % 2) == 1) {
        printf("Odd number of disks");
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
            
            for (j = currentLBA; j < (currentLBA + numberOfReads) /*size*/; j++) { // number of blocks we have to write to
                int flag1 = 1;
                
                temp = j / strip;
                
                stripLayer = temp / (disks / 2);
                blockOfStrip = j % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                
                diskNumber = 2 * (temp % (disks / 2)); //algorithm to calculate the disk we write to
                
                disk_array_read( my_disk_array, diskNumber, blockNumber, data );
                
                //has disk failed?
                if (!working_disks[diskNumber]) {
                    ++diskNumber;
                    disk_array_read( my_disk_array, diskNumber, blockNumber, data );
                    if (!working_disks[diskNumber]) {
                        printf("ERROR ");
                        flag1 = 0;
                    }
                }
                if (flag1)
                    printf("%d ", atoi(data));
            }
        }
        
        
        else if (strcmp("WRITE", commandLine[0]) == 0) { //WRITE LBA SIZE VALUE
            char *data = commandLine[3];
            int numberOfWrites = atoi(commandLine[2]);
            int currentLBA = atoi(commandLine[1]);
            int j;
            int blockNumber; //starting LBA
            int diskNumber;
            int stripLayer;
            int blockOfStrip;
            int temp;
            
            for (j = currentLBA; j < (currentLBA + numberOfWrites) /*size*/; j++) { // number of blocks we have to write to
                temp = j / strip;
                
                stripLayer = temp / (disks / 2);
                blockOfStrip = j % strip;
                blockNumber = stripLayer * strip + blockOfStrip;
                
                diskNumber = 2 * (temp % (disks / 2)); //algorithm to calculate the disk we write to
                
                disk_array_write( my_disk_array, diskNumber, blockNumber, data );
                disk_array_write( my_disk_array, (diskNumber + 1), blockNumber, data );
            }
        }
        
        else if(strcmp("FAIL", commandLine[0]) == 0) { //FAIL DISK
            disk_array_fail_disk( my_disk_array, atoi(commandLine[1]));
            working_disks[atoi(commandLine[1])] = FALSE;
        }
        
        
        //
        else if (strcmp("RECOVER", commandLine[0]) == 0) { //RECOVER DISK
            disk_array_recover_disk( my_disk_array, atoi(commandLine[1]));
            
            //Personal Note: HERE or after following portion?
            working_disks[atoi(commandLine[1])] = TRUE;
            int mirror_disk;
            
            //even?
            if(atoi(commandLine[1]) % 2)
                mirror_disk = (atoi(commandLine[1]) - 1);
            else
                mirror_disk = (atoi(commandLine[1]) + 1);
            
            //Personal Note:
            if (working_disks[mirror_disk]){
                int k = 0;
                
                for(k = 0; k < size; ++k){
                    disk_array_read( my_disk_array, mirror_disk, k, data);
                    disk_array_write( my_disk_array, atoi(commandLine[1]), k, data);
                }
                
            } else {
                //debugging
                printf("Both disks had previously failed!\n");
                
                write(STDERR_FILENO, error_msg, strlen(error_msg));
                exit(-1);
            }
        }
        
        
        else if (strcmp("END", commandLine[0]) == 0) { // END
            disk_array_print_stats(my_disk_array);
            break;
        }
        
        else {
            //debugging
            printf("%s\n","Trace File Error");
            
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(-1);
        }
        free(command);
    }
    
}

/*
 * find the resulting parity bit from passed in values
 *
 *@param int parityDisk, int blockNumber, in diskNumber
 *@returns char *parityValue
 */
void toParity(int parityDisk, int blockNumber, int dataDisk, char * dataBlock) {
    char blocks[disks][1024];
    int i, j;
    for(i = 0; i < disks; i++){
        if((i != parityDisk) && (i != dataDisk)) // we don't want to read either parityDisk or old datadisk data
            disk_array_read(my_disk_array, i, blockNumber, blocks[i]); //if this doesnt work change "block[i]" into "&block[i][0]"
    }
    memcpy(&blocks[dataDisk][0], &dataBlock, 1024);
    for(i = 0; i < 1024; i++){
        int parity = 0;
        for(j = 0; j < disks; j++){
            if(j == parityDisk)
                continue; //do nothing
            parity = parity ^ blocks[j][i];
        }
        blocks[parityDisk][i] = parity;
    }
    disk_array_write(my_disk_array, parityDisk, blockNumber, blocks[parityDisk]);
    
    printf("new parity is: %s", blocks[parityDisk]);
    //if this doesnt work change "block[parityDisk]" into "&block[parityDisk][0]"
    
}


/*
 * used in disk recovery to resturn the value of the missing data
 *
 *@param int diskNumber, int blockNumber
 *@returns char *parityValue
 */
char fromParity(int blockNumber, int dataDisk) {
    char blocks[disks][1024];
    int i, j;
    for(i = 0; i < disks; i++){
        if(i != dataDisk){ // we don't want to read old datadisk data
            disk_array_read(my_disk_array, i, blockNumber, blocks[i]); //if this doesnt work change "block[i]" into "&block[i][0]"
        }
    }
    for(i = 0; i < 1024; i++){
        int parity = 0;
        for(j = 0; j < disks; j++){
            if(j == dataDisk)
                continue; //do nothing
            parity = parity ^ blocks[j][i];
        }
        blocks[dataDisk][i] = parity;
    }
    disk_array_write(my_disk_array, dataDisk, blockNumber, blocks[dataDisk]);
    //if this doesnt work change "block[parityDisk]" into "&block[parityDisk][0]"
    
    return 0;
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
	if ((argc != 11) && (argc != 12)) {
		//debugging
		printf("%s\n","Arg Error");
        
		write(STDERR_FILENO, error_msg, strlen(error_msg));
		exit(-1);
	}
    
    
	//set
	for (counter = 0; counter <= (argc)/2; ++counter) {
		if (strcmp("-level", argv[2*counter+1]) == 0) {
			level = atoi(argv[2*counter+2]);
            
		} else if (strcmp("-strip", argv[2*counter+1]) == 0) {
			strip = atoi(argv[2*counter+2]);
            
		} else if (strcmp("-disks", argv[2*counter+1]) == 0) {
			disks = atoi(argv[2*counter+2]);
            
		} else if (strcmp("-size", argv[2*counter+1]) == 0) {
			size = atoi(argv[2*counter+2]);
            
		} else if (strcmp("-trace", argv[2*counter+1]) == 0) {
			trace = argv[2*counter+2];
            
		} else if (strcmp("-verbose", argv[2*counter+1]) == 0) {
			verbose = 1;
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
	for(i = 0; i < disks; ++i) {
		working_disks[i] = TRUE;
    }
    
    //  for(i = 0; i < disks; i++){
    //   disk_array_recover_disk( my_disk_array, i);
    //  }
    
    
    chooseSystem(level);
    
    fclose(trace_file);
    
    return 0;
}
