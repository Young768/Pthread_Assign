#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include<stdlib.h>
//shared variable
int NUM=0;
 
using namespace std;

pthread_mutex_t lock;

pthread_cond_t cond;

struct pthre
{
  pthread_t thr;
  struct pthre* next;
};

static void pthre_add(struct pthre **head, struct pthre *pthre)
{
	struct pthre *temp;

	if(NULL == *head)
	{
		*head = pthre;
		(*head)->next = NULL;
	}
	else
	{
		temp = *head;
		while(temp)
		{
			if(NULL == temp->next)
			{
				temp->next = pthre;
				pthre->next = NULL;
			}
			temp = temp->next;
		}
	}
}

static struct pthre *pthre_head = NULL;

void * consumer(void *argv)
{
    //int data;
    while(1)
    {
        pthread_mutex_lock(&lock);
        while(NUM != 100)
        // if(head->_next==NULL)
        {
            printf("producer is not ready\n\n");
            pthread_cond_wait(&cond,&lock);
            //break;
        }
        // else{
        // printf("producter is ready...\n");
        printf("REACH 100\n\n");
        pthread_mutex_unlock(&lock);
        break;

        //sleep(1);
    }
}

void * producer(void * argv)
{
    //int n_thread;
    //n_thread = atoi(argv[1]);
    //pthread_t wthread[n_thread];
    while(NUM < 100)
    {
        usleep(100000);
        pthread_mutex_lock(&lock);
	//printf("before num %d\n", NUM);
        NUM++;
	//printf("after num %d\n", NUM);
        
        if(NUM == 100)
        {
        pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&lock);
	//break;
    }
}

int main(int argc,char *argv[])
{
    struct timeval tv1, tv2;
    int n_thread = atoi(argv[1]);
    pthread_t pthre1;
    struct pthre *pth;
    pthread_mutex_init(&lock, NULL);
    gettimeofday(&tv1, NULL);
    pthread_create(&pthre1,NULL,consumer,NULL);

    for (int i=0; i<n_thread-1; i++)
    {
      pthread_t pthr;
      pth = (struct pthre *)malloc(sizeof(struct pthre));
      pth -> thr = pthr;
      pthre_add(&pthre_head, pth);
    }

    struct pthre *cur = pthre_head;
    while (cur)
    {
      pthread_create(&cur->thr, NULL, producer, NULL);
      cur = cur->next;
    }
    pthread_join(pthre1, NULL);
    struct pthre *cur1 = pthre_head;
    while (cur1)
    {
      pthread_join(cur1->thr, NULL);
      cur1 = cur1->next;
    }
    
    //pthread_join(id2,NULL);
    gettimeofday(&tv2, NULL);
 
    if (tv1.tv_usec > tv2.tv_usec)
    {
        tv2.tv_sec--;
        tv2.tv_usec += 1000000;
    }
    printf("Time for cvlock of %s processes is: %ld.%lds\n", argv[1], tv2.tv_sec - tv1.tv_sec,
        tv2.tv_usec - tv1.tv_usec);
    pthread_mutex_destroy(&lock);
}
