#include "WindowsInput.h"

static LRESULT CALLBACK EventHandler(HWND, unsigned, WPARAM, LPARAM);

static HWND whandler;

WindowsInput::WindowsInput()
{
	packaged_task init([&]
	{
		WNDCLASS window_class = {};
		window_class.lpfnWndProc = EventHandler;
		window_class.hInstance = GetModuleHandle(nullptr);
		window_class.lpszClassName = L"Metalwork";

		if (!RegisterClass(&window_class))
			throw runtime_error(format("RegisterClass failed. Error {}", GetLastError()));

		whandler = CreateWindow(window_class.lpszClassName, L"Metalwork Input", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
		if (whandler == nullptr)
			throw runtime_error(format("CreateWindow failed. Error {}", GetLastError()));

		SetWindowLongPtr(whandler, GWLP_USERDATA, LONG_PTR(this));

		SetTimer(whandler, 0, 250, nullptr);
	});

	thread = jthread([&, this](stop_token st)
	{
		init();
		stop_callback stop(st, [=]{ DestroyWindow(whandler); });

		MSG msg = {};
		while (GetMessage(&msg, nullptr, 0, 0) && msg.message != WM_DESTROY)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	});

	init.get_future().get();
}

void WindowsInput::RegisterDevices()
{
	unsigned num = 0;
	GetRegisteredRawInputDevices(nullptr, &num, sizeof(RAWINPUTDEVICE));

	vector<RAWINPUTDEVICE> devices(num);
	GetRegisteredRawInputDevices(devices.data(), &num, sizeof(RAWINPUTDEVICE));

	auto mouse = ranges::find_if(devices, [](RAWINPUTDEVICE& dev){ return dev.usUsagePage == 0x01 && dev.usUsage == 0x02; });
	if (mouse != devices.end() && mouse->hwndTarget != whandler)
	{
		mouse->hwndTarget = whandler;
		RegisterRawInputDevices(devices.data(), num, sizeof(RAWINPUTDEVICE));
	}
}

static LRESULT CALLBACK EventHandler(HWND hwnd, unsigned msg, WPARAM wparam, LPARAM lparam)
{
	WindowsInput* winput = (WindowsInput*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
		case WM_INPUT:
		{
			unsigned size = sizeof(RAWINPUT);
			RAWINPUT raw = {};
			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

			if (raw.header.dwType == RIM_TYPEMOUSE)
			{
				winput->pointer += {raw.data.mouse.lLastX, -raw.data.mouse.lLastY};

				Input input;
				input.pointer = winput->pointer;
				input.time = steady_clock::now();

				/*if (raw.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
					int wheel = bit_cast<short>(raw.data.mouse.usButtonData) / WHEEL_DELTA;*/

				winput->inputs.push(input);
			}

			return 0;
		}

		case WM_TIMER: // reregister raw input target for mouse input
		{
			winput->RegisterDevices();
			SetTimer(hwnd, 0, 250, nullptr);
			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
