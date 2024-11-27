#include "test_string.h"

#include <iostream>
#include <Windows.h>
#include "util/UTF8_string.h"

int test_string()
{
	SetConsoleOutputCP(CP_UTF8);

	scgf::util::CUTF8String str(u8"你好，世界！Hello, World!啦啦啦");
	std::cout << str << std::endl;
	std::cout << str.data() << std::endl;

	str.pop_back();
	std::cout << str << std::endl;

	str.push_back(u8"恭喜发财");
	str += u8"吉祥如意";
	std::cout << str << std::endl;
	std::cout << reinterpret_cast<const char *>(str.to_string().c_str()) << std::endl;
	std::cout << reinterpret_cast<const char *>(str.c_str()) << std::endl;

	for (int i = 0; i < 5; ++i)
		str.pop_back();

	std::cout << str << std::endl;
	std::cout << reinterpret_cast<const char *>(str.to_string().c_str()) << std::endl;
	std::cout << reinterpret_cast<const char *>(str.c_str()) << std::endl;

	for (int i = 0; i < 10; ++i)
		str.pop_back();

	std::cout << str << std::endl;
	std::cout << reinterpret_cast<const char *>(str.to_string().c_str()) << std::endl;
	std::cout << reinterpret_cast<const char *>(str.c_str()) << std::endl;

	scgf::util::CUTF8String str1(u8"你好，世界！啦啦啦");
	scgf::util::CUTF8String str2(u8"你好，世界！");
	scgf::util::CUTF8String str3(u8"你好，世界！");
	scgf::util::CUTF8String str4(u8"你好，lalala世界！");
	scgf::util::CUTF8String str5(u8"你好，lalala世界！");
	scgf::util::CUTF8String str6(u8"你好，lalahi世界！");
	printf("str1 == str2: %d\n", str1 == str2);
	printf("str3 == str2: %d\n", str3 == str2);
	printf("str4 == str5: %d\n", str4 == str5);
	printf("str4 == str6: %d\n", str4 == str6);
	printf("str4 != str6: %d\n", str4 != str6);

	printf("str1.length() = %llu\n", str1.length());
	printf("str1.size() = %llu\n", str1.size());

	auto itI = str1.insert(str1.begin(), u8"Hello 帅哥");
	printf("str1 = %s\n", str1.c_str());
	std::cout << "*itI: " << *itI << std::endl;

	auto itE1 = str1.erase(str1.begin());
	printf("str1 = %s\n", str1.c_str());
	std::cout << "*itE1: " << *itE1 << std::endl;
	auto itE2 = str1.erase(str1.begin() + 5);
	printf("str1 = %s\n", str1.c_str());
	std::cout << "*itE2: " << *itE2 << std::endl;

	printf("str4 = %s\n", str4.c_str());
	printf("str4.length() = %llu\n", str4.length());
	str4.erase(str4.begin() + 9, str4.end());
	printf("str4 = %s\n", str4.c_str());
	printf("str4.length() = %llu\n", str4.length());

	return 0;
}