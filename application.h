#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore/QtCore>

#define dApp Application::instance()

class QueryInterface;

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    Application(int argc, char **argv);
    ~Application();

    static Application* instance();
    QueryInterface* queryInterface();

private:
    QueryInterface *m_queryInterface;
};

#endif // APPLICATION_H
