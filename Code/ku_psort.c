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

    if(getpid() == parentProcessID) { //parent
        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = dataLength;
        messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
        mqd_t message = mq_open("/sortedPart", O_CREAT | O_RDWR, 0666, &messageQueueAttribute);
        int* tempArray = (int*)calloc(dataLength, sizeof(int));
        
        for(int i = 0; i < processNumber; i++) waitpid(childProcessID[i]);
        for(int i = 0; i < processNumber; i++) {
<<<<<<< HEAD
<<<<<<< HEAD
            int start = dataLength * i / processNumber;
            int end = dataLength * (i + 1) / processNumber;

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of 3e65c21... 완성2
=======
>>>>>>> parent of 3e65c21... 완성2
=======
            mq_receive(message, tempArray, dataLength * sizeof(int), NULL);
            for(int j = start; j < end; j++) data[j] = tempArray[j];
            merge(0, start, end);
        }
>>>>>>> acfc9dadb06f2b3c1f3ca29a8d751dce0a1b24dc
=======
=======
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
            int result = mq_receive(message, tempArray, dataLength * sizeof(int), NULL);
            printf("result : %d\n", result);
            for(int j = 0; j < dataLength; j++) printf("%d\t", tempArray[j]);
            printf("\n");
        }      
        mq_close(message);
<<<<<<< HEAD
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
=======
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
    }
    else {
        int start = dataLength * myProcessNumber / processNumber;
        int end = dataLength * (myProcessNumber + 1) / processNumber;
        recursiveMergeSort(start, end);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        int sendDataLength = end - start;

        messageQueueAttribute.mq_maxmsg = sendDataLength;
        messageQueueAttribute.mq_msgsize = sendDataLength * sizeof(int);
        mqd_t message = mq_open(messageQueueFileName, O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        mq_send(message, (char*)&data[start], sendDataLength * sizeof(int), processNumber - myProcessNumber);
=======
=======
>>>>>>> parent of 3e65c21... 완성2
        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = dataLength;
        messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
        mqd_t message = mq_open(path, O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        mq_send(message, (char*)data, dataLength * sizeof(int), processNumber - myProcessNumber);
<<<<<<< HEAD
>>>>>>> parent of 3e65c21... 완성2
=======
>>>>>>> parent of 3e65c21... 완성2
=======
=======
=======
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = dataLength;
        messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
        
        mqd_t message = mq_open("/sortedPart", O_CREAT | O_RDWR, 0666, &messageQueueAttribute);
<<<<<<< HEAD
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
=======
>>>>>>> parent of acfc9da... Merge pull request #1 from bbondd/AllDataSend
        mq_send(message, data, dataLength * sizeof(int), processNumber - myProcessNumber);
>>>>>>> acfc9dadb06f2b3c1f3ca29a8d751dce0a1b24dc
        mq_close(message);
    }
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