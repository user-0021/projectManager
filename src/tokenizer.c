#include <string.h>
#include <stdlib.h>
#include <tokenizer.h>
#include <stdio.h>

#define RESURVED_CHAR "[]=,"
#define INVALIED_CHAR " \r\t"
#define COMENT_OUT_CHAR "#"
#define PUNCTUATION_CHAR "\n"

TOKEN* tokenize(char* str){
	if(str == NULL)
		return NULL;

	//make head
	TOKEN* head, *now;
	head = malloc(sizeof(TOKEN));
	memset(head,0,sizeof(TOKEN));
	now = head;
	
	
	while((*str) != '\0'){
		if(strchr(RESURVED_CHAR,*str) != NULL){
			// resurved type
			TOKEN* new;
			new = malloc(sizeof(TOKEN));
			memset(new,0,sizeof(TOKEN));
			new->type = TOKEN_RESURVED;
			new->str = str;

			now->next = new;
			now = now->next;
		}else if(strchr(COMENT_OUT_CHAR,*str) != NULL){
			while (((*str) != '\n') && (*str) != '\0')
				str++;
			
			str--;
		}else if(strchr(PUNCTUATION_CHAR,*str) != NULL){
			// resurved type
			TOKEN* new;
			new = malloc(sizeof(TOKEN));
			memset(new,0,sizeof(TOKEN));
			new->type = TOKEN_PUN;
			new->str = str;

			now->next = new;
			now = now->next;
		}else if(strchr(INVALIED_CHAR,*str) == NULL){
			//check invalid
			if((*str) == '\\' && (*(str + 1)) == '\n'){
				str++;
				continue;
			}

			//str type
			unsigned int len = 1;
			while (1)
			{
				char* iter = (str + len);
				if(strchr(RESURVED_CHAR,*iter) || strchr(INVALIED_CHAR,*iter) || strchr(COMENT_OUT_CHAR,*iter) || strchr(PUNCTUATION_CHAR,*iter)){
					break;
				}
				
				if(*iter == '\\' && *(iter+1) == '\n'){
					len += 2;
					continue;
				}

				len++;
			}
			


			TOKEN* new;
			new = malloc(sizeof(TOKEN));
			memset(new,0,sizeof(TOKEN));
			new->type = TOKEN_STR;
			new->str = malloc(len + 1);
			memset(new->str,'\0',len + 1);
			memcpy(new->str,str,len);

			str += (len - 1);
			now->next = new;
			now = now->next;
		}

		str++;
	}


	TOKEN* new;
	new = malloc(sizeof(TOKEN));
	memset(new,0,sizeof(TOKEN));
	new->type = TOKEN_EOF;
	now->next = new;
	now = now->next;

	return head;
}