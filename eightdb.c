#include <stdio.h>
#include <string.h>

#define record_count 512

char listhead[256]={0};
char listfoot[256]={0};
int working=1;

typedef struct {
	char name[32];
	char value[480];
} edb_record;

edb_record database[record_count];

int find_free_record(){
	int i;
	for(i=0;i<record_count;i++){
		if(!database[i].name[0]){
			return i;
		}
	}
	return 511;
}

int find_specified_record(char* specifier){
	int i;
	int find_by_id=*specifier=='#'?atoi(specifier+1):0;
	if(find_by_id){
		return find_by_id-1;
	}
	for(i=0;i<record_count;i++){
		if(!strcmp(database[i].name,specifier)){
			return i;
		}
	}
}

void print_row(int id,char* name,char* value){
	printf("#%d\t%s\t%s\n",id+1,name,value);
}

void csv_export(char* target_filename){
	FILE* fp=fopen(target_filename,"w");
	int i;
	for(i=0;i<record_count;i++){
		if(*database[i].name){
			fprintf(fp,"%s,%s\n",database[i].name,database[i].value);
		}
	}
	fclose(fp);
}

void xml_export(char* target_filename){
	FILE* fp=fopen(target_filename,"w");
	int i;
	fprintf(fp,"<database>");
	for(i=0;i<record_count;i++){
		if(*database[i].name){
			fprintf(fp,"<record><name>%s</name><value>%s</value></record>",database[i].name,database[i].value);
		}
	}
	fprintf(fp,"</database>");
	fclose(fp);
}

void json_export(char* target_filename){
	FILE* fp=fopen(target_filename,"w");
	int i,first=1;
	fprintf(fp,"{");
	for(i=0;i<record_count;i++){
		if(*database[i].name){
			if(!first){
				fputc(',',fp);
			}
			fprintf(fp,"\"%s\":\"%s\"",database[i].name,database[i].value);
			first=0;
		}
	}
	fprintf(fp,"}");
	fclose(fp);
}

void shell_run(char* command){
	int i,j;
	FILE* fp;
	FILE* script;
	if(*command==0 || *command=='%'){
	}else if(!strncmp(command,"exit",4)){
		puts("Goodbye.");
		working=0;
	}else if(!strncmp(command,"load ",5)){
		fp=fopen(command+5,"rb");
		if(!fp){
			printf("Not found %s.\n\n",command+5);
		}else{
			fread(database,sizeof(edb_record),record_count,fp);
			fclose(fp);
			puts("Database loaded.\n");
		}
	}else if(!strncmp(command,"save ",5)){
		fp=fopen(command+5,"wb");
		fwrite(database,sizeof(edb_record),record_count,fp);
		fclose(fp);
		puts("Database saved.");
	}else if(!strncmp(command,"init",4)){
		for(i=0;i<record_count;i++){
			for(j=0;j<480;j++){
				database[i].value[j]=0;
				if(j<32){
					database[i].name[j]=0;
				}
			}
		}
		puts("Database initialized.\n");
	}else if(!strncmp(command,"list",4)){
		if(*listhead){
			puts(listhead);
		}
		if(command[4]==' '){
			for(i=0;i<record_count;i++){
				if(!strcmp(database[i].name,command+5)){
					print_row(i,database[i].name,database[i].value);
				}
			}
		}else{
			for(i=0;i<record_count;i++){
				if(strlen(database[i].name)){
					print_row(i,database[i].name,database[i].value);
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
		printf("Record %s added at #%d.\n",command+4,target+1);
	}else if(!strncmp(command,"update ",7)){
		char* value=strchr(command+7,' ')+1;
		*(value-1)=0;
		int target=find_specified_record(command+7);
		strcpy(database[target].value,value);
		printf("Updated record %s.\n",command+7);
	}else if(!strncmp(command,"delete ",7)){
		int target=find_specified_record(command+7);
		database[target].name[0]=0;
		printf("Deleted record %s.\n",command+7);
	}else if(!strncmp(command,"search ",7)){
		for(i=0;i<128;i++){
			if(!strcmp(database[i].value,command+7)){
				print_row(i,database[i].name,database[i].value);
			}
		}
	}else if(!strncmp(command,"head ",5)){
		strcpy(listhead,command+5);
	}else if(!strncmp(command,"foot ",5)){
		strcpy(listfoot,command+5);
	}else if(!strncmp(command,"export csv:",11)){
		csv_export(command+11);
		puts("CSV Export done.");
	}else if(!strncmp(command,"export xml:",11)){
		xml_export(command+11);
		puts("XML Export done.");
	}else if(!strncmp(command,"export json:",12)){
		json_export(command+12);
		puts("JSON Export done.");
	}else if(!strncmp(command,"info ",5)){
		fprintf(stderr,"%s\n",command+5);
	}else if(!strncmp(command,"submit ",7)){
		script=fopen(command+7,"r");
		char scriptline[640]={0};
		char run;
		run=1;
		if(script<1){
			printf("Not found %s.\n",command+7);
		}else{
			while(run && !feof(script)){
				fgets(scriptline,640,script);
				for(i=0;i<strlen(scriptline);i++){
					if(scriptline[i]=='\n' || scriptline[i]=='\r'){
						scriptline[i]=0;
					}
				}
				if(!strcmp(scriptline,"exit")){
					run=0;
				}else shell_run(scriptline);
			}
		}
	}else if(!strncmp(command,"help",4)){
		puts("init                       Initialize database (delete all records).");
		puts("list                       List all records.");
		puts("list <name>                List records with specified name.");
		puts("add <name> <value>         Add record.");
		puts("update <name> <value>      Update record.");
		puts("update #<id> <value>       Update record specified by ID.");
		puts("delete <name>              Delete record.");
		puts("delete #<id>               Delete record specified by ID.");
		puts("search <value>             Search for record.");
		puts("head <text>                Set header (for list command).");
		puts("foot <text>                Set footer (for list command).");
		puts("load <filename>            Load database from file.");
		puts("save <filename>            Save database in a file, .edb extension recommended.");
		puts("export <format>:<filename> Export database. Format can be csv, xml or json.");
		puts("help                       Display help.");
		puts("submit <filename>          Run script, .esc extension recommended.");
		puts("info <text>                Display message (like echo in shell).");
		puts("exit                       Exit EightDB console.\n");
	}else{
		printf("Uknown command: %s\n\n",command);
	}
}

int main(){
	char command[640];
	puts("EightDB 3.0 by Jan Mleczko\nType \"help\" and press Enter for more information.\n");
	while(working){
		putchar('>');
		gets(command);
		shell_run(command);
		putchar('\n');
	}
	return 0;
}
