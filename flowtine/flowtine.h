/*   
flowtine.h library
json-style parser
*/

#include <string>
#include <vector>
#include <iostream>
#include <optional>

namespace ftn {
    std::string externfc = ".";
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
            enum Types type{WHITESPACE};
            std::string text;
            int linenum = 0;

    };

    class Parse {
        public:
            void parse(std::vector<Tok> toks);
        private:
            std::optional<Tok> expOp(const std::string& name = std::string());
            std::optional<Tok> expIden(const std::string& name = std::string());

            bool seeDec();

            std::vector<Tok>::iterator currTok;
            std::vector<Tok>::iterator endTok;

            std::string error;
            std::string linenum;

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

    std::string val(std::string name) {
        Parse parse;
        Tokize tokize;
        if (externfc == ".") { std::cout << "flowtine: error: file not found!\n"; return "NULL"; }

        std::vector<Tok> toks = tokize.tokize(externfc); 
        
        parse.parse(toks);
        
        int n = 0;
        for (std::string i : vals) {
            if (i == name) {
                return vals[n + 1];
            }

            n++;
        }

        return "null";
    }

    std::vector<Tok> Tokize::tokize(std::string &filecontents) {
        std::vector<Tok> toks;
        Tok currTok;

        currTok.linenum = 1;
        
        for (char chr : filecontents) {
            switch (chr) {
                case '=':
                case ';':
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

        return toks;
    }

    void Tokize::endTok(Tok &tok, std::vector<Tok> &toks) {
        if (tok.type == COMM) {
            // ..
        } 
        else if (tok.type != WHITESPACE) {
            toks.push_back(tok);
        }

        tok.type = WHITESPACE;
        tok.text.erase();
    }

    // ....
    void Parse::parse(std::vector<Tok> toks) {
        currTok = toks.begin();
        endTok = toks.end();

        while (currTok != endTok) {
            if (seeDec()) {
                // ..
            }
            else {
                std::cout << "error: " << error << " on line: " << currTok->linenum << std::endl;
                ++currTok;
            }
        }
    }

    bool Parse::seeDec() {
        std::vector<Tok>::iterator pStart = currTok;
        std::optional<Tok> ifName = expIden();

        if (ifName.has_value()) {
            if (expOp("=").has_value()) {
                std::optional<Tok> ifVal = expIden();

                if (ifVal.has_value()) {
                    if (expOp(";").has_value()) {
                        vals.push_back(ifName->text);
                        vals.push_back(ifVal->text);
                        return true;
                    }
                    else {
                        error = "invalid syntax";
                        currTok = pStart;
                    }
                }
                else {
                    error = "no value found";
                    currTok = pStart;
                }
            }
            else {
                error = "invalid syntax";
                currTok = pStart;
            }
        }
        else {
            error = "no names found";
            currTok = pStart;
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
}