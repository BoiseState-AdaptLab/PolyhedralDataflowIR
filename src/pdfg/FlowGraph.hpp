/**
 * \class FlowGraph
 *
 * \ingroup PolyExt
 * (Note, this needs exactly one \defgroup somewhere)
 *
 * \brief Polyhedral Dataflow Graph class
 *
 * C++ implementation of polyhedral dataflow graphs (PDFGs).
 *
 * \author Eddie Davis
 * \version $Revision: 0.1 $
 * \date $Date: 2018/12/03
 * Contact: eddiedavis@boisestate.edu
 */

#ifndef POLYEXT_FLOWGRAPH_H
#define POLYEXT_FLOWGRAPH_H

#include <algorithm>
using std::distance;
using std::find;
#include <iostream>
using std::ostream;
using std::endl;
#include <fstream>
using std::ofstream;
#include <map>
using std::map;
//#include <memory>
//using std::unique_ptr;
#include <numeric>
using std::accumulate;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <initializer_list>
using std::initializer_list;
#include <utility>
using std::make_pair;
using std::pair;
// C Includes:
#include <ctype.h>
#include <unistd.h>
// My Includes:
#include <pdfg/Digraph.hpp>
#include <util/Strings.hpp>

namespace pdfg {
    enum MemAlloc {NONE, AUTO, STATIC, DYNAMIC, STRUCT};

    struct Expr;
    ostream& operator<<(ostream& os, const Expr& expr);

    struct Node {
    public:
        //explicit Node(const string& space, const string& label = "") {
        explicit Node(Expr* expr, const string& label = "", initializer_list<string> attrs={}) {
            init(expr, label, attrs);
        }

        virtual ~Node() {
            delete _expr;
        }

        string attr(const string& key) const {
            string val;
            auto iter = _attrs.find(key);
            if (iter != _attrs.end()) {
                val = iter->second;
            }
            return val;
        }

        void attr(const string& key, const string& val) {
            _attrs[key] = val;
        }

        const string& label() const {
            return _label;
        }

        Expr* expr() const {
            return _expr;
        }

        void label(const string& label) {
            size_t pos = label.find('(');
            if (pos == string::npos) {
                pos = label.find('[');
            }
            if (pos != string::npos) {
                _label = label.substr(0, pos);
            } else {
                _label = label;
            }
        }

        void expr(Expr* expr) {
            _expr = expr;
        }

        const char type() const {
            return _type;
        }

        friend ostream& operator<<(ostream& os, Node& node) {
            ostringstream ss;
            ss << *(node.expr());
            os << "{ \"label\": \"" << node._label << "\", \"space\": \""
               << Strings::replace(ss.str(), "\\\n", " ") << "\"";
            if (node._attrs.size() > 2) {
                os << ", \"attrs\": { ";
                unsigned n = 0, nattrs = node._attrs.size();
                for (const auto& iter : node._attrs) {
                    //if (iter.first != "shape" && iter.first != "color") {
                    os << "\"" << iter.first << "\": \"" << iter.second << "\"";
                    if (n < nattrs - 1) {
                        os << ", ";
                    }
                    n += 1;
                    //}
                }
                os << " }";
            }
            os << " }";
            return os;
        }

        bool is_comp() {
            return _type == 'C';
        }

        bool is_data() {
            return _type == 'D';
        }

    protected:
        void init(Expr* expr, const string& label = "", initializer_list<string> attrs={}) {
            _expr = expr;
            this->label(label);
            if (attrs.size() > 0) {
                unsigned n = 0;
                string key;
                for (const string &attr : attrs) {
                    string val;
                    if (n % 2 == 0) {
                        key = attr;
                    } else {
                        _attrs[key] = attr;
                    }
                    n += 1;
                }
            } else {
                _attrs["color"] = "white";
                _attrs["shape"] = "";
            }
            _type = 'N';
        }

        Expr* _expr;
        string _label;
        char _type;
        map<string, string> _attrs;
    };

    struct Edge {
    public:
        explicit Edge(Node* source, Node* dest, const string& label = "") :
                _source(source), _dest(dest), _label(label) {
        }

        Node* source() const {
            return _source;
        }

        Node* dest() const {
            return _dest;
        }

        const string& label() const {
            return _label;
        }

        void source(Node* source) {
            _source = source;
        }

        void dest(Node* dest) {
            _dest = dest;
        }

        void label(const string& label) {
            _label = label;
        }

        friend ostream& operator<<(ostream& out, Edge& edge) {
            out << "{ ";
            if (!edge._label.empty()) {
                out << "\"label\": \"" << edge._label << "\", ";
            }
            out << "\"src\": \"" << edge._source->label()  << "\", \"dest\": \"" << edge._dest->label() << "\" }";
            return out;
        }

    protected:
        Node* _source;
        Node* _dest;
        string _label;
    };

    struct DataNode : public Node {
    public:
        explicit DataNode(Expr* access, const string& label = "", Expr* size = nullptr, const string& type = "real",
                          const string& defval = "", const MemAlloc& alloc = DYNAMIC) :
            Node(new Expr(*access), label), _size(size), _datatype(type), _defval(defval), _alloc(alloc) {
            attr("shape", "box");
            attr("color", "gray");
            _type = 'D';
        }

        ~DataNode() {
            delete _size;
        }

        Expr* size() const {
            return _size;
        }

        void size(Expr* size) {
            delete _size;
            _size = size;
        }

        string datatype() const {
            return _datatype;
        }

        void datatype(const string& type) {
            _datatype = type;
        }

        bool is_float() const {
            return (_datatype[0] == 'd' || _datatype[0] == 'f');
        }

        bool is_int() const {
            return (_datatype[0] == 'i' || _datatype[0] == 'u');
        }

        unsigned typesize() const {
            char dtype = _datatype[0];
            unsigned dsize = 1;
            switch (dtype) {
                case 'i':
                case 'u':
                    dsize = sizeof(int);
                    break;
                case 'f':
                    dsize = sizeof(float);
                    break;
                case 'd':
                    dsize = sizeof(double);
                    break;
            }
            return dsize;
        }

        string defval() const {
            return _defval;
        }

        void defval(const string& val) {
            _defval = val;
        }

        MemAlloc alloc() const {
            return _alloc;
        }

        void alloc(const MemAlloc& alloc) {
            _alloc = alloc;
        }

        bool is_scalar() const {
            ostringstream os;
            os << *_size;
            return Strings::isNumber(os.str());
        }

        friend ostream& operator<<(ostream& out, DataNode& node) {
            out << "{ \"label\": \"" << node._label << "\", ";
            out << "\"size\": " << node._size << ", ";
            out << "\"defval\": " << node._defval << ", ";
            out << "\"type\": \"" << node._datatype << "\" }";
            return out;
        }

    protected:
        Expr* _size;
        string _datatype;
        string _defval;
        MemAlloc _alloc;
    };

    struct CompNode : public Node {
    public:
        explicit CompNode(Comp* comp, const string& label = "") : Node(new Comp(*comp), label) {
            attr("shape", "invtriangle");
            _type = 'C';
            _igraph = nullptr;
            _shifts = nullptr;
        }

        ~CompNode() {
            for (auto& read : _reads) {
                delete read.second;
            }
            for (auto& write : _writes) {
                delete write.second;
            }
            delete _igraph;
            delete _shifts;
        }

        Comp* comp() const {
            return (Comp*) _expr;
        }

        void comp(Comp* comp) {
            delete _expr;
            _expr = comp;
        }

        bool is_parent() const {
            return !_children.empty();
        }

        CompNode* find_child(const string& pattern) {
            // Look for exact match first...
            for (int i = _children.size() - 1; i >= 0; i--) {
                CompNode* child = _children[i];
                if (child->label() == pattern) {
                    return child;
                }
                // Try partial match...
                for (Expr statement : child->comp()->statements()) {
                    if (Strings::in(statement.text(), pattern, true)) {
                        return child;
                    }
                }
            }
            return nullptr;
        }

        Digraph* iter_graph() {
            if (_igraph == nullptr) {
                _igraph = new Digraph(this->label());
                string inode = _igraph->node("*", "", {"shape", "none"});
                IntTuple path;
                inode = _igraph->insertSchedule(Strings::convert<Iter>(this->schedules()[0]), path);    // Add first node (this one)
                _igraph->insertLeaf(inode, this->label(), path);                // Add leaf node (the statement)
            }
            return _igraph;
        }

        void iter_graph(Digraph* graph) {
            delete _igraph;
            _igraph = graph;
        }

        string first_label() const {
            string label = _label;
            size_t pos = label.find('+');
            if (pos != string::npos) {
                label = label.substr(0, pos);
            }
            return label;
        }

        vector<Tuple> schedules() const {
            Comp* comp = this->comp();
            vector<Tuple> tuples;
            for (const Rel& rel : comp->schedules()) {
                Tuple sched = rel.dest().iterators();
                tuples.push_back(sched);
            }
            for (CompNode* child : this->children()) {
                Comp* other = child->comp();
                for (const Rel& rel : other->schedules()) {
                    Tuple sched = rel.dest().iterators();
                    tuples.push_back(sched);
                }
            }
            return tuples;
        }

        void schedules(const map<string, Tuple>& sched_map) {
            string label = this->first_label();
            Comp* comp = this->comp();
            auto iter = sched_map.find(label);
            if (iter != sched_map.end()) {
                comp->reschedule(0, iter->second);
            }

            for (CompNode* child : this->children()) {
                label = child->label();
                comp = child->comp();
                iter = sched_map.find(label);
                if (iter != sched_map.end()) {
                    comp->reschedule(0, iter->second);
                }
            }
        }

        void schedules(const vector<Tuple>& tuples) {
            unsigned i, n = 0;
            Comp* comp = this->comp();
            for (i = 0; i < comp->nschedules(); i++) {
                comp->reschedule(i, tuples[n]);
                n += 1;
            }
            for (CompNode* child : this->children()) {
                Comp* other = child->comp();
                for (i = 0; i < other->nschedules(); i++) {
                    other->reschedule(i, tuples[n]);
                    n += 1;
                }
            }
        }

        vector<Access*> accesses(const string& space = "") const {
            vector<Access*> accesses;
            for (auto& read : _reads) {
                if (space.empty() || read.first.find(space + read.second->refchar()) == 0) {
                    accesses.push_back(read.second);
                }
            }
            for (auto& write : _writes) {
                if (space.empty() || write.first.find(space + write.second->refchar()) == 0) {
                    accesses.push_back(write.second);
                }
            }
            for (CompNode* child : _children) {
                vector<Access*> childAccs = child->accesses(space);
                accesses.insert(accesses.end(), childAccs.begin(), childAccs.end());
            }
            return accesses;
        }

        map<string, Access*> reads() const {
            return _reads;
        }

        map<string, Access*> writes() const {
            return _writes;
        }

        IntTuple* shifts() const {
            return _shifts;
        }

        void shifts(const IntTuple& shifts) {
            delete _shifts;
            _shifts = new IntTuple(shifts);
        }

        void add_read(const Access& read) {
            string key = read.text();
            auto itr = _reads.find(key);
            if (itr == _reads.end()) {
                _reads[key] = new Access(read);
            }
        }

        void add_write(const Access& write) {
            string key = write.text();
            auto itr = _writes.find(key);
            if (itr == _writes.end()) {
                _writes[key] = new Access(write);
            }
        }

        vector<CompNode*> children() const {
            return _children;
        }

        void fuse(CompNode* other) {
            this->label(this->label() + "+" + other->label());
            _children.push_back(other);
        }

        unsigned flops() const {
            unsigned nflops = 0;
            vector<string> funcs({"exp", "log", "sqrt", "sin", "cos", "tan"});

            Comp* comp = (Comp*) _expr;
            for (const auto& stmt : comp->statements()) {
                string text = stmt.text();
                for (char ch : text) {
                    if (ch == '^' || ch == '+' || ch == '-' || ch =='*' || ch == '/' || ch == '%') {
                        nflops += 1;
                    }
                }

                for (const string& func : funcs) {
                    size_t pos = text.find(func);
                    while (pos != string::npos) {
                        nflops += 10;                           // Add 10 flops for each transitive fxn
                        pos = text.find(func, pos + func.size());
                    }
                }
            }

            for (CompNode* child : _children) {
                nflops += child->flops();
            }

            return nflops;
        }

        void tile(initializer_list<string> iters, initializer_list<unsigned> sizes, initializer_list<string> t_iters = {}) {
            Digraph* ig = this->iter_graph();
            vector<Tuple> schedules = this->schedules();
            vector<string> tile_iters(t_iters.begin(), t_iters.end());
            vector<unsigned> tile_sizes(sizes.begin(), sizes.end());

            string tnode;
            unsigned inum = 0;
            for (const string& iter : iters) {
                if (tile_iters.size() <= inum) {
                    tile_iters.push_back("t" + iter);
                }
                IntTuple path;
                string node = ig->find(iter, path);
                if (!node.empty()) {
                    string tile_iter = tile_iters[inum];        // Tile iterator
                    string rem_iter = "r" + iter;               // Remainder iterator
                    if (tnode.empty()) {
                        tnode = node;
                    }
                    ig->insert(tnode, tile_iter, tile_iter, {"tile_iter", tile_iter, "rem_iter", rem_iter,
                        "orig_iter", iter, "tile_size", to_string(tile_sizes[inum])});
                }
                inum += 1;
            }
            this->comp()->tiled(!tnode.empty());
        }

    protected:
        vector<CompNode*> _children;
        map<string, Access*> _reads;
        map<string, Access*> _writes;
        IntTuple* _shifts;
        Digraph* _igraph;
    };

    struct RelNode : public Node {
    public:
        explicit RelNode(Expr* rel, const string& label = "") : Node(rel, label) { //}, _rel(rel) {
            _type = 'R';
        }

        Rel* relation() const {
            return (Rel*) _expr;
        }

        void relation(Rel* rel) {
            _expr = rel;
        }
    };

    struct FlowGraph {
    public:
        explicit FlowGraph(const string& name = "", const string& retType = "void", const string& retName = "",
                           const string& defVal = "", unsigned tileSize = 0, bool ignoreCycles = true) :
                           _name(name), _returnType(retType), _returnName(retName), _indexType("unsigned"),
                           _defaultVal(defVal), _tileSize(tileSize), _ignoreCycles(ignoreCycles) {
            _nodes.reserve(100);
            _edges.reserve(100);
            _alignIters = false;
        }

        virtual ~FlowGraph() {
            for (Node* node : _nodes) {
                delete node;
            }
            for (Edge* edge : _edges) {
                delete edge;
            }
        }

        friend ostream& operator<<(ostream& os, FlowGraph& graph) {
            os << "{ \"name\": \"" << graph._name << "\", \"tilesize\": " << graph._tileSize << ", \"nodes\": [\n";
            unsigned n = 0, size = graph._nodes.size();
            for (Node* node : graph._nodes) {
                os << *node;
                if (n < size - 1) {
                    os << ',';
                }
                os << "\n";
                n++;
            }
            os << "], \"edges\": [\n";
            n = 0, size = graph._edges.size();
            for (Edge* edge : graph._edges) {
                os << *edge;
                if (n < size - 1) {
                    os << ',';
                }
                os << "\n";
                n++;
            }
            os << "] }";
            return os;
        }

        bool contains(const string& name) const {
            string fname = formatName(name);
            return (_symtable.find(fname) != _symtable.end());
        }

        bool contains(Node* source, Node* dest, const string& label = "") {
            pair<Node*, Node*> key = make_pair(source, dest);
            return _edgemap.find(key) != _edgemap.end();
        }

        Node* get(const string& name) {
            string fname = formatName(name);
            auto itr = _symtable.find(fname);
            if (itr != _symtable.end()) {
                return itr->second;
            } else {
                for (const auto& sym : _symtable) {
                    if (sym.second->label().find(name) != string::npos) {
                        return sym.second;
                    }
                }
                return nullptr;
            }
        }

        CompNode* get(const Comp& comp) {
            CompNode* node = (CompNode*) this->get(comp.name());
            if (!node->is_parent() && node->expr() != &comp) {
                node->comp(new Comp(comp));
            }
            return node;
        }

        Node* add(Node* node, const string& name = "") {
            string key;
            if (!name.empty()) {
                key = formatName(name);
            } else {
                key = node->label();
            }

            auto iter = _symtable.find(key);
            if (iter == _symtable.end()) {
                _symtable[key] = node;
                _nodes.push_back(node);
            } else {
                auto pos = find(_nodes.begin(), _nodes.end(), iter->second);
                _nodes[distance(_nodes.begin(), pos)] = node;
            }

//            if (node->is_comp()) {
//                addBounds(((CompNode*) node)->comp()->space());
//            } else if (node->is_data()) {
            if (node->is_data()) {
                DataNode* dnode = (DataNode*) node;
                if (isReturn(node)) {
                    returnType(dnode->datatype());
                }
                if (dnode->defval().empty() && !_defaultVal.empty()) {
                    dnode->defval(_defaultVal);
                }
            }

            return node;
        }

        Edge* add(Node* source, Node* dest, const string& label = "") {
            Edge* edge;
            pair<Node*, Node*> key = make_pair(source, dest);
            auto iter = _edgemap.find(key);
            if (iter == _edgemap.end()) {
                edge = new Edge(source, dest, label);
                _edgemap[key] = edge;
                _edges.push_back(edge);
            } else {
                edge = iter->second;
            }
            return edge;
        }

        void remove(Node* node, const string& name = "") {
            string key;
            if (!name.empty()) {
                key = formatName(name);
            } else {
                key = node->label();
            }
            auto iter = _symtable.find(key);
            if (iter != _symtable.end()) {
                _nodes.erase(find(_nodes.begin(), _nodes.end(), node));
                _symtable.erase(iter);
            }
        }

        void remove(Edge* edge) {
            remove(edge->source(), edge->dest(), edge->label());
        }

        void remove(Node* source, Node* dest, const string& label = "") {
            pair<Node*, Node*> key = make_pair(source, dest);
            auto iter = _edgemap.find(key);
            if (iter != _edgemap.end()) {
                _edges.erase(find(_edges.begin(), _edges.end(), iter->second));
                _edgemap.erase(iter);
            }
        }

        bool alignIters() const {
            return _alignIters;
        }

        void alignIters(bool align) {
            _alignIters = align;
        }

        const string& name() const {
            return _name;
        }

        const string& indexType() const {
            return _indexType;
        }

        void indexType(const string& indexType) {
            _indexType = indexType;
        }

        const string& returnName() const {
            return _returnName;
        }

        void returnName(const string& returnName) {
            _returnName = returnName;
        }

        const string& returnType() const {
            return _returnType;
        }

        void returnType(const string& returnType) {
            _returnType = returnType;
        }

        const string& defaultValue() const {
            return _defaultVal;
        }

        void defaultValue(const string& defVal) {
            _defaultVal = defVal;
        }

        int output(const string& name) {
           auto itr = _outputs.find(name);
           if (itr != _outputs.end()) {
               return itr->second;
           }
           return -1;
        }

        vector<string> outputs() const {
            vector<string> outs(_outputs.size(), "");
            for (const auto& itr : _outputs) {
                outs[itr.second] = itr.first;
            }
            return outs;
        }

        void outputs(initializer_list<string> outputs) {
            for (const string& output : outputs) {
                _outputs[output] = _outputs.size();
            }
        }

        const vector<Node*>& nodes() const {
            return _nodes;
        }

        const vector<Edge*>& edges() const {
            return _edges;
        }

        vector<Edge*> inedges(const Node* node) const {
            vector<Edge*> edges;
            for (Edge* edge : _edges) {
                if (edge->dest()->label() == node->label()) {
                    edges.emplace_back(edge);
                }
            }
            return edges;
        }

        vector<Edge*> outedges(const Node* node) const {
            vector<Edge*> edges;
            for (Edge* edge : _edges) {
                if (edge->source()->label() == node->label()) {
                    edges.emplace_back(edge);
                }
            }
            return edges;
        }

        vector<CompNode*> comp_nodes() const {
            vector<CompNode*> nodes;
            for (Node* node : _nodes) {
                if (node->is_comp()) {
                    nodes.push_back((CompNode*) node);
                }
            }
            return nodes;
        }

        vector<CompNode*> data_nodes() const {
            vector<CompNode*> nodes;
            for (Node* node : _nodes) {
                if (node->is_data()) {
                    nodes.push_back((CompNode*) node);
                }
            }
            return nodes;
        }

        bool isReturn(Node* node) const {
            return _returnName == node->label();
        }

        // Source node => no incoming edges
        bool isSource(Node* node) const {
            for (const auto& edge : _edges) {
                if (node->label() == edge->dest()->label()) {
                    return false;
                }
            }
            return true;
        }

        // Sink node => no outgoing edges
        bool isSink(Node* node) const {
            for (const auto& edge : _edges) {
                if (node->label() == edge->source()->label()) {
                    return false;
                }
            }
            return true;
        }

        // Return input nodes, those with no incoming edges.
        vector<Node*> inNodes() const {
            vector<Node*> inputs;
            for (const auto& node : _nodes) {
                if (isSource(node)) {
                    inputs.push_back(node);
                }
            }
            return inputs;
        }

        // Return output nodes, those with no outgoing edges.
        vector<Node*> outNodes() const {
            vector<Node*> outputs;
            for (const auto& node : _nodes) {
                if (isSink(node)) {
                    outputs.push_back(node);
                }
            }
            return outputs;
        }

        void fuse(Comp& lhs, Comp& rhs) {
            CompNode* first = this->get(lhs);
            CompNode* next = this->get(rhs);

            // Update iteration graph...
            updateIterGraph(first, next);

            // Move incoming edges...
            vector<Edge*> ins = inedges(next);
            for (Edge* in : ins) {
                cerr << "Removing edge '" << in->source()->label() << "' -> '" << in->dest()->label() << "'\n";
                remove(in);
                cerr << "Adding edge '" << in->source()->label() << "' -> '" << first->label() << "'\n";
                add(in->source(), first, in->label());
            }

            // Move outgoing edges...
            vector<Edge*> outs = outedges(next);
            for (Edge* out : outs) {
                cerr << "Removing edge '" << out->source()->label() << "' -> '" << out->dest()->label() << "'\n";
                remove(out);
                _child_prods[out->dest()->label()] = out->source();
                cerr << "Adding edge '" << first->label() << "' -> '" << out->dest()->label() << "'\n";
                add(first, out->dest(), out->label());
            }

            // Fuse nodes...
            cerr << "Removing node '" << next->label() << "'\n";
            first->fuse(next);
            remove(next);
        }

        void tile(initializer_list<string> iters, initializer_list<unsigned> sizes) {
            for (CompNode* node : this->comp_nodes()) {
                node->tile(iters, sizes);
            }
        }

        bool ignoreCycles() const {
            return _ignoreCycles;
        }

        void ignoreCycles(bool status) {
            _ignoreCycles = status;
        }

        unsigned tileSize() const {
            return _tileSize;
        }

        void tileSize(unsigned size) {
            _tileSize = size;
        }

    protected:
        void updateIterGraph(CompNode* prev, CompNode* curr) {
            string inode, inext, iprev, iter;
            Tuple sched;
            IntTuple path;
            IntTuple shifts;

            vector<Tuple> prevScheds = prev->schedules();
            vector<Tuple> currScheds = curr->schedules();

            if (_alignIters) {
                unsigned nPrevScheds = prevScheds[0].size();
                unsigned nCurrScheds = currScheds[0].size();
                if (nPrevScheds != nCurrScheds && !Lists::match<Iter>(prevScheds[0], currScheds[0], nPrevScheds - 1)) {
                    if (nPrevScheds < nCurrScheds) {
                        prev->comp()->interchanged(alignIterators(currScheds, prevScheds[0][0]));
                    } else {
                        curr->comp()->interchanged(alignIterators(prevScheds, currScheds[0][0]));
                        prev->schedules(prevScheds);
                    }
                }
            }

            Digraph* ig = prev->iter_graph();
            inode = ig->last_node();
            inext = ig->last_leaf();

            // TODO: Assuming the node to be fused has only one schedule for now...
            sched = currScheds[0];
            path.clear();

//            if (curr->label() == "smul_d1") {
//                cerr << ig->to_dot() << endl;
//                int stop = 1;
//            }

            // Construct set of data dependences...
            bool is_parent = false;
            vector<CompNode*> producers = getProducers(prev, curr, &is_parent);
            map<string, IntTuple> prod_paths;
            int shift_sum = 0;

            if (is_parent) {
                shifts = nodeOffsets(prev, curr, sched);
                shift_sum = accumulate(shifts.begin(), shifts.end(), 0);
            } else {
                for (CompNode *prod : producers) {
                    IntTuple prod_path;
                    ig->find(prod->label(), prod_path);
                    prod_paths[prod->label()] = prod_path;

                    shifts = absmax(shifts, nodeOffsets(prod, curr, sched));
                    shift_sum = accumulate(shifts.begin(), shifts.end(), 0);

                    if (prod->shifts()) {
                        shifts = absmax(shifts, *prod->shifts());
                        shift_sum = accumulate(shifts.begin(), shifts.end(), 0);
                    }
                }
            }

            inode = ig->root();
            for (unsigned j = 0; j < sched.size() - 1; j++) {
                iter = sched[j].text();
                inext = ig->find(inode, iter, path);

                // Check producer paths for dependence violations...
                for (const auto& itr : prod_paths) {
                    IntTuple prod_path = itr.second;
                    iprev = inext;
                    vector<string> skips;
                    while (!inext.empty() && path[j] < prod_path[j]) {
                        // Find next valid iterator node, skipping those already visited...
                        path.resize(j);
                        skips.push_back(inext);
                        inext = ig->find(inode, iter, path, skips);
                    }
                    if (!inext.empty() && j > 1 && shifts[j-1] != 0) {
                        // TODO: This horrible code is intended to solve the problem of leaf nodes being placed
                        // with next to siblings whose parents have different shifts, which results in incorrect
                        // (and ugly) code generation by Omega. Refactor!!!
                        int pos = ig->size(inext) - 1;
                        if (pos >= 0) {
                            string isib = ig->child(inext, pos);
                            CompNode* sib_comp = prev->find_child(isib);
                            if (sib_comp && sib_comp->shifts()) {
                                int sib_shift = sib_comp->shifts()->at(j-2);
                                if (sib_shift != 0 && sib_shift != shifts[j-2]) {
                                    inext = "";
                                }
                            }
                        }

                    }
                    if (iprev != inext) {
                        break;
                    }
                }

                if (inext.empty()) {    // Iterator not found on current path, create a new one...
                    inext = ig->node(iter, iter);
                    ig->edge(inode, inext, ig->size(inode));
                }

                iprev = inode;
                inode = inext;
            }

            inode = ig->insertLeaf(inode, curr->label(), path);         // Add leaf node (the statement)
            if (shift_sum) {
                if (is_parent) {
                    // Apply shift to parent...
                    prev->shifts(shifts);
                    inode = ig->find(ig->root(), prev->first_label());
                } else {
                    curr->shifts(shifts);
                }

                string shift = Strings::str<int>(shifts).substr(1);
                ig->attr(inode, "shift", shift.substr(0, shift.size() - 1));
            }

//            if (curr->label() == "smul_d1") {
//                cerr << ig->to_dot() << endl;
//                int stop = 1;
//            }
        }

        string formatName(const string& name) const {
            string fname = name;
            size_t pos = fname.find('(');
            if (pos != string::npos) {
                fname = fname.substr(0, pos);
            }
            pos = fname.find('[');
            if (pos != string::npos) {
                fname = fname.substr(0, pos);
            }
            return fname;
        }

        vector<CompNode*> getProducers(CompNode* prev, CompNode* curr, bool* is_parent) {
            vector<CompNode*> producers;
            for (Edge* edge : this->inedges(curr)) {
                DataNode* dnode = (DataNode*) edge->source();
                vector<Edge*> ins = this->inedges(dnode);
                for (Edge* in : ins) {
                    if (in->source() == prev) {     // Search for child node that produces this dest...
                        CompNode* child = nullptr;
                        auto iter = _child_prods.find(dnode->label());
                        if (iter != _child_prods.end()) {
                            child = (CompNode*) iter->second;
                        } else {
                            child = prev->find_child(dnode->label());
                        }
                        if (child) {
                            producers.push_back(child);
                        } else if (!(*is_parent)) {
                            *is_parent = true;
                        }
                    }
                }
            }
            return producers;
        }

        bool alignIterators(vector<Tuple>& tuples, Iter& iter) {
            bool changed = false;
            if (!iter.empty()) {
                for (Tuple& tuple : tuples) {
                    if (find(tuple.begin(), tuple.end(), iter) != tuple.end()) {
                        while (!tuple[0].equals(iter)) {
                            tuple.insert(tuple.end() - 1, tuple[0]);
                            tuple.erase(tuple.begin());
                            changed = true;
                        }
                    }
                }
            }
            return changed;
        }

        IntTuple nodeOffsets(CompNode* prev, CompNode* curr, const Tuple& sched) {
            IntTuple shifts;
            IntTuple max_offsets, min_offsets;

            getOffsets(sched, prev->writes(), max_offsets);
            getOffsets(sched, curr->reads(), max_offsets);
            getOffsets(sched, prev->writes(), min_offsets, -1);
            getOffsets(sched, curr->reads(), min_offsets, -1);

            for (unsigned i = 0; i < max_offsets.size() && i < min_offsets.size(); i++) {
                int offset = max_offsets[i] - min_offsets[i];
                shifts.push_back(min_offsets[i] + offset);
            }

            return shifts;
        }

        void getOffsets(const Tuple& schedule, const map<string, Access*> accmap, vector<int>& offsets, int comp = 1) {
            for (unsigned i = 0; i < schedule.size(); i++) {
                if (offsets.size() <= i) {
                    offsets.push_back(0);
                }

                for (const auto& itr : accmap) {
                    Access *acc = itr.second;
                    string space = acc->space();
                    ExprTuple tuple = acc->tuple();

                    bool match = false;
                    for (unsigned j = 0; j < tuple.size() && !match; j++) {
                        if (!schedule[i].is_int()) {
                            string expr = tuple[j].text();
                            string iter = schedule[i].text();
                            size_t pos = expr.find(iter);
                            match = (pos != string::npos);
                            if (match && expr != iter) {
                                expr.erase(pos, iter.size());
                                int offset = unstring<int>(expr);
                                if ((comp > 0 && offset > offsets[i]) ||
                                    (comp < 0 && offset < offsets[i])) {
                                    offsets[i] = offset;
                                }
                            }
                        }
                    }
                }
            }
        }

//        void addBounds(const Space& space) {
//            // Only want iterators that all spaces have...
//            vector<Expr> bounds = space.bounds({"z", "y", "x"});
//            string key = Strings::str<Expr>(bounds);
//            if (_boundCounts.find(key) == _boundCounts.end()) {
//                _boundCounts[key] = 0;
//            }
//            _boundCounts[key] += 1;
//        }

        string _name;
        string _indexType;
        string _returnName;
        string _returnType;
        string _defaultVal;

        bool _alignIters;
        bool _ignoreCycles;
        unsigned _tileSize;

        map<string, Node*> _symtable;
        map<pair<Node*, Node*>, Edge*> _edgemap;
        map<string, unsigned> _outputs;
        map<string, Node*> _child_prods;
//        map<string, unsigned> _boundCounts;        // Keep track of polyhedral bounding boxes...

        vector<Node*> _nodes;
        vector<Edge*> _edges;
    };
}

#endif  // POLYEXT_FLOWGRAPH_H
