/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

int semaphore, size_queue, num_job, num_producer, num_consumer,jobID,jobc_id;
int **queue;

int main (int argc, char **argv)
{
	int* producer_id;
	int* consumer_id;
	initialize_parameter(queue,size_queue, num_job,num_producer, num_consumer,jobID,jobc_id,producer_id,consumer_id,argc,argv); 

        semaphore=initialise_semaphore(size_queue);

	if(semaphore==-1)
	{
		cerr<<"Error in creating semaphore. Abort"<<endl;
		return semaphore;
	}

	pthread_t producer_t[num_producer];///array of threads
	pthread_t consumer_t[num_consumer];

	for (int i=0;i<num_producer;i++)
	{
	    pthread_create(&producer_t[i], NULL, producer, (void *) &producer_id[i]);
	}
	for (int i=0; i<num_consumer;i++)
	{
		pthread_create(&consumer_t[i],NULL,consumer,(void* ) & consumer_id[i]);
	}

	for (int i=0;i<num_producer;i++)
	{
	    pthread_join(producer_t[i],NULL);///wait
	}

	for (int i=0;i<num_consumer;i++)
	{
		pthread_join(consumer_t[i], NULL);///wait
	}
	sem_close(semaphore);

        clean_up(queue,producer_id,consumer_id,size_queue); //deletes pointers queue, producer_id, consumer_id

	return 0;
}

void *producer (void *id) 
{

		
	int* prod_id=(int* )  id;
        int num_job_1=num_job;
	int err;
        while(1)
	{

		if(num_job_1>0) 
		{
			num_job_1--;//produce item
			
		} else {
			sem_wait(semaphore,output);///avoid overlapping output
			cout<<"Producer("<<*prod_id<<"): no more jobs to generate."<<endl;
			sem_signal(semaphore,output);
			pthread_exit(0);
		}
        

        err=sem_wait_time(semaphore,space);	/// https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/s_bostechref/semop.html
	if (err==-1)
	{
            	sem_wait(semaphore,output);
		cout<<"Producer("<<*prod_id<<"): waited too long to add job to the queue. timeout!"<<endl;
		sem_signal(semaphore,output);
		pthread_exit(0);
	}

                sem_wait(semaphore,mutex);
	
		jobID=(jobID+1)%size_queue;
	for (int j=jobID%size_queue;;j=(j+1)%size_queue)
	{
	         time_t time=clock();
		if ((time-clock())/CLOCKS_PER_SEC>=20) //safety measure to check code loop error.
		{
			sem_wait(semaphore,output);
			cerr<<"Error: waited too long to deposit job. Abort"<<endl;
			sem_signal(semaphore,output);
			sem_signal(semaphore,mutex);
			pthread_exit(0);
		}
		if (queue[j][0]==0)
		{                      
	            queue[j][0]=j+1;//deposit item
		    queue[j][1]=rand_between(1,10);//job duration
		    sem_wait(semaphore,output);
	            cout<<"Producer("<<*prod_id<<"): Job id "<<j+1<<" duration "<<queue[j][1]<<endl;
		    sem_signal(semaphore,output);
		    sem_signal(semaphore,mutex);
                    sem_signal(semaphore,item);//send signal to consumer jobs available to consume
		    goto SKIPP;//exit loop; (deposit one job even if multiple space available)
		}
		
	}
	
                   SKIPP:sleep(rand_between(1,5));
         
	}
}



void *consumer (void *id) 
{
	int* cons_id=(int*) id;
	int err;
        while(1) 
	{
                 
               err=sem_wait_time(semaphore,item);
	       if(err==-1) 
	       {
                   sem_wait(semaphore,output);
		   cout<<"Consumer("<<*cons_id<<"): no more jobs left."<<endl;
		   sem_signal(semaphore,output);
		   pthread_exit(0);
	       }
	               


	                sem_wait(semaphore,mutex);
			jobc_id=(jobc_id+1)%size_queue;

			for (int j=jobc_id%size_queue;;j=(j+1)%size_queue) 
			{
				time_t time=clock();///additional safety measure in case loops keeps going
				if((clock()-time)/CLOCKS_PER_SEC>=20)
				{
					sem_wait(semaphore,output);
					cerr<<"Error: waited too long to fetch job. Abort"<<endl;
					sem_signal(semaphore,output);
					sem_signal(semaphore,mutex);
					pthread_exit(0);
				}

				if (queue[j][0]!=0)
				{
				        const int id=j+1;
					const int duration=queue[j][1];
		                        queue[j][0]=0;//fetch item
					sem_signal(semaphore,mutex);
					sem_signal(semaphore,space);
                      			sem_wait(semaphore,output);
		  		        cout<<"Consumer("<<*cons_id<<"): Job id "<<id<<" executing sleep duration "<<duration<<endl;
		    			sem_signal(semaphore,output);
					sleep(queue[j][1]);
					sem_wait(semaphore,output);
					cout<<"Consumer("<<*cons_id<<"): Job id "<<id<<" completed."<<endl;
					sem_signal(semaphore,output);
		    			goto SKIP;
		    		
				}
			
			}
                  SKIP: ;
	}
}
