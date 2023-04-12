#include "./transfer_manager.hpp"

#include <solanaceae/core/tox_interface.hpp>

#include <memory>
#include <vector>
#include <cassert>

struct FileRMem : public TransferManager::FileRI {
	std::vector<uint8_t> data;

	FileRMem(const std::vector<uint8_t>& data_) : data(data_) {}
	virtual ~FileRMem(void) {}

	std::vector<uint8_t> read(uint64_t pos, uint32_t size) {
		if (pos >= data.size()) {
			return {};
		}

		// TODO: optimize
		std::vector<uint8_t> chunk;
		for (size_t i = 0; i < size && i+pos < data.size(); i++) {
			chunk.push_back(data[pos+i]);
		}
		return chunk;
	}
};

TransferManager::TransferManager(ToxI& t, ToxEventProviderI& tep) : _t(t) {
	tep.subscribe(this, Tox_Event::TOX_EVENT_FILE_RECV);
	tep.subscribe(this, Tox_Event::TOX_EVENT_FILE_RECV_CONTROL);
	tep.subscribe(this, Tox_Event::TOX_EVENT_FILE_RECV_CHUNK);
	tep.subscribe(this, Tox_Event::TOX_EVENT_FILE_CHUNK_REQUEST);
}

TransferManager::~TransferManager(void) {
}

void TransferManager::iterate(void) {
}

bool TransferManager::friendSendMem(uint32_t friend_number, uint32_t file_kind, std::string_view filename, const std::vector<uint8_t>& data) {
	// TODO: change this?
	const auto data_hash = _t.toxHash(data);
	assert(data_hash.size() >= TOX_FILE_ID_LENGTH);

	const auto&& [transfer_id, err] = _t.toxFileSend(friend_number, file_kind, data.size(), data_hash, filename);
	if (err == TOX_ERR_FILE_SEND_OK) {
		_friend_transfers_sending[friend_number][transfer_id.value()] = std::make_unique<FileRMem>(data);
		return true;
	} else {
		return false;
	}
}


bool TransferManager::onToxEvent(const Tox_Event_File_Recv* e) {
	return false;
}

bool TransferManager::onToxEvent(const Tox_Event_File_Recv_Control* e) {
	tox_event_file_recv_control_get_friend_number(e);
	tox_event_file_recv_control_get_file_number(e);
	tox_event_file_recv_control_get_control(e);
	return false;
}

bool TransferManager::onToxEvent(const Tox_Event_File_Recv_Chunk* e) {
	return false;
}

bool TransferManager::onToxEvent(const Tox_Event_File_Chunk_Request* e) {
	const auto friend_number = tox_event_file_chunk_request_get_friend_number(e);
	const auto file_number = tox_event_file_chunk_request_get_file_number(e);
	const auto position = tox_event_file_chunk_request_get_position(e);
	const auto length = tox_event_file_chunk_request_get_length(e);

	if (!_friend_transfers_sending.count(friend_number) || !_friend_transfers_sending[friend_number].count(file_number)) {
		return false; // shrug, we don't know about it, might be someone else's
	}

	const auto data = _friend_transfers_sending[friend_number][file_number]->read(position, length);

	const auto err = _t.toxFileSendChunk(friend_number, file_number, position, data);

	return true;
}

