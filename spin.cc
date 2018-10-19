#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <list>
#include <pthread.h>
#include<stdlib.h>
//shared variable
int NUM=0;
 
using namespace std;
 
list<int> the_list;
 
#ifdef USE_SPINLOCK
pthread_spinlock_t spinlock;
#else
pthread_mutex_t mutex;
#endif
//Get the thread id
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

    struct member *temp;
    temp = (struct member *)ptr;
    int n = temp->num;
    while (n>0)
    {
#ifdef USE_SPINLOCK
        pthread_spin_lock(&spinlock);
#else
        pthread_mutex_lock(&mutex);
#endif
        if (n<=0)
        {
#ifdef USE_SPINLOCK
            pthread_spin_unlock(&spinlock);
#else
            pthread_mutex_unlock(&mutex);
#endif
            break;
        }
        NUM++;

#ifdef USE_SPINLOCK
        pthread_spin_unlock(&spinlock);
#else
        pthread_mutex_unlock(&mutex);
#endif
       n--;
    }
 
    return NULL;
}


int main(int argc,char *argv[])
{
    int n_thread,rnd;
    n_thread = atoi(argv[1]);
    rnd = atoi(argv[2]);
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
    round ->num = rnd;
#ifdef USE_SPINLOCK
    pthread_spin_init(&spinlock, 0);
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
    struct pthre *cur1 = pthre_head;
    while (cur1)
    {
      pthread_join(cur1->thr, NULL);
      cur1 = cur1->next;
    }
 
    // Measuring time after threads finished...
    gettimeofday(&tv2, NULL);
 
    if (tv1.tv_usec > tv2.tv_usec)
    {
        tv2.tv_sec--;
        tv2.tv_usec += 1000000;
    }
#ifdef USE_SPINLOCK
    printf("Time for spinlock of %s process %s times: %ld.%lds\n", argv[1], argv[2], tv2.tv_sec - tv1.tv_sec,
        tv2.tv_usec - tv1.tv_usec);
#else
    printf("Time for mutex of %s process %s times: %ld.%lds\n", argv[1], argv[2], tv2.tv_sec - tv1.tv_sec,
        tv2.tv_usec - tv1.tv_usec);
#endif
 
#ifdef USE_SPINLOCK
    pthread_spin_destroy(&spinlock);
#else
    pthread_mutex_destroy(&mutex);
#endif
 
    return 0;
}
