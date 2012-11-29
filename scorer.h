#ifndef SCORER_H
#define SCORER_H

class Job;

class Scorer
{
public:
    Scorer();
    ~Scorer();
    void updateScore(Job *job);
};

#endif // SCORER_H