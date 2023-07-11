#include <Base/pinyinParser.h>

int Lute::PinyinParser::InitPinyinMap(std::string path) {
    if (path.empty()) {
        std::cout << "path emtpy, invalid param" << std::endl;
        return -1;
    }

    // 读取拼音文件
    std::ifstream is(path.c_str());
    if (!is.is_open()) {
        std::cout << "open file:" << path << " error" << std::endl;
        return -1;
    }

    while (!is.eof()) {
        std::string tmp_pinyin;

        // 每次读取一行，这里拼音文件格式：王=wang1,wang2，数字表示声调
        getline(is, tmp_pinyin);
        // std::cout<<"getline:"<<tmp_pinyin<<std::endl;
        if (tmp_pinyin.find("=") != std::string::npos) {
            std::string zh, pinyin;
            size_t i = tmp_pinyin.find_first_of('=');
            if (i != std::string::npos && i != tmp_pinyin.size() - 1) {
                // 发音有多个，我们只取一个
                zh.assign(tmp_pinyin, 0, i);
                pinyin.assign(tmp_pinyin, i + 1, tmp_pinyin.size() - i - 1);

                // 去掉拼音末尾声调
                if (pinyin.find(",") != std::string::npos) {
                    size_t j = pinyin.find(",");
                    pinyin.assign(pinyin, 0, j - 1);
                } else {
                    pinyin.assign(pinyin.begin(), pinyin.end() - 1);
                }
                // 取出来后放到字典里
                map_[zh] = pinyin;
            }
        }
    }

    // 关闭文件
    is.close();
    return 0;
}

int Lute::PinyinParser::ConvertZh2Pinyin(std::string& src, std::string& dst) {
    if (src.empty()) {
        std::cout << "src is empty, invalid" << std::endl;
        return -1;
    }

    {
        size_t i = 0;
        std::string tmp_str;

        // 逐个字符检查
        while (i < src.length()) {
            try {
                tmp_str.clear();
                const char* p = &src.at(i);
                // 中文占用三字节，utf8中三字节编码第一个字节前四位是1110
                if ((*p & 0xF0) == 0xE0 && IsZh(p)) {
                    tmp_str.append(&src.at(i), 3);
                    // 从字典里找对应的拼音
                    if (map_.find(tmp_str) != map_.end()) {
                        dst.append(map_[tmp_str]);
                    } else {
                        std::cout << "can't find zh pinyin,zh:" << tmp_str
                                  << std::endl;
                        dst.append(tmp_str);
                    }
                    i += 3;
                } else {
                    dst.append(p, 1);
                    i += 1;
                }
            } catch (...) {
                std::cout << "exception occurs" << std::endl;
                return -1;
            }
        }
    }

    return 0;
}

bool Lute::PinyinParser::IsZh(const char* p) {
    if (NULL == p) {
        std::cout << "input param null" << std::endl;
        return false;
    }

    // 中文unicode编码范围是0X4E00~0X9FA5,即utf8范围0xe4b880 ~ 0xe9baa5
    // 使用utf8编码占3个字节，下面是分别对三个字节校验，utf8编码介绍
    // javascript:void(0)

    if ((*p & 0xF0) == 0xE0) {
        if (*(p + 1) == '\0' || *(p + 2) == '\0') {
            return false;
        }
        unsigned char v = *p;
        unsigned char v1 = *(p + 1);
        unsigned char v2 = *(p + 2);

        if ((v1 & 0xC0) != 0x80 || (v2 & 0xC0) != 0x80) {
            return false;
        }

        if (v < 0xE4) {
            return false;
        }
        if (v == 0xE4 && v1 < 0xB8) {
            return false;
        }
        if (v == 0xE4 && v1 == 0xB8 && v2 < 0x80) {
            return false;
        }

        if (v > 0xE9) {
            return false;
        }
        if (v == 0xE9 && v1 > 0xBE) {
            return false;
        }
        if (v == 0xE9 && v1 == 0xBE && v2 > 0xB5) {
            return false;
        }

        p += 3;

        return true;
    }

    return false;
}
