#include <comparechecker.h>

#include <QtCore/QtCore>

CompareChecker::CompareChecker(QObject *parent)
    : AbstractChecker(parent)
{
}

CompareChecker::~CompareChecker()
{
}

void CompareChecker::setOutput(QByteArray output)
{
    m_output = output;
}

void CompareChecker::setExpectedOutput(QByteArray expectedOutput)
{
    m_expectedOutput = expectedOutput;
}

void CompareChecker::start()
{
    emit statusChanged("compareStarted");
    compare();
}

void CompareChecker::compare()
{
    if (m_output == m_expectedOutput)
    {
        emit statusChanged("correctAnswer");
    }
    else
    {
        emit statusChanged("wrongAnswer");
    }
    emit finished();
}