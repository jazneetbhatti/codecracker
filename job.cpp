#include <job.h>

#include <QtCore/QProcess>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtSql/QtSql>

#include <queryinterface.h>
#include <application.h>

#include <comparechecker.h>

Job::Job(QObject *parent):
    QObject(parent),
    m_process(0),
    m_checker(0)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
}

Job::~Job()
{
    m_process->kill();
}

QString Job::compilerPath()
{
    // FIXME: return path according to m_compilerId
    return QString("/usr/bin/gcc");
}

QString Job::sourceFilePath()
{
    return QString("/tmp/codecracker-src-") + m_id + ".c";
}

QString Job::executableFilePath()
{
    return QString("/tmp/codecracker-exe-") + m_id;
}

int Job::retrieveTimeoutFromDatabase()
{
    QueryInterface *queryInterface = dApp->queryInterface();
    QByteArray result = queryInterface->query("SELECT timeout FROM problems WHERE problemId = '" + problemId()  + "'",
                   "codecrackerdb_problems_timeout_problemId" + problemId());
    int timeout = result.toInt();
    return timeout;
}

QByteArray Job::retrieveInputFromDatabase()
{
    QueryInterface *queryInterface = dApp->queryInterface();
    QByteArray result = queryInterface->query("SELECT input FROM problems WHERE problemId = '" + problemId()  + "'",
                                              "codecrackerdb_problems_input_problemId" + problemId());
    return result;
}

QByteArray Job::retrieveOutputFromDatabase()
{
    QueryInterface *queryInterface = dApp->queryInterface();
    QByteArray result = queryInterface->query("SELECT output FROM problems WHERE problemId = '" + problemId()  + "'",
                                              "codecrackerdb_problems_output_problemId" + problemId());
    return result;
}

void Job::start()
{
    setStatus("creatingSourceFile");
    createSourceFile();
    compileSourceFile();
}

void Job::createSourceFile()
{
    QFile sourceFile(sourceFilePath());
    if (!sourceFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        return;

    QTextStream sourceStream(&sourceFile);
    sourceStream << m_sourceCode;
    sourceStream.flush();
    sourceFile.close();
}

void Job::compileSourceFile()
{
    setStatus("compiling");
    m_process = new QProcess(this);
    QStringList arguments;
    arguments << sourceFilePath() << "-o" << executableFilePath();
    connect (m_process, SIGNAL(finished(int)), this, SLOT(compileFinished(int)));
    m_process->start(compilerPath(), arguments);
    // use some const value instead of hard coding
    connect (m_timer, SIGNAL(timeout()), this, SLOT(compileTimeout()));
    m_timer->start(10000);
}

void Job::compileTimeout()
{
    disconnect(m_process);
    disconnect(m_timer);
    m_process->kill();
    setStatus("compileTimeout");
    emit finished();
}

void Job::compileFinished(int exitCode)
{
    m_process->disconnect(this);
    m_timer->disconnect(this);
    if (exitCode != 0)
    {
        setStatus("compileError");
        emit finished();
        return;
    }

    executeFile();
}

void Job::executeFile()
{
    setStatus("executing");
    connect(m_process, SIGNAL(finished(int)), this, SLOT(executionFinished(int)));
    m_process->start(executableFilePath());
    QByteArray input = retrieveInputFromDatabase();
    m_process->write(input);
    m_process->closeWriteChannel();
    int timeoutDuration = retrieveTimeoutFromDatabase();
    connect (m_timer, SIGNAL(timeout()), this, SLOT(executionTimeout()));
    m_timer->start(timeoutDuration);
}

void Job::executionTimeout()
{
    m_process->disconnect(this);
    m_timer->disconnect(this);
    m_process->kill();
    setStatus("executionTimeout");
    emit finished();
}

void Job::executionFinished(int exitCode)
{
    m_process->disconnect(this);
    m_timer->disconnect(this);
    if (exitCode != 0)
    {
        setStatus("runtimeError");
        emit finished();
        return;
    }

    checkOutput();
}


void Job::checkOutput()
{
    setStatus("checking");
    QByteArray output = m_process->readAllStandardOutput();
    QByteArray expectedOutput = retrieveOutputFromDatabase();
    m_checker = new CompareChecker();
    m_checker->setOutput(output);
    m_checker->setExpectedOutput(expectedOutput);
    connect(m_checker, SIGNAL(statusChanged(QString)), this, SLOT(setStatus(QString)));
    connect(m_checker, SIGNAL(finished()), this, SLOT(checkingFinished()));
    m_checker->start();
}

void Job::checkingFinished()
{
    emit finished();
}

void Job::setStatus(QString status)
{
    m_status = status;
    QSqlQuery statusQuery(QString("UPDATE jobs SET status = '%1', updated = NOW() WHERE jobId = %2").arg(status).arg(id()));
}