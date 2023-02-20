#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<stdbool.h>
#include<pthread.h>

//-----------------------------------VARIABLES----------------------------------------------

	char *file_names[10];
	int fileIndex[10]; 
	int in = 0;
	int r_pipe, w_pipe;
	char response_text[3];
	pthread_t thr[5];
	pthread_mutex_t lock;
//------------------------------------SEPERATE COMMANDS--------------------------------------------
void seperateArray(char *commandString, char **Temp){
		int i;
		for (i = 0; i < 100; i++) {
			Temp[i] = strsep(&commandString, " "); 	//alınan stringi " " boşluklarına göre ayırır. Temp değişkenine atıyor.

			if (Temp[i] == NULL) 			// stringin sonuna ulaştığımızı null öperatöründen anlayıp çıkıyoruz.
				break;
			if (strlen(Temp[i]) == 0)
				i--;
		}
}

//--------------------------------------------FUNCTIONS----------------------------------------

void *createFile(char *parsed){
pthread_mutex_lock(&lock);
	bool ekle = false;
	bool isThere = false;
	FILE *fp;
	int size = strlen(parsed);
	char fileName[size];
	strcpy(fileName,parsed);
	fileName[size-1] = '\0';
	parsed[size-1]= '\0';	

	printf("dosya ismi = %s\n", parsed);
	for(int i=0; i< 10; i++){
		if(!strcmp(file_names[i],fileName)){
			printf("\n böyle bir dosya zaten var. Dosya eklenemedi. \n");
			strcpy(response_text,"01");
			write(w_pipe, response_text, strlen(response_text)+1);
			ekle = false;
			isThere = true;
			break;
		}
		if(!strcmp(file_names[i],"null")){
			in = i;
			ekle = true;
			break;
		}
	}
	
	if(!isThere && !ekle){
		printf("Yeterli alan kalmadı. Birkaç dosya siliniz.\n");
		strcpy(response_text,"02");
		write(w_pipe, response_text, strlen(response_text)+1);
	}
	
	if(ekle){
	strncpy(file_names[in],fileName,size);
	for(int k =0; k<10;k++)
	{
		printf("%d : = %s \n",(k+1),file_names[k]);
	}

	fp = fopen(parsed,"a+");
	strcpy(response_text,"03");
	write(w_pipe, response_text, strlen(response_text)+1);
	fclose(fp);
	}
	pthread_mutex_unlock(&lock);
}

void *deleteFile(char *parsed){
pthread_mutex_lock(&lock);
	int size = strlen(parsed);
	char fileName[size];
	strcpy(fileName,parsed);
	fileName[size-1] = '\0';
	parsed[size-1]= '\0';	
	
	bool isThere= false;
	FILE *fp;
	int status;
	
	for(int i =0; i< 10; i++){
	
		if(!strcmp(file_names[i],fileName)){ 
			status = remove(fileName);
			if(status == 0){
				printf("dosya başarı ile silindi. \n");
				strcpy(response_text,"04");
				write(w_pipe, response_text, strlen(response_text)+1);
				isThere = true;
				strncpy(file_names[i],"null",sizeof("null"));
			}
			break;
		}	
	}
	if(!isThere){
		printf(" böyle bir dosya yok, dosya silinemedi.\n" );
		strcpy(response_text,"05");
		write(w_pipe, response_text, strlen(response_text)+1);
	}
	
	for(int k =0; k<10;k++)
	{
		printf("%d : = %s \n",(k+1),file_names[k]);
	}
	pthread_mutex_unlock(&lock);
}

void *readFile(char *parsed){
pthread_mutex_lock(&lock);
	//strcpy(response_text,"-1");
	//write(w_pipe, response_text, strlen(response_text)+1);
	long numbytes;
	int size = strlen(parsed);
	char fileName[size];
	strcpy(fileName,parsed);
	fileName[size-1] = '\0';
	parsed[size-1]= '\0';
		
	bool isThere = false;
	FILE *fp;
	char *file_text;
	
	for(int i =0;i<10;i++){
	
		if(!strcmp(file_names[i],fileName) ){
			isThere= true;
			break;
		}
	}
	if(isThere){
		
		fp = fopen(fileName,"r");
		
		fseek(fp,0L, SEEK_END);
		numbytes = ftell(fp);

		fseek(fp,0L,SEEK_SET);
		
		file_text = (char*)calloc(numbytes,sizeof(char));
		
		if(file_text == NULL){
			printf("memory yer açılamadı. \n");
			return;
		}
		
		fread(file_text, sizeof(char),numbytes,fp);
		printf("\n %s", file_text);
		
		printf("Dosya başarı ile okundu. \n"); 
		strcpy(response_text,"08");
		write(w_pipe, response_text, strlen(response_text)+1);
		write(w_pipe, file_text, strlen(file_text)+1);
		free(file_text);
	}else { 
		printf("Böyle bir dosya yok. \n"); 
		strcpy(response_text,"06");
		write(w_pipe, response_text, strlen(response_text)+1);
	}
	pthread_mutex_unlock(&lock);

}

void *writeFile( char **seperated, char *parsed){
pthread_mutex_lock(&lock);
printf("yazma fonksiyonu çalıştı. \n ");

	FILE *fp;
	bool isThere = false;
	int size = strlen(seperated[1]);
	char fileName[size];
	strcpy(fileName,seperated[1]);
	parsed[size-1]= '\0';	
	

	for(int k =0;k<10;k++){
		if(!strcmp(file_names[k],fileName)){
			isThere= true;
			break;
		}
	}
	
	if(isThere){
	fp = fopen(fileName,"a+");
	
		int i=2;
		while(seperated[i] != NULL){
			fputs(seperated[i],fp);
			fputs(" ",fp);
			i++;
		}
		printf("Başarıyla dosyaya yazıldı. \n");
		strcpy(response_text,"09");
		write(w_pipe, response_text, strlen(response_text)+1);
		fclose(fp);
	}else{ 	
		printf("Yazmak istenilen dosya listede mevcut değil. \n"); 
		strcpy(response_text,"07");
		write(w_pipe, response_text, strlen(response_text)+1);
		
		}
		pthread_mutex_unlock(&lock);
}





void *checkCommand(char **temp){
	pthread_mutex_lock(&lock);
if(!strcmp(temp[0],"create") && temp[1] != NULL){
	pthread_create(&thr[1],NULL,createFile,temp[1]);
}else if(!strcmp(temp[0],"delete") && temp[1] != NULL){
	pthread_create(&thr[2],NULL,deleteFile,temp[1]);
}else if(!strcmp(temp[0],"read") && temp[1] != NULL){
	pthread_create(&thr[3],NULL,readFile,temp[1]);
}else if(!strcmp(temp[0],"write") && temp[1] != NULL && temp[2] != NULL){
	pthread_create(&thr[4],NULL,writeFile,temp);
}else if(!strcmp(temp[0],"exit\n")){
	printf("Çıkış yapılıyor\n");
	strcpy(response_text,"10");
	write(w_pipe, response_text, strlen(response_text)+1);
	close(r_pipe);
	exit(0);
}else{ 	
	strcpy(response_text,"00");
	write(w_pipe, response_text, strlen(response_text)+1);
	printf("Yanlış yada eksik komut kullandınız. \n");
	}
	pthread_mutex_unlock(&lock);
}


//-----------------------------------------------MAIN------------------------------------------

int main(){
	pthread_mutex_init(&lock, NULL);
	for(int i = 0; i<10;i++){
	 file_names[i] = (char*)malloc(25);
	 strncpy(file_names[i],"null",sizeof("null"));
	}
	
	char * myPipeline = "client_manager";
	char * responsePipe = "response";
	char commandString[1000], *Temp[100];

	mkfifo(myPipeline,0666);
	mkfifo(responsePipe,0666);
		
	pthread_join(thr[0],NULL);	
	pthread_join(thr[1],NULL);	
	pthread_join(thr[2],NULL);	
	pthread_join(thr[3],NULL);	
	pthread_join(thr[4],NULL);	
	
	while (1){
 		r_pipe = open(myPipeline, O_RDONLY);
 		w_pipe = open(responsePipe,O_WRONLY);	
		read(r_pipe, commandString, sizeof(commandString));	
		seperateArray(commandString,Temp);
		pthread_create(&thr[0],NULL,checkCommand,Temp);
		close(r_pipe);
	}
	pthread_mutex_destroy(&lock);
	return 0;
}











