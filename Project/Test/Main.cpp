#include "gtest/gtest.h"
#include "CommandLineArgParser.h"

std::string CaptureOutput(const std::function<void()>& func, const char* tempfile) {
    // ����ԭ stdout
    fflush(stdout);
    int stdout_copy = dup(fileno(stdout));

    // �ض�����ʱ�ļ�
    FILE* f = freopen(tempfile, "w", stdout);
    assert(f != nullptr);

    // ִ�б��⺯��
    func();
    fflush(stdout);

    // �ָ�ԭ stdout
    auto ret = dup2(stdout_copy, fileno(stdout));
    close(stdout_copy);

    // ��ȡ��ʱ�ļ�����
    std::ifstream fs(tempfile);
    return std::string((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
}

TEST(ExampleTest, BasicTest) {

    const char* argvTest[] = {
    	"Placeholder",
    	"--example",
    };
    const int argc = sizeof(argvTest) / sizeof(const char*);
    const char** argv = argvTest;

#define TEST_DESC "This is test example description"
#define TEST_PARSER_PRINTS_FILE_PATH "ParserPrints.txt"
    using namespace CommandLine;
    CCommandLineArgParser parser(argc, argv);
    parser.RegisterDefaultHelps(TEST_DESC, 1, 2, 3);
    auto output = CaptureOutput([&]()
        {
            EXPECT_FALSE(parser.Parse());
        }, TEST_PARSER_PRINTS_FILE_PATH);
    EXPECT_EQ(output, TEST_DESC);
    EXPECT_EQ(std::remove(TEST_PARSER_PRINTS_FILE_PATH), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}