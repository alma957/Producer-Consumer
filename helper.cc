/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"
int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_wait_time(int id, short unsigned int num) // https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/s_bostechref/semop.html
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  const struct timespec timeout = {20,0};
  int err= semtimedop (id, op, 1,&timeout);
  return err;
}


void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}

///function initialise paramters
//
void initialize_parameter(int**& queue,int &size_queue, int &num_job,int &num_producer,int &num_consumer,int& jobID, int& jobc_id,int*& producer_id,int*&  consumer_id, int argc, char** argv)
{
  if (argc==5)
  {
	  size_queue=atoi(argv[1]);
          num_job=atoi(argv[2]);
          num_producer=atoi(argv[3]);
          num_consumer=atoi(argv[4]);

	  queue=new int*[size_queue];
          for (int i=0;i<size_queue;i++)
                  queue[i]=new int [2];

          for (int i=0;i<size_queue;i++)
          {
                  for (int j=0;j<2;j++)
                  {
                          queue[i][j]=0;
                  }
          }
	 
	  producer_id=new int[num_producer];
	  for (int i=0;i<num_producer;i++)
                producer_id[i]=i+1;
          consumer_id=new int[num_consumer];
	  for (int i=0;i<num_consumer;i++)
             consumer_id[i]=i+1;

	  jobID=-1;
	  jobc_id=-1;


  } else {
	 cerr<<"Number of parameters inserted wrong. Parameter passed on the command line must be four. Exiting Program..."<<endl;
          exit(1);
 }
}

//initialise semaphore and return int
int initialise_semaphore(int size_queue)
{
	int semaphore=sem_create(SEM_KEY,4);
	int err=sem_init(semaphore,mutex,1);
	if (err==-1)
		return err;
	err=sem_init(semaphore,space,size_queue);
	if (err==-1)
		return err;
	err=sem_init(semaphore,item,0);
	if(err==-1)
		return err;
	err=sem_init(semaphore,output,1);
	if(err==-1)
		return err;

	return semaphore;
}
//generate random number between min and max
int rand_between(int min, int max)
{
        int random=rand()%(max-min+1)+min;
        return random;
}
///to avoid memory leakes.
void clean_up(int**& queue, int*& producer_id,int*& consumer_id,int size_queue)
{
	 for (int i=0;i<size_queue;i++)
                delete [] queue[i];

        delete [] queue;
        
	delete [] consumer_id;
	delete [] producer_id;
}

