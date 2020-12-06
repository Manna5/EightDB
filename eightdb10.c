#include <stdio.h>
#include <string.h>

typedef struct {
	char name[32];
	char value[256];
} edb_record;

edb_record database[128];

int find_free_record(){
	int i;
	for(i=0;i<128;i++){
		if(!strlen(database[i].name)){
			return i;
		}
	}
	return 127;
}

int find_specified_record(char* specifier){
	int i;
	int find_by_id=*specifier=='#'?atoi(specifier+1):0;
	if(find_by_id){
		return find_by_id-1;
	}
	for(i=0;i<128;i++){
		if(!strcmp(database[i].name,specifier)){
			return i;
		}
	}
}

int main(){
	int working=1;
	char command[480];
	int i,j;
	char listhead[256]={0};
	char listfoot[256]={0};
	puts("EightDB 1.0 by Jan Mleczko\nType \"help\" and press Enter for more information.\n");
	while(working){
		putchar('>');
		gets(command);
		if(!strncmp(command,"exit",4)){
			puts("Goodbye.");
			working=0;
		}else if(!strncmp(command,"init",4)){
			for(i=0;i<128;i++){
				for(j=0;j<256;j++){
					database[i].value[j]=0;
					if(j<32){
						database[i].name[j]=0;
					}
				}
			}
			puts("Database initialized.\n");
		}else if(!strncmp(command,"list",4)){
			if(strlen(listhead)){
				puts(listhead);
			}
			if(command[4]==' '){
				for(i=0;i<128;i++){
					if(!strcmp(database[i].name,command+5)){
						printf("#%d\t%s\t%s\n",i+1,database[i].name,database[i].value);
					}
				}
			}else{
				for(i=0;i<128;i++){
					if(strlen(database[i].name)){
						printf("#%d\t%s\t%s\n",i+1,database[i].name,database[i].value);
					}
				}
			}
			if(strlen(listfoot)){
				puts(listfoot);
			}else putchar('\n');
		}else if(!strncmp(command,"add ",4)){
			char* value=strchr(command+4,' ')+1;
			int target=find_free_record();
			*(value-1)=0;
			strcpy(database[target].name,command+4);
			strcpy(database[target].value,value);
			printf("Record %s added at #%d.\n\n",command+4,target+1);
		}else if(!strncmp(command,"update ",7)){
			char* value=strchr(command+7,' ')+1;
			*(value-1)=0;
			int target=find_specified_record(command+7);
			strcpy(database[target].value,value);
			printf("Updated record %s.\n\n",command+7);
		}else if(!strncmp(command,"delete ",7)){
			int target=find_specified_record(command+7);
			database[target].name[0]=0;
			printf("Deleted record %s.\n\n",command+7);
		}else if(!strncmp(command,"search ",7)){
			for(i=0;i<128;i++){
				if(!strcmp(database[i].value,command+7)){
					printf("#%d\t%s\t%s\n",i+1,database[i].name,database[i].value);
				}
			}
		}else if(!strncmp(command,"head ",5)){
			strcpy(listhead,command+5);
		}else if(!strncmp(command,"foot ",5)){
			strcpy(listfoot,command+5);
		}else if(!strncmp(command,"help",4)){
			puts("init                  Initialize database (delete all records).");
			puts("list                  List all records.");
			puts("list <name>           List records with specified name.");
			puts("add <name> <value>    Add record.");
			puts("update <name> <value> Update record.");
			puts("update #<id> <value>  Update record specified by ID.");
			puts("delete <name>         Delete record.");
			puts("delete #<id>          Delete record specified by ID.");
			puts("search <value>        Search for record.");
			puts("head <text>           Set header (for list command).");
			puts("foot <text>           Set footer (for list command).");
			puts("help                  Display help.");
			puts("exit                  Exit EightDB console.\n");
		}else{
			printf("Uknown command: %s\n\n",command);
		}
	}
	return 0;
}
