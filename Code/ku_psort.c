#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include "ku_psort.h"

#define MAX_MESSAGE 10

int dataLength;
int* data;

void readDataFromFile(char* fileName) {
	FILE* inputFile = fopen(fileName, "r");
	int* fileData = (int*)calloc(dataLength, sizeof(int));
	for(int i = 0; i < dataLength; i++) fscanf(inputFile, "%d", &fileData[i]);
	fclose(inputFile);
	
    data = fileData;
}

void writeDataToFile(char* fileName) {
    FILE* outputFile = fopen(fileName, "w");
    for(int i = 0; i < dataLength; i++) fprintf(outputFile, "%d\n", data[i]);
    fclose(outputFile);
}

void merge(int start, int middle, int end) {
    int i = 0, left = start, right = middle;
    int* tempData = (int*)calloc(end - start, sizeof(int));

    while(left < middle && right < end) {
        if(data[left] < data[right]) tempData[i++] = data[left++];
        else tempData[i++] = data[right++];
    }

    while(left < middle) tempData[i++] = data[left++];
    while(right < end) tempData[i++] = data[right++];

    for(int j = 0; j < end - start; j++) data[start + j] = tempData[j];
    
    free(tempData);
}

void recursiveMergeSort(int start, int end) {
    if (start + 1 == end) return;

    int middle = (start + end) / 2;
    recursiveMergeSort(start, middle);
    recursiveMergeSort(middle, end);

    merge(start, middle, end);
}

void multiProcessMergeSort(int processNumber) {
    int* childProcessID = (int*)calloc(processNumber, sizeof(int));
    int parentProcessID = getpid(), myProcessNumber;
    for(int i = 0; i < processNumber; i++)
        if((childProcessID[i] = fork()) == 0) {
            myProcessNumber = i;
            break;
        }

    char messageQueuePath[] = "/sortedPart";
    struct mq_attr messageQueueAttribute;
    messageQueueAttribute.mq_maxmsg = MAX_MESSAGE;
    messageQueueAttribute.mq_msgsize = MAX_MESSAGE * sizeof(int);
    mqd_t message = mq_open(messageQueuePath, O_CREAT | O_RDWR, 0666, &messageQueueAttribute);

    if(getpid() == parentProcessID) {
        for(int i = 0; i < processNumber; i++) waitpid(childProcessID[i]);
        
        int* tempData = (int*)calloc((dataLength / MAX_MESSAGE + 1) * MAX_MESSAGE, sizeof(int));
        for(int i = 0; i < processNumber; i++) {
        int start = dataLength * i / processNumber;
        int end = dataLength * (i + 1) / processNumber;

            for(int j = 0; j < dataLength / MAX_MESSAGE + 1; j++) {
                mq_receive(message, &tempData[j * MAX_MESSAGE], MAX_MESSAGE * sizeof(int), NULL);
                printf("rcv");
            }
            
            for(int j = start; j < end - start; j++) 
                data[start] = tempData[start];
        }

        for(int i = 0; i < processNumber; i++) {
            int start = i * myProcessNumber / processNumber;
            int end = i * (myProcessNumber + 1) / processNumber;
            //merge(0, start, end);
        }
    }
    else {
        int start = dataLength * myProcessNumber / processNumber;
        int end = dataLength * (myProcessNumber + 1) / processNumber;
        recursiveMergeSort(start, end);
        
        int* tempData = (int*)calloc((dataLength / MAX_MESSAGE + 1) * MAX_MESSAGE, sizeof(int));
        for(int i = 0; i < dataLength; i++) tempData[i] = data[i];

        for(int i = 0; i < dataLength / MAX_MESSAGE + 1; i++) {
            int result = mq_send(message, &tempData[i * MAX_MESSAGE], MAX_MESSAGE * sizeof(int), processNumber - myProcessNumber);
        }
        mq_close(message);
        exit(3);
    }
    mq_close(message);
}

void main(int argc, char* argv[]) {
    dataLength = atoi(argv[1]);
    readDataFromFile(argv[3]);
    multiProcessMergeSort(atoi(argv[2]));
    writeDataToFile(argv[4]);
}