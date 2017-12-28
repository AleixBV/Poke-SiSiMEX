#include "MCP.h"
#include "UCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	// TODO: Add other states
	ST_REQUESTING_NEGOTIATION,
	ST_NEGOTIATING,
	ST_FINISHED
};

MCP::MCP(Node *node, uint16_t itemId) :
	Agent(node),
	_itemId(itemId),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

MCP::~MCP()
{
}

void MCP::update()
{
	switch (state())
	{
	case ST_INIT:
		queryMCCsForItem(_itemId);
		setState(ST_REQUESTING_MCCs);
		break;

	// TODO: Handle other states

	case ST_REQUESTING_NEGOTIATION:
		if (_mccRegisters.empty()) {
			setState(ST_FINISHED);
			break;
		}
		sendNegotiationRequest(_mccRegisters[_mccRegisterIndex]);
		break;

	case ST_NEGOTIATING:
		if (_ucp->negotiationFinished())
		{
			setState(ST_FINISHED);
			if (_ucp->negotiationAgreement())
				_negotiationAgreement = true;
			else
				_negotiationAgreement = false;
		}
		break;

	default:;
	}
}

void MCP::finalize()
{
	destroyChildUCP();
	finish();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REQUESTING_MCCs && packetType == PacketType::ReturnMCCsForItem)
	{
		iLog << "OnPacketReceived PacketType::ReturnMCCsForItem " << _itemId;

		// Read the packet
		PacketReturnMCCsForItem packetData;
		packetData.Read(stream);

		for (auto &mccdata : packetData.mccAddresses)
		{
			uint16_t agentId = mccdata.agentId;
			const std::string &hostIp = mccdata.hostIP;
			uint16_t hostPort = mccdata.hostPort;
			iLog << " - MCC: " << agentId << " - host: " << hostIp << ":" << hostPort;
		}

		// Collect MCC compatible from YP
		_mccRegisters.swap(packetData.mccAddresses);

		// Select MCC to negociate
		_mccRegisterIndex = 0;
		setState(ST_REQUESTING_NEGOTIATION);

		socket->Disconnect();
	}

	// TODO: Handle other responses
	else if (state() == ST_REQUESTING_NEGOTIATION && packetType == PacketType::AwnserMCPNegotiation)
	{
		iLog << "MCC with id: " << packetHeader.srcAgentId << " agreeds to start a negotiation";
		setState(ST_NEGOTIATING);

		PacketAnswerMCPNegotiation packetData;
		packetData.Read(stream);
		AgentLocation ucc;
		ucc.hostIP = socket->RemoteAddress().GetIPString();
		ucc.hostPort = LISTEN_PORT_AGENTS;
		ucc.agentId = packetData.UCC_ID;

		createChildUCP(ucc);
	}
}

bool MCP::negotiationFinished() const
{
	// TODO
	return (state() == ST_FINISHED);
}

bool MCP::negotiationAgreement() const
{
	// TODO
	return _negotiationAgreement;
}


bool MCP::queryMCCsForItem(int itemId)
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::QueryMCCsForItem;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketQueryMCCsForItem packetData;
	packetData.itemId = _itemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	// 1) Ask YP for MCC hosting the item 'itemId'
	return sendPacketToYellowPages(stream);
}

bool MCP::sendNegotiationRequest(const AgentLocation &mccRegister)
{
	// TODO
	//IHL, TotalLength, SourceAdress, DestinationAdress

	PacketHeader packetHead;
	packetHead.packetType = PacketType::RequestMCCNegotiation;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = mccRegister.agentId;
	OutputMemoryStream stream;
	packetHead.Write(stream);

	return sendPacketToHost(mccRegister.hostIP, mccRegister.hostPort, stream);
	return false;
}

void MCP::createChildUCP(const AgentLocation &uccLoc)
{
	// TODO
	_ucp = std::make_shared<UCP>(node(), _itemId, uccLoc);
	g_AgentContainer->addAgent(_ucp);
}

void MCP::destroyChildUCP()
{
	// TODO
	if (_ucp)
		_ucp->finalize();
}
