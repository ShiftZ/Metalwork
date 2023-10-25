#pragma once

#include "InputInterface.h"

class WindowsInput : public InputInterface
{
	jthread thread;

public:
	WindowsInput();
	void Start() override { RegisterDevices(); }
	void RegisterDevices();
};

