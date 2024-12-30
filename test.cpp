#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class TextProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

public:
    TextProcessor() {
        logger = spdlog::get("text_processor_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("text_processor_logger");
        }
        spdlog::set_level(spdlog::level::info);
    }

    // Method to read file content into a string
    std::string readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", filename);
            throw std::runtime_error("File not found or could not be opened.");
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        logger->info("File successfully read: {}", filename);
        return content;
    }

    // Method to write text to a file
    void writeFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", filename);
            throw std::runtime_error("File could not be opened for writing.");
        }
        file << content;
        logger->info("Content successfully written to file: {}", filename);
    }

    // Method to remove a substring from each line of a multiline string
    std::string removeSubstringFromLines(const std::string& text, const std::string& substring) {
        std::istringstream stream(text);
        std::ostringstream result;
        std::string line;
        bool firstLine = true;

        while (std::getline(stream, line)) {
            size_t pos;
            while ((pos = line.find(substring)) != std::string::npos) {
                line.erase(pos, substring.length());
                logger->info("Removed occurrence of substring '{}' from line.", substring);
            }
            // Trim trailing spaces
            line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), line.end());
            if (!firstLine) {
                result << "\n";
            }
            result << line;
            firstLine = false;
        }
        return result.str();
    }
};

// Test class
class TextProcessorTest : public ::testing::Test {
protected:
    TextProcessor* processor;

    void SetUp() override {
        processor = new TextProcessor();
    }

    void TearDown() override {
        delete processor;
        processor = nullptr;
    }
};

// Test cases
TEST_F(TextProcessorTest, RemoveSubstringWithNumbersTest) {
    std::string text = "123 456 123";
    std::string substring = "123";
    std::string expected = " 456"; // ”брали пробел в конце
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, RemoveSubstringWithPunctuationTest) {
    std::string text = "Hello, world! Hello, again!";
    std::string substring = "Hello, ";
    std::string expected = "world! again!";
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, ReadFromFileTest) {
    std::string filename = "input.txt";
    std::ofstream outfile(filename);
    outfile << "This is a test file.\nContaining multiple lines.\nEND";
    outfile.close();

    std::string expected = "This is a test file.\nContaining multiple lines.\nEND";
    ASSERT_EQ(processor->readFile(filename), expected);
}

TEST_F(TextProcessorTest, WriteToFileTest) {
    std::string filename = "output.txt";
    std::string content = "This is test content.";

    processor->writeFile(filename, content);

    std::ifstream infile(filename);
    std::string fileContent((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    ASSERT_EQ(fileContent, content);
}

TEST_F(TextProcessorTest, RemoveOverlappingSubstringsTest) {
    std::string text = "aaa aaa aaa";
    std::string substring = "aa";
    std::string expected = "a a a"; // ќжидаемый результат
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, RemoveSubstringWithSpacesTest) {
    std::string text = "Hello   world";
    std::string substring = "   ";
    std::string expected = "Helloworld"; // ќжидаемый результат
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, RemoveSubstringAtLineStartTest) {
    std::string text = "test This is a line.\ntest Another line.";
    std::string substring = "test ";
    std::string expected = "This is a line.\nAnother line."; // ќжидаемый результат
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, RemoveSubstringAtLineEndTest) {
    std::string text = "This is a line test\nAnother line test";
    std::string substring = " test";
    std::string expected = "This is a line\nAnother line"; // ќжидаемый результат
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}

TEST_F(TextProcessorTest, RemoveSubstringWithSpecialCharsTest) {
    std::string text = "Hello @world!\nGoodbye @world!";
    std::string substring = "@world";
    std::string expected = "Hello !\nGoodbye !"; // ќжидаемый результат
    ASSERT_EQ(processor->removeSubstringFromLines(text, substring), expected);
}


// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
