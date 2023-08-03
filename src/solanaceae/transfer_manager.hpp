#pragma once

#include <solanaceae/toxcore/tox_event_interface.hpp>

#include <string_view>
#include <vector>
#include <map>
#include <memory>

// fwd
struct ToxI;

class TransferManager : public ToxEventI {
	ToxI& _t;

	public:
		struct FileRI {
			virtual ~FileRI(void) {}
			virtual std::vector<uint8_t> read(uint64_t pos, uint32_t size) = 0;
		};

	private:
		// friend_number -> transfer_id -> FileRI
		std::map<uint32_t, std::map<uint32_t, std::unique_ptr<FileRI>>> _friend_transfers_sending;

	public:
		TransferManager(ToxI& t, ToxEventProviderI& tep);
		~TransferManager(void);

		void iterate(void);

	public:
		bool friendSendMem(uint32_t friend_number, uint32_t file_kind, std::string_view filename, const std::vector<uint8_t>& data);

	protected: // events
		bool onToxEvent(const Tox_Event_File_Recv* e) override;
		bool onToxEvent(const Tox_Event_File_Recv_Control* e) override;
		bool onToxEvent(const Tox_Event_File_Recv_Chunk* e) override;
		bool onToxEvent(const Tox_Event_File_Chunk_Request* e) override;
};

