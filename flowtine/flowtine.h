/*
flowtine.h library
json-style parser
*/

#include <string>
#include <vector>
#include <iostream>
#include <optional>

namespace ftn {
    std::string externfc = "";
    std::vector<std::string> vals;

    enum Types {
        IDEN,
        OP,
        WHITESPACE,
        COMM,
        IFCOM
    };


    class Tok {
        public:
            enum Types type = WHITESPACE;
            std::string text;
            int linenum = 0;
    };

    class Parse {
        public:
            void parse(std::vector<Tok> toks);
        private:
            std::optional<Tok> expOp(const std::string& name = std::string());
            std::optional<Tok> expIden(const std::string& name = std::string());
            bool expType(std::string name);

            bool seeDec();
            bool seeArr();

            std::vector<Tok>::iterator currTok;
            std::vector<Tok>::iterator endTok;

            std::string error;
            std::string linenum;

            std::string arrRet;
            bool ifArr = false;

            std::vector<std::string> types = {"arr", "var"};
    };

    class Tokize {
        void endTok(Tok &tok, std::vector<Tok> &toks);
    public:
        std::vector<Tok> tokize(std::string &filecontents);
    };

    void openFile(const char* filename) {
        FILE* file = fopen(filename, "r");
        if (!file) {
            std::cerr << "flowtine: error: file not found!\n";
        }
        fseek(file, 0, SEEK_END);
        int filesize = ftell(file);
        fseek(file, 0, SEEK_SET);
        std::string filecontents(filesize, ' ');
        fread(filecontents.data(), 1, filesize, file);

        externfc = filecontents;

        Tokize tokize;
        Parse parse;
        std::vector<Tok> toks = tokize.tokize(filecontents);

        parse.parse(toks);
    }

    std::string val(std::string name = "", std::string arr = "") {
        bool ifVal, ifArr;
        Parse parse;
        Tokize tokize;
        if (externfc == "") {
            std::cerr << "flowtine: error: file not found or is empty\n"; 
            exit(1);
        }

        if (name == "") { return "null"; }

        int n = 2;
        std::vector<Tok> tokens;

        for (std::string val : vals) {
            std::vector<Tok> toks = tokize.tokize(val);
            // adds the toks vector to the tokens vector
            tokens.insert(tokens.end(), toks.begin(), toks.end());
        }

        for (int i = 0; i < tokens.size(); i++) {
            Tok tok = tokens[i];
            if (arr == "") {
                if (tok.text == "arr" || ifArr) {
                    ifArr = true;

                    if (tok.text == "}") {
                        ifArr = false;
                        continue;
                    }
                    else {
                        continue;
                    }
                }

                if (tok.text == "name") {
                    if (tokens[i + 2].text == name) {
                        return tokens[i + 5].text;
                    }
                }

                // std::cout << tok.text << " " << i << std::endl;
            }

            else {
                // std::cout << tok.text << " " << i << std::endl;

                if (tok.text == "arr") {
                    ifArr = true;
                    if (arr == tokens[i + 2].text) {
                        if (tokens[i + 4].text == "name") {
                            if (tokens[i + 6].text == name) {
                                return tokens[i + 13].text;
                            }
                        }
                    }
                }
                continue;
            }
        
        }

        // if no val is found, return null
        return "null";
    }

    // lexer
    std::vector<Tok> Tokize::tokize(std::string &filecontents) {
        std::vector<Tok> toks;
        Tok currTok;

        currTok.linenum = 1;
        
        for (char chr : filecontents) {
            switch (chr) {
                case '=':
                case ';':
                case '{':
                case '}':
                case ':':
                    endTok(currTok, toks);
                    currTok.type = OP;
                    currTok.text.append(1, chr);
                    endTok(currTok, toks);
                    break;
                case '/':
                    if (currTok.type == IFCOM) {
                        currTok.type = COMM;
                        currTok.text.erase();
                    }
                    else {
                        endTok(currTok, toks);
                        currTok.type = IFCOM;
                        currTok.text.append(1, chr);
                    }
                    break;
                case ' ':
                case '\t':
                    if (currTok.type == COMM) {
                        currTok.text.append(1, chr);
                    }
                    else {
                        endTok(currTok, toks);
                    }
                    break;
                case '\r':
                case '\n':
                    endTok(currTok, toks);
                    ++currTok.linenum;
                    break;
                default:
                    if (currTok.type == WHITESPACE) {
                        endTok(currTok, toks);
                        currTok.type = IDEN;
                        currTok.text.append(1, chr);
                    }
                    else {
                        currTok.text.append(1, chr);
                    }
                    break;
                

            }
        }

        endTok(currTok, toks);
        return toks;
    }

    void Tokize::endTok(Tok &tok, std::vector<Tok> &toks) {
        if (tok.type == COMM) {
            // do nothing
        } 
        else if (tok.type != WHITESPACE) {
            toks.push_back(tok);
        }

        tok.type = WHITESPACE;
        tok.text.erase();
    }

    // Parsing stage
    void Parse::parse(std::vector<Tok> toks) {
        currTok = toks.begin();
        endTok = toks.end();

        while (currTok != endTok) {
            if (!seeDec()) {
                std::cout << "error: " << error << " on line: " << currTok->linenum << std::endl;
                ++currTok;
            }
        }
    }

    bool Parse::seeDec() {       
        std::vector<Tok>::iterator pStart = currTok;
        if (expType("var")) {
            std::optional<Tok> ifName = expIden();

            if (ifName.has_value()) {
                if (expOp("=").has_value()) {
                    std::optional<Tok> ifVal = expIden();

                    if (ifVal.has_value()) {
                        if (expOp(";").has_value()) {
                            vals.push_back("name:" + ifName->text);
                            vals.push_back("val:" + ifVal->text);
                            return true;
                        }
                        else {
                            std::cerr << "invalid syntax - ';' missing on line: " << currTok->linenum + 2;
                            pStart = currTok;
                            return true;
                        }
                    }
                    else {
                        error = "no value found";
                        pStart = currTok;
                        return false;
                    }
                }
                else {
                    error = "invalid syntax - '=' missing";
                    pStart = currTok;
                    return false;
                }
            }
            else {
                error = "no name found";
                pStart = currTok;
                return false;
            }
        }
        else if (expType("arr") || ifArr) {
            std::optional<Tok> ifName = expIden();

            if (ifName.has_value() || ifArr) {
                if (expOp("=").has_value() || ifArr) {
                    if (expOp("{").has_value() || ifArr) {
                        ifArr = true;
                        arrRet = ifName->text + ":";
                        
                        while (!expOp("}").has_value()) {
                            if (!seeArr()) {
                                std::cerr << "error: " << error << " on line: " << currTok->linenum << std::endl;
                                ++currTok;
                            }
                            if (currTok == endTok) {
                                break;
                            }

                            if (expOp("}").has_value()) {
                                if (expOp(";").has_value()) {
                                    ifArr = false;
                                    break;
                                }
                                else {
                                    error = "invalid syntax - ';' missing";
                                    pStart = currTok;
                                }
                            }
                            else {
                                error = "invalid syntax - '{' missing";
                                pStart = currTok;
                            }
                        }
                        return true;
                    } 
                    else {
                        error = "invalid syntax - '{' missing";
                        pStart = currTok;
                    }
                }
                else {
                    error = "invalid syntax - '=' missing";
                    pStart = currTok;
                }
            }
            else {
                error = "no name found";
                pStart = currTok;
            }
            return false;
        }
        else {
            error = "no type found";
            pStart = currTok;
        }

        return false;
    }

    bool Parse::seeArr() {
        std::optional<Tok> ifName = expIden();
        std::vector<Tok>::iterator pStart = currTok;

        if (ifName.has_value()) {
            if (expOp("=").has_value()) {
                std::optional<Tok> ifVal = expIden();
                
                if (ifVal.has_value()) {
                    if (expOp(";").has_value()) {
                        vals.push_back("arr:" + arrRet + "name:" + ifName->text);
                        vals.push_back("arr:" + arrRet + "val:" + ifVal->text);
                        vals.push_back("}");
                        return true;
                    }
                    else {
                        error = "invalid syntax - ';' missing";
                        pStart = currTok;
                    }
                }
                else {
                    error = "no value found";
                    pStart = currTok;
                }
            }
            else {
                error = "invalid syntax - '=' missing";
                pStart = currTok;
            }
        }
        else {
            error = "no name found";
            pStart = currTok;
        }
        return false;
    }

    std::optional<Tok> Parse::expIden(const std::string& name) {
        if (currTok == endTok) { return std::nullopt; }
        if (currTok->type != IDEN) { return std::nullopt; }
        if (currTok->text != name && !name.empty()) { return std::nullopt; }

        Tok returnTok = *currTok;
        ++currTok;
        return returnTok; 
    }

    std::optional<Tok> Parse::expOp(const std::string& name) {
        if (currTok == endTok) { return std::nullopt; }
        if (currTok->type != OP) { return std::nullopt; }
        if (currTok->text != name && !name.empty()) { return std::nullopt; }

        Tok returnTok = *currTok;
        ++currTok;
        return returnTok; 
    }

    bool Parse::expType(std::string name) {
        if (currTok == endTok) { return false; }
        if (currTok->text != name && !name.empty()) { return false; }
        
        for (std::string i : types) {
            if (i == name) {
                ++currTok;
                return true;
            }
        }

        ++currTok;
        return false;
    }
}
