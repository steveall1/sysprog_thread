#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define KEY_NUM 3112

#define MSQ1_KEY_NUM 1111
#define MSQ2_KEY_NUM 2222
#define MSQ3_KEY_NUM 3333

#define PID1 1
#define PID2 2
#define PID3 3

#define SEND 2
#define RCV 1

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
    	char buf_text[S_SIZE];
 
        key_t key1 = MSQ1_KEY_NUM; // For 3 Client
        key_t key2 = MSQ2_KEY_NUM;
        key_t key3 = MSQ3_KEY_NUM;

        int que_id1 = msgget(key1, IPC_CREAT|0666); // Build 3 MsgQueue
        int que_id2 = msgget(key2, IPC_CREAT|0666);
    	int que_id3 = msgget(key3, IPC_CREAT|0666);

    	MsgType msg;
	    int msg_size = sizeof(msg) - sizeof(msg.mtype);

    	ChatLog *chatlog;
    	int size = sizeof(ChatLog) * LEN;

        int shm_id;

        if(-1 == (shm_id = shmget((key_t)KEY_NUM , size , IPC_CREAT|0666))){
            printf("shmget fail!\n");        
            return -1;
        }

        if((void*)-1 == (chatlog = (ChatLog *)shmat(shm_id, (void *)0, 0))){       
            printf("shmat fail!\n");
            return -1;
        }

        printf("===================================\n");
        printf("|             Chat-Log            |\n");
        printf("===================================\n");
        printf("[PID] : [MESSAGE]\n");
        printf("-----------------------------\n");

    	while(1){
    		if(msgrcv(que_id1, &msg, msg_size, RCV, IPC_NOWAIT) != (ssize_t)-1){
    			/* When target process exist */
                if(msg.to == PID1){
    				msg.mtype = SEND;	
                    msg.from = PID1;
		    		msgsnd(que_id1, &msg, msg_size, IPC_NOWAIT);
                }else if(msg.to == PID2){
			    	msg.mtype = SEND;	
                    msg.from = PID1;
    				msgsnd(que_id2, &msg, msg_size, IPC_NOWAIT);
		    	}else if(msg.to == PID3){
                    msg.mtype = SEND;
                    msg.from = PID1;
                    msgsnd(que_id3, &msg, msg_size, IPC_NOWAIT);
    			}else{
    			/* Just write on log */	
	    			chatlog[log_cnt].PID = PID1;
	    			strcpy(chatlog[log_cnt].msg_text, msg.mtext);
    				log_cnt++;
    				printf("[1] : %s\n", msg.mtext);
                }    
            }else if(msgrcv(que_id2, &msg, msg_size, RCV, IPC_NOWAIT) != (ssize_t)-1){
           	    /* when target process exist */
                if(msg.to == PID1){
			    	msg.mtype = SEND;
                    msg.from = PID2;
			    	msgsnd(que_id1, &msg, msg_size, IPC_NOWAIT);
                }else if(msg.to == PID2){
				    msg.mtype = SEND;	
                    msg.from = PID2;
				    msgsnd(que_id2, &msg, msg_size, IPC_NOWAIT);
			    }else if(msg.to == PID3){
                    msg.mtype = SEND;
                    msg.from = PID2;
                    msgsnd(que_id3, &msg, msg_size, IPC_NOWAIT);
			    }else{
    			/* Just write on log */	
				    chatlog[log_cnt].PID = PID2;
    				strcpy(chatlog[log_cnt].msg_text, msg.mtext);
	    			log_cnt++;
		    		printf("[2] : %s\n", msg.mtext);
                }      
            }else if(msgrcv(que_id3, &msg, msg_size, RCV, IPC_NOWAIT) != (ssize_t)-1){        
      			/* when target process exist */
                if(msg.to == PID1){
	    			msg.mtype = SEND;	
                    msg.from = PID3;
    				msgsnd(que_id1, &msg, msg_size, IPC_NOWAIT);
                }else if(msg.to == PID2){
    				msg.mtype = SEND;	
                    msg.from = PID3;
    				msgsnd(que_id2, &msg, msg_size, IPC_NOWAIT);
    			}else if(msg.to == PID3){
                    msg.mtype = SEND;
                    msg.from = PID3;
                    msgsnd(que_id3, &msg, msg_size, IPC_NOWAIT);
	    		}else{
    			/* Just write on log */	
		    		chatlog[log_cnt].PID = PID3;
    				strcpy(chatlog[log_cnt].msg_text, msg.mtext);
    				log_cnt++;
    				printf("[3] : %s\n", msg.mtext);
                }    
            }else;
        }
}


