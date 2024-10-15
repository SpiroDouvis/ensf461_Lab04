//GROUP 05 
//2024-10-14
//ENSF461-LAB04

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

// total jobs
int numofjobs = 0;

struct job
{
    // job id is ordered by the arrival; jobs arrived first have smaller job id, always increment by 1
    int id;
    int arrival; // arrival time; safely assume the time unit has the minimal increment of 1
    int length;
    int tickets; // number of tickets for lottery scheduling
    // TODO: add any other metadata you need to track here
    struct job *next;
 
    int original_length;
    int old_length;
};

// the workload list
struct job *head = NULL;

void append_to(struct job **head_pointer, int arrival, int length, int tickets)
{

    // TODO: create a new job and init it with proper data
    struct job *new = (struct job *)malloc(sizeof(struct job));
    new->id = numofjobs++;
    new->arrival = arrival;
    new->length = length;
    new->tickets = tickets;

    //ADDED

    new->original_length = length;
    new->old_length = length;

    if (*head_pointer == NULL)
    {
        *head_pointer = new;
    }
    else
    {
        struct job *ptr = *head_pointer;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = new;
    }
    return;
}

void read_job_config(const char *filename)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int tickets = 0;

    char *delim = ",";
    char *arrival = NULL;
    char *length = NULL;

    // TODO, error checking
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // TODO: if the file is empty, we should just exit with error
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[read - 1] == '\n')
            line[read - 1] = 0;
        arrival = strtok(line, delim);
        length = strtok(NULL, delim);
        tickets += 100;

        append_to(&head, atoi(arrival), atoi(length), tickets);
    }
    if (head == NULL)
    {
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    if (line)
        free(line);
}

void policy_SJF(int analysis)
{
    printf("Execution trace with SJF:\n");

    int time = 0;

    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int job_count = 0;
    int** results;
    if (analysis)
        results = (int**)malloc(sizeof(int*) * numofjobs);

    // TODO: implement SJF policy

    while (head != NULL) {
        struct job* shortest_job = head;
        struct job* prev = NULL;
        struct job* ptr = head;

        // find shortest node in list and set prev to previous node
        while (ptr->next != NULL && ptr->next->arrival <= time) {
            if (ptr->next->length < shortest_job->length) {
                shortest_job = ptr->next;
            }
            prev = ptr;
            ptr = ptr->next;
        }

        if (head->next != NULL && prev == NULL && time == 0) {
            while (ptr->next->arrival > time)
                time++;
            continue;
        }

        if (analysis) {
            results[shortest_job->id] = (int*)malloc(sizeof(int[4]));

            int response_time = time - shortest_job->arrival;
            int turnaround_time = response_time + shortest_job->length;
            int wait_time = response_time;

            results[shortest_job->id][0] = shortest_job->id;
            results[shortest_job->id][1] = response_time;
            results[shortest_job->id][2] = turnaround_time;
            results[shortest_job->id][3] = wait_time;
        

            total_response_time += response_time;
            total_turnaround_time += turnaround_time;
            total_wait_time += wait_time;
            job_count++;
        }
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, shortest_job->id, shortest_job->arrival, shortest_job->length);
        

        time += shortest_job->length;

        if (prev == NULL) { // only node that can run
            if (head->next != NULL) {
                while (ptr->next->arrival > time)
                    time++;
                head = head->next;
            }
            else {
                head = NULL;
            }
        }
        else if (shortest_job->next == NULL) { // last node in list
            prev->next = NULL;
        }
        else if (shortest_job == head) { // first node in list
            head = shortest_job->next;
        }
        else { // middle node
            prev->next = shortest_job->next;
        }
        free(shortest_job);
    }
    printf("End of execution with SJF.\n");

    if (analysis) {
        printf("Begin analyzing SJF:\n");

        for (int i = 0; i < numofjobs; i++) {
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
                results[i][0], results[i][1], results[i][2], results[i][3]);
            free(results[i]);
        }
        free(results);

        double avg_response_time = (double)total_response_time / job_count;
        double avg_turnaround_time = (double)total_turnaround_time / job_count;
        double avg_wait_time = (double)total_wait_time / job_count;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
            avg_response_time, avg_turnaround_time, avg_wait_time);

        printf("End analyzing SJF.\n");

    }
}


void policy_STCF(int analysis)
{
    printf("Execution trace with STCF:\n");
      
    int current_time=0;
    int total_response_time=0;
    int total_turnaround_time=0;
    int total_wait_time=0;
    int job_count=0;
    int **results=NULL;
    if(analysis){
        results = (int**)malloc(sizeof(int*)*numofjobs);
        for (int i=0;i<numofjobs;i++){
            results[i]=NULL;
        }
    }
    //ARRAY OF PTRS

    struct job *lastJob = NULL;

    while (head!=NULL || lastJob!=NULL){
        //continues until all jobs are processed!!!
        struct job *shortest_job=NULL;
        struct job *prev=NULL;
        struct job *old_shortest=NULL;
        struct job *ptr=head;

        
        while(ptr!=NULL&&ptr->arrival<=current_time){
            //FIND SHORTEST JOB
            if(shortest_job==NULL || ptr->length<shortest_job->length){
                //NEW SHORTEST JOB
                shortest_job=ptr;
                old_shortest=prev;
            }
            prev=ptr;
            ptr=ptr->next;
            //ITERATE... 
    
        }
        
        //IF NO JOBS ARRIVED - ADD AND RELOOP
        if (head!=NULL && lastJob==NULL) {
            if(shortest_job==NULL){
                current_time++;
                continue;
            } else if (shortest_job->arrival>current_time) { 
                current_time=shortest_job->arrival;
                continue;
            }
        }

        if (analysis){
            struct job* ptr = shortest_job;
            if (shortest_job==NULL) {
                ptr = lastJob;
            }
            if (results[ptr->id]==NULL) {
                results[ptr->id]=(int*)malloc(sizeof(int[4]));
                int response_time=current_time-ptr->arrival;
                results[ptr->id][1]=response_time;
            }
        }


        if (lastJob!=shortest_job && lastJob!=NULL){
            int ranfor = lastJob->old_length - lastJob->length;
            int id = lastJob->id;
            int arrival_time = lastJob->arrival;
            int time = current_time - ranfor;
            printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, id, arrival_time, ranfor);
            if (lastJob->length <= 0) {
                free(lastJob);
                lastJob = NULL;
            } else {
                lastJob->old_length -= ranfor;
            }
         }
    
        if (head == NULL) {
            break;
        }

        //IF SHORTEST JOB NOT DONE/HAS REMAINING TIME...
        if (shortest_job == NULL){
            continue;
        }
        
        if (shortest_job->length >0){
            if(shortest_job->length == shortest_job->original_length){
                //ADD RESPONSE TIME OF SHORTEST JOB IF IT HASNT YET RUN- THIS IS FIRST TIME RUNNING
                total_response_time+=current_time-shortest_job->arrival;
            }

            //DECREMENT REMAINING LENGTH
            shortest_job->length--;
            lastJob = shortest_job;
            current_time++;
        }


        if(shortest_job->length==0){
            //IF JOB COMPLETE..
            int turnaround_time = current_time - shortest_job->arrival;
            int wait_time = turnaround_time - (shortest_job->original_length);

            total_turnaround_time+=turnaround_time;
            total_wait_time+=wait_time;
            //ADD TURNAROUND TIME AND WAIT TIME
            job_count++;
            //ADD COMPLETED JOB

            if(analysis){
                
                results[shortest_job->id][0]=shortest_job->id;
                results[shortest_job->id][2]=turnaround_time;
                results[shortest_job->id][3]=wait_time;
            }
            if(old_shortest==NULL){
                head=shortest_job->next;
            }
            else{
                old_shortest->next=shortest_job->next;
            }
        }
    }
    printf("End of execution with STCF.\n");


    if (analysis) {
        printf("Begin analyzing STCF:\n");

        if (results != NULL) {
            for (int i = 0; i < numofjobs; i++) {
                if (results[i] != NULL) {
                    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", results[i][0], results[i][1], results[i][2], results[i][3]);
                    free(results[i]);
                }
            }
            free(results);

            if (job_count>0){
                //'if any jobs processed'
                double avg_response_time=(double)total_response_time/job_count;
                double avg_turnaround_time=(double)total_turnaround_time/job_count;
                double avg_wait_time=(double)total_wait_time/job_count;

                printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",avg_response_time,avg_turnaround_time,avg_wait_time);
            }
        }
        printf("End analyzing STCF.\n");
    }
}


void policy_RR(int slice, int analysis)
{
    printf("Execution trace with RR:\n");

    int time = 0;

    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int job_count = 0;
    int** results;
    if (analysis)
        results = (int**)malloc(sizeof(int*) * numofjobs);

    for (int i = 0; i < numofjobs; i++) {
        if (analysis) {
            results[i] = NULL;
        }
    }

    // TODO: implement RR policy
        
    struct job* current_job = head;

    while (head != NULL) {

        
        if (head->arrival > time) {
            time = head->arrival;
        }

        if (current_job == NULL) {
            current_job = head;
        }

        // run job and print
        if (current_job->length > 0) {
            int run_time = min(slice, current_job->length);


            if (analysis) {
                if (results[current_job->id] == NULL) {
                    results[current_job->id] = (int*)malloc(sizeof(int[4]));
                    int response_time = time - current_job->arrival;
                    results[current_job->id][0] = current_job->id;
                    results[current_job->id][1] = response_time;
                    total_response_time += response_time;
                }
            }

            printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, current_job->id, current_job->arrival, run_time);
            
            current_job->length -= run_time;
            time += run_time;
        }
        


        // if job is done, remove it from the list
        if (current_job->length <= 0) {
            int turnaround_time = time - current_job->arrival;
            int wait_time = turnaround_time - current_job->original_length;

            total_turnaround_time += turnaround_time;
            total_wait_time += wait_time;
            job_count++;

            if (analysis) {
                if (results[current_job->id] == NULL) {
                    results[current_job->id] = (int*)malloc(sizeof(int[4]));
                    int response_time = time - current_job->arrival;
                    results[current_job->id][0] = current_job->id;
                    results[current_job->id][1] = response_time;
                }
                results[current_job->id][2] = turnaround_time;
                results[current_job->id][3] = wait_time;
            }

            struct job* temp = current_job;

            if (head == temp) {
                head = head->next;
            } else {
                struct job* ptr = head;
                while (ptr->next != temp) {
                    ptr = ptr->next;
                }
                ptr->next = temp->next;
            }

            free(temp);
        } 

        if (current_job->next == NULL || current_job->next->arrival >= time) {
            current_job = head;
        } else {
            current_job = current_job->next;
        }

    }

    printf("End of execution with RR.\n");

    if (analysis) {
    printf("Begin analyzing RR:\n");

        for (int i = 0; i < numofjobs; i++) {
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
                results[i][0], results[i][1], results[i][2], results[i][3]);
            free(results[i]);
        }
        free(results);

        double avg_response_time = (double)total_response_time / job_count;
        double avg_turnaround_time = (double)total_turnaround_time / job_count;
        double avg_wait_time = (double)total_wait_time / job_count;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
            avg_response_time, avg_turnaround_time, avg_wait_time);

        printf("End analyzing RR.\n");

    }
}

void policy_LT(int slice, int analysis)
{
    printf("Execution trace with LT:\n");

    // Leave this here, it will ensure the scheduling behavior remains deterministic
    srand(42);
    
    int current_time=0;
    struct job *ptr=head;
    int total_tickets=0;
    //total ticket among ALL JOBS


    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int job_count = 0;

    int** results;
    if (analysis)
        results = (int**)malloc(sizeof(int*) * numofjobs);

    for (int i = 0; i < numofjobs; i++) {
        if (analysis) {
            results[i] = NULL;
        }
    }

    while (ptr!=NULL){
        total_tickets+=ptr->tickets;
        //iterate thru all jobs and add tickets
        ptr=ptr->next;
    }

    while(head!=NULL){
        //as long as jobs remaining.. duh

        total_tickets=0;
        ptr=head;
        while(ptr!=NULL&&ptr->arrival<=current_time){ 
            total_tickets+=ptr->tickets;
            ptr=ptr->next;
        }

        if(total_tickets==0){
            current_time++;
            continue;
        }

        int winning_ticket=rand()%total_tickets;
        //winning ticket is chosen as random number between 0 and total tickets- were told to use this
        struct job *winning_job=head;
        //winning job is head by dflt
        struct job *prev=NULL;
        //job before job before winning job
        struct job *prev_winning_job =NULL;
        //job before winning job
        //(prev->prev_winning_job->winning_job)
        int ticket_sum= 0;
        //used to keep track of ticket sum as we traverse LL
    
        for(ptr=head;ptr!=NULL&&ptr->arrival<= current_time; ptr=ptr->next){
            ticket_sum+=ptr->tickets;
            //add tickets of each job to total
            if (ticket_sum>winning_ticket){
                winning_job=ptr;
                prev_winning_job=prev;
                break;
            }
            prev=ptr;
            //iterate prev aswell as ptr which is done in for
        }

        int rt=min(slice, winning_job->length);

        if (analysis) {
            if (results[winning_job->id] == NULL) {
                results[winning_job->id] = (int*)malloc(sizeof(int[4]));
                int response_time = current_time - winning_job->arrival;
                results[winning_job->id][0] = winning_job->id;
                results[winning_job->id][1] = response_time;
                total_response_time += response_time;
            }
        }
        //either runs for the given slice or until cmpletion if theres < slice left
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", current_time, winning_job->id, winning_job->arrival, rt);
        winning_job->length-=rt;
        //decrememt time by how much it ran
        current_time+=rt;
        //increment time by how much winning job just ran

        if(winning_job->length<=0){
            //if winning job completed execution
            int turnaround_time=current_time-winning_job->arrival;
            int wait_time=turnaround_time-winning_job->original_length;
            if (analysis) {
                if (results[winning_job->id] == NULL) {
                    results[winning_job->id] = (int*)malloc(sizeof(int[4]));
                    int response_time = current_time - winning_job->arrival;
                    total_response_time += response_time;
                    results[winning_job->id][0] = winning_job->id;
                    results[winning_job->id][1] = response_time;
                }
                results[winning_job->id][2] = turnaround_time;
                results[winning_job->id][3] = wait_time;
                total_turnaround_time += turnaround_time;
                total_wait_time += wait_time;
                job_count++;
            }
            if(prev_winning_job==NULL){
                //winning job is head
                head=winning_job->next;            
            }else{
                prev_winning_job->next=winning_job->next;
                //skips over where winning job was
            }
            total_tickets-=winning_job->tickets;
            //remove num tickets winning job had from total- dont want null winning..
            free(winning_job);
        }
    } 

    // In the following, you'll need to:
    // Figure out which active job to run first
    // Pick the job with the shortest remaining time
    // Considers jobs in order of arrival, so implicitly breaks ties by choosing the job with the lowest ID

    // To achieve consistency with the tests, you are encouraged to choose the winning ticket as follows:
    // int winning_ticket = rand() % total_tickets;
    // And pick the winning job using the linked list approach discussed in class, or equivalent

    printf("End of execution with LT.\n");

    if (analysis) {
        printf("Begin analyzing LT:\n");

        for (int i = 0; i < numofjobs; i++) {
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
                results[i][0], results[i][1], results[i][2], results[i][3]);
            free(results[i]);
        }
        free(results);

        double avg_response_time = (double)total_response_time / job_count;
        double avg_turnaround_time = (double)total_turnaround_time / job_count;
        double avg_wait_time = (double)total_wait_time / job_count;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
            avg_response_time, avg_turnaround_time, avg_wait_time);

        printf("End analyzing LT.\n");
    }
}

void policy_FIFO(int analysis)
{
    if (analysis)
    {
        printf("Begin analyzing FIFO:\n");
    }
    else
    {
        printf("Execution trace with FIFO:\n");
    }

    struct job *current_job = head;
    int current_time = 0;

    int total_response_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;
    int job_count = 0;

    while (current_job != NULL)
    {

        if (current_time < current_job->arrival)
        {
            current_time = current_job->arrival;
        }

        if (analysis)
        {
            int response_time = current_time - current_job->arrival;
            int turnaround_time = response_time + current_job->length;
            int wait_time = response_time;

            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
                   current_job->id, response_time, turnaround_time, wait_time);

            total_response_time += response_time;
            total_turnaround_time += turnaround_time;
            total_wait_time += wait_time;
            job_count++;
        }
        else
        {
            printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n",
                   current_time, current_job->id, current_job->arrival, current_job->length);
        }

        current_time += current_job->length;

        current_job = current_job->next;
    }

    if (analysis)
    {

        double avg_response_time = (double)total_response_time / job_count;
        double avg_turnaround_time = (double)total_turnaround_time / job_count;
        double avg_wait_time = (double)total_wait_time / job_count;

        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n",
               avg_response_time, avg_turnaround_time, avg_wait_time);
    }

    if (analysis)
    {
        printf("End analyzing FIFO.\n");
    }
    else
    {
        printf("End of execution with FIFO.\n");
    }
}

int main(int argc, char **argv)
{

    static char usage[] = "usage: %s analysis policy slice trace\n";

    int analysis;
    char *pname;
    char *tname;
    int slice;

    if (argc < 5)
    {
        fprintf(stderr, "missing variables\n");
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }

    // if 0, we don't analysis the performance
    analysis = atoi(argv[1]);

    // policy name
    pname = argv[2];

    // time slice, only valid for RR
    slice = atoi(argv[3]);

    // workload trace
    tname = argv[4];

    read_job_config(tname);

    if (strcmp(pname, "FIFO") == 0)
    {
        policy_FIFO(0);
        if (analysis)
            policy_FIFO(analysis);
    }
    else if (strcmp(pname, "SJF") == 0)
    {
        policy_SJF(analysis);
    }
    else if (strcmp(pname, "STCF") == 0)
    {
        policy_STCF(analysis);
    }
    else if (strcmp(pname, "RR") == 0)
    {
        policy_RR(slice, analysis);
    }
    else if (strcmp(pname, "LT") == 0)
    {
        policy_LT(slice, analysis);
    }

    exit(0);
}