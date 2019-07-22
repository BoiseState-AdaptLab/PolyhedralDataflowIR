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

#ifndef _VISITOR_H_
#define _VISITOR_H_

#include <stack>
using std::stack;

#include <pdfg/GraphIL.hpp>
#include <util/OS.hpp>
using util::OS;
#include <util/Parser.hpp>
using util::Parser;

namespace pdfg {
    struct DFGVisitor {
    public:
        virtual void visit(Node* node) {
            enter(node);
            exit(node);
        }

        virtual void enter(Node *node) {
            //cerr << "Visiting node '" << node->label() << "'" << endl;
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

        virtual void enter(CompNode*) {}

        virtual void enter(DataNode*) {}

        virtual void exit(Node* node) {
            switch (node->type()) {
                case 'D':
                    exit((DataNode *) node);
                    break;
                case 'C':
                    exit((CompNode *) node);
                    break;
                case 'R':
                    exit((RelNode *) node);
                    break;
                default:    // Throw exception, maybe?
                    break;
            }
        }

        virtual void exit(CompNode*) {}

        virtual void exit(DataNode*) {}

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

    struct ReverseVisitor : public DFGVisitor {
    public:
        virtual void walk(FlowGraph* graph) {
            setup(graph);
            vector<Node*> nodes = graph->nodes();
            for (int i = nodes.size() - 1; i >= 0; i--) {
                visit(nodes[i]);        //node->accept(this);
            }
            finish(graph);
        }
    };

    struct DOTVisitor : public DFGVisitor {
    protected:
        Digraph* _dag;

    public:
        explicit DOTVisitor() {
            _dag = nullptr;
        }

        virtual ~DOTVisitor() {
            delete _dag;
        }

        void setup(FlowGraph* graph) override {
            _graph = graph;
            _dag = new Digraph(_graph->name());
            _dag->node("*", "", {"shape", "none"});
        }

        void enter(DataNode* node) override {
            string root = _dag->root();
            string name = _dag->node(node->label());
            _dag->attr(name, "shape", "rect");

            vector<Edge*> ins = _graph->in_edges(node);
            vector<Edge*> outs = _graph->out_edges(node);

            if (ins.size() < 1) {
                _dag->edge(root, name, _dag->size(root));
            }

            if (ins.size() < 1 || outs.size() < 1) {
                _dag->attr(name, "color", "gray");
            }

            string label, next, prev;
            for (Edge* edge : ins) {
                CompNode* src = (CompNode*) edge->source();
                label = src->label();
                prev = _dag->find(root, label);
                if (prev.empty()) {
                    prev = _dag->node(label);
                }
                _dag->edge(prev, name, _dag->size(prev));
            }

            for (Edge* edge : outs) {
                CompNode* dest = (CompNode*) edge->dest();
                label = dest->label();
                next = _dag->find(root, label);
                if (next.empty()) {
                    next = _dag->node(label);
                    _dag->attr(next, "shape", "invtriangle");
                }
                _dag->edge(name, next, _dag->size(name));
            }
        }

        friend ostream& operator<<(ostream& os, const DOTVisitor& dot) {
            os << dot._dag->to_dot();
            return os;
        }
    };

    struct CodeGenVisitor: public DFGVisitor {
    public:
        explicit CodeGenVisitor(const string &path = "", const string &lang = "C", bool profile = false,
                unsigned niters = 15) : _path(path), _niters(niters), _lang(lang), _profile(profile) {
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
            if (!Strings::in(_mathFunctions, name.substr(0, name.find('(')), true)) {
                _defines.push_back(make_pair<string, string>(string(name), string(defn)));
            }
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

        void add_size(const string& name, unsigned size) {
            _data_sizes[name] = size;
        }

        void setup(FlowGraph* graph) override {
            _graph = graph;
            addComment();
            addIncludes();
            addTypeDefs();
            addFunctions();
        }

        void enter(DataNode* node) override {
            string label = node->label();
            string defval = node->defval();
            unsigned tilesize = _graph->tileSize();

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
                        if (tilesize > 0) {
                            os << "aligned_alloc(" << tilesize << ",(";
                        } else {
                            os << "malloc(";
                        }
                        auto itr = _data_sizes.find(label);
                        if (itr != _data_sizes.end()) {
                            os << itr->second;
                        } else {
                            os << *node->size();
                        }
                        os << "*sizeof(" << node->datatype() << "));";
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
        }

        void enter(CompNode* node) override {
            vector<string> names;
            map<string, vector<string> > guards;
            map<string, vector<string> > statements;
            map<string, vector<string> > schedules;

            updateComp((Comp*) node->expr(), names, statements, guards, schedules);
            addMappings(node);

            for (CompNode* child : node->children()) {
                updateComp((Comp*) child->expr(), names, statements, guards, schedules);
                addMappings(child);
            }

            string code = _poly.codegen(names, statements, guards, schedules, _ompsched, "", true);
            code = "// " + node->label() + "\n" + code;
            _body.push_back(code);
        }

        void enter(RelNode* node) {
            int stop = 4;
        }

        void finish(FlowGraph* graph) override {
            addDefines();
            addHeader();
            addFooter();

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
            include({"stdio", "stdlib", "stdint", "math", "string"});
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

        void addHeader() {
            // Check return type...
            string returnName = _graph->returnName();
            string returnType = _graph->returnType();
            if (!returnName.empty()) {
                DataNode* returnNode = (DataNode*) _graph->get(returnName);
                string returnSize = returnNode->size()->text();
                if (!returnSize.empty() && atoi(returnSize.c_str()) != 1) {
                    returnType += "*";
                }
            }

            string line = returnType + " " + _graph->name() + "(";
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
        }

        void addFooter() {
            string line;
            for (const string& free : _frees) {
                if (free != _graph->returnName()) {
                    line = _indent + "free(" + free + ");";
                    _body.push_back(line);
                }
            }

            if (!_graph->returnName().empty()) {
                line = "\n" + _indent + "return (" + _graph->returnName() + ");";
                _body.push_back(line);      // Return value...
            }

            line = "}" + _indent + "// " + _graph->name();
            _body.push_back(line);      // Close the fxn
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
            for (const auto& itr : _poly.macros()) {
                define(itr);
            }
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

        void addMappings(CompNode* node) {
            // Define data mappings (one per space)
            for (Access* access : node->accesses()) {
                if (access->has_iters() && _mappings.find(access->space()) == _mappings.end()) {
                    string mapping = createMapping(node->comp(), access);
                    if (!mapping.empty()) {
                        string accstr = stringify<Access>(*access);
                        define(accstr, mapping);
                        _mappings[access->space()] = mapping;
                    }
                }
            }
        }

        string createMapping(const Comp* comp, const Access* access) {
            string sname = access->space();
            map<string, int> offsets = access->offset_map();

            Space space = getSpace(sname);
            Tuple tuple = space.iterators();
            unsigned size = tuple.size();

            // TODO: Come back here to resolve interchange issue!
            if (comp->interchanged()) {
                int stop=1;
            }

            ostringstream os;
            if (size < 1) {
                os << sname;
            } else if (tuple.at(0).type() != 'N' && access->refchar() != '[') {
                os << sname << '[';
                if (size > 1) {
                    os << "offset" << size << '(';
                }
                for (unsigned i = 0; i < size; i++) {
                    string iter = tuple.at(i).text();
                    os << '(' << iter << ')';
                    auto offset = offsets.find(iter);
                    if (offset != offsets.end() && offset->second != 0) {
                        if (offset->second > 0) {
                            os << '+';
                        }
                        os << offset->second;
                    }
                    if (size > 1) {
                        os << ',';
                    }
                }
                if (size > 1) {
                    vector<Iter> subs(tuple.begin() + 1, tuple.end());
                    string subsize = stringify<Math>(space.slice(1, size - 1).size());
                    subsize = Strings::fixParens(subsize);   // Remove redundant parens...
                    os << Strings::replace(subsize, "*", ",") << ')';
                }
                auto itr = _data_sizes.find(sname);
                if (itr != _data_sizes.end()) {
                    os << '&' << (itr->second - 1);
                }
                os << ']';
            }

            return os.str();
        }

        bool _profile;
        unsigned _niters;

        const string _mathFunctions = "sqrt|log|log10|exp|pow|sin|cos|tan";

        string _indent;
        string _lang;
        string _path;
        string _ompsched;

        map<string, string> _mappings;
        map<string, unsigned> _data_sizes;

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
            Comp* comp = node->comp();

//            if (node->label() == "smul_d1") {
//                int stop = 1;
//            }

            unsigned nReadsF = 0;
            unsigned nReadsI = 0;
            unsigned nWritesF = 0;
            unsigned nWritesI = 0;

            unsigned nchildren = node->children().size();
            vector<CompNode*> nodes(nchildren + 1);
            unsigned i = 0;
            nodes[i++] = node;
            for (CompNode* child : node->children()) {
                nodes[i++] = child;
            }

            for (i = 0; i < nodes.size(); i++) {
                node = nodes[i];
                map<string, Access *> reads = node->reads();
                for (const auto &itr : reads) {
                    Access *access = itr.second;
                    DataNode *source = (DataNode *) _graph->get(access->space());
                    if (source->is_int()) {
                        nReadsI += 1;
                        if (!source->is_scalar()) {
                            inStreamsI += 1;
                        }
                    } else {
                        nReadsF += 1;
                        if (!source->is_scalar()) {
                            inStreamsF += 1;
                        }
                    }
                }

                map<string, Access *> writes = node->writes();
                for (const auto &itr : writes) {
                    Access *access = itr.second;
                    DataNode *dest = (DataNode *) _graph->get(access->space());
                    if (dest->is_int()) {
                        nWritesI += 1;
                        if (!dest->is_scalar()) {
                            outStreamsI += 1;
                        }
                    } else {
                        nWritesF += 1;
                        if (!dest->is_scalar()) {
                            outStreamsF += 1;
                        }
                    }
                }
            }

            string intSize = to_string(sizeof(int));
            string floatSize = to_string(sizeof(double));
            string isSizeExpr = Strings::fixParens(comp->space().size().text());

            string inSizeExprF = to_string(nReadsF) + "*" + floatSize + "*(" + isSizeExpr + ")";
            string inSizeExprI = to_string(nReadsI) + "*" + intSize + "*(" + isSizeExpr + ")";
            string outSizeExprF = to_string(nWritesF) + "*" + floatSize + "*(" + isSizeExpr + ")";
            string outSizeExprI = to_string(nWritesI) + "*" + intSize + "*(" + isSizeExpr + ")";

            node = nodes[0];
            nIOPs = 1;                  // TODO: count int ops later (maybe), but assume at least one
            nFLOPs = node->flops();     // Count FLOPs...
            string flopsExpr = to_string(nFLOPs) + "*(" + isSizeExpr + ")";
            string iopsExpr = to_string(nIOPs) + "*(" + isSizeExpr + ")";

            node->attr("isize_in", inSizeExprI);
            node->attr("istreams_in", to_string(inStreamsI));
            node->attr("fsize_in", inSizeExprF);
            node->attr("fstreams_in", to_string(inStreamsF));

            node->attr("isize_out", outSizeExprI);
            node->attr("istreams_out", to_string(outStreamsI));
            node->attr("fsize_out", outSizeExprF);
            node->attr("fstreams_out", to_string(outStreamsF));

            node->attr("flops", flopsExpr);
            node->attr("iops", iopsExpr);
        }
    };

    struct ScheduleVisitor : public DFGVisitor {
    public:
        explicit ScheduleVisitor(Digraph* itergraph = nullptr) : _itergraph(itergraph) {
        }

        void walk(FlowGraph* graph) override {
            // Assign iteration graph if present...
            if (_itergraph) {
                CompNode* node =  graph->comp_nodes()[0];
                node->iter_graph(_itergraph);
            }
            DFGVisitor::walk(graph);
        }

        void enter(CompNode* node) override {
            Digraph* igraph = node->iter_graph();
            if (igraph) {
                Tuple tuple;
                map<string, Tuple> schedules;

                cerr << igraph->to_dot() << endl;

                // 1) Traverse the iteration graph
                visit(igraph, igraph->root(), tuple, schedules);

                // 2) Replace schedules with updated tuples...
                node->schedules(schedules);

                // 3) Apply tiling if applicable...
                applyTiling(node);

                // 4) Update node attributes
                if (!_shifts.empty()) {
                    node->attr("shifts", "[(" + Strings::join(_shifts, "),(") + ")]");
                }
                if (!_tile_sizes.empty()) {
                    node->attr("tiles", Strings::str<unsigned>(_tile_sizes));
                }
            }
        }

        /// For each computation node, generate and assign a scheduling (scattering) function.
        /// \param node Computation node
//        void enter(CompNode* node) override {
//            unsigned i, j, n;
//            unsigned level = 0;
//            unsigned maxiter = 0;
//            vector<int> offsets;
//
//            // Skip if no fusion for now...
//            if (node->children().size() < 1) {
//                return;
//            }
//
//            Comp* comp = node->comp();
//
//            // 0) Collect current schedules and offsets
//            _schedules = node->schedules();
//
//            // 1) Calculate maximum # of iterators in tuples.
//            unsigned nschedules = _schedules.size();
//            for (i = 0; i < nschedules; i++) {
//                unsigned niter = _schedules[i].size();
//                maxiter = (niter > maxiter) ? niter : maxiter;
//            }
//
//            // 2) Determine fusion type and calculate maximum offset from each iterator.
//            char fuseType = 'S';        // Simple loop-only fusion if no data dependences
//            CompNode* prev = node;
//            vector<CompNode*> nodes(nschedules, nullptr);
//            nodes[0] = node;
//
//            i = 1;
//            for (CompNode* child : node->children()) {
////                map<string, Access*> isect = intersect(prev->writes(), child->reads());
////                if (!isect.empty()) {   // P-C Fuse
//////                    maxOffsets(_schedules[n], prev->writes(), offsets);
//////                    maxOffsets(_schedules[n], child->reads(), offsets);
////                    fuseType = 'P';
////                } else {
////                    isect = intersect(prev->reads(), child->reads());
////                    if (!isect.empty()) {   // R-R Fuse
////                        // This might be a don't-care, these are just reads so order should not matter.
//////                        maxOffsets(_schedules[n], prev->reads(), offsets);
//////                        maxOffsets(_schedules[n], child->reads(), offsets);
////                        fuseType = 'R';
////                    } else {
////                        isect = intersect(prev->writes(), child->writes());
////                        if (!isect.empty()) {   // W-W Fuse
////                            // Potential WAW hazard -- how to handle this case? Error?
//////                            maxOffsets(_schedules[n], prev->writes(), offsets);
//////                            maxOffsets(_schedules[n], child->reads(), offsets);
////                            fuseType = 'W';
////                        }
////                    }
////                }
////                prev = child;
//                nodes[i++] = child;
//            }
//
//            vector<Tuple> newfxns = _schedules;
//
//            // 3) Find the deepest common level
//            int index = -1;
//            level = getCommonLevel(newfxns, &index);
//
//            // Handle case w/o common iterators.
//            Iter first;
//            bool interchange = (level < 1);
//            if (interchange) {
//                if (newfxns[level].size() > newfxns[index].size()) {
//                    index = level;
//                }
//                first = newfxns[index][0];
//                newfxns[index].erase(newfxns[index].begin());
//                level = getCommonLevel(newfxns);
//            }
//
//            for (i = 0; i < nschedules; i++) {
//                if (!newfxns[i][level].is_int()) {
//                    newfxns[i].insert(newfxns[i].begin() + level, Iter(i + '0'));
//                    if (newfxns[i].size() > maxiter) {
//                        newfxns[i].pop_back();
//                    }
//                }
//            }
//
//            // 4) Handle nonzero offsets...
//            unsigned group = 0;
//            vector<bool> shifted(maxiter, false);
//            for (i = 1; i < nschedules; i++) {
//                // Check whether offset exists at this level
//                map<string, Access*> isect = intersect(nodes[i-1]->writes(), nodes[i]->reads());
//                if (!isect.empty()) {
//                    maxOffsets(_schedules[i], nodes[i-1]->writes(), offsets);
//                    maxOffsets(_schedules[i], nodes[i]->reads(), offsets);
//                }
//                for (n = 0; n < offsets.size(); n++) {
//                    if (offsets[n] != 0) {
//                        if (!shifted[n]) {
//                            // This means insert a tuple here! Or perhaps it means a shift,
//                            //   need to determine when shifts are possible.
//                            for (j = 0; j < nschedules; j++) {
//                                newfxns[j].insert(newfxns[j].begin() + n, Iter(group + '0'));
//                            }
//                            group += 1;
//                            maxiter += 1;
//                            shifted[n] = true;
//                        }
//                        // If dimension is already shifted, just increment the group.
//                        for (j = i; j < nschedules; j++) {
//                            newfxns[j][n] = Iter(group + '0');
//                        }
//                        group += 1;
//                        offsets[n] = 0;
//                    }
//                }
//            }
//
//            // 5) Increment last tuple item to ensure lexicographic ordering.
//            // TODO: Ideally, the last tuple should be in group order, but the generated code should be the same.
//            for (unsigned i = 0; i < nschedules; i++) {
//                unsigned last = newfxns[i].size() - 1;
//                if (newfxns[i][last].is_int()) {
//                    newfxns[i][last] = Iter(i + '0');
//                }
//            }
//
//            if (interchange) {
//                newfxns[index].push_back(first);
//            }
//
//            // 6) Replace schedules with updated tuples...
//            n = 0;
//            for (i = 0; i < comp->nschedules(); i++) {
//                comp->reschedule(i, newfxns[n]);
//                n += 1;
//            }
//            for (CompNode* child : node->children()) {
//                Comp* other = child->comp();
//                for (i = 0; i < other->nschedules(); i++) {
//                    other->reschedule(i, newfxns[n]);
//                    n += 1;
//                }
//            }
//        }

    protected:
        void visit(Digraph* graph, const string& node, Tuple& tuple, map<string, Tuple>& schedules) {
            string label = graph->label(node);
            tuple.push_back(Iter(label));
            vector<Pair> edges = graph->edges(node);

            if (edges.size() < 1) {     // Leaf node
                Iter stmt = tuple.back();
                tuple.pop_back();
                Tuple schedule(tuple.begin() + 1, tuple.end());

                // Check for shift...
                string shift = graph->attr(node, "shift");
                if (!shift.empty()) {
                    applyShift(schedule, shift);
                }

                cerr << stmt << " := " << schedule << endl;
                //schedules.push_back(schedule);
                schedules[stmt.text()] = schedule;
            } else {                    // Visit children
                // Check for tiling...
                string tile_iter = graph->attr(node, "tile_iter");
                if (!tile_iter.empty() && !isTileIter(tile_iter)) {
                    _tile_iters.push_back(tile_iter);
                    _orig_iters.push_back(graph->attr(node, "orig_iter"));
                    _rem_iters.push_back(graph->attr(node, "rem_iter"));
                    _tile_sizes.push_back(atoi(graph->attr(node, "tile_size").c_str()));
                }

                for (Pair& edge : edges) {
                    label = edge.second;
                    tuple.push_back(Iter(label));
                    visit(graph, edge.first, tuple, schedules);
                    tuple.pop_back();
                }
                tuple.pop_back();
            }
        }

        void applyShift(Tuple& tuple, const string& shift) {
            vector<string> shifts = Strings::split(shift, ',');
            unsigned ndx = 0;
            for (Iter& iter : tuple) {
                if (!iter.empty() && !iter.is_int() && !isTileIter(iter.name())) {
                    int shift_val = atoi(shifts[ndx].c_str());
                    if (shift_val < 0) {
                        iter.text(iter.text() + "-" + to_string(shift_val));
                    } else if (shift_val > 0) {
                        iter.text(iter.text() + "+" + to_string(shift_val));
                    }
                    ndx += 1;
                }
            }
            _shifts.push_back(shift);
        }

        void applyTiling(CompNode* node) {
            Comp* comp = node->comp();
            for (Rel& schedule : comp->schedules()) {
                Space dest = schedule.dest();
                vector<Constr> constraints;

                // Build tiling constraints...
                for (unsigned i = 0; i < _tile_iters.size(); i++) {
                    Int tile_size(_tile_sizes[i]);
                    Iter rem_iter(_rem_iters[i]);

                    Iter orig_iter, tile_iter;
                    for (Iter& iter : dest.iterators()) {
                        if (iter.name() == _tile_iters[i]) {
                            tile_iter = iter;
                        } else if (iter.name() == _orig_iters[i]) {
                            orig_iter = iter;
                            break;
                        }
                    }

                    auto constrs = exists(0 <= rem_iter < tile_size ^ orig_iter==tile_iter*tile_size+rem_iter);
                    constraints.insert(constraints.end(), constrs.begin(), constrs.end());
                }

                dest.constraints(constraints);
                schedule.dest(dest);
            }

            for (CompNode* child : node->children()) {
                applyTiling(child);
            }
        }

        bool isTileIter(const string& iter) {
            return find(_tile_iters.begin(), _tile_iters.end(), iter) != _tile_iters.end();
        }

//        map<string, Access*> intersect(const map<string, Access*> lhs, const map<string, Access*> rhs) const {
//            map<string, Access*> isect;
//            for (const auto& itr : lhs) {
//                auto pos = rhs.find(itr.first);
//                if (pos != rhs.end()) {
//                    isect[pos->first] = pos->second;
//                }
//            }
//            return isect;
//        }

//        map<string, ExprTuple> absMaxDist(const map<string, Access*> accmap) const {
//            map<string, ExprTuple> distmap;
//            map<string, ExprTuple> maxmap;
//
//            // Initialize distances to zero...
//            ExprTuple zeros;
//            for (const auto& itr : accmap) {
//                Access* acc = itr.second;
//                string space = acc->space();
//                if (distmap.find(space) == distmap.end()) {
//                    zeros = ExprTuple(acc->tuple().size(), Int(0));
//                    distmap[space] = zeros;
//                    ExprTuple tmax(zeros.begin(), zeros.end());
//                    maxmap[space] = tmax;
//                }
//            }
//
//            // Find max delta for each tuple.
//            for (const auto& itr : accmap) {
//                Access* acc = itr.second;
//                string space = acc->space();
//                ExprTuple tuple = acc->tuple();
//                ExprTuple tmax = maxmap[space];
//
//                ExprTuple diff = abs(tuple - tmax);
//                if (distmap[space] < diff) {
//                    distmap[space] = diff;
//                    maxmap[space] = tuple;
//                }
//            }
//
//            return distmap;
//        }

//        unsigned getCommonLevel(const vector<Tuple>& tuples, int* ndx = nullptr) {
//            unsigned i, n, level = 0;
//            for (n = 0; n < tuples[0].size(); n++) {
//                if (!tuples[0][n].is_int()) {
//                    bool match = true;
//                    for (i = 1; i < tuples.size() && match; i++) {
//                        if (tuples[i].size() < n || !tuples[i][n].equals(tuples[0][n])) {
//                            match = false;
//                            if (ndx != nullptr) {
//                                *ndx = i;
//                            }
//                        }
//                    }
//                    if (match) {
//                        level = n + 1;
//                    }
//                }
//            }
//            return level;
//        }

        Digraph* _itergraph;

        vector<string> _shifts;
        vector<string> _tile_iters;
        vector<string> _rem_iters;
        vector<string> _orig_iters;
        vector<unsigned> _tile_sizes;
    };

    struct DataReduceVisitor : public DFGVisitor {
    public:
        explicit DataReduceVisitor() {}

        void enter(DataNode* node) override {
            vector<Edge*> ins = _graph->in_edges(node);
            vector<Edge*> outs = _graph->out_edges(node);

            // A node with no incoming edges is an input, and no outgoing is an output, these cannot be reduced.
            unsigned size = ins.size();
            vector<CompNode*> prodNodes(size);
            bool reducible = (size > 0 && size == outs.size());

            if (reducible) {
                for (unsigned i = 0; i < size && reducible; i++) {
                    prodNodes[i] = (CompNode*) ins[i]->source();
                    CompNode* consumer = (CompNode*) outs[i]->dest();
                    reducible = (prodNodes[i]->label() == consumer->label());
                }
            }

            if (reducible) {
                Access nodeAcc = Access::from_str(node->expr()->text());
                IntTuple intTuple = to_int(nodeAcc.tuple());

                //for (CompNode* producer : prodNodes) {
                CompNode* producer = prodNodes[0];
                vector<Access*> accesses = producer->accesses(node->label());
                IntTuple maxTuple(intTuple.size(), 0);
                for (unsigned i = 0; i < accesses.size(); i++) {
                    IntTuple accTuple = to_int(accesses[i]->tuple());
                    maxTuple = absmax(maxTuple, accTuple);
                }

                // Update the data space...
                Space space = getSpace(node->label());
                Tuple iters = space.iterators();
                ConstrTuple constrs = space.constraints();
                Space newspace(space.name());

                int tupleSum = accumulate(maxTuple.begin(), maxTuple.end(), 0);
                if (tupleSum != 0) {
                    unsigned i = 0;
                    for (; i < maxTuple.size() && maxTuple[i] == 0; i++);
                    if (i == 0 && maxTuple[i+1] == 0) {
                        addIter(iters[0], space.constraints(), newspace);
                    } else {
                        for (; i < maxTuple.size(); i++) {
                            addIter(iters[i], space.constraints(), newspace);
                        }
                    }
                }

                newSpace(newspace);
                Math* resize = (Math*) getSize(*producer->comp(), Func(newspace));
                node->size(resize);
                //}
            }
        }

    protected:
        void addIter(const Iter& iter, const vector<Constr>& constraints, Space& newspace) {
            newspace.add(iter);
            for (const Constr &constr : constraints) {
                if (constr.lhs().equals(iter) || constr.rhs().equals(iter)) {
                    newspace.add(constr);
                }
            }
        }
    };

    struct MemTableEntry {
        unsigned size;
        unsigned prev_size;
        bool active;
        bool resized;
    };

    bool MemTableCommpare(MemTableEntry& lhs, MemTableEntry& rhs) {
        return (lhs.size < rhs.size);
    }

    struct MemAllocVisitor : public ReverseVisitor {
    protected:
        map<string, Const> _constants;
        map<string, unsigned> _space_map;
        vector<MemTableEntry> _entries;
        unordered_map<string, vector<string> > _producers;
        unordered_map<string, bool> _visited;

        string replaceConsts(const string& expr) const {
            string result = expr;
            for (const auto& iter : _constants) {
                result = Strings::replace(result, iter.first, to_string(iter.second.val()), true);
            }
            return result;
        }

        unsigned find(const unsigned& size) {
            // 1) 1st pass: search for a (non-live) entry of exact size.
            unsigned index = 0;
            for (MemTableEntry& entry : _entries) {
                if (!entry.active) {
                    if (entry.size == size) {
                        entry.active = true;
                        cerr << "MemAllocVisitor: using entry " << index << " of size " << entry.size << endl;
                        return index;
                    }
                }
                index += 1;
            }

            // 2) 2nd pass: find a (non-live) entry of greater size.
            index = 0;
            unsigned minIndex, minDiff = INT_MAX;
            for (MemTableEntry& entry : _entries) {
                if (!entry.active) {
                    if (entry.size > size) {
                        entry.active = true;
                        cerr << "MemAllocVisitor: using entry " << index << " of size " << entry.size << endl;
                        return index;
                    }
                    int diff = size - entry.size;
                    if (diff < minDiff) {
                        minDiff = diff;
                        minIndex = index;
                    }
                }
                index += 1;
            }

            // 3) Resize an existing entry if possible (excluding scalars).
            index = minIndex;
            if (minDiff < INT_MAX && _entries[index].size > 1) {
                _entries[index].active = _entries[index].resized = true;
                _entries[index].prev_size = _entries[index].size;
                _entries[index].size = size;
                cerr << "MemAllocVisitor: resizing entry " << index << " from size " << _entries[index].prev_size
                     << " to " << size << endl;
                return index;
            }

            // 4) Create a new entry
            MemTableEntry entry{size, 0, true, false};
            index = _entries.size();
            _entries.push_back(entry);
            cerr << "MemAllocVisitor: added entry " << index << " of size " << size << endl;

            return index;
        }

        void process(DataNode* node) {
            bool isTemp = _graph->isTemp(node);
            //cerr << "MemAllocVisitor: Node '" << node->label() << "' " << (isTemp ? "IS" : "NOT") << " temporary.\n";
            if (isTemp) {
                // Get access and space from data node.
                Access access = Access::from_str(node->expr()->text());
                Space space = getSpace(access.space());
                if (_space_map.find(space.name()) == _space_map.end()) {
                    string expr = Strings::fixParens(space.size().text());

                    // Calculate the size from the size expression.
                    unsigned size = 1;
                    if (!expr.empty()) {
                        size = Parser().eval(replaceConsts(expr));
                    }

                    // Find a space of the proper size or get a new onee.
                    unsigned index = find(size);
                    cerr << "MemAllocVisitor: assigned entry " << index << " to '"
                         << space.name() << "' (" << expr << ")\n";
                    _space_map[space.name()] = index;

                    node->attr("mem_table_entry", to_string(index));
                    node->attr("mem_table_size", to_string(_entries[index].size));
                }
            }
        }

    public:
        explicit MemAllocVisitor(const map<string, Const>& constants) : _constants(constants) {
        }

        map<string, Const> constants() const {
            return _constants;
        }

        map<string, unsigned> spaces() const {
            return _space_map;
        }

        vector<MemTableEntry> entries() const {
            return _entries;
        }

        void enter(CompNode* node) override {
            cerr << "MemAllocVisitor: enter '" << node->label() << "'\n";
            for (Edge* in : _graph->in_edges(node)) {
                DataNode* input = (DataNode*) in->source();
                for (Edge* prod :  _graph->in_edges(input)) {
                    _producers[input->label()].push_back(prod->source()->label());
                }
            }

            for (Edge* out : _graph->out_edges(node)) {
                process((DataNode *) out->dest());
            }

            for (Edge* in : _graph->in_edges(node)) {
                process((DataNode*) in->source());
            }
        }

        virtual void exit(CompNode* node) {
            // Mark entries produced by this node as inactive when leaving comp node...
            for (Edge* edge : _graph->out_edges(node)) {
                DataNode* output = (DataNode*) edge->dest();
                string space = output->label();

                auto iter = _space_map.find(space);
                if (iter != _space_map.end()) {
                    bool active = false;
                    vector<string> prod_names = _producers[output->label()];
                    for (unsigned i = 0; i < prod_names.size() && !active; i++) {
                        active = (prod_names[i] != node->label() && !_visited[prod_names[i]]);
                    }
                    if (!active) {
                        unsigned index = iter->second;
                        cerr << "MemAllocVisitor: marked entry " << index << " as inactive for space '" << space << "'\n";
                        _entries[index].active = active;
                    }
                }
            }
            _visited[node->label()] = true;
        }

        friend ostream& operator<<(ostream& os, const MemAllocVisitor& alloc) {
            os << "Entry\tSize\tAlive\tResized\n";
            unsigned index = 0;
            for (const MemTableEntry& entry : alloc._entries) {
                os << index << "\t" << entry.size << "\t" << (entry.active ? 1 : 0)
                   << "\t" << (entry.resized ? 1 : 0) << "\n";
                index++;
            }
            os << "\n";

            os << "Space\tEntry\n";
            for (const auto& iter : alloc._space_map) {
                os << iter.first << "\t" << iter.second << "\n";
            }

            return os;
        }
    };
}

#endif  // _VISITOR_H_
