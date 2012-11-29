#include <worker.h>
#include <application.h>

int main(int argc, char* argv[])
{
    Application app(argc, argv);
    Worker w(&app);
    app.exec();
}