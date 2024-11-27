#include "test_pb.h"

#include "util/compressor.h"
#include "util/cryptor.h"
#include "util/protobuf_helper.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int test_pb()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

	scgf::util::CProtobufHelper::get_instance().init();

	const char8_t *longMsg =
		u8"1、“人生永没有终点。”只有等到你瞑目的那一刻，才能说你走完了人生路，在此"
		"之前，新的第一次始"
		"终有，新的挑战依然在，新的感悟不断涌现…… "
		"2、当第一缕阳光照耀大地的时候，我们便和千千万万的中学生一样，开始了漫长"
		"的求学之路。走进教"
		"室，再也找不见小时候踏入这里的欣喜，取而代之的竟是无比的压抑。看着那一张"
		"张陌生的脸，心中又"
		"升起点点惆怅。昔日那熟悉的脸庞，如今早已不知去向。 "
		"3、昏暗的台灯下，我凝视着这一杯茶，沸水一次又一次的冲击，让我感到了茶的"
		"清香。那苦涩中略微"
		"含着的一点甘甜，也被我贪婪的嘴给霸占了，眼的朦胧，勾勒出朦胧的记忆，可记"
		"忆却已不再朦胧。 "
		"4、每晚，车床的隆隆声都在我的耳畔回响，我不再感觉这是噪音，我不再因此而"
		"难以入眠，这声音比"
		"妈妈轻柔的摇篮曲更优美，我就在这声音的怀抱中枕着父爱入眠。 "
		"5、母爱是一种无私的感情，母爱像温暖的阳光，洒落在我们心田，虽然悄声无息"
		"，但它让一棵棵生命"
		"的幼苗感受到了雨后的温暖。 "
		"6、亲情能滋润人的心田，使生命之舟洒满阳光；它是一种隐藏在每个人内心深处"
		"的一种而深厚的情感"
		"，它可以冲破时间空间的阻碍，让两颗心紧紧相拥。 "
		"7、你是润物的细雨，你是醉人的春风，你是厚重的抚摸、深情的凝望，你是雾海"
		"中的航标灯，你是捉"
		"不到的阳光，你是看不见的空气……啊，亲情，你在哪里？ "
		"8、女儿抬起头望着远方延伸的长路，轻扬着僵硬的嘴角，又望着身边的老母亲，"
		"幸福满足的继续挪移"
		"着脚步，我应该感到那步子是坚定而轻盈的。 "
		"9、朋友，你读懂亲情了吗？亲情是寒冷时父亲为你披上的一件外套，是深夜里母"
		"亲为你冲的一杯牛奶"
		"；亲情是孩子柔嫩的小手为父亲擦去额上的汗珠，是母亲疲惫时递上茶水时的体贴"
		"。 "
		"10、其实，人生的道路从来都不平坦，谁都会遭遇各种各样的坎。要像刘翔那样勇"
		"往直前，你就能飞跃"
		"一道又一道坎，你的人生就充满灿烂和喜悦。 "
		"11、其实成长，就是一颗美味的糖果，那唇齿留香的滋味回味我们无穷无尽的遐想"
		"。可是如果不加以节"
		"制，我们便尝不到那最初的那份甜蜜，取而代之的必定将是阵阵疼痛。所以成长中"
		"的我要学会排解烦恼"
		"，保护好我们的“牙齿”。只有这样，我们才会品味到更多的甜蜜与快乐！ "
		"12、亲情不仅是孟郊的“慈母手中线，游子身上衣”的长长牵挂，它也是王维“独在"
		"异乡为异客，每逢佳"
		"节倍思亲”的深沉感叹，是苏轼“但愿人长久，千里共婵娟”的美好祝愿，是辛弃疾“"
		"最喜小儿无赖，溪头"
		"卧剥莲蓬”的天伦之乐…… "
		"13、亲情能点燃生活的希望，为我们撑起心灵的蓝天；它是一种沾了点汗、湿了点"
		"泪、带了点痛的，是"
		"一种平凡而伟大的美丽。";

	protocol::scgf::cs::NTFServerMessage ntfMsg;
	ntfMsg.set_is_admin_msg(true);
	ntfMsg.set_type(12345);
	ntfMsg.set_message(reinterpret_cast<const char *>(longMsg));

	std::cout << "original msg, size(" << ntfMsg.ByteSizeLong() << "):" << std::endl;
	std::cout << ntfMsg.Utf8DebugString() << std::endl;

	std::string bytes = ntfMsg.SerializeAsString();

	auto [compressedData, compressedSize] =
		scgf::util::CCompressor::get_instance().compress(bytes.data(), bytes.size());
	std::cout << "Compressed size: " << compressedSize << std::endl << std::endl;

	// auto [decompressedData, decompressedSize] =
	// decompress(compressedData.get(), compressedSize);
	std::pair<std::unique_ptr<char[]>, size_t> decompressedResult =
		scgf::util::CCompressor::get_instance().decompress(compressedData.get(), compressedSize);
	std::unique_ptr<char[]> &decompressedData = decompressedResult.first;
	size_t decompressedSize					  = decompressedResult.second;

	auto msg = std::unique_ptr<protocol::scgf::cs::NTFServerMessage>(
		dynamic_cast<protocol::scgf::cs::NTFServerMessage *>(
			scgf::util::CProtobufHelper::get_instance().create_message(
				protocol::scgf::cs::SCGF_CS_SERVER_MESSAGE
			)
		)
	);

	if (msg != nullptr)
	{
		msg->ParseFromArray(decompressedData.get(), decompressedSize);
		std::cout << "Parsed msg size(" << msg->ByteSizeLong() << "):" << std::endl;
		std::cout << "Parsed msg after decompression:\n" << msg->Utf8DebugString() << std::endl;
	}
	return 0;
}