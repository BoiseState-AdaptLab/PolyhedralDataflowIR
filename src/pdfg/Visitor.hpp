/**
 * \class DFGVisitor
 *
 * \ingroup PolyExt
 * (Note, this needs exactly one \defgroup somewhere)
 *
 * \brief Polyhedral Dataflow Graph Visitor
 *
 * C++ implementation of polyhedral dataflow graphs (PDFGs).
 *
 * \author Eddie Davis
 * \version $Revision: 0.1 $
 * \date $Date: 2018/12/03
 * Contact: eddiedavis@boisestate.edu
 */

#ifndef POLYEXT_VISITOR_H
#define POLYEXT_VISITOR_H

#include <pdfg/GraphIL.hpp>

namespace pdfg {
    struct DFGVisitor {
    public:
        virtual void visit(Node* node) {
            enter(node);
            exit(node);
        }

        virtual void enter(Node*) = 0;

        virtual void enter(CompNode*) = 0;

        virtual void enter(DataNode*) = 0;

        virtual void exit(Node*) {}

        virtual void finish(FlowGraph* graph) {}

        virtual void setup(FlowGraph* graph) {}

        virtual void walk(FlowGraph* graph) {
            setup(graph);
            for (Node* node : graph->nodes()) {
                visit(node);        //node->accept(this);
            }
//          for i in range(g.nrows):
//            for j in range(g.ncols):
//              if (i, j) in g:
//                node = g[(i, j)]
//                node.accept(self)
            finish(graph);
        }
    };

    struct CodeGenVisitor: DFGVisitor {
    public:
        explicit CodeGenVisitor(const string &path = "", const string &lang = "C", bool profile = false, unsigned niters = 5) :
                _path(path), _niters(niters), _lang(lang), _profile(profile) {
            init();
        }

        void include(const initializer_list<string> &libraries) {
            for (const string &library : libraries) {
                include(library);
            }
        }

        void include(const string &library) {
            _includes.push_back(library);
        }

        void define(const initializer_list<string> &defines) {
            unsigned n = 0;
            string name;
            for (const string &defn : defines) {
                if (n % 2 == 0) {
                    name = defn;
                } else {
                    define(name, defn);
                }
                n += 1;
            }
        }

        void define(const map<string, string>& defines) {
            for (const auto& iter : defines) {
                define(iter.first, iter.second);
            }
        }

        void define(const pair<string, string>& defpair) {
            define(defpair.first, defpair.second);
        }

        void define(const string &name, const string &defn = "") {
            _defines.push_back(make_pair<string, string>(string(name), string(defn)));
        }

        void function(const string &header, const string &body) {
            _functions.push_back(make_pair<string, string>(string(header), string(body)));
        }

        void typdef(const string &type, const string &alias) {
            _typedefs.push_back(make_pair<string, string>(string(type), string(alias)));
        }

        string ompSchedule() const {
            return _ompsched;
        }

        void ompSchedule(const string& schedule) {
            _ompsched = schedule;
        }

        string path() const {
            return _path;
        }

        void path(const string &p) {
            _path = p;
            if (_path.find('/') == string::npos) {
                char cwd[1024];
                getcwd(cwd, sizeof(cwd));
                _path = string(cwd) + '/' + _path;
            }
            if (_path.find('.') == string::npos) {
                _path += "." + string(tolower(_lang[0]), 1);
            }
        }

        vector<string> header() const {
            return _header;
        }

        vector<string> allocs() const {
            return _allocs;
        }

        vector<string> body() const {
            return _body;
        }

        void setup(FlowGraph* graph) override {
            _graph = graph;
            addIncludes();
            addDefines();
            addTypeDefs();
            addFunctions();
        }

        void enter(Node *node) override {
            cerr << "Visiting node '" << node->label() << "'" << endl;
            switch (node->type()) {
                case 'D':
                    enter((DataNode *) node);
                    break;
                case 'C':
                    enter((CompNode *) node);
                    break;
                case 'R':
                    enter((RelNode *) node);
                    break;
                default:    // Throw exception, maybe?
                    break;
            }
        }

        void enter(DataNode* node) override {
            string label = node->label();
            if (!_graph->isReturn(node) && _graph->isSource(node)) {
                // Input Data
                string param = "const " + node->datatype();
                if (!node->is_scalar()) {
                    param += "*";
                }
                param += " " + label;
                _params.push_back(param);
            } else if (!_graph->isReturn(node) && _graph->isSink(node)) {
                // Output Data
                string param = node->datatype() + "* " + label;
                _params.push_back(param);
            } else if (node->alloc() != NONE) {
                // Temporary Data
                ostringstream os;
                os << _indent;
                if (node->is_scalar()) {
                    os << node->datatype() << ' ' << label<< " = " << node->defval() << ';';
                } else if (node->alloc() == DYNAMIC) {
                    os << node->datatype() << "* ";
                    bool isC = (_lang.find("+") == string::npos);
                    if (isC) {
                        os << "restrict ";
                    }
                    os << label << " = ";
                    if (!isC) {
                        os << '(' << node->datatype() << "*) ";
                    }
                    os << "calloc("   << *node->size() << ",sizeof(" << node->datatype() << "));";
                    _frees.push_back(node->label());
                } else {
                    if (node->alloc() == STATIC) {
                        os << "static ";
                    }
                    os << node->datatype() << ' ' << label << '['
                       << *node->size() << "] = {" << node->defval() << "};";
                }
                string line = os.str();
                _allocs.push_back(line);
                if (node->alloc() == DYNAMIC && node->defval() != "0") {
                    os.str(_indent);
                    os << "arrinit(" << label << ',' << node->defval() << ',' << *node->size() << ");";
                    line = os.str();
                    _allocs.push_back(line);
                }
            }
            if (!node->mapping().empty()) {
                define(label, node->mapping());
            }
        }

        void enter(CompNode* node) override {
            vector<string> names;
            map<string, vector<string> > guards;
            map<string, vector<string> > statements;
            map<string, vector<string> > schedules;

            Comp* comp = (Comp*) node->expr();
            updateComp(comp, names, statements, guards, schedules);

            for (CompNode* child : node->children()) {
                comp = (Comp*) child->expr();
                updateComp(comp, names, statements, guards, schedules);
            }

            //string code = _poly.codegen(space.name(), "", "", true, statements, guards, schedules);
            string code = _poly.codegen(names, statements, guards, schedules, _ompsched);
            code = "// " + node->label() + "\n" + code;
            _body.push_back(code);
        }

        void enter(RelNode* node) {
            int stop = 4;
        }

        void finish(FlowGraph* graph) override {
            // Create header
            string line = graph->returnType() + " " + graph->name() + "(";
            unsigned n = 0, nparams = _params.size();
            for (const string& param : _params) {
                line += param;
                if (n < nparams - 1) {
                    line += ", ";
                } else {
                    line += ")";
                }
                n += 1;
            }
            _header.push_back(line + ";");
            _header.push_back("inline " + line + " {");

            // Define iterators.
            line = _indent + _graph->indexType() + " ";
            for (unsigned i = 1; i < _niters; i++) {
                line += "t" + to_string(i) + ",";
            }
            line += "t" + to_string(_niters) + ";";
            _header.push_back(line);

            for (const string& free : _frees) {
                line = _indent + "free(" + free + ");";
                _body.push_back(line);
            }

            if (!graph->returnName().empty()) {
                line = "\n" + _indent + "return (" + graph->returnName() + ");";
                _body.push_back(line);      // Return value...
            }

            line = "}" + _indent + "// " + graph->name();
            _body.push_back(line);      // Close the fxn

            // Undefine macros for the safety of including functions.
            undoDefines();

            if (!_path.empty()) {
                ofstream fout(_path.c_str());
                fout << str() << endl;
                fout.close();
            }
            _graph = nullptr;
        }

        string str() const {
            string out = Strings::join(_header, "\n") + "\n";
            if (!_allocs.empty()) {
                out += Strings::join(_allocs, "\n") + "\n\n";
            }
            out += Strings::join(_body, "\n") + "\n";
            return out;
        }

        friend ostream& operator<<(ostream& os, const CodeGenVisitor& cgen) {
            os << cgen.str();
            return os;
        }

    protected:
        void init() {
            // Add defines...
            define({"min(x,y)", "(((x)<(y))?(x):(y))",
                    "max(x,y)", "(((x)>(y))?(x):(y))",
                    "abs(x)", "((x)<0?-(x):(x))",
                    "absmin(x,y)", "((x)=min(abs((x)),abs((y))))",
                    "absmax(x,y)", "((x)=max(abs((x)),abs((y))))",
                    "floord(x,y)", "((x)/(y))",
                    "sgn(x)", "((x)<0?-1:1)",
                    "offset2(i,j,M)", "((j)+(i)*(M))",
                    "offset3(i,j,k,M,N)", "((k)+((j)+(i)*(M))*(N))",
                    "offset4(i,j,k,l,M,N,P)", "((l)+((k)+((j)+(i)*(M))*(N))*(P))",
                    "arrinit(ptr,val,size)", "for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)",
                    "arrprnt(name,arr,size)", "{\\\nfprintf(stderr,\"%s={\",(name));\\\nfor(unsigned __i__=0;__i__<(size);__i__++) fprintf(stderr,\"%lg,\",(arr)[__i__]);\\\nfprintf(stderr,\"}\\n\");}"
                   });

            // Add includes...
            include({"stdio", "stdlib", "stdint", "math"});
            if (_profile) {
                include("sys/time");
            }

            _indent = "    ";
            //_ompsched = "runtime";
        }

        void addIncludes() {
            if (!_includes.empty()) {
                for (string& include : _includes) {
                    if (include.find(".h") == string::npos) {
                        include += ".h";
                    }
                    string line = "#include <" + include + ">";
                    _header.push_back(line);
                }
                _header.emplace_back("");
            }
        }

        void addDefines() {
            if (!_defines.empty()) {
                for (auto& define : _defines) {
                    string line = "#define " + define.first + " " + define.second;
                    _header.push_back(line);
                }
                _header.emplace_back("");
            }
        }

        void addTypeDefs() {
            if (!_typedefs.empty()) {
                for (auto& tdef : _typedefs) {
                    string line = "typedef " + tdef.first + " " + tdef.second + ";";
                    _header.push_back(line);
                }
                _header.emplace_back("");
            }
        }

        void addFunctions() {
            if (!_functions.empty()) {
                for (auto& fxn : _functions) {
                    string line = fxn.first + " { " + fxn.second + " }";
                    if (line.find("inline") == string::npos) {
                        line = "inline " + line;
                    }
                    _header.push_back(line);
                }
                _header.emplace_back("");
            }
        }

        void undoDefines() {
            if (!_defines.empty()) {
                _body.emplace_back("");
                for (auto& define : _defines) {
                    string undef = define.first;
                    size_t pos = undef.find('(');
                    if (pos != string::npos) {
                        undef = undef.substr(0, pos);
                    }
                    string line = "#undef " + undef;
                    _body.push_back(line);
                }
            }
        }

        void updateComp(Comp* comp, vector<string>& names,
                        map<string, vector<string> >& statements,
                        map<string, vector<string> >& guards,
                        map<string, vector<string> >& schedules) {
            //string iegstr = Strings::replace(comp.space().to_iegen(), "N/8", "N_R");
            string iegstr = comp->space().to_iegen();
            string norm = _poly.add(iegstr);
            string sname = comp->space().name();

            names.push_back(sname);
            for (const Constr& guard : comp->guards()) {
                guards[sname].emplace_back(stringify<Constr>(guard));
            }
            for (const Math& statement : comp->statements()) {
                statements[sname].emplace_back(stringify<Math>(statement));
            }
            for (const auto& schedule : comp->schedules()) {
                schedules[sname].push_back(schedule.to_iegen());
            }
        }

        bool _profile;
        unsigned _niters;

        string _indent;
        string _lang;
        string _path;
        string _ompsched;

        vector<pair<string, string> > _defines;
        vector<pair<string, string> > _typedefs;
        vector<pair<string, string> > _functions;

        vector<string> _includes;
        vector<string> _header;
        vector<string> _params;
        vector<string> _body;
        vector<string> _allocs;
        vector<string> _frees;

        PolyLib _poly;
        FlowGraph* _graph;
    };
}

#endif  // POLYEXT_VISITOR_H
