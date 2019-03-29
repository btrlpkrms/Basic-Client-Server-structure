#define MAX_SIZE_STRING 255
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int Boolean;

typedef struct
{
char name;
char IP[MAX_SIZE_STRING];
char MAC[MAX_SIZE_STRING];
char portNumber[MAX_SIZE_STRING];
struct queue *incomingQueue;
struct queue *outgoingQueue;
char targetArray[4];
char destArray[4];
char checkIn[5];
char checkOut[5];
int hops;
}client;
struct physical_Layer
{
	char senderMacAddress[MAX_SIZE_STRING];
	char receiverMacAddress[MAX_SIZE_STRING];
};
struct networkLayer
{
	char senderIpAddress[MAX_SIZE_STRING];
	char receiverIpAddress[MAX_SIZE_STRING];
};
struct transportLayer
{
	char senderPortNumber[MAX_SIZE_STRING];
	char receiverPortNumber[MAX_SIZE_STRING];
};
struct applicationLayer
{
	char senderId;
	char receiverId;
	char messageChunk[MAX_SIZE_STRING];
};


typedef struct{
	struct	physical_Layer physicalLayer;
	struct	networkLayer ntwrkLyr;
	struct	transportLayer trnsprLyr;
	struct	applicationLayer aplctnLyr;
}Layer;



typedef struct{
	client *clientArray;
	int clientCount;
}clientList;

clientList clients = { 0 };

struct Stack {

    Layer *layers;
    int top,size,max_size;

};


struct queue{
	
	struct Stack *arr;
	int front,rear,max_size;

};

struct Stack *stack_creator(int stack_size) {

    struct Stack *s =(struct Stack*)malloc(sizeof(struct Stack));
    s->layers = (Layer*)malloc(sizeof(Layer)*stack_size);
    s->top = -1;
    s->max_size= stack_size;
    s->size = 0;
    return s;

}


int is_stack_full(struct Stack *s) {
    return s->size == s->max_size;
}

int is_stack_empty(struct Stack *s) {
    return s->size == 0;
}

void push(struct Stack *s, Layer element) {
    if(is_stack_full(s)){
    	printf("Stack full hata mesaji *2*\n");
	}
	else{
	    s->layers[++(s->top)] = element;
	    s->size++;
	}
}

void pop(struct Stack *s) {
    if(is_stack_empty(s)) {
        printf("Empty stack!\n");
        return;
    }
    Layer element = s->layers[(s->top)--];
    s->size--;
    
}

struct queue *queue_creator(int queue_size){
	struct queue *q=(struct queue*)malloc(sizeof(struct queue));
	q->arr = (struct Stack*)malloc(sizeof(struct Stack)*queue_size);
	q->front=-1;
	q->rear=-1;
	q->max_size=queue_size;
	return q;
}

int is_queue_full(struct queue *q)
{
    if( (q->front == (q->rear) + 1) || (q->front == 0 && q->rear == (q->max_size)-1)){
    	return 1;
	}
    return 0;
}

int is_queue_empty(struct queue *q){
	if(q->front == -1){
		return 1;
	}
	return 0;
}

void enQueue(struct queue *q,struct Stack element){
    if(is_queue_full(q)){
    	return ;
	}
    else
    {
        if(q->front == -1){
        	q->front = 0;	
		} 
        q->rear = (q->rear + 1) % q->max_size;
        q->arr[q->rear] = element;
        
    }
}

struct Stack deQueue(struct queue *q){
    struct Stack element;
    if(is_queue_empty(q)) {
    	printf("stack is empty");
    	struct Stack emptyStack;
        return(emptyStack);
    } else {
        element = q->arr[q->front];
        if (q->front == q->rear){
            q->front = -1;
            q->rear = -1;
        }
        else {
            q->front = (q->front + 1) % q->max_size;
            
        }
        return(element);
    }
}

Boolean startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return strncmp(pre, str, lenpre) == 0;
}

Boolean containNumber(char *string){
	int i;
	char number[] = {'1','2','3','4','5','6','7','8','9','0'};
	for(i = 0;i<10;i++){
		if(number[i] == string[0]){
			return 0;
		}
		
	}
		
}


int findClient(char id){
	int i;
	for(i = 0;i<clients.clientCount;i++){
		if(clients.clientArray[i].name == id){
			return i;
		}
		
	}
}
int findDestination(client sender,client receiver){
	int i;
	char goingClient;
	for(i = 0;i<clients.clientCount-1;i++){
		if(sender.targetArray[i] == receiver.name){
			 goingClient = sender.destArray[i];
		}
	}
	for(i = 0;i<clients.clientCount;i++){
		if(goingClient == clients.clientArray[i].name){
			return i;
		}
	}
	
}



void readCommands(FILE *f,double max_msg_size,char *senderPort,char *receiverPort){
	
	char *pos;
	char commandNumber;
	int frameNumber;
	int goingClientIndex;
	int tempHops=0;
	char messageCommand[MAX_SIZE_STRING];
	char showFrameInfo[MAX_SIZE_STRING];
	char showQueueInfo[MAX_SIZE_STRING];
	char sendCommand[MAX_SIZE_STRING];
	char printLog[MAX_SIZE_STRING];
	char line[MAX_SIZE_STRING];
	char message[MAX_SIZE_STRING];
	int messageLen = 0;
	int abc = max_msg_size;
	char splittedMessage[abc];
	char senderClient;
	char receiverClient;
	commandNumber = getc(f);
	rewind(f);
	int i;
	int clientSendIndex;
	int clientReceiverIndex;
	while(fgets(line,sizeof(line),f)){
		if(startsWith("MESSAGE",line)){
			printf("---------------------------------------------------------------------------------------\n");
			printf("Command: %s",line);
			printf("---------------------------------------------------------------------------------------\n");
			if ((pos=strchr(line, '\n')) != NULL){
			*pos = '\0';
    	}
    		
			strcpy(messageCommand,line);
			char *tokenClients = strtok(messageCommand," ");//message komutu
			tokenClients = strtok(NULL," ");//sender client
			senderClient = *tokenClients;
			
			tokenClients = strtok(NULL," ");//receiver client
			receiverClient = *tokenClients;
			
			tokenClients = strtok(NULL,"#");
			strcpy(message,tokenClients);
			printf("Message to be sent: %s\n\n",message);
			float a = strlen(message) / max_msg_size;
			frameNumber = ceil(a);
			
			int tempo = 0;
			clientSendIndex = findClient(senderClient);
			clientReceiverIndex = findClient(receiverClient);
			clients.clientArray[clientSendIndex].outgoingQueue = queue_creator(frameNumber);
			clients.clientArray[clientSendIndex].incomingQueue = queue_creator(frameNumber);
			clients.clientArray[clientReceiverIndex].incomingQueue = queue_creator(frameNumber);
			
			for(i=0;i<frameNumber;i++){
				struct Stack *frame = stack_creator(4);
				Layer aplyr;
				Layer trnsprtlyr;
				Layer ntwrklyr;
				Layer physicalayer;
				
				memcpy(splittedMessage,message+tempo,abc);
				
				aplyr.aplctnLyr.senderId = senderClient;
				aplyr.aplctnLyr.receiverId = receiverClient;
				strcpy(aplyr.aplctnLyr.messageChunk,splittedMessage);
				push(frame,aplyr);
				
				strcpy(trnsprtlyr.trnsprLyr.senderPortNumber,senderPort);
				strcpy(trnsprtlyr.trnsprLyr.receiverPortNumber,receiverPort);
				push(frame,trnsprtlyr);
				
				strcpy(ntwrklyr.ntwrkLyr.senderIpAddress,clients.clientArray[clientSendIndex].portNumber);
				strcpy(ntwrklyr.ntwrkLyr.receiverIpAddress,clients.clientArray[clientReceiverIndex].portNumber);
				push(frame,ntwrklyr);
				
				strcpy(physicalayer.physicalLayer.senderMacAddress,clients.clientArray[clientSendIndex].MAC);
				strcpy(physicalayer.physicalLayer.receiverMacAddress,
				clients.clientArray[findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex])].MAC);
				push(frame,physicalayer);
				
				enQueue(clients.clientArray[clientSendIndex].outgoingQueue,*frame);
				printf("Frame #%d\n",i+1);
				printf("Sender MAC address: %s, Receiver MAC address: %s\n",frame->layers[3].physicalLayer.senderMacAddress,frame->layers[3].physicalLayer.receiverMacAddress);
				printf("Sender IP address: %s, Receiver IP address: %s\n",clients.clientArray[clientSendIndex].portNumber,clients.clientArray[clientReceiverIndex].portNumber);
				printf("Sender port number: %s, Receiver port number: %s\n",senderPort,receiverPort);
				printf("Sender ID: %c, Receiver ID: %c\n",clients.clientArray[clientSendIndex].name,clients.clientArray[clientReceiverIndex].name);
				printf("Message chunk carried: %s\n",frame->layers[0].aplctnLyr.messageChunk);
				printf("--------\n");
				tempo = tempo + max_msg_size;
			}		
			
		}
		else if(startsWith("SHOW_FRAME_INFO",line)){
			if ((pos=strchr(line, '\n')) != NULL){
			*pos = '\0';
    	}
    		
    		printf("---------------------------------\n");
    		printf("Command: %s\n",line);
    		printf("---------------------------------\n");
    		char in_out[10];
			strcpy(showFrameInfo,line);
			char *tokenCommand = strtok(showFrameInfo," ");
			tokenCommand = strtok(NULL," ");//which clients frame C
			int requestIndex = findClient(tokenCommand[0]);
			tokenCommand = strtok(NULL," ");//which queueu this clients
			strcpy(in_out,tokenCommand);
			
			tokenCommand = strtok(NULL," \n");
			int whichFrame = atoi(tokenCommand);
			if(whichFrame<=frameNumber){
				
			
			if(strcmp(in_out,"out")==0){
				printf("Current Frame #%d on the outgoing queue of client %c\n",whichFrame,clients.clientArray[requestIndex].name);
				printf("Carried Message:  \"%s\"\n",clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[0].aplctnLyr.messageChunk);
				printf("Layer 0 info: Sender ID: %c, Receiver ID: %c\n",clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[0].aplctnLyr.senderId
				,clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[0].aplctnLyr.receiverId);
				printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n",senderPort,receiverPort);
				printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n",clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[2].ntwrkLyr.senderIpAddress,
				clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[2].ntwrkLyr.receiverIpAddress);	
				printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n",clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[3].physicalLayer.senderMacAddress,
				clients.clientArray[requestIndex].outgoingQueue->arr[whichFrame-1].layers[3].physicalLayer.receiverMacAddress);
				printf("Number of hops so far: %d\n",clients.clientArray[requestIndex].hops);
			}
			if(strcmp(in_out,"in")==0){
				printf("Current Frame #%d on the incoming queue of client %c\n",whichFrame,clients.clientArray[requestIndex].name);
				printf("Carried Message:  %s\n",clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[0].aplctnLyr.messageChunk);
				printf("Layer 0 info: Sender ID: %c, Receiver ID: %c\n",clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[0].aplctnLyr.senderId
				,clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[0].aplctnLyr.receiverId);
				printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n",senderPort,receiverPort);
				printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n",clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[2].ntwrkLyr.senderIpAddress,
				clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[2].ntwrkLyr.receiverIpAddress);	
				printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n",clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[3].physicalLayer.senderMacAddress,
				clients.clientArray[requestIndex].incomingQueue->arr[whichFrame-1].layers[3].physicalLayer.receiverMacAddress);
				printf("Number of hops so far: %d\n",clients.clientArray[requestIndex].hops);
			}
		}
		else{
			printf("No such frame.\n");
		}
		}
		else if(startsWith("SHOW_Q_INFO",line)){
			if ((pos=strchr(line, '\n')) != NULL){
				*pos = '\0';
    		}	
    		char qin_our[10];
			strcpy(showQueueInfo,line);
			printf("---------------------------------\n");
    		printf("Command: %s\n",line);
    		printf("---------------------------------\n");
    		char *tokenQueueCommand = strtok(showQueueInfo," ");
    		
			tokenQueueCommand = strtok(NULL," ");
    		int requestWhichClient = findClient(tokenQueueCommand[0]);
    		tokenQueueCommand = strtok(NULL," ");
    		strcpy(qin_our,tokenQueueCommand);
    		if(strcmp(qin_our,"out")==0){
			printf("Client %c Outgoing Queue Status\n",clients.clientArray[requestWhichClient].name);
			printf("Current total number of frames: %d\n",clients.clientArray[requestWhichClient].outgoingQueue->rear+1);
			}
			if(strcmp(qin_our,"in")==0){
			printf("Client %c Incoming Queue Status\n",clients.clientArray[requestWhichClient].name);
			printf("Current total number of frames: %d\n",clients.clientArray[requestWhichClient].incomingQueue->rear+1);
			}
			
			
			
		}
		else if(startsWith("SEND",line)){
			
			if ((pos=strchr(line, '\n')) != NULL){
			*pos = '\0';
    	}
    		
			printf("---------------------------------\n");
    		printf("Command: %s\n",line);
    		printf("---------------------------------\n");
    		
    		int k;
			strcpy(sendCommand,line);
			strcpy(clients.clientArray[clientSendIndex].checkOut,"Yes");
			strcpy(clients.clientArray[clientSendIndex].checkIn,"No");
			clients.clientArray[clientSendIndex].hops = tempHops;
			
			while(clientSendIndex != clientReceiverIndex){
			
			goingClientIndex = findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex]);
			strcpy(clients.clientArray[goingClientIndex].checkIn,"Yes");
			strcpy(clients.clientArray[goingClientIndex].checkOut,"Yes");
			clients.clientArray[goingClientIndex].incomingQueue = queue_creator(frameNumber);
			clients.clientArray[goingClientIndex].outgoingQueue = queue_creator(frameNumber);
			if(clients.clientArray[goingClientIndex].name != clients.clientArray[clientReceiverIndex].name){
			printf("A message received by client %c, but intended for client %c. Forwarding...\n",clients.clientArray[goingClientIndex].name,clients.clientArray[clientReceiverIndex].name);
			
			}
			
			for(k = 0;k<frameNumber;k++){
				
				enQueue(clients.clientArray[goingClientIndex].incomingQueue,deQueue(clients.clientArray[clientSendIndex].outgoingQueue));
				
				enQueue(clients.clientArray[goingClientIndex].outgoingQueue,deQueue(clients.clientArray[goingClientIndex].incomingQueue));
			}
			
			tempHops++;
			clients.clientArray[goingClientIndex].hops = tempHops;
			if(clients.clientArray[goingClientIndex].name == clients.clientArray[clientReceiverIndex].name){
				clients.clientArray[clientReceiverIndex].hops = tempHops;
				printf("A message received by client %c from client %c after a total of %d hops.\n",clients.clientArray[clientReceiverIndex].name,clients.clientArray[findClient(senderClient)].name,clients.clientArray[clientReceiverIndex].hops);
				printf("Message: %s\n",message);
			}
			clientSendIndex = goingClientIndex;
			if(clients.clientArray[clientSendIndex].name != clients.clientArray[clientReceiverIndex].name){
				
			printf("Frame #1 MAC address change: New sender MAC %s, new receiver MAC %s\n",clients.clientArray[goingClientIndex].MAC,clients.clientArray[findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex])].MAC);
			printf("Frame #2 MAC address change: New sender MAC %s, new receiver MAC %s\n",clients.clientArray[goingClientIndex].MAC,clients.clientArray[findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex])].MAC);
			printf("Frame #3 MAC address change: New sender MAC %s, new receiver MAC %s\n",clients.clientArray[goingClientIndex].MAC,clients.clientArray[findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex])].MAC);
			printf("Frame #4 MAC address change: New sender MAC %s, new receiver MAC %s\n",clients.clientArray[goingClientIndex].MAC,clients.clientArray[findDestination(clients.clientArray[clientSendIndex],clients.clientArray[clientReceiverIndex])].MAC);
			
			}
			
			strcpy(clients.clientArray[clientReceiverIndex].checkIn,"Yes");
			strcpy(clients.clientArray[clientReceiverIndex].checkOut,"No");
			}
	}
		else if(startsWith("PRINT_LOG",line)){
			if ((pos=strchr(line, '\n')) != NULL){
			*pos = '\0';
    	}
    		time_t clk = time(NULL);
    		char showLog;
    		int showLogClientIndex;
    		int logEntryNumber = 0;
    		printf("---------------------------------\n");
    		printf("Command: %s\n",line);
    		printf("---------------------------------\n");
			strcpy(printLog,line);
			char *tokenLog = strtok(printLog," ");
			
			tokenLog = strtok(NULL," ");
			showLog = tokenLog[0];
			showLogClientIndex = findClient(showLog);
			printf("Client %c Logs: \n",clients.clientArray[showLogClientIndex].name);
			printf("--------------\n");
			if(strcmp(clients.clientArray[showLogClientIndex].checkIn,"Yes")==0){
				logEntryNumber++;
				printf("Log Entry #%d\n",logEntryNumber);
				printf("Timestamp:  %s",ctime(&clk));
				printf("Message: %s\n",message);
				printf("Number of frames: %d\n",frameNumber);
				printf("Number of hops: %d\n",clients.clientArray[showLogClientIndex].hops);
				printf("Sender ID: %c\n",clients.clientArray[findClient(senderClient)].name);
				printf("Receiver ID: %c\n",clients.clientArray[clientReceiverIndex].name);
				printf("Activity: Message Received\n");
				printf("Success: Yes\n");
				printf("--------------\n");
			}
			if(strcmp(clients.clientArray[showLogClientIndex].checkOut,"Yes")==0){
				logEntryNumber++;
				printf("Log Entry #%d\n",logEntryNumber);
				printf("Timestamp: %s",ctime(&clk));
				printf("Message: %s\n",message);
				printf("Number of frames: %d\n",frameNumber);
				printf("Number of hops: %d\n",clients.clientArray[showLogClientIndex].hops);
				printf("Sender ID: %c\n",clients.clientArray[findClient(senderClient)].name);
				printf("Receiver ID: %c\n",clients.clientArray[clientReceiverIndex].name);
				printf("Activity: Message Forwarded\n");
				printf("Success: Yes\n");
				printf("--------------\n");
			}
    	
			
			
		}
		else if(containNumber(line) == 0){
			commandNumber = line[0];
		}
		else{
			printf("---------------------------------\n");
    		printf("Command: %s\n",line);
    		printf("---------------------------------\n");
			printf("invalid command\n");
		}
	}
		
	}

void addNewClientToList(clientList *list,client clnt){
	if (!list->clientCount)
	{
		list->clientArray = (client *)malloc(sizeof(clnt));
	}
	else
	{
		list->clientArray = (client *)realloc(list->clientArray, sizeof(clnt) * (list->clientCount + 1));
	}
	list->clientArray[list->clientCount] = clnt;
	list->clientCount++;
	
}	

void readClients(FILE *f){
	client newClient = { 0 };
	char clientNumber;
	char *pos;
	int i;
	char a;
	char line[MAX_SIZE_STRING];
	clientNumber = getc(f);
	a = getc(f);
	while(fgets(line,sizeof(line),f)){
		
		char *token = strtok(line," ");
		newClient.name = *token;
		
		token = strtok(NULL," ");
		strcpy(newClient.portNumber,token);
		
		token = strtok(NULL," ");
		if ((pos=strchr(token, '\n')) != NULL){
			*pos = '\0';
    	}
		strcpy(newClient.MAC,token);
		addNewClientToList(&clients,newClient);
		
	}
	}	
	

void readRouting(FILE *f){
	int k;
	int i = 0;
	int j = 0;
	char temp[MAX_SIZE_STRING];
	int clientCounter = 0;
	char line[MAX_SIZE_STRING];

	while(fgets(line,sizeof(line),f)!= NULL){
		if(!startsWith("-",line)){
		
		char *tokenKey = strtok(line," ");
		clients.clientArray[clientCounter].targetArray[i] = tokenKey[0];
		
				
		tokenKey = strtok(NULL," ");
		clients.clientArray[clientCounter].destArray[i] = tokenKey[0];
		i++;
		if(i == 4){
			i = 0;
		}
		}	
		else if(startsWith("-",line)){
			//printf("asd\n");
			clientCounter++;
		}
	
	}
		
	
	
}
int main(int argc,char * argv[])
{
	FILE *file1 = fopen(argv[1], "r"); 
	FILE *file2 = fopen(argv[2], "r");
    FILE *file3 = fopen(argv[3], "r");
    double max_msg_size = atoi(argv[4]);
	char *outgoing_port = argv[5];
	char *incoming_port = argv[6];
	readClients(file1);
	readRouting(file2);
	readCommands(file3,max_msg_size,outgoing_port,incoming_port);

}
