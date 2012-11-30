#include <scorer.h>

#include <job.h>

#include <QtCore/QtCore>

Scorer::Scorer()
{
}

Scorer::~Scorer()
{
}

void Scorer::updateScore(Job *job)
{
    qDebug() << "score updated for job " << job->id() <<", job status is " << job->status();
}