#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>

int dataLength;
int* data;

void readDataFromFile(char* fileName) {
	FILE* inputFile = fopen(fileName, "r");
	int* fileData = (int*)calloc(dataLength, sizeof(int));
	for(int i = 0; i < dataLength; i++) fscanf(inputFile, "%d", &fileData[i]);
	fclose(inputFile);
	
    data = fileData;
}

void merge(int start, int middle, int end) {
    int i = 0, left = start, right = middle;
    int* tempArray = (int*)calloc(end - start, sizeof(int));

    while(left < middle && right < end) {
        if(data[left] < data[right]) tempArray[i++] = data[left++];
        else tempArray[i++] = data[right++];
    }

    while(left < middle) tempArray[i++] = data[left++];
    while(right < end) tempArray[i++] = data[right++];

    for(int j = 0; j < end - start; j++) data[start + j] = tempArray[j];
    
    free(tempArray);
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

    char path[] = "/sortedPart";
    struct mq_attr messageQueueAttribute;
    messageQueueAttribute.mq_maxmsg = dataLength;
    messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
    mqd_t message = mq_open(path, O_CREAT | O_RDWR, 0666, &messageQueueAttribute);

    if(getpid() == parentProcessID) { //parent
        for(int i = 0; i < processNumber; i++) waitpid(childProcessID[i]);

        int* tempArray = (int*)calloc(dataLength, sizeof(int));
        for(int i = 0; i < processNumber; i++) {
            mq_receive(message, tempArray, dataLength * sizeof(int), NULL);
            int start = dataLength * i / processNumber;
            int end = dataLength * (i + 1) / processNumber;
            for(int j = start; j < end; j++) data[j] = tempArray[j];
            merge(0, start, end);
        }
    }
    else { //child
        int start = dataLength * myProcessNumber / processNumber;
        int end = dataLength * (myProcessNumber + 1) / processNumber;
        recursiveMergeSort(start, end);

        mq_send(message, data, dataLength * sizeof(int), processNumber - myProcessNumber);
        mq_close(message);
        exit(3);
    }
    mq_close(message);
}

void writeDataToFile(char* fileName) {
    FILE* outputFile = fopen(fileName, "w");
    for(int i = 0; i < dataLength; i++) fprintf(outputFile, "%d\n", data[i]);
    fclose(outputFile);
}

void main(int argc, char* argv[]) {
    dataLength = atoi(argv[1]);
    readDataFromFile(argv[3]);
    multiProcessMergeSort(atoi(argv[2]));
    writeDataToFile(argv[4]);
}