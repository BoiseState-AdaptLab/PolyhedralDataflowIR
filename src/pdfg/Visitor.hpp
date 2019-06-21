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
#include <util/OS.hpp>
using util::OS;

namespace pdfg {
    struct DFGVisitor {
    public:
        virtual void visit(Node* node) {
            enter(node);
            exit(node);
        }

        virtual void enter(Node *node) {
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

        virtual void enter(CompNode*) = 0;

        virtual void enter(DataNode*) = 0;

        virtual void exit(Node*) {}

        virtual void finish(FlowGraph* graph) {}

        virtual void setup(FlowGraph* graph) {
            _graph = graph;
        }

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

    protected:
        FlowGraph* _graph;
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
            addComment();
            addIncludes();
            addDefines();
            addTypeDefs();
            addFunctions();
        }

        void enter(DataNode* node) override {
            string label = node->label();
            string defval = node->defval();
            if (!_graph->isReturn(node) && _graph->isSource(node)) {
                // Input Data
                string param = "const " + node->datatype();
                if (!node->is_scalar()) {
                    param += "*";
                }
                param += " " + label;
                _params.push_back(param);
            } else if ((!_graph->isReturn(node) && _graph->isSink(node)) || _graph->output(label) >= 0) {
                // Output Data
                string param = node->datatype() + "* " + label;
                _params.push_back(param);
            } else if (node->alloc() != NONE) {
                // Temporary Data
                ostringstream os;
                os << _indent;
                if (node->is_scalar()) {
                    os << node->datatype() << ' ' << label;
                    if (!defval.empty()) {
                        os << " = " << defval;
                    }
                    os << ';';
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
                    if (defval == "0") {
                        os << "calloc(" << *node->size() << ",sizeof(" << node->datatype() << "));";
                    } else {
                        os << "malloc(" << *node->size() << "*sizeof(" << node->datatype() << "));";
                    }
                    _frees.push_back(node->label());
                } else {
                    if (node->alloc() == STATIC) {
                        os << "static ";
                    }
                    os << node->datatype() << ' ' << label << '['
                       << *node->size() << "] = {" << defval << "};";
                }
                string line = os.str();
                _allocs.push_back(line);
                if (node->alloc() == DYNAMIC && !defval.empty() && defval != "0") {
                    os.str(_indent);
                    os << "arrinit(" << label << ',' << defval << ',' << *node->size() << ");";
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
            std::sort(_params.begin(), _params.end());
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

        void addComment() {
            _header.push_back("// '" + _graph->name() + "' code generated by '" +
                              OS::username() + "' at " + OS::timestamp());
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
    };

    struct PerfModelVisitor : public DFGVisitor {
    public:
        explicit PerfModelVisitor() {}

        /// For each computation node, compute the amount of data read (loaded), written (stored), the number
        /// of input and output streams, the number of int ops (IOPs), floating point ops (FLOPs).
        /// This leads to an estimate for the amount of memory traffic (Q) and total work (W).
        /// Arithmetic intensity can be computed as I = W/Q, the x-axis of the Roofline plot.
        /// \param node Computation node
        void enter(CompNode* node) override {
            unsigned inStreamsI = 0, outStreamsI = 0, inStreamsF = 0, outStreamsF = 0, nIOPs = 0, nFLOPs = 0;
            string inSizeExprI, outSizeExprI, inSizeExprF, outSizeExprF;

            vector<Edge*> inedges = _graph->inedges(node);
            for (Edge* edge : inedges) {
                if (edge->source()->is_data()) {
                    DataNode* source = (DataNode*) edge->source();
                    if (!source->is_scalar()) {
                        string sizeExpr = "+" + source->size()->text();
                        if (source->is_int()) {
                            inStreamsI += 1;
                            inSizeExprI += sizeExpr;
                        } else {
                            inStreamsF += 1;
                            inSizeExprF += sizeExpr;
                        }
                    }
                }
            }

            if (!inSizeExprI.empty()) {
                inSizeExprI = inSizeExprI.substr(1);
                if (inSizeExprI.find('+') != string::npos) {
                    inSizeExprI = "(" + inSizeExprI + ")";
                }
                inSizeExprI += "*" + to_string(sizeof(int));
                node->attr("isize_in", inSizeExprI);
                node->attr("istreams_in", to_string(inStreamsI));
            }

            if (!inSizeExprF.empty()) {
                inSizeExprF = inSizeExprF.substr(1);
                if (inSizeExprF.find('+') != string::npos) {
                    inSizeExprF = "(" + inSizeExprF + ")";
                }
                inSizeExprF += "*" + to_string(sizeof(double));
                node->attr("fsize_in", inSizeExprF);
                node->attr("fstreams_in", to_string(inStreamsF));
            }

            vector<Edge*> outedges = _graph->outedges(node);
            for (Edge* edge : outedges) {
                DataNode* dest = (DataNode*) edge->dest();
                if (!dest->is_scalar()) {
                    string sizeExpr = "+" + dest->size()->text();
                    if (dest->is_int()) {
                        outStreamsI += 1;
                        outSizeExprI += sizeExpr;
                    } else {
                        outStreamsF += 1;
                        outSizeExprF += sizeExpr;
                    }
                }
            }

            if (!outSizeExprI.empty()) {
                outSizeExprI = outSizeExprI.substr(1);
                if (outSizeExprI.find('+') != string::npos) {
                    outSizeExprI = "(" + outSizeExprI + ")";
                }
                outSizeExprI += "*" + to_string(sizeof(int));
                node->attr("isize_out", outSizeExprI);
                node->attr("istreams_out", to_string(outStreamsI));
            }

            if (!outSizeExprF.empty()) {
                outSizeExprF = outSizeExprF.substr(1);
                if (outSizeExprF.find('+') != string::npos) {
                    outSizeExprF = "(" + outSizeExprF + ")";
                }
                outSizeExprF += "*" + to_string(sizeof(double));
                node->attr("fsize_out", outSizeExprF);
                node->attr("fstreams_out", to_string(outStreamsF));
            }

            // TODO: count int ops later (maybe), but assume at least one
            nIOPs = 1;

            // Count FLOPs...
            nFLOPs = node->flops();

            node->attr("flops", to_string(nFLOPs));
            node->attr("iops", to_string(nIOPs));
        }

        void enter(DataNode* node) override {
            // TODO: Do this method need to do any work?
        }

        void finish(FlowGraph* graph) override {
            // TODO: Do this method need to do any work?
        }
    };

    struct ScheduleVisitor : public DFGVisitor {
    public:
        explicit ScheduleVisitor(map<string, vector<Access> > accesses) {
            _accesses = accesses;
        }

        /// For each computation node, generate and assign a scheduling (scattering) function.
        /// \param node Computation node
        void enter(CompNode* node) override {
            unsigned i, j, n;
            unsigned level = 0;
            unsigned maxiter = 0;
            vector<Tuple> schedfxns;

            // Skip if no fusion for now...
            if (node->children().size() < 1) {
                return;
            }

            Comp* comp = node->comp();

            // 0) Collect current schedules
            for (const Rel& rel : comp->schedules()) {
                schedfxns.push_back(rel.dest().iterators());
            }
            for (CompNode* child : node->children()) {
                Comp* other = child->comp();
                for (const Rel& rel : other->schedules()) {
                    schedfxns.push_back(rel.dest().iterators());
                }
            }

            // 1) Calculate maximum # of iterators in tuples.
            for (i = 0; i < schedfxns.size(); i++) {
                unsigned niter = schedfxns[i].size();
                maxiter = (niter > maxiter) ? niter : maxiter;
            }

            // 2) Find the deepest common level
            for (n = 0; n < maxiter && level < 1; n++) {
                unsigned ndx = 0;
                if (!schedfxns[0][n].is_int()) {
                    for (i = 1; i < schedfxns.size() && ndx < 1; i++) {
                        if (n < schedfxns[i].size() && schedfxns[i][n] == schedfxns[0][n]) {
                            ndx = i;
                        }
                    }
                }
                if (ndx < 1) {
                    level = n;
                }
            }

            // 3) Ensure same tuple sizes
            for (i = 0; i < schedfxns.size(); i++) {
                if (schedfxns[i].size() < maxiter) {
                    schedfxns[i].insert(schedfxns[i].begin() + level, Iter('0'));
                }
            }

            // 4) Increment latter schedules to ensure lexicographic ordering.
            for (n = level; n < maxiter; n++) {
                for (i = 1; i < schedfxns.size(); i++) {
                    if (schedfxns[i][n].is_int()) {
                        for (j = n; j < maxiter && !schedfxns[0][j].is_int(); j++);
                        if (j > n && j < maxiter) {
                            // Swap iterators...
                            Iter tmp = schedfxns[i-1][j];
                            schedfxns[i-1][j] = schedfxns[i-1][n];
                            schedfxns[i-1][n] = tmp;
                        }
                        if (schedfxns[i-1][n].is_int()) {
                            schedfxns[i][n].name(schedfxns[i-1][n].name()[0] + 1);
                        } else {
                            // TODO: What to do here, if anything?
                        }
                    }
                }
            }

            // 5) Replace schedules with updated tuples...
            n = 0;
            for (i = 0; i < comp->nschedules(); i++) {
                comp->reschedule(i, schedfxns[n]);
                n += 1;
            }
            for (CompNode* child : node->children()) {
                Comp* other = child->comp();
                for (i = 0; i < other->nschedules(); i++) {
                    other->reschedule(i, schedfxns[n]);
                    n += 1;
                }
            }
        }

        void enter(DataNode* node) override {
            // TODO: Do this method need to do any work?
        }

    protected:
        map<string, vector<Access> > _accesses;
    };
}

#endif  // POLYEXT_VISITOR_H
