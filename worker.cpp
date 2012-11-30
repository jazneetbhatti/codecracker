#include <worker.h>

#include <QtCore/QtCore>
#include <QtSql/QtSql>

Worker::Worker(QObject *parent):
    QObject(parent),
    m_parent(parent),
    m_scorer(0),
    m_jobCount(0),
    m_timer(new QTimer(this))
{
    m_database = QSqlDatabase::addDatabase("QMYSQL");
    m_database.setHostName("127.0.0.1");
    m_database.setDatabaseName("codecrackerdb");
    m_database.setUserName("virgin");
    m_database.setPassword("mojito");
    m_database.open();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(fetchJob()));
    m_timer->start(1000);
}

Worker::~Worker()
{
    m_database.close();
}

void Worker::setScorer(Scorer *scorer)
{
    m_scorer = scorer;
}

void Worker::fetchJob()
{
    qDebug() << "fetching jobs";
    if (m_jobCount >= 3)
    {
        qDebug() << "too many jobs running";
        return;
    }
    QSqlQuery query(QString("SELECT jobId, userId, problemId, compilerId, sourceCode FROM jobs WHERE status = 'new' LIMIT %1").arg(QString::number(3 - m_jobCount)));
    while (query.next())
    {
        m_jobCount++;

        QString jobId = query.value(0).toString();
        QString userId = query.value(1).toString();
        QString problemId = query.value(2).toString();
        QString compilerId = query.value(3).toString();
        QString sourceCode = query.value(4).toString();

        Job *job = new Job(this);
        connect(job, SIGNAL(finished()), this, SLOT(jobFinished()));
        job->setId(jobId);
        job->setUserId(userId);
        job->setProblemId(problemId);
        job->setCompilerId(compilerId);
        job->setSourceCode(sourceCode);
        job->start();
        qDebug() << "job" << job->id() << "dispatched";
    }
}

void Worker::jobFinished()
{
    Job *finishedJob = qobject_cast<Job *>(QObject::sender());
    qDebug() << "job " << finishedJob->id() << " successful , result = " << finishedJob->status();
    if (m_scorer)
    {
        m_scorer->updateScore(finishedJob);
    }
    m_jobCount--;
}