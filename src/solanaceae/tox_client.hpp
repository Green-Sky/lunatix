#pragma once

#include <solanaceae/core/tox_default_impl.hpp>
#include <solanaceae/core/tox_event_interface.hpp>
#include <solanaceae/core/tox_event_provider_base.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <functional>

struct ToxEventI;

class ToxClient : public ToxDefaultImpl, public ToxEventProviderBase {
	private:
		bool _should_stop {false};

		std::function<void(const Tox_Events*)> _subscriber_raw {[](const auto*) {}}; // only 1?

		std::chrono::time_point<std::chrono::high_resolution_clock> _last_time {std::chrono::high_resolution_clock::now()};

		std::string _self_name;

		std::string _tox_profile_path;
		bool _tox_profile_dirty {true}; // set in callbacks

		//std::vector<uint8_t> _join_group_after_dht_connect;

#if 0
		struct Peer {
			std::string name;
		};
		// key groupid, key peerid
		std::map<uint32_t, std::map<uint32_t, Peer>> _groups;
#endif


	public:
		//ToxClient(/*const CommandLine& cl*/);
		ToxClient(std::string_view save_path);
		~ToxClient(void);

	public: // tox stuff
		Tox* getTox(void) { return _tox; }

		void setDirty(void) { _tox_profile_dirty = true; }

		// returns false when we shoul stop the program
		bool iterate(void);
		void stop(void); // let it know it should exit

		void setToxProfilePath(const std::string& new_path) { _tox_profile_path = new_path; }
		void setSelfName(std::string_view new_name) { _self_name = new_name; toxSelfSetName(new_name); }

		//std::string_view getGroupPeerName(uint32_t group_number, uint32_t peer_number) const;
		//TOX_CONNECTION getGroupPeerConnectionStatus(uint32_t group_number, uint32_t peer_number) const;

#if 0
		template<typename FN>
		void forEachGroup(FN&& fn) const {
			for (const auto& it : _groups) {
				fn(it.first);
			}
		}

		template<typename FN>
		void forEachGroupPeer(uint32_t group_number, FN&& fn) const {
			if (_groups.count(group_number)) {
				for (const auto& [peer_number, peer] : _groups.at(group_number)) {
					fn(peer_number);
				}
			}
		}
#endif

	public: // raw events
		void subscribeRaw(std::function<void(const Tox_Events*)> fn);

	private:
		void saveToxProfile(void);
};

