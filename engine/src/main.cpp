#include "Application.h"


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{

	Application& application = Application::getInstance();

	application.Init(SCR_WIDTH, SCR_HEIGHT);

	application.LateInit();

	application.Update();

	return 0;
}