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

void policy_SJF()
{
    printf("Execution trace with SJF:\n");
    int time = 0;

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
}


void policy_STCF()
{
    printf("Execution trace with STCF:\n");

    // TODO: implement STCF policy

    printf("End of execution with STCF.\n");
}

void policy_RR(int slice)
{
    printf("Execution trace with RR:\n");

    // TODO: implement RR policy

    printf("End of execution with RR.\n");
}

void policy_LT(int slice)
{
    printf("Execution trace with LT:\n");

    // Leave this here, it will ensure the scheduling behavior remains deterministic
    srand(42);

    // In the following, you'll need to:
    // Figure out which active job to run first
    // Pick the job with the shortest remaining time
    // Considers jobs in order of arrival, so implicitly breaks ties by choosing the job with the lowest ID

    // To achieve consistency with the tests, you are encouraged to choose the winning ticket as follows:
    // int winning_ticket = rand() % total_tickets;
    // And pick the winning job using the linked list approach discussed in class, or equivalent

    printf("End of execution with LT.\n");
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
        // TODO
        policy_SJF();
    }
    else if (strcmp(pname, "STCF") == 0)
    {
        // TODO
    }
    else if (strcmp(pname, "RR") == 0)
    {
        // TODO
    }
    else if (strcmp(pname, "LT") == 0)
    {
        // TODO
    }

    exit(0);
}