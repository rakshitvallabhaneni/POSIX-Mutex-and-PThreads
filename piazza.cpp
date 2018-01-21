#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include <unistd.h>
#include<iostream>
using namespace std;

struct input{
char* family;
char* name;
int arrival;
int stay;
};

struct input people[100];
static pthread_mutex_t key;
static pthread_cond_t okCap = PTHREAD_COND_INITIALIZER;
static int capuletCount=0;
static int montagueCount=0;


void *capulet(void *arg)
{
		
	int i;
	i = (int) arg;
	
	pthread_mutex_lock(&key);
	while(montagueCount > 0)
			pthread_cond_wait(&okCap, &key);

	cout <<"Capulet "<<people[i].name<<" enters the piazza."<<endl;
	capuletCount++;
	pthread_mutex_unlock(&key);

	sleep(people[i].stay);
	
	pthread_mutex_lock(&key);
	cout <<"Capulet "<<people[i].name<<" leaves the piazza."<<endl;
	capuletCount--;	
	if(capuletCount == 0)
		pthread_cond_broadcast(&okCap);
	pthread_mutex_unlock(&key);

	pthread_exit(NULL);
}

void *montague(void *arg)
{
	int i;
	i = (int) arg;
	pthread_mutex_lock(&key);
	while(capuletCount > 0)	
			pthread_cond_wait(&okCap, &key);

	montagueCount++;
	cout <<"Montague "<<people[i].name<<" enters the piazza."<<endl;
	pthread_mutex_unlock(&key);

	sleep(people[i].stay);
	
	pthread_mutex_lock(&key);
	cout <<"Montague "<<people[i].name<<" leaves the piazza."<<endl;
	montagueCount--;	
	if(montagueCount == 0)
		pthread_cond_broadcast(&okCap);
	pthread_mutex_unlock(&key);


	pthread_exit(NULL);
}




int main()
{

	FILE * fp;
	char* line = (char*)malloc(sizeof(char) * 512);
	size_t len = 0;
	ssize_t read;
	char* token = (char*)malloc(sizeof(char) * 512);

	fp = fopen("input.txt","r");
	if(fp == NULL)
		exit(EXIT_FAILURE);
	int i = 0;
	pthread_mutex_init(&key, NULL);

	while((read = getline(&line, &len, fp)) != -1)
	{
		people[i].family = (char*)malloc(sizeof(char) * 512);
		people[i].name = (char*)malloc(sizeof(char) * 512);
		token = strtok(line," ");
		strcpy(people[i].family,token);
		token = strtok(NULL," ");
		strcpy(people[i].name,token);
		token = strtok(NULL," ");
		people[i].arrival = atoi(token);
		token = strtok(NULL," ");
		people[i].stay = atoi(token);
		i++;
	}


	
	pthread_t tid[i];
	int sleepTime = 0;


	for(int current = 0; current < i; current++)
	{
		if(current > 0)
			sleepTime = people[current].arrival - people[current - 1].arrival;

			int sendArg = current;
			if(strcmp(people[current].family, "Capulet") == 0)
		        {
				sleep(sleepTime);	
				cout <<"Capulet "<<people[current].name<<" arrives at the piazza at time "<<people[current].arrival<<"."<<endl;
				pthread_create(&tid[current], NULL, capulet, (void *) current);
			}
			else
			{
				sleep(sleepTime);
				cout <<"Montague "<<people[current].name<<" arrives at the piazza at time "<<people[current].arrival<<"."<<endl;
				pthread_create(&tid[current], NULL, montague, (void *) current);
			}
	       
	}

    for(int j = 0; j < i; j++)
    {
	pthread_join(tid[j],NULL);
    }

       
    fclose(fp);
return 0;

}
