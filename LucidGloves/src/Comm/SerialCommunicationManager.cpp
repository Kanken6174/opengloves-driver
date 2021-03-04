#include <Comm/SerialCommunicationManager.h>

void SerialManager::Connect() {
	//We're not yet connected
	m_isConnected = false;

	//Try to connect to the given port throuh CreateFile
	m_hSerial = CreateFile(m_configuration.port.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (this->m_hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {

			DebugDriverLog("Serial error: Handle was not attached. Reason: not available.");
		}
		else
		{
			DebugDriverLog("Serial error:Received error connecting to port");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(m_hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			DebugDriverLog("Serial error: failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;

			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//set the parameters and check for their proper application
			if (!SetCommState(m_hSerial, &dcbSerialParams))
			{
				DebugDriverLog("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				m_isConnected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}
		}
	}
}

void SerialManager::BeginListener(const std::function<void(VRCommData_t)>& callback) {
	//DebugDriverLog("Begun listener");
	m_threadActive = true;
	m_serialThread = std::thread(&SerialManager::ListenerThread, this, callback);
}

void SerialManager::ListenerThread(const std::function<void(VRCommData_t)>& callback) {
	//DebugDriverLog("In listener thread");
	std::this_thread::sleep_for(std::chrono::milliseconds(ARDUINO_WAIT_TIME));
	PurgeBuffer();
	std::string receivedString;
	int iteration = 0;
	while (m_threadActive) {
		try {
			int bytesRead = ReceiveNextPacket(receivedString);
			//DebugDriverLog("Received Data: %s", receivedString);
			//For now using base10 but will eventually switch to hex to save space
			//3FF&3FF&3FF&3FF&3FF&1&1&3FF&3FF&1&1\n is 36 chars long.
			//1023&1023&1023&1023&1023&1&1&1023&1023&1&1\n is 43 chars long.

			VRCommData_t commData;

			std::string buf;
			std::stringstream ss(receivedString);

			std::vector<float> tokens;

			while (getline(ss, buf, '&')) tokens.push_back(std::stof(buf));

			for (int i = 0; i < 5; i++) {
				commData.flexion[i] = tokens[i] / c_maxAnalogValue;
				commData.splay[i] = 0.5;
			}

			commData.joyX = (2 * tokens[VRCommDataInputPosition::JOY_X] / c_maxAnalogValue) - 1;
			commData.joyY = (2 * tokens[VRCommDataInputPosition::JOY_Y] / c_maxAnalogValue) - 1;
			commData.trgButton = tokens[VRCommDataInputPosition::BTN_TRG] == 1;
			commData.aButton = tokens[VRCommDataInputPosition::BTN_A] == 1;
			commData.bButton = tokens[VRCommDataInputPosition::BTN_B] == 1;

			commData.grab = tokens[VRCommDataInputPosition::GES_GRAB] == 1;
			commData.pinch = tokens[VRCommDataInputPosition::GES_PINCH] == 1;

			callback(commData);

			receivedString.clear();
		}
		catch (const std::exception& e) {
			DebugDriverLog("Exception caught while trying to convert to int. Skipping...");
		}
	}
}

int SerialManager::ReceiveNextPacket(std::string& buff) {
	DWORD dwCommEvent;
	DWORD dwRead = 0;

	if (!SetCommMask(m_hSerial, EV_RXCHAR)) {
		std::cout << "Error setting comm mask" << std::endl;
	}

	char nextChar;
	int bytesRead = 0;
	if (WaitCommEvent(m_hSerial, &dwCommEvent, NULL)) {
		do {
			if (ReadFile(m_hSerial, &nextChar, 1, &dwRead, NULL))
			{
				buff += nextChar;
				bytesRead++;
			}
			else {
				DebugDriverLog("Read file error");
				break;
			}
		} while (nextChar != '\n');
	}
	else {
		DebugDriverLog("Error in comm event");
	}

	return bytesRead;
}
bool SerialManager::PurgeBuffer() {
	return PurgeComm(m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

void SerialManager::Disconnect() {
	if (m_isConnected) {
		if (m_threadActive) {
			m_threadActive = false;
			m_serialThread.join();
		}
		m_isConnected = false;
		CloseHandle(m_hSerial);


		//Disconnect
	}
}
//May want to get a heartbeat here instead?
bool SerialManager::IsConnected() {
	return m_isConnected;
}