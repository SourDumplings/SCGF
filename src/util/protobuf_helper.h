#pragma once

#include "proto/pb/SCGF_CS.pb.h"
#include "singleton.h"

namespace scgf::util
{
class CProtobufHelper final : public CSingleton<CProtobufHelper>
{
public:
	CProtobufHelper();

	void init();

	google::protobuf::Message* create_message(protocol::scgf::cs::SCGFCSMessageID msgId);

private:
	std::unordered_map<protocol::scgf::cs::SCGFCSMessageID, const google::protobuf::Descriptor*>
		m_registry;
	std::string m_packageName;

	bool ends_with(const std::string& name, const std::string& suffix);

	bool starts_with(const std::string& name, const std::string& prefix);

	bool has_prefix(const std::string& name);
};

}  // namespace scgf::util