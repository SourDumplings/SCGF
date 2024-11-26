#include "protobuf_helper.h"

#include <google/protobuf/descriptor_database.h>
#include <unordered_map>

namespace scgf::util
{
CProtobufHelper::CProtobufHelper()
	: m_packageName("protocol.scgf.cs")
{
}

bool CProtobufHelper::ends_with(const std::string& name, const std::string& suffix)
{
	if (name.length() < suffix.length()) { return false; }
	int j = name.length() - 1;
	for (int i = suffix.length() - 1; i > 0; i--, j--)
	{
		if (suffix[i] != name[j]) return false;
	}
	return true;
}

bool CProtobufHelper::starts_with(const std::string& name, const std::string& prefix)
{
	if (name.length() < prefix.length()) { return false; }
	int j = 0;
	for (int i = 0; i < prefix.length(); i++, j++)
	{
		if (prefix[i] != name[j]) return false;
	}
	return true;
}

bool CProtobufHelper::has_prefix(const std::string& name)
{
	return starts_with(name, m_packageName + ".NTF") || starts_with(name, m_packageName + ".C2S") ||
		starts_with(name, m_packageName + ".S2C");
}

void CProtobufHelper::init()
{
	const google::protobuf::DescriptorPool* pool =
		google::protobuf::DescriptorPool::generated_pool();
	google::protobuf::DescriptorDatabase* db = pool->internal_generated_database();
	if (db == nullptr) { return; }
	std::vector<std::string> file_names;
	db->FindAllFileNames(&file_names);
	for (const std::string& filename : file_names)
	{
		const google::protobuf::FileDescriptor* fileDescriptor = pool->FindFileByName(filename);
		if (fileDescriptor == nullptr) { continue; }
		int msgcount = fileDescriptor->message_type_count();
		for (int i = 0; i < msgcount; i++)
		{
			const google::protobuf::Descriptor* descriptor = fileDescriptor->message_type(i);
			if (descriptor != nullptr)
			{
				const std::string& name = descriptor->full_name();
				if (starts_with(name, m_packageName))
				{
					if (has_prefix(name))
					{
						google::protobuf::MessageOptions opts = descriptor->options();
						protocol::scgf::cs::SCGFCSMessageID v =
							opts.GetExtension(protocol::scgf::cs::MsgID);
						m_registry[v] = descriptor;
					}
				}
			}
		}
	}
}

google::protobuf::Message* CProtobufHelper::create_message(protocol::scgf::cs::SCGFCSMessageID msgId)
{
	auto iter = m_registry.find(msgId);
	if (iter == m_registry.end()) { return nullptr; }
	const google::protobuf::Message* protoType =
		google::protobuf::MessageFactory::generated_factory()->GetPrototype(iter->second);
	if (protoType != nullptr) { return protoType->New(); }
	return nullptr;
}
}  // namespace scgf::util
