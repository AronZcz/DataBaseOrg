#include <thread>
// compiled at start checks PC specs and returns true if the PC meets the requirements if not changes variables in the program to make it compatible with the PC


int ram = 0;
int cpuThreads = 0;
int storageSpace = 0;

int maxWordSize = 0;

void checkSpecs()
{
	cpuThreads = std::thread::hardware_concurrency();
	cpuThreads = cpuThreads == 0 ? 1 : cpuThreads; // If hardware_concurrency returns 0, set it to 1

	maxWordSize = sizeof(void*) * 8; // Get the size of a pointer in bits
}
