#ifndef WORKER_H
#define WORKER_H

#include <job.h>

class Worker : public QObject
{
public:
    Worker(QObject *parent = 0);
    ~Worker();

public slots:
    void setScorer(Scorer *scorer);

private slots:
    void fetchJob();
    void jobFinished(bool success);

    QObject *m_parent;
    DBClientConnection *m_client;
    Scorer *m_scorer;
    int m_workCount;
};

#endif WORKER_H