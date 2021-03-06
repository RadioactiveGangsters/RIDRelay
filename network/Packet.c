#include "Packet.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

#include "../Diagnostic/Log.h"

void(*const csreader[OPC__MAXOPCODE+1])(const int,const int)=
{
	NULL,
	NULL,
	&readcsPing,
	&readcsUpdate,
	&readcsGraph,
	&readcsAlarm,
	&readcsSetBounds,
	NULL,
},(*const screader[OPC__MAXOPCODE+1])(const int,const int)=
{
	NULL,
	&readscLogin,
	&readscPing,
	&readscUpdate,
	&readscGraph,
	&readscAlarm,
	NULL,
	NULL,
};

static uint8_t readByte(const int source,const int destination)
{
	uint8_t i=0;
	(void)recv(source, &i, sizeof(uint8_t),MSG_WAITALL);
	(void)write(destination, &i, sizeof(uint8_t));
	return i;
}

static uint32_t readInt(const int source,const int destination)
{
	uint32_t i=0;
	(void)recv(source, &i, sizeof(uint32_t),MSG_WAITALL);
	(void)write(destination, &i, sizeof(uint32_t));
	// uncomment if in netbyteorder instead of hostbyteorder
	i=ntohl(i);
	return i;
}

#define Output(...) Log(LOGT_TUNNEL,LOGL_RESULT,__VA_ARGS__);

void readcsPing(const int client, const int server)
{
	(void)client;
	(void)server;
	Output("\n> 0x%02x ; PONG",OPC_PING);
}

void readcsUpdate(const int client, const int server)
{
	(void)client;
	(void)server;
	Output("\n> 0x%02x ; update request",OPC_UPDATE);
}

void readcsGraph(const int client, const int server)
{
	uint32_t x;
	Output("\n> 0x%02x ; graph request",OPC_GRAPH);
	x=readInt(client,server);
	Output("\t0x%08x ; name length: %d",x,x);
	while(x-->0)
	{
		uint8_t c=readByte(client,server);
		Output("\t0x%08x ; '%c'",c,c);
	}
}

void readscLogin(const int server, const int client)
{
	int x;
	Output("\n< 0x%02x ; opcode: Login",OPC_LOGIN);
	x=(int)readInt(server,client);
	Output("\t0x%08x ; protocol version: %d",x,x);
}

void readscPing(const int server, const int client)
{
	(void)client;
	(void)server;
	Output("\n< 0x%02x ; PING",OPC_PING);
}

void readscUpdate(const int server,const int client)
{
	int x;
	Output("\n< 0x%02x ; opcode: Update",OPC_UPDATE);
	Output("\t0x%08x ; unittype",readInt(server,client));
	x=(int)readInt(server,client);
	Output("\t0x%08x ; amount of sensors: %d",x,x);
	while(x-->0)
	{
		Output("\t\t0x%08x",readInt(server,client));
	}

}

void readscGraph(const int server,const int client)
{
	int x;
	Output("\n< 0x%02x ; opcode: Graph",OPC_GRAPH);
	Output("\t0x%08x ; unit code", readInt(server,client));
	x=(int)readInt(server,client);
	Output("\t0x%08x ; %d datapoints inbound", x,x);
	while(x-->0)
	{
		Output("\t\t0x%08x", readInt(server, client));
	}
	
}

void readscAlarm(const int server,const int client)
{
	Output("\n< 0x%02x ; opcode: Alarm",OPC_ALARM);
	Output("\t0x%08x ; unit code", readInt(server,client));
	Output("\t0x%08x ; current sensor value", readInt(server,client));
	Output("\t0x%08x ; counteraction code",readInt(server,client));
	Output("\t0x%08x ; sensor number",readInt(server,client));
}

void readcsAlarm(const int client, const int server)
{
	int x;
	Output("\n> 0x%02x ; opcode: Alarm",OPC_ALARM);
	x=(int)readInt(client, server);
	Output("\t0x%08x ; namelength: %d", x,x);
	
	while(x-->0)
	{
		Output("\t0x%08x ; name", readByte(client, server));
	}
}

void readcsSetBounds(const int client, const int server)
{
	int x, min, max;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n> 0x%02x ; opcode: Bounds",OPC_BOUNDS);
	x=(int)readInt(client, server);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; namelength: %d", x,x);
	
	while(x-->0)
	{
		Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; name", readByte(client, server));
	}
	min=(int)readInt(client, server);
	max=(int)readInt(client, server);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; setmin: %d", min, min);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; setmax: %d", max, max);
}
