// -- flowtine.h --

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <iterator>
#include <sstream>
#include <cstring>

// -- definitions --
namespace ftn {
    enum Types {
        IDENTIFIER,
        OPERATOR,
        NOTHING,
        ENDOF
    };

    struct Token {
        Types whatType;
        std::string text;
        int currentLinenum;
    };

    struct Array {
        std::unordered_map<std::string, std::string> vars;
        std::unordered_map<std::string, std::shared_ptr<Array>> arrays;
    };

    class Flowtine {
        public:
            Flowtine(const std::string& filename) {
                // open the file
                std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
                if (!file.is_open()) {
                    std::cerr << "flowtine: Unable to open file: " << filename;
                }

                // get the file length
                file.seekg (0, file.end);
                const int filesize = file.tellg();
                file.seekg (0, file.beg);

                // put the contents into a buffer
                char buffer[filesize];
                file.read(buffer, filesize);
                buffer[filesize] = '\0';
                file.close();

                // lex the file
                std::string filecontents{buffer};
                lex(filecontents);

                if (tokens.empty()) return;
                std::vector<Token>::iterator iter = tokens.begin();

                while (iter->whatType != ENDOF) {
                   std::vector<Token>::iterator start{iter};
                   if (!parse(iter)) {
                        errorHandling("syntax error", start->currentLinenum);
                   }
                }
            }

            std::string get(const std::string variable);
        private:
            // lexer
            bool comment = false;
            std::vector<Token> tokens;
            void lex(const std::string_view contents);
            void clear(Token& token, const int linenum);

            // parser
            Array vars;
            Array* currentArray = &vars;
            bool parse(std::vector<Token>::iterator& iter);
            bool expectArray(std::vector<Token>::iterator& iter, const std::string name);
            std::string expectIdentifier(const std::vector<Token>::iterator&) const;
            bool expectOperator(const std::vector<Token>::iterator& iter, const char* op) const;
            bool increment(std::vector<Token>::iterator& iter, const int advanceBy);

            // error handling
            void errorHandling(const std::string_view errorMessage, const int linenumber) const;
    };

    // get the value from the respected variable and return it to the user
    std::string Flowtine::get(const std::string variable) {
        if (variable.find(".") == std::string::npos) return vars.vars[variable];

        std::istringstream tokenStream(variable);
        currentArray = &vars;
        std::string token;
        std::string delim = ".";
        size_t pos = 0;

        while (std::getline(tokenStream, token, '.')) {
            if (strstr(&variable[pos], ".") == NULL) break;

            if (currentArray->arrays.find(token) == currentArray->arrays.end()) break;
            else currentArray = currentArray->arrays[token].get();

            pos += token.length() + 1;
        }
        if (currentArray->vars.find(token) != currentArray->vars.end()) {
            return currentArray->vars[token];
        }
        return "";
    }

    void Flowtine::errorHandling(const std::string_view errorMessage, const int linenumber) const {
        std::cerr << "flowtine: error at line " << linenumber << ": " << errorMessage << "\n";
    }

    // append the token to the array, and clear it
    void Flowtine::clear(Token& token, const int linenum) {
        if (token.whatType != NOTHING && !comment) {
            token.currentLinenum = linenum;
            tokens.push_back(token);
            token.text.erase();

            token.whatType = NOTHING;
        }
    }

    void Flowtine::lex(const std::string_view contents) {
        Token newTok{NOTHING};
        int linenum = 1;
        for (const char character : contents) {
            switch (character) {
                // is it an operator?
                case '=':
                case '{':
                case '}':
                    clear(newTok, linenum);
                    if (comment) continue;

                    newTok.whatType = OPERATOR;
                    newTok.text.push_back(character);
                    clear(newTok, linenum);
                    break;
                // is it a comment?
                case '#':
                    comment = true;
                    break;
                case '\r':
                case '\n':
                    comment = false;
                    clear(newTok, linenum);
                    linenum++;
                    break;
                // any whitespace characters?
                case ' ':
                case '\t':
                case '\f':
                    if (newTok.whatType == IDENTIFIER)
                        clear(newTok, linenum);
                    break;
                // is it an identiifer?
                default:
                    if (comment) continue;
                    newTok.whatType = IDENTIFIER;
                    newTok.text.push_back(character);
                    break;
            }
        }
        clear(newTok, linenum);
        comment = false;

        newTok.whatType = ENDOF;
        clear(newTok, linenum);
    }

    std::string Flowtine::expectIdentifier(const std::vector<Token>::iterator& iter) const {
        if (iter->whatType != IDENTIFIER) {
            return " ";
        }
        return iter->text;
    }

    bool Flowtine::expectOperator(const std::vector<Token>::iterator& iter, const char* op) const {
        if (iter->text.compare(op)) {
            return false;
        }
        return true;
    }

    bool Flowtine::increment(std::vector<Token>::iterator& iter, const int advanceBy) {
        if ((iter + advanceBy) - tokens.end() > 0) {
            return false;
        }
        std::advance(iter, advanceBy);
        return true;
    }

    bool Flowtine::expectArray(std::vector<Token>::iterator& iter, const std::string name) {
        if (!increment(iter, 1)) return false;

        // create a new tree node for the array
        currentArray->arrays.insert(std::pair<std::string, std::shared_ptr<Array>>(name, std::make_shared<Array>()));
        Array* originalArray = currentArray;
        currentArray = (currentArray->arrays[name].get());

        // keep parsing until there has reached an end brace
        // if there is none, count it as a syntax error
        while (!expectOperator(iter, "}")) {
            std::vector<Token>::iterator start{iter};

            if (iter->whatType == ENDOF) {
                errorHandling("mismatched bracket", (start)->currentLinenum);
                exit(1);
            }
            if (!parse(iter)) {
                errorHandling("syntax error", (start)->currentLinenum);
            }
        }
        if (!increment(iter, 1)) return false;
        currentArray = originalArray;

        return true;
    }

    bool Flowtine::parse(std::vector<Token>::iterator& iter) {
        std::string name;
        std::string value;

        if ((name = expectIdentifier(iter)).compare(" ") && increment(iter, 1)) {
            if (expectOperator(iter, "=") && increment(iter, 1)) {
                if ((value = expectIdentifier(iter)).compare(" ")) {
                    // append the name and value into the variables map
                    currentArray->vars.insert(std::pair<std::string, std::string>(name, value));
                    if (!increment(iter, 1)) return false;
                }
                else if (expectOperator((iter), "{")) {
                    if (!expectArray(iter, name)) return false;
                }
                else {
                    return false;
                }

                return true;
            }
        }
        else {
            if (!increment(iter, 1)) return false;
        }

        return false;
    }
}