#pragma once
#include "lib/tcpip/tcplistener.h"
#include <list>
#include <string.h>


size_t cpy(void* dst, const void* src, size_t len)
{
	memcpy(dst, src, len);
	return len;
}

enum class PackageTypes : uint8_t
{
	Unknown			   = 0,
	EVT_Measurement    = 1,	
	REQ_HorizontalInfo = 2,
	RES_HorizontalInfo = 3,
	REQ_TraceInfo      = 4,
	RES_TraceInfo      = 5,
};
	
enum class HorizontalTypes : uint8_t
{
	Timestamp              = 1,	
	SecondsSinceStart = 2,
};
	
enum class TraceTypes : uint8_t
{
	Analog  = 1,	
	Digital = 2,
	State   = 3,
};

class Command
{
public:
	PackageTypes packageType = PackageTypes::Unknown;
	TCPSocket* sender = NULL;
};

class CMD_RequestHorizontalInfo : public Command
{
public:
	CMD_RequestHorizontalInfo(TCPSocket* sender, uint8_t* data, size_t size)
	{
		if (size < 1) return;
		this->sender = sender;
		packageType = PackageTypes::REQ_HorizontalInfo;
	}
};

class CMD_RequestTraceInfo : public Command
{
public:
	uint8_t traceNo;
	
	CMD_RequestTraceInfo(TCPSocket* sender, uint8_t* data, size_t size)
	{
		if (size < 2) return;
		this->sender = sender;
		packageType = PackageTypes::REQ_TraceInfo;
		traceNo = data[1];
	}
	
};

class CMD_ResponseHorizontalInfo : public Command
{
public:
	HorizontalTypes type;
	double scale;
	double offset;
	
	CMD_ResponseHorizontalInfo(TCPSocket* sender)
	{
		this->sender = sender;
		packageType = PackageTypes::RES_HorizontalInfo;
	}
	
	size_t Serialize(uint8_t* data, size_t size)
	{
		if (size < 18)
			return 0;
		size_t ptr = 0;
		ptr += cpy(&data[ptr], &packageType, sizeof(packageType));
		ptr += cpy(&data[ptr], &type, sizeof(type));
		ptr += cpy(&data[ptr], &scale, sizeof(scale));
		ptr += cpy(&data[ptr], &offset, sizeof(offset));
		return ptr;
	}
	
	
};

class CMD_ResponseTraceInfo : public Command
{
public:
	uint8_t traceNo;
	TraceTypes type;
	double scale;
	double offset;
	
	CMD_ResponseTraceInfo(TCPSocket* sender)
	{
		this->sender = sender;
		packageType = PackageTypes::RES_TraceInfo;
	}
	
	size_t Serialize(uint8_t* data, size_t size)
	{
		if (size < 19)
			return 0;
		size_t ptr = 0;
		ptr += cpy(&data[ptr], &packageType, sizeof(packageType));
		ptr += cpy(&data[ptr], &traceNo, sizeof(traceNo));
		ptr += cpy(&data[ptr], &type, sizeof(type));
		ptr += cpy(&data[ptr], &scale, sizeof(scale));
		ptr += cpy(&data[ptr], &offset, sizeof(offset));
		return ptr;
	}
	
};

class CMD_EvtMeasurement : public Command
{
public:
	uint8_t traceNo;
	double xValue;
	double yValue;
	
	CMD_EvtMeasurement()
	{
		this->packageType = PackageTypes::EVT_Measurement;
	}
	
	size_t Serialize(uint8_t* data, size_t size)
	{
		if (size < 18)
			return 0;
		size_t ptr = 0;
		ptr += cpy(&data[ptr], &packageType, sizeof(packageType));
		ptr += cpy(&data[ptr], &traceNo, sizeof(traceNo));
		ptr += cpy(&data[ptr], &xValue, sizeof(xValue));
		ptr += cpy(&data[ptr], &yValue, sizeof(yValue));
		return ptr;
	}
};
	
class Trace
{
public:
	uint8_t traceNo;
	TraceTypes type;
	double scale;
	double offset;
};

class TraceDebugger
{
public:
	HorizontalTypes HorizontalType = HorizontalTypes::SecondsSinceStart;
	double HorizontalScale = 30 * 60; //30 minutes
	double HorizontalOffset = 0;
	std::list<Trace> Traces;
	
private:	
	FreeRTOS::Queue<Command*> commands;
	FreeRTOS::Task task;
	std::list<TCPSocket*> sockets;
	TCPListener listener; 
	

	void DataReceived(TCPSocket* socket, uint8_t* data, uint32_t size)
	{
		ESP_LOGI("TraceDebugger", "Data rx len=%d", size);
		
		for (int i = 0; i < size; i++)
			printf("%2X ", data[i]);
		printf("\r\n");
		
		Command* cmd = NULL;
		
		switch ((PackageTypes)data[0])
		{
		case PackageTypes::REQ_HorizontalInfo:
			ESP_LOGI("TraceDebugger", "Horizontal info requested");
			cmd = new CMD_RequestHorizontalInfo(socket, data, size);
			break;
			
		case PackageTypes::REQ_TraceInfo:
			ESP_LOGI("TraceDebugger", "Trace info requested");
			cmd = new CMD_RequestTraceInfo(socket, data, size);
			break;
			
		default:
			ESP_LOGE("TraceDebugger", "Unknown command received %d", data[0]);
			break;
		}
		
		if (cmd != NULL)
		{
			if (!commands.Enqueue(&cmd, 100))
				delete cmd;
		}
	}
	
	void OnDisconnected(TCPSocket* socket)
	{
		sockets.remove(socket);
		delete socket;
		ESP_LOGI("TraceDebugger", "Client disconnected");
	}
	
	void ClientAccepted(TCPSocket* socket)
	{
		ESP_LOGI("TraceDebugger", "Client connected");
		sockets.push_back(socket);
		socket->OnDataReceived.Bind(this, &TraceDebugger::DataReceived);
		socket->OnDisconnected.Bind(this, &TraceDebugger::OnDisconnected);
	}
	
	void HandleCommand(CMD_EvtMeasurement* command)
	{
		uint8_t data[32];
		size_t size = command->Serialize(data, sizeof(data));
		for (auto const& socket : sockets) 
		{
			socket->Send(data, size);
		}
	}
	
	void HandleCommand(CMD_RequestHorizontalInfo* command)
	{
		CMD_ResponseHorizontalInfo response(command->sender);
		response.offset = HorizontalOffset;
		response.scale = HorizontalScale;
		response.type = HorizontalType;
		uint8_t data[sizeof(CMD_ResponseHorizontalInfo)];
		size_t size = response.Serialize(data, sizeof(data));
		command->sender->Send(data, size);
		ESP_LOGI("TraceDebugger", "Horizontal info send");
	}
	
	
	void HandleCommand(CMD_RequestTraceInfo* command)
	{
		for (auto& trace : Traces)
		{
			if (trace.traceNo == command->traceNo)
			{
				CMD_ResponseTraceInfo response(command->sender);
				response.offset = trace.offset;
				response.scale = trace.scale;
				response.type = trace.type;
				response.traceNo = trace.traceNo;
				uint8_t data[sizeof(CMD_ResponseTraceInfo)];
				size_t size = response.Serialize(data, sizeof(data));
				command->sender->Send(data, size);
				ESP_LOGI("TraceDebugger", "Trace info send");
			}
		}
	}
	

	void Work(FreeRTOS::Task* task, void* args)
	{
		while (true)
		{
			Command* cmd;
			if (commands.Dequeue(&cmd, 10000))
			{
				ESP_LOGI("TraceDebugger", "Handling cmd %d", (uint8_t)cmd->packageType);
				
				switch ((PackageTypes)cmd->packageType)
				{
				case PackageTypes::REQ_HorizontalInfo:
					HandleCommand((CMD_RequestHorizontalInfo*)cmd);
					break;
				case PackageTypes::REQ_TraceInfo:
					HandleCommand((CMD_RequestTraceInfo*)cmd);
					break;
				case PackageTypes::EVT_Measurement:
					HandleCommand((CMD_EvtMeasurement*)cmd);
					break;
				default:
					break;
						
				}
				delete cmd;
			}
		}
	}
	

	
public:
	
	void SendMeasurementTest(double x, double y, uint8_t traceNo)
	{
		CMD_EvtMeasurement* cmd = new CMD_EvtMeasurement();
		cmd->traceNo = traceNo;
		cmd->xValue = x;
		cmd->yValue = y;
		if (!commands.Enqueue((Command**)&cmd, 1000))
			delete cmd;
	}
	
	TraceDebugger()
	{
		commands.Initialize(20);
		
		listener.OnSocketAccepted.Bind(this, &TraceDebugger::ClientAccepted);
		
		Trace trace
		{
			.traceNo = 1,
			.type = TraceTypes::Analog,
			.scale = 100,
			.offset = 0,
		};
		
		Traces.push_back(trace);
		
		task.SetCallback(this, &TraceDebugger::Work);
		
	}
	
	void Start()
	{
		listener.StartListener();
		task.Run("TraceDebugger", 7, 1024 * 4, NULL);
	}
	
	
};

