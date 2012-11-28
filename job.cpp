#include <job.h>

Job::Job(QObject *parent):
    QObject(parent),
    m_process(0)
{
    m_database = QSqlDatabase::addDatabase("QMYSQL");
    m_database.setHostName("127.0.0.1");
    m_database.setDatabaseName("codecrackerdb");
    m_database.setUserName("virgin");
    m_database.setPassword("mojito");
    m_database.open();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
}

Job::~Job()
{
    m_process->kill();
    m_database.close();
}

QString Job::compilerPath()
{
    // FIXME: return path according to m_compilerId
    return QString("/usr/bin/gcc");
}

QString Job::sourceFilePath()
{
    return QString("/tmp/codecracker-src-") + m_id;
}

QString Job::executableFilePath()
{
    return QString("/tmp/codecracker-exe-") + m_id;
}

int Job::retrieveTimeoutFromDatabase()
{
    QSqlQuery query("SELECT timeout FROM problems WHERE problemId = " + problemId());
    int timeout = query.value(0).toInt();
    return timeout;
}

QByteArray Job::retrieveInputFromDatabase()
{
    QSqlQuery query("SELECT input FROM problems WHERE problemId = " + problemId());
    QByteArray input = query.value(0).toByteArray();
    return input;
}

QByteArray Job::retrieveOutputFromDatabase()
{
    QSqlQuery query("SELECT output FROM problems WHERE problemId = " + problemId());
    QByteArray output = query.value(0).toByteArray();
    return output;
}

void Job::start()
{
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
    m_process = new QProcess(this);
    QStringList arguments;
    arguments << sourceFilePath() << "-o" << executableFilePath();
    connect (m_process, finished(int), this, compileFinished(int));
    m_process->start(compilerPath(), arguments);
    // use some const value instead of hard coding
    connect (m_timer, SIGNAL(timeout()), this, SLOT(compileTimeout()));
    m_timer->start(10000);
}

void Job::compileTimeout()
{
    disconnect(m_process);
    m_process->kill();
    m_result = "compileTimeout";
    emit finished(true);
}

void Job::compileFinished(int exitCode)
{
    disconnect(m_timer);
    if (exitCode != 0)
    {
        m_result = "compileError";
        emit finished(true);
        return;
    }

    executeFile();
}

void Job::executeFile()
{
    disconnect(m_process);
    connect(m_process, finished(int), this, executionFinished(int));
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
    disconnect(m_process);
    m_process->kill();
    m_result = "executionTimeout";
    emit finished(true);
}

void Job::executionFinished(int exitCode)
{
    disconnect(m_timer);
    if (exitCode != 0)
    {
        m_result = "runtimeError";
        emit finished(true);
        return;
    }

    checkOutput();
}


void Job::checkOutput()
{
    QByteArray output = m_process->readAllStandardOutput();
    QByteArray expectedOutput = retrieveOutputFromDatabase();
    if (output == expectedOutput)
    {
        m_result = "match";
        emit finished(true);
    }
    else
    {
        m_result = "wrongAnswer";
        emit finished(true);
    }
}