#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>

#define KEY_NUM 3112 // Shared Memory
#define MSQ_KEY_NUM 3333 // Msg Queue

#define SEND 1
#define RCV 2
 
#define S_SIZE 1024
#define LEN 100


typedef struct{
    	long mtype;
    	int to;
        int from;
    	char mtext[S_SIZE];
}MsgType;

typedef struct{
        int PID;
        char msg_text[S_SIZE];
}ChatLog;

int main(){
        int log_cnt = 0;
	    int option;
    	char buf_text[S_SIZE];

	    key_t key = MSQ_KEY_NUM;
        int que_id = msgget(key, IPC_CREAT|0666);

	    MsgType msg;
	    int msg_size = sizeof(msg) - sizeof(msg.mtype);

        ChatLog * chatlog;
        int size = sizeof(ChatLog) * LEN;

        int shm_id;

        if(-1 == (shm_id = shmget((key_t)KEY_NUM ,size, IPC_CREAT|0666))){
            printf("shmget fail!\n");
            return -1;
        }

        if(-1 == (chatlog = shmat(shm_id, (void *)0, SHM_RDONLY))){
                printf("shmat fail!\n");
                return -1;
        }

    	while(1){
	    	printf("=========================\n");
		    printf("Select Option\n");
    		printf("1. Message Send\n");
	    	printf("2. Message Receive\n");
    		printf("3. Confirm Message_log\n");
    		printf("4. Exit\n");
    		printf("==========================\n");
    		scanf("%d", &option);
    		printf("\n\n");

    		switch(option){
	    		case 1 : // Msg Send
    				system("clear");
    				printf("Target Process # [1][2][3] \n");
    				scanf("%d", &msg.to);
    				printf("Write your Message : \n");
    				scanf("%s", buf_text);
    				msg.mtype = SEND;				
    				strcpy(msg.mtext, buf_text);
    				msgsnd(que_id, &msg, msg_size, NULL); 
    				system("clear");
        			break;

                case 2 : // Msg Receive				
	    			while(msgrcv(que_id, &msg, msg_size, RCV, IPC_NOWAIT) != (ssize_t)-1){
                        switch(msg.from){
                            case 1:
                                printf("[1] : ");
                                break;
                            case 2:
                                printf("[2] : ");
                                break;
                            case 3:
                                printf("[3] : ");
                                break;
                            default:
                                break;
                        }
		    			printf("%s\n", msg.mtext);			                     
                    }
    				printf("\n");
	        		break;

                case 3 : // Access Shared Memory
                    printf("===================================\n");
                    printf("|             Chat-Log            |\n");
                    printf("===================================\n");
                    printf("[PID] : [MESSAGE]\n");
                    printf("-----------------------------\n");                    
                    
                    while(log_cnt < 100){
    					if(chatlog[log_cnt].PID != 0){
        					printf("[%d] : %s\n", chatlog[log_cnt].PID, chatlog[log_cnt].msg_text);
                        }
        				log_cnt++;
    				}
    				log_cnt = 0;
    				printf("\n\n");
        			break;

    			case 4 : // Exit
    				shmdt(chatlog);
    				return 0;

    			default: // Error
    				printf("Undefined Operation\n");
        			break;
    		}
        }
}


