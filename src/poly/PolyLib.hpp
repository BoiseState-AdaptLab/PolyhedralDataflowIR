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
    private:
        IEGenLib _iegen;
        OmegaLib _omega;

    protected:
        void addPragma(const string& schedule, const string& privates, string& code) {
            string pragma = "#pragma omp parallel for schedule(" + schedule + ")";
            if (!privates.empty()) {
                pragma += " private(" + privates + ")";
            }
            code = pragma + "\n" + code;
        }

        void addStatements(unsigned count, const vector<string>& statements, const vector<string>& guards,
                           const vector<string>& schedules, map<string, string>& macros, string& defines) {
            if (!statements.empty()) {
//                vector<string> iterList = _omega.in_iterators();
//                string inIters = Strings::join(iterList, ",");
//                unsigned nIters = inIters.size();

                for (size_t i = 0; i < statements.size(); i++) {
                    defines += "#undef s" + to_string(count + i) + "\n";
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
                        addMacros(macros, guard);
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

    public:
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
                       const string& ompSched = "", const string& iterType = "", bool defineMacros = false) {
            string code = codegen(names, schedules);
            if (code.find("ERROR") == string::npos) {
                string outIters = Strings::join(_omega.out_iterators(), ",");

                if (!ompSched.empty()) {
                    addPragma(ompSched, outIters, code);
                }

                if (!iterType.empty() && !outIters.empty()) {
                    code = iterType + " " + outIters + ";\n" + code;
                }

                map<string, string> macros;
                if (defineMacros) {
                    addMacros(macros, _omega.macros());
                }

                string defines = "";
                unsigned nstatements = 0;
                for (const string& setname : names) {
                    addStatements(nstatements, statements[setname], guards[setname],
                                  schedules[setname], macros, defines);
                    nstatements += statements[setname].size();
                }

                code = defines + "\n" + code;
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
                    //if (!defines.empty()) {
                        code = defines + "\n" + code;
                    //}
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
