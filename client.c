#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


char* readCommands()
{
	char *line = (char *)malloc(sizeof(char) * 1024); 
	char read;
	int position = 0 ;
	int bufferSize = 1024;
	if (!line){ // Fail
		printf("\n Buffer Could Not Allocated."); 
		exit(1); 
	}
	while(1){
		read=getchar();
		if (read == EOF || read == '\n'){   
			line[position] = '\0';
			return line;		
		}
		else{
			line[position] = read;
		}
		position ++;
		
		if (position >= bufferSize){
			bufferSize += 1024;
			line = realloc(line, sizeof(char) * bufferSize);
			if (!line){ // Fail
			printf("\nBuffer Could Not Allocated");
			exit(1); 
			}
		}
	}

}
//-----------------------------------------------VARIABLES---------------------------------------------------


char response_text[3];
int response;
int buff_size;
char *file_text;

char read_text[100];

//--------------------------------------------------------------------------------------------------------------
int main()
{
	
    printf("Client'a bağlanıldı.. \n");

    int pipe; 				 
    char * myPipeline = "client_manager";
    char * responsePipe = "response";
    
    mkfifo(myPipeline,0666);	
    mkfifo(responsePipe,0666);
    
    char commandString[1000];
    while (1)
    {	
    	
   	printf("Client listening > ");
	char *readLine;
									
	pipe = open(myPipeline,O_WRONLY);
	response = open(responsePipe, O_RDONLY);

	    	
    	fgets(commandString, 1000, stdin);
    	
    	write(pipe, commandString, strlen(commandString)+1);
  	read(response,response_text,sizeof(response_text));
    	checkCode();
    	
	close(pipe);
	close(response);
    }
    
    return 0;
    
}



void checkCode(){

//printf("%s", response_text);

if(response_text != NULL){
	if(!strcmp(response_text,"00")){//----------------------------------------------------
		printf("Yanlış kada eksik komut kullandınız. \n");
	}else if(!strcmp(response_text,"01")){//----------------------------------------------
		printf("Dosta listede zaten mevcut. Tekrar oluşturulamaz.\n");
	}else if(!strcmp(response_text,"02")){//----------------------------------------------
		printf("Listede yeterli alan yok. Ekelenemz.  \n");
	}else if(!strcmp(response_text,"03")){//----------------------------------------------
		printf("Dosya başarı ile oluşturuldu. \n");
	}else if(!strcmp(response_text,"04")){//----------------------------------------------
		printf("Başarı ile silindi. \n");
	}else if(!strcmp(response_text,"05")){//----------------------------------------------
		printf("Listede silmek istenilen dosya yok. \n");
	}else if(!strcmp(response_text,"06")){//----------------------------------------------
		printf("Okunmak istenen dosya listede yok. \n");
	}else if(!strcmp(response_text,"08")){
		printf("dosya okundu. \n");
		read(response,read_text,sizeof(read_text));
		printf("%s", read_text);
	}else if(!strcmp(response_text,"07")){//----------------------------------------------
		printf("Yazılmak istenen dosya listede yok. \n");
	}else if(!strcmp(response_text,"09")){//----------------------------------------------
		printf("Başarı ile dosyaya yazıldı.\n");
	}else if(!strcmp(response_text,"10")){//----------------------------------------------
		printf("Çıkış yapılıyor. \n");
		exit(0);
	}

}
}








