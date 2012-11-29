#include <QtCore/QtCore>
#include <application.h>
#include <queryinterface.h>

Application::Application(int argc, char **argv)
    : QCoreApplication(argc, argv)
{
    m_queryInterface = new QueryInterface();
}

Application::~Application()
{
    delete m_queryInterface;
}

QueryInterface* Application::queryInterface()
{
    return m_queryInterface;
}

Application *Application::instance()
{
    return (qobject_cast<Application *>(QCoreApplication::instance()));
}