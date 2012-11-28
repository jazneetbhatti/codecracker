#include <scorer.h>

Scorer::Scorer()
{
}

Scorer::~Scorer()
{
}

void Scorer::updateScore(Job *job)
{
    qDebug() << "score updated for job " << job->id() <<", job status is " << job->result();
}