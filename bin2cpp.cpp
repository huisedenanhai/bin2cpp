/*
 * MIT License
 *
 * Copyright (c) 2019 huisedenanhai
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

void usage() {
    std::cout << "bin2cpp usage:" << std::endl;
    std::cout << "  bin2cpp [-c] [-ce] [-s] [-z] [-ns namespace] [-n var_name] "
                 "[-i indent] [-m line_max] [-h] filename"
              << std::endl;
    std::cout << "  -c const" << std::endl;
    std::cout << "  -ce constexpr" << std::endl;
    std::cout << "  -s static" << std::endl;
    std::cout << "  -z add trailing zero (helpful for text input file)"
              << std::endl;
    std::cout << "  -ns namespace" << std::endl;
    std::cout << "  -n name of the array variable, use filename by defualt"
              << std::endl;
    std::cout << "  -i indent, 2 by default" << std::endl;
    std::cout << "  -m max char cnt in one line, 80 by defualt" << std::endl;
    std::cout << "  -h show this msg" << std::endl;
    std::cout << "  filename the input filename" << std::endl;
}

enum ArgParseState { Namespace, VarName, Indent, MaxChar, Defualt };

int main(int argc, char **argv) {
    std::string ns = "";
    std::string name = "";
    std::string type = "unsigned char";
    std::string fileName = "";
    std::string prefix = "";
    std::string indent = "";
    int indentCnt = 2;
    bool needTrailingZero = false;
    bool isConst = false;
    bool isConstExpr = false;
    bool isStatic = false;
    bool needHelp = false;
    int lineMax = 80;
    ArgParseState parseState = Defualt;

    if (argc <= 1) {
        usage();
        return 0;
    }

    auto parseArg = [&](char *a) {
        std::string astr{a};
        switch (parseState) {
        case Namespace:
            ns = astr;
            parseState = Defualt;
            return;
        case VarName:
            name = astr;
            parseState = Defualt;
            return;
        case Indent:
            indentCnt = std::stoi(astr);
            parseState = Defualt;
            return;
        case MaxChar:
            lineMax = std::stoi(astr);
            parseState = Defualt;
            return;
        case Defualt:
            break;
        }
        if (astr == "-c") {
            isConst = true;
            return;
        }
        if (astr == "-ce") {
            isConstExpr = true;
            return;
        }
        if (astr == "-s") {
            isStatic = true;
            return;
        }
        if (astr == "-z") {
            needTrailingZero = true;
            return;
        }
        if (astr == "-ns") {
            parseState = Namespace;
            return;
        }
        if (astr == "-n") {
            parseState = VarName;
            return;
        }
        if (astr == "-i") {
            parseState = Indent;
            return;
        }
        if (astr == "-m") {
            parseState = MaxChar;
            return;
        }
        if (astr == "-h") {
            needHelp = true;
            return;
        }
        fileName = astr;
    };

    // parse arguments
    for (int i = 1; i < argc; i++) {
        parseArg(argv[i]);
    }

    if (fileName == "") {
        std::cout << "please specify input file" << std::endl;
        usage();
        return 0;
    }

    if (name == "") {
        auto ci = fileName.find('.');
        name = fileName.substr(0, ci);
    }

    indent = std::string(indentCnt, ' ');
    if (isStatic) {
        prefix = "static";
    }
    if (isConstExpr) {
        if (prefix != "")
            prefix += " ";
        prefix += "constexpr";
    }
    if (isConst) {
        if (prefix != "")
            prefix += " ";
        prefix += "const";
    }

    if (needHelp) {
        usage();
    }

    std::ifstream is{fileName, std::ifstream::binary};
    std::ostream &os{std::cout};
    if (!is) {
        std::cout << "can not open input file " << fileName << std::endl;
        return 0;
    }

    auto pPrefix = [&]() -> std::ostream & {
        if (prefix != "") {
            return os << prefix << " ";
        }
        return os;
    };
    int charCnt = 0;
    size_t fileLength = 0;
    bool needNamespace = ns != "";
    if (needNamespace) {
        os << "namespace " << ns << " {" << std::endl;
    }
    pPrefix() << type << " " << name << "[] = {" << std::endl;
    unsigned char c;
    auto printChar = [&]() {
        if (charCnt + 5 > lineMax) {
            os << std::endl;
            charCnt = 0;
        }
        if (charCnt == 0) {
            os << indent;
            charCnt += indent.size();
        }
        os << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)c
           << ",";
        charCnt += 5;
        fileLength += 1;
    };
    while (is) {
        is.read(reinterpret_cast<char *>(&c), sizeof(unsigned char));
        printChar();
    }
    if (needTrailingZero) {
        c = 0;
        printChar();
    }
    os << std::endl;
    os << "};" << std::endl;
    pPrefix() << "unsigned long long " << name << "_length = " << std::dec
              << fileLength << ";" << std::endl;
    if (needNamespace) {
        os << "}" << std::endl;
    }
}
