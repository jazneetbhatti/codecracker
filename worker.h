#ifndef WORKER_H
#define WORKER_H

#include <job.h>
#include <scorer.h>

#include <QtCore/QtCore>
#include <QtSql/QtSql>

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = 0);
    ~Worker();

public slots:
    void setScorer(Scorer *scorer);

private slots:
    void fetchJob();
    void jobFinished();

private:
    QObject *m_parent;
    QSqlDatabase m_database;
    Scorer *m_scorer;
    int m_jobCount;
    QTimer *m_timer;
};

#endif // WORKER_H