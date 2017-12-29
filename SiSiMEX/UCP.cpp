#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO: Add other states...
	ST_WAITING_RESPONSE,
	ST_NEGOTIATING,
	ST_FINISHED
};

UCP::UCP(Node *node, uint16_t requestedItemId, const AgentLocation &uccLocation) :
	Agent(node),
	_requestedItemId(requestedItemId),
	_uccLocation(uccLocation),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

UCP::~UCP()
{
}

void UCP::update()
{
	switch (state())
	{
	case ST_INIT:
		requestItem();
		setState(ST_WAITING_RESPONSE);
		break;

	// TODO: Handle other states
	case ST_NEGOTIATING:
		if (_mcp->negotiationFinished()) {
			if (_mcp->negotiationAgreement()) {
				sendConstraint(_mcp->requestedItemId());
				setState(ST_WAITING_RESPONSE);
			}
			else {
				setState(ST_FINISHED);
			}
		}

	default:;
	}
}

void UCP::finalize()
{
	destroyChildMCP();
	finish();
}

void UCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;
	// TODO: Handle requests

	if (state() == ST_WAITING_RESPONSE && packetType == PacketType::RequestUCPForConstraint)
	{
		iLog << "UCP Requested for Constraint";

		PacketRequestUCCForItem data;
		data.Read(stream);

		if (data.itemId == NULL_ITEM_ID)
		{
			_negotiationAgreement = true;
			setState(ST_FINISHED);
		}
		else
		{
			createChildMCP(data.itemId);
			setState(ST_NEGOTIATING);
		}
	}

	else if (state() == ST_WAITING_RESPONSE && packetType == PacketType::SendItemRequestedUCP)
	{
		iLog << "UCP receiving item requested: " << _requestedItemId;

		_negotiationAgreement = true;
		setState(ST_FINISHED);
	}
}

bool UCP::negotiationFinished() const {
	// TODO
	return (state() == ST_FINISHED);
}

bool UCP::negotiationAgreement() const {
	// TODO
	return _negotiationAgreement;
}


void UCP::requestItem()
{
	// TODO

	//sendPacketToHost(ip, port, ostream);

	iLog << "UCP requesting item: " << _requestedItemId;

	PacketHeader head;
	head.srcAgentId = id();
	head.dstAgentId = _uccLocation.agentId;
	head.packetType = PacketType::RequestUCCForItem;
	PacketRequestUCCForItem data;
	data.itemId = _requestedItemId;

	OutputMemoryStream stream;
	head.Write(stream);
	data.Write(stream);
	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO

	//sendPacketToHost(ip, port, ostream);

	iLog << "UCP sending constraint: " << constraintItemId;

	PacketHeader head;
	head.srcAgentId = id();
	head.dstAgentId = _uccLocation.agentId;
	head.packetType = PacketType::SendConstraintRequestedUCC;
	PacketSendConstraintRequestedUCC data;
	data.itemId = constraintItemId;

	OutputMemoryStream stream;
	head.Write(stream);
	data.Write(stream);
	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::createChildMCP(uint16_t constraintItemId)
{
	// TODO
	_mcp = std::make_shared<MCP>(node(), constraintItemId);
	g_AgentContainer->addAgent(_mcp);
}

void UCP::destroyChildMCP()
{
	// TODO
	if (_mcp) _mcp->finalize();
}
