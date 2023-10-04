#ifndef VOTERS

    #define VOTERS
    int Initialize (int );
    int CreateVoter(int Pin, char * surname, char * name, int PostCode);
    int PrintVoter(int Pin);
    int SetVoted(int Pin);
    int NumberOfVoters (void);
    int NumberOfYesVoters (void);
    int PrintPostalCodes (void);
    int Exit(void);

    class Voter{
        public : 
            Voter(int Pin, const char * surname, const char * name, int PostCode);
            ~Voter();
            int GetPin();
            int DisplayVoter();
            int SetVote();
    };


#endif 