#ifndef COMPARECHECKER_H
#define COMPARECHECKER_H

#include "abstractchecker.h"

#include <QtCore/QString>

class CompareChecker : public AbstractChecker
{
    Q_OBJECT
public:
    CompareChecker(QObject *parent = 0);
    ~CompareChecker();
    QString checkerType() { return QString("CompareChecker"); }

    void setOutput(QByteArray output);
    void setExpectedOutput(QByteArray expectedOutput);

    void start();

private:
    void compare();

    QByteArray m_output;
    QByteArray m_expectedOutput;
};

#endif // COMPARECHECKER_H