#include <worker.h>

Worker::Worker(QObject *parent):
    QObject(parent),
    m_parent(parent),
    m_work(0),
    m_client(new DBClientConnection),
    m_workResult(0),
    m_workCount(0)
{
    m_client->connect("mongodb://127.0.0.1");
}

Worker::~Worker()
{
    delete m_client;
}

Worker::setScorer(Scorer *scorer)
{
    m_scorer = scorer;
}

Worker::fetchJob()
{
    auto_ptr<DBClientCursor> cursor = m_client->query("codecracker.works", BSONObj());
    while (cursor->more() && m_workCount < 3)
    {
        BSONObj jobDesc = cursor->next;
        string jobId = jobDesc.getStringField("jobId");
        string userId = jobDesc.getStringField("userId");
        string problemId = jobDesc.getStringField("problemId");
        string compilerId = jobDesc.getStringField("compilerId");
        string sourceCode = jobDesc.getStringField("sourceCode");
        m_client->remove("codecracker.works", QUERY("jobId" << jobId));

        Job *job = new Job(this);
        connect(job, SIGNAL(finished(bool), this, jobFinished(bool));
        job->setId(jobId);
        job->setUserId(userId);
        job->setProblemId(problemId);
        job->setCompilerId(compilerId);
        job->setSourceCode(sourceCode);
        job->start();
    }
}

Worker::jobFinished(bool success)
{
    if (!success)
        return;
    Job *j = qobject_cast<Job *>(QObject::sender());
    m_scorer->updateScore(Job);
}