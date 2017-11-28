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
    int* processID = (int*)calloc(processNumber, sizeof(int));
    int parentProcessID = getpid(), myProcessNumber;
    for(int i = 0; i < processNumber; i++)
        if((processID[i] = fork()) == 0) {
            myProcessNumber = i;
            break;
        }    
<<<<<<< HEAD
<<<<<<< HEAD

    char path[] = "/sortedPart";

    if(getpid() == parentProcessID) { //parent
    
        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = dataLength;
        messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
        mqd_t message = mq_open(path, O_CREAT | O_RDONLY, 0666, &messageQueueAttribute);
        int* tempArray = (int*)calloc(dataLength, sizeof(int));
        
        for(int i = 0; i < processNumber; i++) waitpid(processID[i]);
<<<<<<< HEAD
<<<<<<< HEAD
        for(int i = 0; i < processNumber; i++) mq_receive(message, data + dataLength * i / processNumber, dataLength * sizeof(int), NULL);  
        for(int i = 0; i < processNumber; i++) merge(0, dataLength * i / processNumber, dataLength * (i + 1) / processNumber);
    
        mq_close(message);
=======
        
        for(int i = 0; i < processNumber; i++) {
            int result = mq_receive(message, tempArray, dataLength * sizeof(int), NULL);
            printf("result : %d\n", result);
            for(int j = 0; j < dataLength; j++) printf("%d\t", tempArray[j]);
            printf("\n");
        }
=======
    

    if(getpid() == parentProcessID) { //parent
>>>>>>> parent of 53c45c4... 일단 완성
=======
    

    if(getpid() == parentProcessID) { //parent
>>>>>>> parent of 53c45c4... 일단 완성

>>>>>>> parent of 3e65c21... 완성2
=======
        
        for(int i = 0; i < processNumber; i++) {
            mq_receive(message, tempArray + dataLength * i / processNumber, dataLength * sizeof(int), NULL);
        }

        for(int i = 0; i < dataLength; i++){
            data[i] = tempArray[i];
        }

        for(int i = 0; i < processNumber; i++) {
            merge(0, dataLength * i / processNumber, dataLength * (i + 1) / processNumber);
        }

        for(int i = 0; i < dataLength; i++) printf("%d\t", data[i]);

        mq_close(message);

>>>>>>> parent of 28e3a44... 완성
    }
    else {
        int start = dataLength * myProcessNumber / processNumber;
        int end = dataLength * (myProcessNumber + 1) / processNumber;
        recursiveMergeSort(start, end);

<<<<<<< HEAD
        int sendDataLength = end - start;
        int* sendData = (int*)calloc(sendDataLength, sizeof(int));
        for(int i = 0; i < sendDataLength; i++) {
            sendData[i] = data[start + i];
        }

        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = sendDataLength;
        messageQueueAttribute.mq_msgsize = sendDataLength * sizeof(int);
<<<<<<< HEAD
        mqd_t message = mq_open(messageQueueFileName, O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        mq_send(message, (char*)&data[start], sendDataLength * sizeof(int), processNumber - myProcessNumber);
=======
        struct mq_attr messageQueueAttribute;
        messageQueueAttribute.mq_maxmsg = dataLength * sizeof(int);
        messageQueueAttribute.mq_msgsize = dataLength * sizeof(int);
<<<<<<< HEAD
<<<<<<< HEAD
        mqd_t message = mq_open(path, O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        mq_send(message, (char*)data, dataLength * sizeof(int), processNumber - myProcessNumber);
>>>>>>> parent of 3e65c21... 완성2
=======
        mqd_t message = mq_open("/part_sorted_data", O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        int i = mq_send(message, (char*)data, dataLength * sizeof(int), myProcessNumber);
        printf("%d", i);
>>>>>>> parent of 53c45c4... 일단 완성
=======
        mqd_t message = mq_open("/part_sorted_data", O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        int i = mq_send(message, (char*)data, dataLength * sizeof(int), myProcessNumber);
        printf("%d", i);
>>>>>>> parent of 53c45c4... 일단 완성
=======
        mqd_t message = mq_open(path, O_CREAT | O_WRONLY, 0666, &messageQueueAttribute);
        mq_send(message, (char*)sendData, sendDataLength * sizeof(int), processNumber - myProcessNumber);
>>>>>>> parent of 28e3a44... 완성
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