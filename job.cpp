#include <job.h>

Job::Job(QObject *parent):
    QObject(parent),
    m_process(0)
{
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
    return QString("/tmp/codecracker-src-") + m_id;
}

QString Job::executableFilePath()
{
    return QString("/tmp/codecracker-exe-") + m_id;
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
}

void Job::compileFinished(int exitCode)
{
    if (exitCode != 0)
    {
        m_result = "compileError";
        emit finished();
        return;
    }

    executeFile();
}

void Job::executeFile()
{
    disconnect (m_process);
    connect (m_process, finished(int), this, executionFinished(int));
    m_process->start(executableFilePath());
    // FIXME: use actual input retrived using problemId
    m_process->write("actual_input_goes_here");
    m_process->closeWriteChannel();
}

void Job::executionFinished(int exitCode)
{
    if (exitCode != 0)
    {
        m_result = "runtimeError";
        emit finished();
        return;
    }

    checkOutput();
}


void Job::checkOutput()
{
    QByteArray output = m_process->readAllStandardOutput();
    // FIXME: use actual output retrived using problemId
    if (output == "actual_output_goes_here")
    {
        m_result = "match";
        emit finished();
    }
    else
    {
        m_result = "wrongAnswer";
        emit finished();
    }
}