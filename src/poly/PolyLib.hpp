#ifndef POLYLIB_HPP
#define POLYLIB_HPP

#include <string>
#include <iegenlib/IEGenLib.hpp>
#include <omega/OmegaLib.hpp>

using std::string;
using iegen::IEGenLib;
using omglib::OmegaLib;

namespace poly {
    struct PolyLib {
    protected:
        const int _max_iters = 15;      // TODO: Actually calculate this from the relation...

        map<string, string> _macros;

        IEGenLib _iegen;
        OmegaLib _omega;

        string buildPragma(const string& partype, const string& privates) {
            string pragma = "#pragma omp ";
            if (partype[0] == 'S') {
                pragma += "simd";
            } else {
                pragma += "parallel for schedule(auto)";
                if (!privates.empty()) {
                    pragma += " private(" + privates + ")";
                }
            }
            return pragma;
        }

        void addPragmas(const vector<string>& parTypes, const string& privates, string& code) {
            string indent = "";
            size_t forPos = code.find("for(");
            for (const string& parType : parTypes) {
                if (!parType.empty() && forPos != string::npos) {
                    if (parType[0] != 'N') {
                        size_t iterPos = code.find('=', forPos + 4);
                        string loopIter = code.substr(forPos, iterPos-forPos-1);
                        string pragma = buildPragma(parType, privates);
                        // Insert the pragma while the loop iterator appears...
                        size_t insPos = forPos;
                        while (insPos != string::npos) {
                            code.insert(insPos, pragma + "\n" + indent);
                            insPos = code.find(loopIter, insPos + pragma.size() + indent.size() + loopIter.size() + 1);
                        }
                        forPos += pragma.size() + indent.size() + 1;
                    }
                    forPos = code.find("for(", forPos + 1);
                }
                indent += "  ";
            }
        }

        void addStatements(const string& name, unsigned count, const vector<string>& statements,
             const vector<string>& guards, const vector<string>& schedules, string& defines) {
            if (!statements.empty()) {
                for (size_t i = 0; i < statements.size(); i++) {
                    defines += "// " + name + "\n#undef s" + to_string(count + i) + "\n";
                }

                for (size_t i = 0; i < statements.size(); i++) {
                    string schedule = schedules[i];
                    size_t pos = schedule.find('[');
                    string inIters = schedule.substr(pos + 1, schedule.find(']') - pos - 1);
                    vector<string> iterList = Strings::split(inIters, ',');
                    unsigned nIters = iterList.size();

                    string statement = statements[i];
                    for (unsigned n = 0; n <= nIters; n++) {
                        string pattern = "#" + to_string(n+1);
                        statement = Strings::replace(statement, pattern, iterList[n]);
                    }

                    string guard;
                    if (i < guards.size()) {
                        guard = guards[i];
                    }

                    if (!guard.empty()) {
                        statement = "(" + guard + ") " + statement;
                        addMacros(_macros, guard);
                    }

                    for (const string& itr : iterList) {
                        statement = Strings::replace(statement, itr, "(" + itr + ")", true);
                    }

                    if (!guard.empty()) {
                        statement = "if " + statement;
                    }

                    defines += "#define s" + to_string(count + i) + '(';
                    defines += inIters + ") " + statement + "\n";
                }
            }
        }

        string out_iterators(const string& code) {
            string iters = "";
            for (unsigned i = 0; i < _max_iters; i++) {
                string iter = "t" + to_string(i);
                if (code.find("(" + iter) != string::npos || code.find("," + iter) != string::npos) {
                    iters += "," + iter;
                }
            }
            if (!iters.empty()) {
                iters = iters.substr(1);
            }
            return iters;
        }

    public:
        map<string, string>& macros() {
            return _macros;
        }

        string add(const string &constStr, const string &name = "") {
            string result;
            if (constStr.find(":=") == string::npos && !name.empty()) {
                result = _iegen.add(name + " := " + constStr);
            } else {
                result = _iegen.add(constStr);
            }
            return result;
        }

        string get(const string &name) {
            return _iegen.get(name);
        }

        string newSet(const string &setStr) {
            return _iegen.newSet(setStr);
        }

        string newRelation(const string &relStr) {
            return _iegen.newRelation(relStr);
        }

        string apply(const string& relName, const string& setName, const string& resName = "") {
            string result = _iegen.apply(relName, setName, resName);
            if (!resName.empty() && result.find(ASN_OP) == string::npos) {
                result = resName + ' ' + ASN_OP + ' ' + result;
            }
            return result;
        }

        string domain(const string &name) {
            return _iegen.domain(name);
        }

        string inverse(const string& relName) {
            return _iegen.inverse(relName);
        }

        string union_(const string& lhsName, const string &rhsName) {
            return _iegen.union_(lhsName, rhsName);
        }

        string compose(const string& lhsName, const string &rhsName) {
            return _iegen.compose(lhsName, rhsName);
        }

        string codegen(const string& setName, const vector<string>& schedules = {}) {
            string setStr = _iegen.get(setName);
            string outStr;
            if (setStr.find("ERROR") == string::npos) {
                outStr = _omega.codegen(setStr, setName, schedules);
            } else {
                outStr = "ERROR: Set '" + setName + "' does not exist in 'codegen'.";
            }
            return outStr;
        }

        string codegen(const string& setStr, const string& setName, bool skipNorm, const vector<string>& schedules = {}) {
            string outStr;
            if (setStr.find("ERROR") == string::npos) {
                outStr = _omega.codegen(setStr, setName, schedules);
            } else {
                outStr = "ERROR: Set '" + setName + "' does not exist in 'codegen'.";
            }
            return outStr;
        }

        string codegen(const vector<string>& names, map<string, vector<string> >& schedules) {
            map<string, string> setdefs;
            for (const string& setname : names) {
                string setstr = _iegen.get(setname);
                if (setstr.find("ERROR") == string::npos) {
                    setdefs[setname] = _iegen.get(setname);
                } else {
                    return "ERROR: Set '" + setname + "' does not exist in 'codegen'.";
                }
            }
            return _omega.codegen(names, setdefs, schedules);
        }

        string codegen(vector<string>& names, map<string, vector<string> >& statements,
                       map<string, vector<string> >& guards, map<string, vector<string> >& schedules,
                       const vector<string>& parTypes = {}, const string& iterType = "", bool defineMacros = false) {
            string code = codegen(names, schedules);
            if (code.find("ERROR") == string::npos && code.find("error") == string::npos) {
                string outIters = out_iterators(code);

                // Replace 'intFloor' with 'floord' to maintain compatibility w/ ISCC generated code.
                code = Strings::replace(code, "intFloor", "floord", true);

                if (!parTypes.empty()) {
                    addPragmas(parTypes, outIters, code);
                }

                if (!iterType.empty() && !outIters.empty()) {
                    code = iterType + " " + outIters + ";\n" + code;
                }

                if (defineMacros) {
                    addMacros(_macros, _omega.macros());
                }

                string defines = "";
                unsigned nstatements = 0;
                for (const string& setname : names) {
                    addStatements(setname, nstatements, statements[setname], guards[setname], schedules[setname], defines);
                    nstatements += statements[setname].size();
                }

                code = defines + "\n" + code;
            } else {
                cerr << code << endl;
            }

            return code;
        }

        string codegen(const string& setName, const string& iterType, const string& ompSched = "",
            bool defineMacros = false, const vector<string>& statements = {},
            const vector<string>& guards = {}, const vector<string>& schedules = {}) {
            string code = codegen(setName, schedules);
            if (code.find("ERROR") == string::npos) {
                unsigned nIters = 0;
                string inIters;
                string outIters;
                string defines;
                map<string, string> macros;
                string setStr = _iegen.get(setName);

                // Replace 'intFloor' with 'floord' to maintain compatibility w/ ISCC generated code.
                code = Strings::replace(code, "intFloor", "floord", true);

                size_t pos = setStr.find('[');
                for (pos++; pos < setStr.length() && setStr[pos] != ']'; pos++) {
                    char setChr = setStr[pos];
                    if (setChr == ',') {
                        nIters++;
                    }
                    if (setChr != ' ' && setChr != '{' && setChr != '}') {
                        inIters += setChr;
                    }
                }
                if (nIters > 0) {
                    for (unsigned i = 0; i <= nIters; i++) {
                        outIters += "t" + to_string(i + 1);
                        if (i < nIters) {
                            outIters += ",";
                        }
                    }
                }

                if (!ompSched.empty()) {
                    string pragma = "#pragma omp parallel for schedule(" + ompSched + ")";
                    if (!outIters.empty()) {
                        pragma += " private(" + outIters + ")";
                    }
                    code = pragma + "\n" + code;
                }

                if (!iterType.empty() && !outIters.empty()) {
                    code = iterType + " " + outIters + ";\n" + code;
                }

                if (defineMacros) {
                    addMacros(macros, _omega.macros());
                }

                if (!statements.empty()) {
                    for (size_t i = 0; i < statements.size(); i++) {
                        defines += "#undef s" + to_string(i) + "\n";
                    }

                    vector<string> iterList = Strings::split("" + inIters, ",");
                    for (size_t i = 0; i < statements.size(); i++) {
                        string statement = statements[i];
                        for (unsigned n = 0; n <= nIters; n++) {
                            string pattern = "#" + to_string(n+1);
                            statement = Strings::replace(statement, pattern, iterList[n]);
                        }

                        string guard;
                        if (i < guards.size()) {
                            guard = guards[i];
                        }

                        if (!guard.empty()) {
                            statement = "(" + guard + ") " + statement;
                            addMacros(macros, guard);
                        }

                        for (const string& itr : iterList) {
                            statement = Strings::replace(statement, itr, "(" + itr + ")", true);
                        }

                        if (!guard.empty()) {
                            statement = "if " + statement;
                        }

                        defines += "#define s" + to_string(i) + '(';
                        defines += inIters + ") " + statement + "\n";
                    }
                    code = defines + "\n" + code;
                }

                if (defineMacros) {
                    defines = "";
                    for (auto itr = macros.begin(); itr != macros.end(); ++itr) {
                        defines += "#define " + itr->first + " " + itr->second + "\n";
                    }
                    code = defines + "\n" + code;
                }
            }

            if (!code.empty() && code[code.size()-1] != '\n') {
                code += '\n';
            }

            return code;
        }

        void addMacros(map<string, string>& macros, const map<string, string>& others) {
            for (const auto& keyval : others) {
                macros[keyval.first] = keyval.second;
            }
        }

        void addMacros(map<string, string>& macros, const string& code) {
            vector<string> items = Strings::words(code);
            for (const string& item : items) {
                if (macros.find(item) == macros.end()) {
                    string call = item + '(';
                    size_t pos = code.find(call);
                    if (pos != string::npos) {
                        unsigned nargs = 1;
                        bool nested = false;
                        for (pos += 1; pos < code.size(); pos += 1) {
                            char chr = code[pos];
                            if (chr == '(') {
                                nested = true;
                            } else if (chr == ')') {
                                if (nested) {
                                    nested = false;
                                } else {
                                    break;
                                }
                            } else if (chr == ',') {
                                nargs += 1;
                            }
                        }

                        string lhs = call;
                        string rhs = item + "[(";
                        for (unsigned i = 0; i < nargs; i++) {
                            string iter = string(1, 'i' + i);
                            lhs += iter + ",";
                            rhs += iter + "),(";
                        }
                        lhs = lhs.substr(0, lhs.size() - 1) + ')';
                        rhs = rhs.substr(0, rhs.size() - 2) + ']';
                        macros[lhs] = rhs;
                    }
                }
            }
        }
    };
}

#endif  // POLYLIB_HPP
