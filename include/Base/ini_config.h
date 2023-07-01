///
/// @brief 一个使用简单的 API 和最小的占用空间来操作 INI 文件
///
/// 它符合标准的 INI 格式:
///     - 部分和键不区分大小写, 所有前导和尾随空格都被忽略。
///     - 注释是以分号开头的行, 节线上允许尾随注释。
///
/// 文件按需读取，数据保存在内存中并关闭文件
/// 支持惰性写入，只将更改和更新写入文件并保留自定义格式和注释
/// 由 write() 调用的惰性写入将读取输出文件，查找已进行的更改并相应地更新文件
///
/// 若只需要生成文件，用 generate(),section和key顺序在读取、写入和插入时保留
///
/// #define KEY_CASE_SENSITIVE
/// #define KEY_LOWER  // KEY_NOT_CASE_SENSITIVE and TRANSFORM to LOWER
/// #define KEY_UPPER  // *KEY_NOT_CASE_SENSITIVE and TRANSFORM to UPPER
///
/// @example
///     xxx.cc:
///         #define INI_FILE_NAME "conf/system.ini"
///         std::shared_ptr<Lute::ini::INI> g_iniFilePtr =
///             Lute::SingletonPtr<Lute::ini::INI>::GetInstance(INI_FILE_NAME);
///         std::shared_ptr<Lute::ini::INIStructure> g_iniContentPtr =
///             Lute::SingletonPtr<Lute::ini::INIStructure>::GetInstance();
///         int main() {
///             Lute::ini::initIniConfig(INI_FILE_NAME);
///
///             // Write
///             INI_WRITE("person", "body", "Lute");
///             INI_WRITE("money", "RMB", "100");
///             INI_WRITE("money", "Dollar", "13.87");
///
///             // Read
///             std::cout << INI_READ("person", "body") << std::endl;
///             std::cout << INI_READ("money", "RMB") << std::endl;
///             std::cout << INI_READ("money", "Dollar") << std::endl;
///         }
///

#pragma once

#include <Base/fsUtils.h>
#include <Base/string_view.h>
#include <Base/utils.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#define KEY_UPPER

namespace Lute {
namespace ini {
    class INI;
    template <typename T>
    class INIMAP;
    using INIStructure = INIMAP<INIMAP<std::string>>;

#ifdef KEY_CASE_SENSITIVE
    inline void iniTransform(string& str) {}
#else  // KEY_NOT_CASE_SENSITIVE
#ifdef KEY_LOWER
    inline void iniTransform(string& str) { Lute::toLower(str); }
#else  // KEY_UPPER
    inline void iniTransform(std::string& str) { Lute::toUpper(str); }

#endif
#endif

#ifdef _WIN32
    const char* const endl = "\r\n";
#else
    const char* const endl = "\n";
#endif

    const char* const whitespaceDelimiters = " \t\n\r\f\v";

    // Windows 的记事本会给 UTF-8 文件添加 BOM 头 [0xEFBBBF]
    const char utf8_BOM[3] = {static_cast<char>(0xEF), static_cast<char>(0xBB),
                              static_cast<char>(0xBF)};

    /**
     * @brief std::pair<string, T>
     */
    template <typename T>
    class INIMAP {
    private:
        using string = std::string;
        using DataIdxMap = std::unordered_map<string, std::size_t>;
        using DataItem = std::pair<string, T>;
        using DataContainer = std::vector<DataItem>;
        using MultiArgs = typename std::vector<std::pair<string, T>>;

        DataIdxMap dataIndexMap_;
        DataContainer data_;

        inline std::size_t setEmpty(string& key) {
            std::size_t index = data_.size();
            dataIndexMap_[key] = index;
            data_.emplace_back(key, T());
            return index;
        }

    public:
        INIMAP() {}
        INIMAP(const INIMAP& other) {
            std::size_t data_size = other.data_.size();
            for (std::size_t i = 0; i < data_size; ++i) {
                const auto& key = other.data_[i].first;
                const auto& obj = other.data_[i].second;
                data_.emplace_back(key, obj);
            }
            dataIndexMap_ = DataIdxMap(other.dataIndexMap_);
        }

        T& operator[](string key) {
            trim(key);
            iniTransform(key);
            auto it = dataIndexMap_.find(key);
            bool hasIt = (it != dataIndexMap_.end());
            std::size_t index = (hasIt) ? it->second : setEmpty(key);
            return data_[index].second;
        }

        T get(string key) const {
            trim(key);
            iniTransform(key);
            auto it = dataIndexMap_.find(key);
            if (it == dataIndexMap_.end()) {
                return T();
            }
            return T(data_[it->second].second);
        }

        bool has(string key) const {
            trim(key);
            iniTransform(key);
            return (dataIndexMap_.count(key) == 1);
        }

        void set(string key, T obj) {
            trim(key);
            iniTransform(key);
            auto it = dataIndexMap_.find(key);
            if (it != dataIndexMap_.end()) {
                data_[it->second].second = obj;
            } else {
                dataIndexMap_[key] = data_.size();
                data_.emplace_back(key, obj);
            }
        }

        void set(const MultiArgs& multiArgs) {
            for (const auto& it : multiArgs) {
                const auto& key = it.first;
                const auto& obj = it.second;
                set(key, obj);
            }
        }

        bool remove(string key) {
            trim(key);
            iniTransform(key);
            auto it = dataIndexMap_.find(key);
            if (it != dataIndexMap_.end()) {
                std::size_t index = it->second;
                data_.erase(data_.begin() + index);
                dataIndexMap_.erase(it);
                for (auto& it2 : dataIndexMap_) {
                    auto& vi = it2.second;
                    if (vi > index) {
                        vi--;
                    }
                }
                return true;
            }
            return false;
        }

        void clear() {
            data_.clear();
            dataIndexMap_.clear();
        }

        std::size_t size() const { return data_.size(); }
        typename DataContainer::const_iterator begin() const {
            return data_.begin();
        }
        typename DataContainer::const_iterator end() const {
            return data_.end();
        }
    };

    /**
     * @brief Core INI class
     */
    class INI {
        using string = std::string;
        using ParseValues = std::pair<string, string>;
        using LineData = std::vector<string>;
        using LineDataPtr = std::shared_ptr<LineData>;

    public:
        /// non-copyable
        INI(const INI&) = delete;
        INI& operator=(INI&) = delete;

        /// NONE / COMMIT / SECTOIN / KEYVALUE / UNKNOW
        enum class INILineType : char {
            PDATA_NONE,
            PDATA_COMMENT,
            PDATA_SECTION,
            PDATA_KEYVALUE,
            PDATA_UNKNOWN
        };
        bool isBOM_;
        bool prettyPrint_;

    private:
        string fileName2Read_;
        string fileName2Write_;
        std::ifstream fileReadStream_;
        std::ofstream fileWriteStream_;

        LineDataPtr lineDataPtr_;
        std::size_t fileSize_;

    private:
        /// `CORE` - Parse INI line
        inline INILineType parseLine(string line, ParseValues& parseData) {
            parseData.first.clear();
            parseData.second.clear();
            trim(line);

            // NONE
            if (line.empty()) {
                return INILineType::PDATA_NONE;
            }

            char firstCharacter = line[0];

            // COMMIT
            if (firstCharacter == ';') {
                return INILineType::PDATA_COMMENT;
            }

            // SECTION [section]
            if (firstCharacter == '[') {
                auto commentAt = line.find_first_of(';');
                if (commentAt != string::npos) {
                    line = line.substr(0, commentAt);
                }
                auto closingBracketAt = line.find_last_of(']');
                if (closingBracketAt != string::npos) {
                    auto section = line.substr(1, closingBracketAt - 1);
                    trim(section);
                    parseData.first = section;
                    return INILineType::PDATA_SECTION;
                }
            }

            // KEYVALUE key=value
            string lineNorm = line;
            replace(lineNorm, "\\=", "  ");
            auto equalsAt = lineNorm.find_first_of('=');
            if (equalsAt != string::npos) {
                string key = line.substr(0, equalsAt);
                trim(key);
                replace(key, "\\=", "=");

                string value = line.substr(equalsAt + 1);
                trim(value);

                parseData.first = key;
                parseData.second = value;
                return INILineType::PDATA_KEYVALUE;
            }

            // UNKNOWN
            return INILineType::PDATA_UNKNOWN;
        }

        /// Get the bytes of file
        inline std::size_t getFileSize() {
            fileReadStream_.open(fileName2Read_,
                                 std::ios::in | std::ios::binary);

            if (!fileReadStream_.is_open()) {
                const char err[] = "fileReadStream open failed.";
                ::write(1, err, sizeof(err));
                return 0;
            }
            fileReadStream_.seekg(0, std::ios::end);
            auto bytes = static_cast<std::size_t>(fileReadStream_.tellg());
            fileReadStream_.seekg(0, std::ios::beg);

            fileReadStream_.close();

            return bytes;
        }

        /// Get the contents of file
        inline string getFileContent() {
            fileSize_ = getFileSize();

            fileReadStream_.open(fileName2Read_,
                                 std::ios::in | std::ios::binary);

            if (!fileReadStream_.is_open()) {
                const char err[] = "fileReadStream open failed.";
                ::write(1, err, sizeof(err));
                return {};
            }

            string fileContents;
            fileContents.resize(fileSize_);

            fileReadStream_.seekg(isBOM_ ? 3 : 0, std::ios::beg);
            fileReadStream_.read(&fileContents[0],
                                 static_cast<std::streamsize>(fileSize_));
            fileReadStream_.close();

            return fileContents;
        }

        /// Get LineData from `Lux.ini`
        LineData getLineData() {
            fileSize_ = getFileSize();
            fileReadStream_.open(fileName2Read_,
                                 std::ios::in | std::ios::binary);

            if (!fileReadStream_.is_open()) {
                const char err[] = "fileReadStream open failed.";
                ::write(1, err, sizeof(err));
                return {};
            }

            // Remove BOM in windows.
            if (fileSize_ >= 3) {
                const char header[3] = {
                    static_cast<char>(fileReadStream_.get()),
                    static_cast<char>(fileReadStream_.get()),
                    static_cast<char>(fileReadStream_.get())};
                isBOM_ = (header[0] == static_cast<char>(0xEF) &&
                          header[1] == static_cast<char>(0xBB) &&
                          header[2] == static_cast<char>(0xBF));
            } else {
                isBOM_ = false;
            }
            fileReadStream_.close();

            string fileContents = getFileContent();

            LineData output;
            if (fileSize_ == 0) {
                return output;
            }

            string buffer;
            buffer.reserve(50);
            for (std::size_t i = 0; i < fileSize_; ++i) {
                char& c = fileContents[i];
                if (c == '\n') {
                    output.emplace_back(buffer);
                    buffer.clear();
                    continue;
                }
                if (c != '\0' && c != '\r') {
                    buffer += c;
                }
            }
            output.emplace_back(buffer);
            return output;
        }

        /// Get LineDataPtr
        inline LineDataPtr getLines() { return lineDataPtr_; }

        /// Write `LineData` into file
        inline bool writeLines2File(bool fileIsBOM, LineData output) {
            std::ofstream fileWriteStream(fileName2Write_,
                                          std::ios::out | std::ios::binary);
            if (fileWriteStream.is_open()) {
                if (fileIsBOM) {
                    fileWriteStream.write(utf8_BOM, sizeof(utf8_BOM));
                }

                if (output.size()) {
                    auto line = output.begin();
                    for (;;) {
                        fileWriteStream << *line;
                        if (++line == output.end()) {
                            break;
                        }
                        fileWriteStream << endl;
                    }
                }
                return true;
            }
            return false;
        }

        ///
        LineData getLazyOutput(const LineDataPtr& lineData, INIStructure& data,
                               INIStructure& original) {
            LineData output;
            ParseValues parseData;
            string sectionCurrent;
            bool parsingSection = false;
            bool continueToNextSection = false;
            bool discardNextEmpty = false;
            bool writeNewKeys = false;
            std::size_t lastKeyLine = 0;

            for (auto line = lineData->begin(); line != lineData->end();
                 ++line) {
                // SECTION / KEYVALUE / UNKNOWN
                if (!writeNewKeys) {
                    INILineType parseResult = parseLine(*line, parseData);

                    if (parseResult == INILineType::PDATA_SECTION) {
                        if (parsingSection) {
                            writeNewKeys = true;
                            parsingSection = false;
                            --line;
                            continue;
                        }
                        sectionCurrent = parseData.first;
                        if (data.has(sectionCurrent)) {
                            parsingSection = true;
                            continueToNextSection = false;
                            discardNextEmpty = false;
                            output.emplace_back(*line);
                            lastKeyLine = output.size();
                        } else {
                            continueToNextSection = true;
                            discardNextEmpty = true;
                            continue;
                        }
                    } else if (parseResult == INILineType::PDATA_KEYVALUE) {
                        if (continueToNextSection) {
                            continue;
                        }
                        if (data.has(sectionCurrent)) {
                            auto& collection = data[sectionCurrent];
                            const string& key = parseData.first;
                            const string& value = parseData.second;
                            if (collection.has(key)) {
                                string outputValue = collection[key];
                                if (value == outputValue) {
                                    output.emplace_back(*line);
                                } else {
                                    trim(outputValue);
                                    string lineNorm = *line;
                                    replace(lineNorm, "\\=", "  ");
                                    auto equalsAt = lineNorm.find_first_of('=');
                                    auto valueAt = lineNorm.find_first_not_of(
                                        whitespaceDelimiters, equalsAt + 1);
                                    string outputLine =
                                        line->substr(0, valueAt);
                                    if (prettyPrint_ &&
                                        equalsAt + 1 == valueAt) {
                                        outputLine += " ";
                                    }
                                    outputLine += outputValue;
                                    output.emplace_back(outputLine);
                                }
                                lastKeyLine = output.size();
                            }
                        }
                    } else {
                        if (discardNextEmpty && line->empty()) {
                            discardNextEmpty = false;
                        } else if (parseResult != INILineType::PDATA_UNKNOWN) {
                            output.emplace_back(*line);
                        }
                    }
                }

                // New key-value
                if (writeNewKeys || std::next(line) == lineData->end()) {
                    LineData linesToAdd;
                    if (data.has(sectionCurrent) &&
                        original.has(sectionCurrent)) {
                        const auto& collection = data[sectionCurrent];
                        const auto& collectionOriginal =
                            original[sectionCurrent];
                        for (const auto& it : collection) {
                            string key = it.first;
                            if (collectionOriginal.has(key)) {
                                continue;
                            }
                            string value = it.second;
                            replace(key, "=", "\\=");
                            trim(value);
                            linesToAdd.emplace_back(
                                key + ((prettyPrint_) ? " = " : "=") + value);
                        }
                    }
                    if (!linesToAdd.empty()) {
                        output.insert(
                            output.begin() + static_cast<long>(lastKeyLine),
                            linesToAdd.begin(), linesToAdd.end());
                    }
                    if (writeNewKeys) {
                        writeNewKeys = false;
                        --line;
                    }
                }
            }

            // [section]
            // key=value
            for (const auto& it : data) {
                const string& section = it.first;
                if (original.has(section)) {
                    continue;
                }

                if (prettyPrint_ && output.size() > 0 &&
                    !output.back().empty()) {
                    output.emplace_back();
                }
                output.emplace_back("[" + section + "]");

                const auto& collection = it.second;
                for (const auto& it2 : collection) {
                    string key = it2.first;
                    string value = it2.second;
                    replace(key, "=", "\\=");
                    trim(value);

                    output.emplace_back(key + ((prettyPrint_) ? " = " : "=") +
                                        value);
                }
            }
            return output;
        }

        /**
         * @brief Set `data` into `Lux.ini`
         */
        bool setINIStructure(const INIStructure& data) {
            if (!data.size()) {
                return true;
            }

            fileWriteStream_.open(fileName2Write_,
                                  std::ios::out | std::ios::binary);
            if (!fileWriteStream_.is_open()) {
                const char err[] = "fileWriteStream open failed.";
                ::write(1, err, sizeof(err));
                return false;
            }

            auto it = data.begin();
            for (;;) {
                const string& section = it->first;
                const auto& collection = it->second;
                fileWriteStream_ << "[" << section << "]";

                if (collection.size()) {
                    fileWriteStream_ << endl;
                    auto it2 = collection.begin();
                    for (;;) {
                        string key = it2->first;
                        replace(key, "=", "\\=");

                        string value = it2->second;
                        trim(value);
                        fileWriteStream_
                            << key << ((prettyPrint_) ? " = " : "=") << value;

                        if (++it2 == collection.end()) {
                            break;
                        }
                        fileWriteStream_ << endl;
                    }
                }

                if (++it == data.end()) {
                    break;
                }

                fileWriteStream_ << endl;

                if (prettyPrint_) {
                    fileWriteStream_ << endl;
                }
            }

            fileWriteStream_.close();
            return true;
        }

        /**
         * @brief Set `data` into `Lux.ini`
         */
        bool setINIStructure(INIStructure& data) {
            struct stat buf;
            bool fileExists = (stat(fileName2Write_.c_str(), &buf) == 0);

            if (!fileExists) {
                return generate(data);
            }

            INIStructure originalData;
            LineDataPtr lineData;
            bool readSuccess = false;
            bool fileIsBOM = false;

            lineDataPtr_ = std::make_shared<LineData>();
            if ((readSuccess = getINIStructure(originalData))) {
                lineData = getLines();
                fileIsBOM = isBOM_;
            }

            if (!readSuccess) {
                return false;
            }

            LineData output = getLazyOutput(lineData, data, originalData);
            return writeLines2File(fileIsBOM, output);
        }

        /**
         * @brief Get INIStructure `data`
         */
        bool getINIStructure(INIStructure& data) {
            LineData lines = getLineData();

            string section;
            bool inSection = false;
            ParseValues parseData;
            for (const auto& line : lines) {
                // Parse line
                INILineType parseResult = parseLine(line, parseData);

                if (parseResult == INILineType::PDATA_SECTION) {
                    inSection = true;
                    data[section = parseData.first];
                } else if (inSection &&
                           parseResult == INILineType::PDATA_KEYVALUE) {
                    const string& key = parseData.first;
                    const string& value = parseData.second;
                    data[section][key] = value;
                }
                if (lineDataPtr_ && parseResult != INILineType::PDATA_UNKNOWN) {
                    if (parseResult == INILineType::PDATA_KEYVALUE &&
                        !inSection) {
                        continue;
                    }
                    lineDataPtr_->emplace_back(line);
                }
            }
            return true;
        }

    public:
        /**
         * @brief Construct a new INI object
         */
        INI(const string& fileName2Read, const string& fileName2Write,
            bool pretty = true, bool keepLineData = false)
            : isBOM_(false),
              prettyPrint_(pretty),
              fileName2Read_(fileName2Read),
              fileName2Write_(fileName2Write),
              fileSize_(0) {
            (void)keepLineData;
        }

        INI(const string& fileName, bool pretty = true,
            bool keepLineData = false)
            : INI(fileName, fileName, pretty, keepLineData) {}

        ~INI() {}

        bool read(INIStructure& data) {
            if (data.size()) {
                data.clear();
            }
            if (fileName2Read_.empty()) {
                return false;
            }

            return getINIStructure(data);
        }

        bool generate(const INIStructure& data, bool pretty = true) {
            if (fileName2Write_.empty()) {
                return false;
            }

            prettyPrint_ = pretty;
            return setINIStructure(data);
        }

        bool write(INIStructure& data, bool pretty = true) {
            if (fileName2Write_.empty()) {
                return false;
            }

            prettyPrint_ = pretty;
            return setINIStructure(data);
        }
    };

}  // namespace ini
}  // namespace Lute

extern std::shared_ptr<Lute::ini::INI> g_iniFilePtr;
extern std::shared_ptr<Lute::ini::INIStructure> g_iniContentPtr;

namespace Lute {
namespace ini {
    ///
    /// @brief Initialize ini config file
    ///
    inline void initIniConfig(const Lute::string_view& iniFileName) {
        if (!Lute::FSUtil::touch(std::string(iniFileName.data()))) return;
        g_iniFilePtr->generate(*g_iniContentPtr);
    }
}  // namespace ini
}  // namespace Lute

#define LUTE_INI_ASSERT(x)                                             \
    static_assert(std::is_same<decltype(x), Lute::string_view>::value, \
                  #x " must be Lute::string_view")

#define LUTE_INI_TYPE(x) x##_sv

/// Read section / key
#define LUTE_INI_READ(SECTION, KEY)                           \
    [&]() {                                                   \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(SECTION));              \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(KEY));                  \
        g_iniFilePtr->read(*g_iniContentPtr);                 \
        return Lute::string_view(                             \
            (*g_iniContentPtr)[LUTE_INI_TYPE(SECTION).data()] \
                              [LUTE_INI_TYPE(KEY).data()]);   \
    }()

/// Add or Update section / key-value and save
#define LUTE_INI_WRITE(SECTION, KEY, VALUE)                \
    do {                                                   \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(SECTION));           \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(KEY));               \
        LUTE_INI_ASSERT(LUTE_INI_TYPE(VALUE));             \
        (*g_iniContentPtr)[LUTE_INI_TYPE(SECTION).data()]  \
                          [LUTE_INI_TYPE(KEY).data()] =    \
                              LUTE_INI_TYPE(VALUE).data(); \
        g_iniFilePtr->write(*g_iniContentPtr);             \
    } while (0)
