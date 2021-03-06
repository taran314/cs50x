#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
    //  copies command-line aruements into candidates[]
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    //for each voter i get their preferences j
    for (int i = 0; i < voter_count; i++)
    {
        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    for(int i = 0; i < candidate_count; i++)
    {
        if(strcmp(name, candidates[i].name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;

}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    for(int i = 0; i < voter_count; i++)
    {
        //start with [0][0]
        for(int j = 0; j < candidate_count; j++)
        {
            //check if first pref is eliminated
            if(candidates[preferences[i][j]].eliminated == false)
            {
                candidates[preferences[i][j]].votes++;
                //go to outer loop
                break;
            }
        }
    }
        
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    // if even we want strictly more than 50%
    if(voter_count%2 == 0)
    {
        for(int i = 0; i < candidate_count; i++)
        {
            if( candidates[i].votes > (voter_count / 2) )
            {
            printf("%s\n", candidates[i].name);
            return true;
            }  
        }
        return false;
    }
    // in this case candidate count is odd so greater than or equal to works
    for(int i = 0; i < candidate_count; i++)
    {
        if( candidates[i].votes >= round(voter_count / 2) )
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    int remaining_candidates = 0;
    int remaining[candidate_count];
    
    // populates an array which contains the numbers of votes for remaining candidates
    for(int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false)
        {
            remaining[remaining_candidates] = candidates[i].votes;
            remaining_candidates++;
        }
    }
    // if only one candidate left
    if(remaining_candidates == 1)
    {
        return remaining[0];
    }
    // if more than one then must compare
    
    for(int i = 0; i < remaining_candidates - 1; i++)
    {

        if (remaining[0] > remaining[i+1])
        {
            remaining[0] = remaining[i+1];
        }
    }
    return remaining[0];
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    for(int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false)
        {
            if(min != candidates[i].votes)
            {
                return false;
            }
        }
    }
    return true;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{
    for(int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false)
        {
            if(min == candidates[i].votes)
            {
                candidates[i].eliminated = true;
            }
        }
    }
}
