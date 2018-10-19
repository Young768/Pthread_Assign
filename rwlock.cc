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
 
#ifdef USE_RWLOCK
pthread_rwlock_t rwlock;
#else
pthread_mutex_t mutex;
#endif


pid_t gettid() { return syscall( __NR_gettid ); }

struct member
{
     int num;
};

struct pthre
{
  pthread_t thr;
  struct pthre* next;
};

static struct pthre *pthre_head = NULL;

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

void *consumer(void *ptr)
{

    //printf("The ID of writing reading is %lun", (unsigned long)gettid());
    struct member *temp;
    int read;
    temp = (struct member *)ptr;
    int n = temp->num;
    while (n > 0)
    {
        #ifdef USE_RWLOCK
        pthread_rwlock_rdlock(&rwlock);
        #else
        pthread_mutex_lock(&mutex);
        #endif
        read = NUM;
        #ifdef USE_RWLOCK
        pthread_rwlock_unlock(&rwlock);
        #else
        pthread_mutex_unlock(&mutex);
        #endif   
        n--;
    }
 
    return NULL;
}

void *writer(void *ptr)
{

    //printf("The ID of writing reading is %lu\n", (unsigned long)gettid());
    struct member *temp;
    int read;
    temp = (struct member *)ptr;
    int n = temp->num;
    while (n > 0)
    {
        #ifdef USE_RWLOCK
        pthread_rwlock_wrlock(&rwlock);
        #else
        pthread_mutex_lock(&mutex);
        #endif
        NUM++;
        //printf("Thread %lu is writing\n", (unsigned long)gettid());
        #ifdef USE_RWLOCK
        pthread_rwlock_unlock(&rwlock);
        #else
        pthread_mutex_unlock(&mutex);
        #endif   
        n--;
    }
 
    return NULL;
}


int main(int argc,char *argv[])
{
    int n_thread;
    n_thread = atoi(argv[1]);
    pthread_t wthread;
    struct member *round;
    struct pthre *pth;
    struct timeval tv1, tv2;

    for (int i=0; i<n_thread; i++)
    {
      pthread_t pthr;
      pth = (struct pthre *)malloc(sizeof(struct pthre));
      pth -> thr = pthr;
      pthre_add(&pthre_head, pth);
    }
    round = (struct member *)malloc(sizeof(struct member));
    round ->num = 10000;
#ifdef USE_RWLOCK
    pthread_rwlock_init(&rwlock, NULL);
#else
    pthread_mutex_init(&mutex, NULL);
#endif
    
 
    // Measuring time before starting the threads...
    gettimeofday(&tv1, NULL);
 
    struct pthre *cur = pthre_head;
    while (cur)
    {
      pthread_create(&cur->thr, NULL, consumer, (void*)round);
      cur = cur->next;
    }
    #ifdef USE_RWLOCK
    pthread_create(&wthread,NULL,writer,(void*)round);
    #else
    pthread_create(&wthread,NULL,writer,(void*)round);
    struct pthre *cur1 = pthre_head;
    while (cur1)
    {
      pthread_join(cur1->thr, NULL);
      cur1 = cur1->next;
    }
    pthread_join(wthread, NULL);
    #endif
    
 
    // Measuring time after threads finished...
    gettimeofday(&tv2, NULL);
 
    if (tv1.tv_usec > tv2.tv_usec)
    {
        tv2.tv_sec--;
        tv2.tv_usec += 1000000;
    }
#ifdef USE_RWLOCK
    printf("Time for writer/reader lock of %s process is: %ld.%lds\n", argv[1], tv2.tv_sec - tv1.tv_sec,
        tv2.tv_usec - tv1.tv_usec);
#else
    printf("Time for mutex lock of %s process is: %ld.%lds\n", argv[1], tv2.tv_sec - tv1.tv_sec,
        tv2.tv_usec - tv1.tv_usec);
#endif

#ifdef USE_RWLOCK
    pthread_rwlock_destroy(&rwlock);
#else
    pthread_mutex_destroy(&mutex);
#endif
    

 
    return 0;
}
