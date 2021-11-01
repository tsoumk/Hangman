#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define WORDLEN 30

/* domh sthn opoia apothikevetai mia lexi apo to lexiko */
typedef struct list1_node {
	char lexi_listas[WORDLEN];
	struct list1_node *next;
} lexiko_list;

/* domi sthn opoia apothikevetai mia katigoria lexewn */
typedef struct list2_node {
	char antip_node[WORDLEN];
	int size_node;
	lexiko_list **pinakas;
	struct list2_node *next;
} katig_list;


int get_length(int l);
int get_orismata(int *length, int *turns, char *filename, int argc, char *argv[]);
void free_lexiko_list(lexiko_list *list_head);
lexiko_list *get_lexiko_list(int length, char dictionary[]);
katig_list *get_katig_list(lexiko_list *lexiko_list, char word[]);
void free_katig_list(katig_list *list_head);
void make_letters(char used_let[], char unused_let[], char c);
katig_list *find_max_katig(katig_list *katig_root_curr, char c);
katig_list *find_node(katig_list *katig_root, char antip_node[]);
void make_antip(char c, char lexh_node[], char antip_node[]);
char *getWord(char *filename);

/*******************************************
NA SVHSTEI H EPOMENH SYNARTHSH    **********/
void printlist(katig_list *root) {
	int i;

	if (root==NULL) return;
	printf("%s,%d\n",root->antip_node, root->size_node);
	for (i=0; i<root->size_node; i++) {
		printf("%s\t",root->pinakas[i]->lexi_listas);
	}
	printf("\n");
	printlist(root->next);
}

int main (int argc, char *argv[])
{
	int length;	/* mikos lexhs (1o orisma */
	int turns;  /* arithmos gyrwn (2o orisma) */
	char dictionary[100]; /* onoma arxeiou (3o orisma) */
	lexiko_list *lexiko_root;	/* kefalh ths listas lexikoy */
	int stop=0;	/* metavliti elegxou an to paixnidi synexizetai */
	char word[WORDLEN];	/* krymenh lexh */
	katig_list *katig_root;	/* kefalh listas katigoriwn */
	int rem_turns;	/* gyroi pou apomenoun */
	char c;
	char used_let[53],unused_let[53]; /* pinakes used kai unused grammatwn */

	if (get_orismata(&length, &turns, dictionary, argc, argv)==0)
	{
		if (argc<4)
		{
			printf("Insufficient arguments.\n");
		} else
		{
			printf("Too many arguments.\n");
		}
		printf("./hangman LENGTH TURNS DICTIONARY\n");
		printf("\tLENGTH: requested length of mystery word. Must be >1, <30, !=26 !=27\n");
		printf("\tTURNS: requested number of turns. Must be >0\n");
		printf("\tDICTIONARY: name of dictionary file\n");
	} else
	{
		lexiko_root=get_lexiko_list(length, dictionary);	/* dhmioyrgia listas lexikou */
		while (stop==0)
		{
			/* arxikopoihsh olwn twn metavlitwn tou paixndiou */
			rem_turns=turns;
			/* arxiki mystikh lexh kai arxikos antiproswpos */
			strcpy(word,"_____________________________");
			word[length]='\0';
			/* string grammatwn */
			strcpy(used_let,"");
			strcpy(unused_let,"a b c d e f g h i j k l m n o p q r s t u v w x y z ");
			katig_root=get_katig_list(lexiko_root, word);	/* dhmiourgia arxikou kombou katigoriwn lexewn */
			do {
				do {
					printf("Remaining guesses: %d\n",rem_turns);
					printf("Used letters: %s\n",used_let);
					printf("Unused letters: %s\n",unused_let);
					printf("Word: %s\n",word);
					do {
						printf("Guess a letter: ");
						scanf(" %c",&c);
					} while (! isalpha(c));
					c=tolower(c);
					if ( strchr(unused_let,c) == NULL)
					{
						printf("You have already guessed this letter\n");
					}
				} while(strchr(unused_let,c) == NULL);
				make_letters(used_let,unused_let,c);
				katig_root=find_max_katig(katig_root, c);	/* vriskei thn epomenh kalyterh kathgoria */
				strcpy(word, katig_root->antip_node);
				printf("Category size: %d\n",katig_root->size_node);
				/* elegxos gia nikiti */
				if (strcmp(word,katig_root->pinakas[0]->lexi_listas)==0) {
					printf("YOU WON! The word is %s\n",word);
					break;
				}
				rem_turns--;
			} while (rem_turns>0);
			if (rem_turns==0) {
				printf("YOU LOST! The word was %s\n",katig_root->pinakas[0]->lexi_listas);
			}
			free_katig_list(katig_root); /* apodesmeush ths mnhmhs */
			/* rwta an thelei na paixei xana */
			printf("\n");
			printf("Play again? (y/n): ");;
			scanf(" %c",&c);
			c=tolower(c);
			while (c!='y' && c!='n')
			{
				printf("Please enter y for yes or n for no\n");
				printf("Play again? (y/n): ");
				scanf(" %c",&c);
				c=tolower(c);
			}
			if (c=='n') stop=1;
		}

		free_lexiko_list(lexiko_root);	/* apodesmeush mnhmhs listas lexikou */
	}
	return 0;
}

/* me bash to gramma dhmioyrgei ton antiproswpo ths lexhs */
void make_antip(char c, char lexh_node[], char antip_node[])
{
	int i;

	for (i=0; i<strlen(lexh_node); i++)
	{
		if (lexh_node[i]==c)
		{
			antip_node[i]=c;
		}
	}
}

/* vriskei an o antiproswpos mias lexhs exei apothikeuthei sth lista */
katig_list *find_node(katig_list *katig_root, char antip_node[])
{
	katig_list *curr;

	for (curr=katig_root; curr != NULL; curr=curr->next)
	{
		if (strcmp(curr->antip_node,antip_node)==0) break;
	}

	return curr;
}


/* dhmioyrgei tis nees katigories lexewn kai vriskei ayth me to megalytero arithmo lexewn */
katig_list *find_max_katig(katig_list *katig_root_curr, char c) {
	int n,i,pin_pos, max_node;
	lexiko_list **pinakas;
	char *lexh_node;
	char antip_node[WORDLEN];
	katig_list *katig_root, *found_node, *new_node, *curr;;

	n=katig_root_curr->size_node;
	pinakas=katig_root_curr->pinakas;

	for (i=0; i<n; i++)
	{
		lexh_node=pinakas[i]->lexi_listas;
		strcpy(antip_node, katig_root_curr->antip_node);
		make_antip(c, lexh_node, antip_node);
		found_node=find_node(katig_root, antip_node);
		if (found_node != NULL)
		{
			pin_pos=found_node->size_node;
			found_node->pinakas=(lexiko_list **) realloc(found_node->pinakas,(pin_pos+1)*sizeof(lexiko_list *));
			if (new_node->pinakas==NULL)
			{
				printf("There is no memory left\n");
				exit(1);
			}
			found_node->pinakas[pin_pos]=pinakas[i];
			found_node->size_node=found_node->size_node+1;
		} else
		{
			new_node=(katig_list *) malloc(sizeof(katig_list));
			if (new_node==NULL)
			{
				printf("There is no memory left\n");
				exit(1);
			}
			strcpy(new_node->antip_node, antip_node);	/* antiproswpos */
			new_node->size_node=1;
			new_node->pinakas=(lexiko_list **) malloc(sizeof(lexiko_list *));
			if (new_node->pinakas==NULL)
			{
				printf("There is no memory left\n");
				exit(1);
			}
			new_node->pinakas[0]=pinakas[i];
			/* prosthiki neoi komvou sthn arxh ths listas */
			new_node->next=katig_root;
			katig_root=new_node;
		}
	}
	free_katig_list(katig_root_curr);	/* apodesmeysh mnhmhs trexoysas kathgorias */
/********************************
NA SVHSTEI H EPOMENH GRAMMH *****/
printlist(katig_root);

	/* vriskei to komvo me ton megalitero arithmo lexewn*/
	max_node=0;
	found_node=NULL;
	for(curr=katig_root; curr!=NULL;  curr=curr->next)
	{
		if (curr->size_node>max_node) {
			max_node=curr->size_node;
			found_node=curr;
		}
	}

	/* vriskei ton proigoumeno komvo apo ton komvo me to megalitero arithmo lexewn*/
	curr=katig_root;
	if (curr!=found_node)
	{
		for(curr=katig_root; curr->next!=NULL;  curr=curr->next)
		{
			if (curr->next==found_node) {
				break;
			}
		}
			curr->next=found_node->next;
	}	else
	{
		katig_root=found_node->next;	/* o prwtos htan o megaliteros */
	}

	found_node->next=NULL;

	free_katig_list(katig_root);	/* apodesmeush mnhmhs newn kathgoriwn lexewn */

	return found_node;
}

/* dhmoyrgei tous pinakes twn grammatwn */
void make_letters(char used_let[], char unused_let[], char c)
{
	int i,j;
	char *pos_found;

	/* vriskei ti thesi stin opoia tha prepei na mpei o neos xaraktiras */
	for(i=0; used_let[i]!='\0'; i++)
	{
		if (used_let[i]>c) break;
	}

	/* sprwxnei ta grammata pou vriskontai dexia kata 2 theseis */
	for(j=strlen(used_let); j>=i; j--) {
		used_let[j+2]=used_let[j];
	}

	used_let[i]=c;			/* vazei to xaraktira */
	used_let[i+1]=' ';		/* vazei ena keno     */

	/* afairesh apo ton pinaka twn unused */
	pos_found=strchr(unused_let, c);
	strcpy(pos_found, pos_found+2);
}

/* dhmiourgia ths arxikhs kathgorias lexewn */
katig_list *get_katig_list(lexiko_list *lexiko_root, char word[])
{
	katig_list *katig_root;

	katig_root=(katig_list *) malloc(sizeof(katig_list));
	if (katig_root==NULL)
	{
		printf("There is no memory left\n");
		exit(1);
	}
	strcpy(katig_root->antip_node, word);	/* antiproswpos */

	/* syndesh twn komvwn ths listas lexikou me tous komvous tou pinaka ths arxikhs kathogrias */
	katig_root->size_node=0;
	do
	{
		if (katig_root->size_node==0)
		{
			katig_root->pinakas=(lexiko_list **) malloc(sizeof(lexiko_list *));
		} else
		{
			katig_root->pinakas=(lexiko_list **) realloc(katig_root->pinakas,(katig_root->size_node+1)*sizeof(lexiko_list *));
		}
		if (katig_root->pinakas==NULL)
		{
			printf("There is no memory left\n");
			exit(1);
		}
		katig_root->pinakas[katig_root->size_node]=lexiko_root;
		katig_root->size_node=katig_root->size_node+1;
		lexiko_root=lexiko_root->next;
	} while (lexiko_root != NULL);

	katig_root->next=NULL;
	return katig_root;
}

/* diafrafh ths listas twn kathgoriwn apo th mnhmh */
void free_katig_list(katig_list *list_head)
{
	katig_list *list_curr;

	while (list_head != NULL)
	{
		list_curr=list_head;
		list_head=list_curr->next;
		free(list_curr->pinakas);
		free(list_curr);
	}
}


/* diafrafh ths listas tou lexikou apo th mnhmh */
void free_lexiko_list(lexiko_list *list_head)
{
	lexiko_list *list_curr;

	while (list_head != NULL)
	{
		list_curr=list_head;
		list_head=list_curr->next;
		free(list_curr);
	}
}

/* diavazei mia mia oles tis lexeis tou lexikou kai dhmioyrgei th lista tou lexikou me tis lexeis pou exoyn to swsto mikos */
lexiko_list *get_lexiko_list(int length, char dictionary[])
{
	lexiko_list *list_head=NULL, *list_new, *list_last;
	char *word;
	int stop=0;

	while (stop==0)
	{
		word=getWord(dictionary);
		if (word==NULL)
		{
			stop=1;	/* telos tou arxeiou, tha prepei na termatisei to loop */
		} else
		{
			if (strlen(word)==length)
			{
				/* h lexh exei to swsto mhkos, tha prepei na mpei sth lista */
				if (list_head==NULL)
				{
					/* dhmiourgia arxikou komvou listas */
					list_head=(lexiko_list *) malloc(sizeof(lexiko_list));
					if (list_head==NULL) {
						printf("There is no memory left\n");
						exit(1);
					}
					strcpy(list_head->lexi_listas,word);
					list_head->next=NULL;
					list_last=list_head;
				} else
				{
					/* dhmiourgia neou komvou kai prosthiki sto telos tis listas */
					list_new=(lexiko_list *) malloc(sizeof(lexiko_list));
					if (list_new==NULL) {
						printf("There is no memory left\n");
						free_lexiko_list(list_head);	/* apodesmeush ths mnhmhs */
						exit(1);
					}
					strcpy(list_new->lexi_listas,word);
					list_new->next=NULL;
					list_last->next=list_new;
					list_last=list_new;
				}

			}
		}
	}

	return list_head;
}


/* diavazei mia nea timi gia to mikos tis lexis mexri na einai entos twn epitreptwn oriwn */
int get_length(int l)
{
	int length;

	length=l;
	do {
		printf("There are no words of length %d in the dictionary.\n",length);
		printf("Specify a different length: ");
		scanf("%d",&length);
	} while (length<=1 || length>=30 || length==26 || length==27);

	return length;
}

/* epexergasia orismatwn */
int get_orismata(int *length, int *turns, char dictionary[], int argc, char *argv[])
{
	if (argc != 4) return 0;				/* lanhtasmeno plithos orismatwn */

	strcpy(dictionary,argv[3]);		/* 3o orisma=onoma arxeiou lexikou */

	/* 1o orisma = mikos lexis, an den einai entos twn epitreptwn timwn, diavazei nea timi */
	*length=atoi(argv[1]);
	if (*length==26 || *length==27)
	{
		*length=get_length(*length);
	} else
	{
		if (*length<=1)
		{
			*length=get_length(*length);
		} else
		{
			if (*length>=30)
			{
				*length=get_length(*length);
			}
		}
	}

	/* 2o orisma = arithmos gyrwn, and den einai entos twn epitreptwn timwn, diavazei nea timi */
	*turns=atoi(argv[2]);
	if (*turns<1) {
		while (*turns<1)
		{
			printf("You may not have fewer than one turns.\n");
			printf("Specify a different number of turns: ");
			scanf("%d",turns);
		}
	}

	return 1;
}

/* Given the name of a file, read and return the next word from it,
or NULL if there are no more words */

char *getWord(char *filename)  {
	char formatstr[15], *word;
	static FILE *input;
	static int firstTime = 1;
	if (firstTime) {
		input = fopen(filename, "r");
		if (input == NULL) {
			printf("ERROR: Could not open file \"%s\"\n", filename);
			exit(1);
		}
		firstTime = 0;
	}
	word = (char*)malloc(sizeof(char)*WORDLEN);
	if (word == NULL) {
		printf("ERROR: Memory allocation error in getWord\n");
		exit(1);
	}
	sprintf(formatstr, "%%%ds", WORDLEN-1);
	fscanf(input, formatstr, word);
	if (feof(input)) {
		fclose(input);
		return NULL;
	}
	return word;
}



