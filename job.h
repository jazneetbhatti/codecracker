#ifndef JOB_H
#define JOB_H

#include <Qt/QtCore>

class Job
{
public:
    Job(QObject *parent = 0);
    QString id() { return m_id; }
    QString userId() { return m_userId; }
    QString problemId() { return m_problemId; }
    QString compilerId() { return m_compilerId; }
    QString sourceCode() { return m_sourceCode; }
    QString result() { return m_result; }

    int retrieveTimeoutFromDatabase();
    QByteArray retrieveInputFromDatabase();
    QByteArray retrieveOutputFromDatabase();

public signals:
    void finished(bool success);

public slots:
    void setId(QString &id) { m_id = id; };
    void setUserId(QString &userId) { m_userId = userId; };
    void setProblemId(QString &problemId) { m_problemId = problemId };
    void setCompilerId(QString &compilerId) { m_compilerId = compilerId };
    void setSourceCode(QString &sourceCode) { m_sourceCode = sourceCode };
    void start();

private slots:
    void compileTimeout();
    void compileFinished(int exitCode);
    void executionTimeout();
    void executionFinished(int exitCode);

private:
    QString Job::compilerPath();
    QString Job::sourceFilePath();
    QString Job::executableFilePath();

    void createSourceFile();
    void compileSourceFile();
    void executeFile():
    void checkOutput();

private:
    QProcess *m_process;
    QTimer *m_timer;

    QString m_id;
    QString m_userId;
    QString m_problemId;
    QString m_compilerId;
    QString m_sourceCode;
    QString m_result;
};

#endif // JOB_H