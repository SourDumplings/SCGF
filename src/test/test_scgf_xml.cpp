#include "test_scgf_xml.h"

#include "rapidxml/rapidxml.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>
#include "util/compressor.h"
#include "util/cryptor.h"
#include "util/file_helper.h"

int parse_xml(const std::string& xmlString)
{
	// 将 XML 字符串转换为 char 数组，因为 rapidxml 需要修改字符串
	std::vector<char> xmlCopy(xmlString.begin(), xmlString.end());
	xmlCopy.push_back('\0');  // 确保以 null 结尾

	// 创建 XML 文档对象
	rapidxml::xml_document<> doc;

	// 解析 XML 字符串
	try
	{
		// std::cout << xmlCopy[0] << std::endl;

		doc.parse<0>(&xmlCopy[0]);
	}
	catch (const rapidxml::parse_error& e)
	{
		std::cerr << "Parse error: " << e.what() << std::endl;
		return 1;
	}

	// 获取根节点
	rapidxml::xml_node<>* root = doc.first_node("imgdir");
	if (!root)
	{
		std::cerr << "No root imgdir node found!" << std::endl;
		return 1;
	}

	// 遍历子节点
	for (rapidxml::xml_node<>* child = root->first_node("imgdir"); child;
		 child						 = child->next_sibling("imgdir"))
	{
		// 获取子节点的 name 属性
		rapidxml::xml_attribute<>* attr = child->first_attribute("name");
		if (attr) { std::cout << "Child name: " << attr->value() << std::endl; }

		// 获取子节点的值
		if (child->value()) { std::cout << "Child value: " << child->value() << std::endl; }
	}

	return 0;
}

int parse_scgf_xml()
{
	std::filesystem::path cwd = scgf::util::get_executable_dir();
	std::filesystem::path filePath =
		cwd / std::filesystem::path("../res/dist/scgf/design/100000000.img.scgf");

	try
	{
		// 读取文件
		auto [fileData, fileDataSize] = scgf::util::read_file(filePath.string().c_str());

		// 解密
		auto [decryptedData, decryptedSize] =
			scgf::util::CCryptor::get_instance().decrypt_scgf(fileData.get(), fileDataSize);

		auto [decompressedData, decompressedSize] =
			scgf::util::CCompressor::get_instance().decompress(decryptedData.get(), decryptedSize);

		// 转换为字符串输出
		std::string xmlString(decompressedData.get(), decompressedSize);
		// std::cout << "Decrypted and decompressed XML: " << xmlString << std::endl;
		// std::cout << "Decrypted and decompressed XML size: " << decompressedSize << std::endl;

		parse_xml(xmlString);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}

int test_scgf_xml()
{
	parse_scgf_xml();
	return 0;
}