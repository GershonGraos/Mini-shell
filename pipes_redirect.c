#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

char** readTokens(FILE* stream);
void freeTokens(char** tokens);
void verifyPipeRedirection(char** tokens);
void divideTokenPipe(char** tokens);
void rigthHetz(char** tokens); 
void leftHetz(char** tokens);
void tokenRedirection(char** tokens);

int is_Pipe_Redirection = 0; 
int litlle_redir = 0;
int isExit = 0;  //This parameter tell whether exit was clicked
int lengthTok = 0;  // size of token

//******************************************************************************
//******************************************************************************

int main()
{
        char** tok;
        char* user;  // user name
        char host[50];  // computer name 
        host[50]='\0';  // last char
        int status;  // the status of the process
        pid_t child_pid;
        
        user = getlogin();  //getting the user name
        gethostname(host,sizeof(host));  //getting the host name
        
        if(!user)  //impossible user name
        {
                perror("login error");
                exit(-1);
        }
        
        if(!host)  //impossible host name
        {
                perror("hostname error");
                exit(-1);
        }
        
        signal(SIGINT,SIG_IGN);  // ignoring Ctrl+c
        while(isExit == 0) // the program run until the input is 'exit'
        {
	      printf("\n%s@%s$ ",user,host);   
 	      tok = readTokens(stdin);//array divided to words
 	      
 	      if(isExit == 1)
 	        exit(1);
 	        
 	      verifyPipeRedirection(tok);
 	      
 	      if(is_Pipe_Redirection == 0)
 	      {
	        if(tok != NULL)
	        {
                child_pid=fork();
   		        if(child_pid >= 0)  // fork succeded
 	  	        {
 	                        if(child_pid==0)  // the child process
	                        {
			                    signal(SIGINT,SIG_DFL);// the signal for child proces
			                    execvp(tok[0],tok);// sending the array to the running 
			                    printf("Command not found\n"); // if no managed           		
			                    freeTokens (tok); // getting the memory free each time
			                    kill(getpid(),SIGTERM); // "killing" the child process
        	                }
      		                else   // the father process
       		                {
			                    signal(SIGINT,SIG_IGN); // ignoring Crl+c
            		            wait(&status);// waitting for the child to finish
       	                    }                                                

                }
                
                else   // fork did not succed
                {
                      perror("could not make it\n");
                      //freeTokens(tok); //free the allocated memory
                      exit(0);
	            }                              
              }
              
            }
        }
        
        freeTokens(tok);//getting the memory free each time
        status = 0;
        exit(0);
}

//******************************************************************************
//******************************************************************************

char** readTokens(FILE* stream) //will gather the sentence to an array
{
        char** token;  //the final array a function return
        char line[512];  //sentences:input from the user
        char copyLine[512];  //a copy of the sentences, no change mi line
        char* ptr;  // use with strtok = sort of an index by word
        
        fgets(line,512,stream);  // saving the input there, 512 == size of(line)
        strcpy(copyLine,line); // copy the sentences to copyLine
                     
        int counter = 1;  // how many words in the input/sentences
        ptr = strtok(line , " \n");  
        while(ptr != NULL) // counting my words in the sentences
        {
                counter++;
                ptr=strtok(NULL," \n"); 
        } 
        if(counter == 1)
                return NULL;
                
        token = (char**)malloc(counter*sizeof(char*));
        if(token == NULL)
        {
                printf("No assignment memmory");
                return NULL;
        }
      
        //for each word inside the array:
        int i = 0;  // run of the array 
        ptr = strtok(copyLine, " \n");
    
        while(ptr != NULL)
        {
                int length = strlen(ptr)+1;  //this is it length
                token[i] = (char*)malloc(length*sizeof(char)); 
                if(token[i] == NULL)  //wheather the malloc worked
                {
                        printf("No assignment memory");
                        return NULL;
                }

                strcpy(token[i],ptr);
                ptr = strtok(NULL," \n");
                 
                if(token[i][0] == '$')
                {
                        printf("Our minishell cannot handle the $ sign for argument %d\n", i+1);
                        return NULL;
                }
                
                i++; 
        }      
         
        token[i] = NULL;  // lecture request/pidio esto
       
        if(strcmp(token[0],"cd")==0)
        {             
                if(counter == 3)
                {
                        int x=5;
                        x=chdir(token[1]);  //system calls
                        if (x!=0)
                                printf("The folder is not exist");
                                
                        return NULL;
                }  
                else
                {
                        printf("This requiers two arguments");            
                        return NULL; 
                }     
        } 
                               
        if(strcmp(token[0],"exit")==0 && token[1] == NULL )//user want to only to quit
        {
                isExit = 1;
                return NULL;
        }
        
        return token;
}

//******************************************************************************
//******************************************************************************

void freeTokens(char** tokens)//free all the memory
{
        int i = 0;
        if(tokens == NULL)
                return;
                
        while(tokens[i] != NULL)
        {
                free(tokens[i]);
                //tokens[i] = NULL;
                i++;
        }                 
        free(tokens);
}

//******************************************************************************
//******************************************************************************

void verifyPipeRedirection(char** tokens)
{
    int i=0;
    int j=0;
     
    while(tokens[i] != NULL)
    {
        lengthTok++;  // size of token
        i++;
    }
        
    is_Pipe_Redirection = 0;  // put cero again into my flag
    
        
    while(tokens[j] != NULL)
    {       
        if(strcmp(tokens[j], "|") == 0)
        {
            divideTokenPipe(tokens);
            is_Pipe_Redirection++;  // Carried out the order only once
        }       
                        
        if(strcmp(tokens[j], "<")==0) 
        {
            if(is_Pipe_Redirection == 0)   // no haven't pipe
            {
                leftHetz(tokens); 
                //is_Pipe_Redirection++;
            }
        } 
        
        if(strcmp(tokens[j], ">")==0 || strcmp(tokens[j],">>")==0)
        {
            if(is_Pipe_Redirection == 0)   // no haven't pipe
            {
                rigthHetz(tokens); 
            }
        }      
                      
        j++;              
    }
        
    lengthTok=0;
}

//******************************************************************************
//******************************************************************************

void divideTokenPipe(char** tokens)
{
        char** firstCommand;  // without "<"
        char** secondCommand; // without ">"
        char** thirdCommand;  // in the case to have "<"
        char** forthCommand;  // in the case to have ">"
        int status;
        int only_redir = 0;       // use is have only ">" 
        int i;    // use to arrays
        int p=0;
        int j=0;  // place of the pipe in the token
        int k=0;
        int z=0;  // size of secondCommand 
        
        firstCommand = (char**)malloc(512*sizeof(char*));
        secondCommand = (char**)malloc(512*sizeof(char*));
        thirdCommand = (char**)malloc(512*sizeof(char*)); 
        forthCommand = (char**)malloc(512*sizeof(char*));
        
        //-------- CHECK ASSIGNMENT MEMORY ------------
        if(firstCommand == NULL)
        {
            printf("No assignment memmory");
            exit(255);
        }
        
        if(secondCommand == NULL)
        {
            printf("No assignment memmory");
            exit(255);
        }
        
        if(thirdCommand == NULL)
        {
            printf("No assignment memmory");
            exit(255);
        }
                
        //-------------- DIVIDE TOKEN ----------------
        while(tokens[p] != NULL)
        {
            if(strcmp(tokens[p], "|") == 0)
                j += p;     // j = place of the pipe in the token
            
            p++;      
        }
        
        
        for(i=0; i<j; i++)     // check before "|"
        {
            if(strcmp(tokens[i], "<") != 0)
                firstCommand[i] = tokens[i];
        }
        firstCommand[j] = NULL;
          
          
        for(i=0; i<j; i++)   // can be with "<"
        {
            thirdCommand[i] = tokens[i];
        }
        thirdCommand[j] = NULL;     
       
          
        for(i=j+1; i<lengthTok; i++)  // check after "|"
        {        
            if(strcmp(tokens[i], ">") != 0)
            {
                secondCommand[z] = tokens[i];
                z++;
            }   
        }
        secondCommand[z] = NULL;
        
        
       
        for(i=j+1; i<lengthTok; i++)  // check if into forthCommand exist ">"
        {
            forthCommand[z] = tokens[i];
            z++;
        }
        forthCommand[z+1] = NULL;
           
          
        for(i=0; i<j ; i++)  // check if into thirdCommand exist "<"
        {
            if(strcmp(thirdCommand[i], "<") == 0)        
                litlle_redir++;  // = 1         
        }
        
        
        for(i=j+1; i<lengthTok; i++)  
        {        
            if(strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0)
                only_redir++;  // = 1        
        }
        
        
        //--------------- READY FUNCTION PIPE ----------------------
        int tempPipe[2];  
        if(pipe(tempPipe) == -1)  // create pipe
            perror("Failed to produce pipe \n");
                     
        int child1,child2;
        child1 = fork();  
      
        if(child1 == 0 ) //sun process is workink, i want to write!(escribir)
        {
            close(tempPipe[0]); // close reading
            close(1);  // close the stdout! because i want to 
            dup(tempPipe[1]);
            if( litlle_redir != 0)
                leftHetz(thirdCommand);
            else
                execvp(firstCommand[0],firstCommand);
        } 
        
        if(child1 > 0) // father process is working
        {
            waitpid(child1, &status, WUNTRACED); // wait to child1
            if(WEXITSTATUS(status))
                exit(255);   
    
            child2 = fork(); // create sun
            if(child2 == 0)  // i want to reading(leer)
            {
                close(tempPipe[1]); // close write
                close(0); // close stdin
                dup(tempPipe[0]);
                execvp(secondCommand[0],secondCommand);
            }       
                               
            close(tempPipe[0]);
            close(tempPipe[1]);
            waitpid(child2, &status, WUNTRACED);
                        
                        
        }                                                        
        
    litlle_redir = 0;
}

//******************************************************************************
//******************************************************************************

void rigthHetz(char** tokens)
{
    char** firstCommand;   // exec
    char path[510];  // path(camino) to directory
    int fd;
    int i;
    int j=0;     // place of the redirection in the token
    
    if(getcwd(path, 510) == NULL)
        perror("Error\n");
       
    
    firstCommand = (char**)malloc(512*sizeof(char*));
    if(firstCommand == NULL)
    {
        printf("No assignment memmory");
        exit(255);
    }
        
    for(i=0 ; i<3 ; i++)
    {
        if(strcmp(tokens[i], ">")==0 || strcmp(tokens[i], ">>")==0)
            j += i;          
    }
     
    for(i=0; i<j; i++)    
        firstCommand[i] = tokens[i];
    firstCommand[j] = NULL;
    
    char slash[2]={"/"};  
    strcat(path,slash); // add "/" to the path 
    strcat(path,tokens[j+1]); 
    
    if(strcmp(tokens[j],">") == 0)                 
        fd = open(path,O_WRONLY|O_CREAT ,S_IRUSR | S_IWUSR| S_IRGRP | S_IROTH);    
     
    else if(strcmp(tokens[j],">>") == 0)                 
        fd = open(path,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR | S_IWUSR| S_IRGRP | S_IROTH);  
    
    if(fd == -1)
    {
        fprintf(stderr , "ERROR on file output");
        freeTokens(tokens);
		exit(1);    
    }
    
    else
    {
        close(1); // close stdout
        dup(fd);  // put in the file 
        tokens[j] = NULL;
        execvp(firstCommand[0] , firstCommand);
        fprintf(stderr , "ERROR in file output");
        freeTokens(tokens);
    }
}
    	
//******************************************************************************
//******************************************************************************    
    
void leftHetz(char** tokens)
{    
    char** firstCommand;   // exec
    char path[510];  // path(camino) to directory
    int fd;  // use to open file
    int i;   // run in the array
    int j=0;     // place of the redirection in the token
    
    if(getcwd(path, 510) == NULL)
        perror("Error\n");
    
    firstCommand = (char**)malloc(512*sizeof(char*));
    if(firstCommand == NULL)
    {
        printf("No assignment memmory");
        exit(255);
    }
        
    for(i=0 ; i<3 ; i++)
    {
        if(strcmp(tokens[i], "<")==0)
            j += i;          
    }
     
    for(i=0; i<j; i++)    
        firstCommand[i] = tokens[i];
    firstCommand[j] = NULL;    
    
    char slash[2]={"/"};  
    strcat(path,slash); // add "/" to the path 
    strcat(path,tokens[j+1]);
     
    fd=open(path,O_RDONLY); //read from the fille that in path lineInput
					
    if(fd==-1)
    {
        fprintf(stderr , "ERROR on open file \nwc");
        freeTokens(tokens);
	    exit(1);
    }
			
    close(0); // close the outpout
    dup(fd); // now will insert into file insted to the screen
    tokens[j] = NULL;
	
	//---------------- exec < file ------------------------
    if(tokens[j+2] == NULL)	
    {
        execvp(firstCommand[0],firstCommand);
        printf("Command not found\n") ;
	    freeTokens(tokens);
	    exit(1);
    }

    //--------------- exec < file1 > file2 ------------------------
    if(tokens[j+2]!= NULL && tokens[j+3]!=NULL) //check the next tokens if exists and chek them 
    {
        if(strcmp(tokens[j+2],">")==0 || strcmp(tokens[j+2],">>")==0)
        {									
            strcat(path,slash);
			strcat(path,tokens[j+3]);
		
			if(strcmp(tokens[j+2],">")==0) //write or create
	            fd = open(path,O_WRONLY|O_CREAT,S_IRUSR | S_IWUSR| S_IRGRP | S_IROTH);
				
			else if(strcmp(tokens[j+2],">>")==0)
				fd = open(path,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR | S_IWUSR| S_IRGRP | S_IROTH);		
				 	
			if(fd==-1)
	        {
		        fprintf(stderr , "ERROR on open file \n");
		        freeTokens(tokens);
		        exit(1);
            }
				
			else
			{				
				close(1); // close the outpout
				dup(fd); // now will insert into file insted to the screen	
				execvp(firstCommand[0],firstCommand);
				printf("Command not found\n") ;
				freeTokens(tokens);
				exit(1);
			}
		}
	} 
    
}    
    
    

    


					
				






