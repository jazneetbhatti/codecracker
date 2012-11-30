#ifndef ABSTRACTCHECKER_H
#define ABSTRACTCHECKER_H

#include <QtCore/QtCore>

class AbstractChecker : public QObject
{
    Q_OBJECT
public:
    AbstractChecker(QObject *parent = 0) : QObject(parent) {};
    ~AbstractChecker() {};
    virtual QString checkerType() = 0;

    virtual void setOutput(QByteArray output) {Q_UNUSED(output);};
    virtual void setExpectedOutput(QByteArray expectedOutput) {Q_UNUSED(expectedOutput);}
    virtual void setVeirfier(QStringList verifierArgList) {Q_UNUSED(verifierArgList);}

    virtual void start() = 0;

signals:
    void statusChanged(QString status);
    void finished();
};

#endif // ABSTRACTCHECKER_H