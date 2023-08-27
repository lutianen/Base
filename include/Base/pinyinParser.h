/**
 *
 * @file pinyinParser.h
 * @author Tianen Lu (tianenlu957@gmail.com)
 * @brief Conver 中文 to pinyin
 * @example
 *   Lute::PinyinParser parser;
 *   if (parser.InitPinyinMap(PINYIN_FILE) != 0) {
 *       cout<<"GetPinYinMap error, ret:" << endl;
 *       return -1;
 *   }
 *   string zh = "哈哈";
 *   string zhpinyin;
 *   if (parser.ConvertZh2Pinyin(zh, zhpinyin) != 0) {
 *       cout<<"parse "<<zh<<" error"<<endl;
 *       return -2;
 *   }
 *
 * @copyright Copyright (c) 2023 XXX
 * For study and research only, no reprinting
 */

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

// NOTE it must be modified to your own path
#define PINYIN_FILE \
    "/home/lux/githubWorkplace/Lute/Base/include/Base/pinyin.txt"

namespace Lute {
class PinyinParser {
public:
    PinyinParser() { map_.clear(); }

    int InitPinyinMap(std::string path = "");

    int ConvertZh2Pinyin(std::string& src, std::string& dst);

private:
    bool IsZh(const char* p);

    std::map<std::string, std::string> map_;
};
}  // namespace Lute
