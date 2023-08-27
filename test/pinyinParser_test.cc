#include <Base/pinyinParser.h>

using std::cout;
using std::string;
using std::endl;

int main() {
    Lute::PinyinParser parser;
    if (parser.InitPinyinMap(PINYIN_FILE) != 0) {
        cout << "GetPinYinMap error, ret:" << endl;
        return -1;
    }

    string zh = "哈哈";
    string zhpinyin;
    if (parser.ConvertZh2Pinyin(zh, zhpinyin) != 0) {
        cout << "parse " << zh << " error" << endl;
        return -2;
    }

    cout << "parse succ, zh: " << zh << ", pinyin: " << zhpinyin << endl;
    return 0;
}