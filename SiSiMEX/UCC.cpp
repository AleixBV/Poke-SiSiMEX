#include "UCC.h"



enum State
{
	// TODO: Add some states
	ST_IDLE,
	ST_CONSTRAINT_REQUEST,
	ST_FINISHED
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId),
	_negotiationAgreement(false)
{
	setState(ST_IDLE);
}

UCC::~UCC()
{
}

void UCC::update()
{
	// Nothing to do
}

void UCC::finalize()
{
	finish();
}

void UCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	// TODO Receive requests and send back responses...
	if (state() == ST_IDLE && packetType == PacketType::RequestUCCForItem)
	{
		PacketRegisterMCC data;
		data.Read(stream);

		if (data.itemId == _contributedItemId)
		{
			if (_constraintItemId != NULL_ITEM_ID)
			{
				iLog << "UCC constraint: " << _constraintItemId;
				setState(ST_CONSTRAINT_REQUEST);

				PacketHeader head;
				head.srcAgentId = id();
				head.dstAgentId = packetHeader.srcAgentId;
				head.packetType = PacketType::RequestUCPForConstraint;
				PacketRegisterMCC data;
				data.itemId = _constraintItemId;

				OutputMemoryStream stream;
				head.Write(stream);
				data.Write(stream);
				sendPacketToHost(socket->RemoteAddress().GetIPString(), LISTEN_PORT_AGENTS, stream);
			}
			else
			{
				iLog << "UCC doesn't have constraint, sending Item to UCP";
				sendItem(socket->RemoteAddress().GetIPString(), packetHeader.srcAgentId);
			}
		}
		else
			iLog << "You shouldn't be here";
	}

	else if (state() == ST_CONSTRAINT_REQUEST && packetType == PacketType::SendConstraintRequestedUCC)
	{
		iLog << "UCC constraint solved: " << _constraintItemId << " Sending Item to agent: " << packetHeader.srcAgentId;
		sendItem(socket->RemoteAddress().GetIPString(), packetHeader.srcAgentId);
	}
}

void UCC::sendItem(const std::string IPHost, const uint16_t dst)
{
	setState(ST_FINISHED);
	_negotiationAgreement = true;

	PacketHeader head;
	head.srcAgentId = id();
	head.dstAgentId = dst;
	head.packetType = PacketType::SendItemRequestedUCP;
	PacketRegisterMCC data;
	data.itemId = _contributedItemId;

	OutputMemoryStream stream;
	head.Write(stream);
	data.Write(stream);
	sendPacketToHost(IPHost, LISTEN_PORT_AGENTS, stream);
	iLog << "Item sent";
}

bool UCC::negotiationFinished() const {
	// TODO
	bool ret = (state() == ST_FINISHED);
	return ret;
}

bool UCC::negotiationAgreement() const {
	// TODO
	return _negotiationAgreement;
}
