//Kyle Schultz

#include "multi-lookup.h"
#include "util.c"

int main(int argc, char* argv[])
{
    struct timeval t1;
    struct timeval t2;
    gettimeofday(&t1, NULL);
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_t condc, condp;
    pthread_cond_init(&condc, NULL);
    pthread_cond_init(&condp, NULL);
    FILE * res_fp;
    FILE * req_fp;
    res_fp = fopen(argv[4], "w");
    if (res_fp == NULL)
    {
        perror("Failed to open resolvers output file");
        exit(EXIT_FAILURE);
    }
    req_fp = fopen(argv[3], "w");
    if (req_fp == NULL)
    {
        perror("Failed to open requesters output file");
        exit(EXIT_FAILURE);
    }
    int num_reqs = atoi(argv[1]);
    int num_res = atoi(argv[2]);
    int num_files = argc-5;
    pthread_t requesters[num_reqs];
    pthread_t resolvers[num_res];
    args* ar = malloc(sizeof(args));

    ar->num_files = num_files;
    ar->file_index = 0;
    ar->requester_done = 0;
    ar->buffer_size = 0;
    ar->output = res_fp;
    ar->request_output = req_fp;
    ar->the_mutex = &mutex;
    ar->condc = &condc;
    ar->condp = &condp;
    ar->files = malloc(num_files * sizeof(char*));
    
    for(int i = 0; i < num_files; i++)
    {
       ar->files[i] = argv[i+5];
    }
    for(int j = 0; j < num_reqs; j++)
    {
        pthread_create(&requesters[j], NULL, request, (void*)ar);
    }

    for(int l = 0; l < num_res; l++)
    {
        pthread_create(&resolvers[l], NULL, resolve, (void*)ar);
    }
    
    for (int k = 0; k < num_reqs; k++)
    {
        pthread_join(requesters[k], NULL);
    }


    ar->requester_done = 1;
    pthread_cond_signal(ar->condc);

    for (int m = 0; m < num_res; m++)
    {
        pthread_join(resolvers[m], NULL);
    }
    gettimeofday(&t2, NULL);
    int end_time = (t2.tv_sec - t1.tv_sec);
    printf("Time of execution: %d\n", end_time);
    fclose(res_fp);
    free(ar->files);
    free(ar);
    return 0;
}

void* request(void* arg)
{
    int count = 0;
    struct args* a = arg;
    while(a->file_index < a->num_files)
    {   
        pthread_mutex_lock(a->the_mutex);
    	FILE * fp;

        fp = fopen(a->files[a->file_index], "r");
        count++;
        if (fp == NULL)
        {
            if(a->file_index >= a->num_files)
            {
                pthread_mutex_unlock(a->the_mutex);
                break;
            }
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
        
        a->file_index++;
        pthread_mutex_unlock(a->the_mutex);

        char temp[MAX_NAME_LENGTH];

        while(fscanf(fp, "%1024s", temp) > 0)
        {
            pthread_mutex_lock(a->the_mutex);
            while(a->buffer_size >= 50)
            {
                pthread_cond_wait(a->condp,a->the_mutex);
            }
            strcpy(a->buffer[a->buffer_size],temp);
            a->buffer_size++;
            pthread_cond_signal(a->condc);
            pthread_mutex_unlock(a->the_mutex);
        }
    }
    pid_t tid1;

    tid1 = syscall(SYS_gettid); 
    //printf("req tid : %d serviced %d files\n",tid1, count);
    fprintf(a->request_output, "Thread %d serviced %d files.\n",tid1,count);

    pthread_exit(0);
    
    return NULL;
}

void* resolve(void* arg)
{
    struct args *a = arg;
    while(a->requester_done == 0 || a->buffer_size > 0){
        
        char domain[MAX_NAME_LENGTH];
        char ipstr[INET6_ADDRSTRLEN];
        
        pthread_mutex_lock(a->the_mutex);
        while(a->buffer_size == 0 && a->requester_done == 0)
        {
            pthread_cond_wait(a->condc, a->the_mutex);
        }
        if(a->requester_done == 1 && a->buffer_size == 0)
        {
            pthread_mutex_unlock(a->the_mutex);
            pthread_cond_signal(a->condc);
            break;
        }
        strcpy(domain,a->buffer[a->buffer_size-1]);
        if(dnslookup(domain, ipstr, sizeof(ipstr)) == UTIL_FAILURE)
        {
            fprintf(stderr, "dnslookup error for domain: %s\n", domain);
            strncpy(ipstr, "", sizeof(ipstr));
        }
        //printf("%s,%s\n",domain,ipstr);
        fprintf(a->output, "%s,%s\n",domain,ipstr);
        a->buffer_size--;
        pthread_cond_signal(a->condp);
        pthread_mutex_unlock(a->the_mutex);
    
    }
    pthread_exit(0);

    return NULL;
}