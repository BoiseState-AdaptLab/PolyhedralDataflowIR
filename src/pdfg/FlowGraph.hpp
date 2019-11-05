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

    struct GraphItem {
    public:
        virtual string attr(const string& key) const {
            string val;
            auto iter = _attrs.find(key);
            if (iter != _attrs.end()) {
                val = iter->second;
            }
            return val;
        }

        virtual void attr(const string& key, const string& val) {
            _attrs[key] = val;
        }

        virtual string label() const {
            return _label;
        }

        virtual void label(const string& label) {
            _label = label;
        }

        virtual void print_attrs(ostream& os) {
            if (_attrs.size() > 2) {
                os << ", \"attrs\": { ";
                unsigned n = 0, nattrs = _attrs.size();
                for (const auto &iter : _attrs) {
                    os << "\"" << iter.first << "\": \"" << iter.second << "\"";
                    if (n < nattrs - 1) {
                        os << ", ";
                    }
                    n += 1;
                }
                os << " }";
            }
        }

    protected:
        string _label;
        map<string, string> _attrs;
    };

    struct Node : public GraphItem {
    public:
        //explicit Node(const string& space, const string& label = "") {
        explicit Node(Expr* expr = nullptr, const string& label = "", initializer_list<string> attrs={}) {
            init(expr, label, attrs);
        }

        virtual ~Node() {
            delete _expr;
        }

        Node(const Node& other) {
            copy(other);
        }

        Node& operator=(const Node& other) {
            copy(other);
            return *this;
        }

        Expr* expr() const {
            return _expr;
        }

        string label() const override {
            return _label;
        }

        void label(const string& label) override {
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
            node.print_attrs(os);
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

        void copy(const Node& other) {
            _label = other._label;
            _attrs = other._attrs;
            _expr = new Expr(other._expr->text(), other._expr->type());
            _type = other._type;
        }

        Expr* _expr;
        char _type;
    };

    struct Edge : public GraphItem {
    public:
        explicit Edge(Node* source, Node* dest, const string& label = "") :
                _source(source), _dest(dest) {
            _label = label;
        }

        Edge(const Edge& other) {
            copy(other);
        }

        Edge& operator=(const Edge& other) {
            copy(other);
            return *this;
        }

        Node* source() const {
            return _source;
        }

        Node* dest() const {
            return _dest;
        }

        void source(Node* source) {
            _source = source;
        }

        void dest(Node* dest) {
            _dest = dest;
        }

        friend ostream& operator<<(ostream& os, Edge& edge) {
            os << "{ ";
            if (!edge._label.empty()) {
                os << "\"label\": \"" << edge._label << "\", ";
            }
            os << "\"src\": \"" << edge._source->label()  << "\", \"dest\": \"" << edge._dest->label() << "\" ";
            edge.print_attrs(os);
            os << "}";
            return os;
        }

    protected:
        void copy(const Edge& other) {
            _attrs = other._attrs;
            _source = other._source;
            _dest = other._dest;
        }

        Node* _source;
        Node* _dest;
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

        DataNode(const DataNode& other) {
            copy(other);
        }

        DataNode& operator=(const DataNode& other) {
            copy(other);
            return *this;
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
            return Strings::isNumber(os.str()) && stoi(os.str()) < 2;
        }

        friend ostream& operator<<(ostream& os, DataNode& node) {
            os << "{ \"label\": \"" << node._label << "\", ";
            os << "\"size\": \"" << *node._size << "\", ";
            os << "\"defval\": \"" << node._defval << "\", ";
            os << "\"type\": \"" << node._datatype << "\"";
            node.print_attrs(os);
            os << " }";
            return os;
        }

    protected:
        void copy(const DataNode& other) {
            Node::copy(other);
            _datatype = other._datatype;
            _defval = other._defval;
            _alloc = other._alloc;
            _size = new Expr(other._size->text(), other._size->type());
        }

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

        CompNode(const CompNode& other) {
            copy(other);
        }

        CompNode& operator=(const CompNode& other) {
            copy(other);
            return *this;
        }

        static vector<string> mathFunctions() {
            return {"exp", "pow", "log", "log10", "sqrt", "sin", "cos", "tan"};
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
            for (int i = _children.size() - 1; i >= 0; i--) {
                CompNode *child = _children[i];
                if (child->label() == pattern) {
                    return child;
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
            unsigned nflops = stoi(this->attr("flops"));

            for (CompNode* child : _children) {
                nflops += child->flops();
            }

            // Assume at least 1 FLOP...
            if (nflops < 1) {
                nflops = 1;
            }

            return nflops;
        }

        void tile(initializer_list<string> iters, initializer_list<unsigned> sizes, initializer_list<string> outs = {}) {
            vector<string> tile_iters(iters.begin(), iters.end());
            vector<unsigned> tile_sizes(sizes.begin(), sizes.end());
            vector<string> out_iters(outs.begin(), outs.end());
            tile(tile_iters, tile_sizes, out_iters);
        }

        void tile(vector<string> iters, vector<unsigned> sizes, vector<string> t_iters = {}) {
            vector<Tuple> schedules = this->schedules();
            vector<string> tile_iters(t_iters.begin(), t_iters.end());
            vector<unsigned> tile_sizes(sizes.begin(), sizes.end());

            Digraph* ig = this->iter_graph();
            string top_node = ig->int_nodes()[0];
            string tile_node;

            unsigned inum = 0;
            for (const string& iter : iters) {
                if (tile_iters.size() <= inum) {
                    tile_iters.push_back("t" + iter);
                }

                string node = top_node; //ig->find(iter);
                if (!node.empty()) {
                    string tile_iter = tile_iters[inum];        // Tile iterator
                    string rem_iter = "r" + iter;               // Remainder iterator
                    if (tile_node.empty()) {
                        tile_node = node;
                    }
                    ig->insert(tile_node, tile_iter, tile_iter, {"tile_iter", tile_iter, "rem_iter", rem_iter,
                        "orig_iter", iter, "tile_size", to_string(tile_sizes[inum])});
                }
                inum += 1;
            }
            this->comp()->tiled(!tile_node.empty());
        }

    protected:
        void copy(const CompNode& other) {
            Node::copy(other);
            _expr = new Comp(*((Comp*) other._expr));

            _shifts = other._shifts;
            if (_shifts) {
                _shifts = new IntTuple(*_shifts);
            }
            _igraph = other._igraph;
            if (_igraph) {
                _igraph = new Digraph(*_igraph);
            }

            for (const auto& iter : other._reads) {
                _reads[iter.first] = new Access(*iter.second);
            }

            for (const auto& iter : other._writes) {
                _writes[iter.first] = new Access(*iter.second);
            }

            // Not sure how to copy children, let us assume copying un-fused graphs for now.
            for (CompNode* other_child : other._children) {
                CompNode* this_child = nullptr; //(CompNode*) this->get(other_child->label());
                if (this_child) {
                    _children.push_back(this_child);
                }
            }
        }

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

        RelNode(const RelNode& other) {
            copy(other);
        }

        RelNode& operator=(const RelNode& other) {
            copy(other);
            return *this;
        }

        Rel* relation() const {
            return (Rel*) _expr;
        }

        void relation(Rel* rel) {
            _expr = rel;
        }

    protected:
        void copy(const RelNode& other) {
            Node::copy(other);
            _expr = new Rel(*((Rel*) other._expr));
        }
    };

    struct FlowGraph : public GraphItem {
    public:
        explicit FlowGraph(const string& name = "", const string& retType = "void", const string& retName = "",
                           const string& defVal = "", unsigned tileSize = 0, bool ignoreCycles = true) :
                            _returnType(retType), _returnName(retName), _indexType("unsigned"),
                           _defaultVal(defVal), _tileSize(tileSize), _ignoreCycles(ignoreCycles) {
            _label = name;
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

        FlowGraph(const FlowGraph& other) {
            copy(other);
        }

        FlowGraph& operator=(const FlowGraph& other) {
            copy(other);
            return *this;
        }

        friend ostream& operator<<(ostream& os, FlowGraph& graph) {
            os << "{ \"name\": \"" << graph._label << "\"";
            if (graph._tileSize > 0) {
                os << ", \"tile_size\": " << graph._tileSize;
            }
            graph.print_attrs(os);
            os << ", \"nodes\": [\n";

            unsigned n = 0, size = graph._nodes.size();
            for (Node* node : graph._nodes) {
                if (node->is_data()) {
                    os << *((DataNode*) node);
                } else {
                    os << *node;
                }
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
            return _label;
        }

        void name(const string& name) {
            _label = name;
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

        void outputs(const vector<string>& outputs) {
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

        vector<Edge*> in_edges(const Node* node) const {
            vector<Edge*> edges;
            for (Edge* edge : _edges) {
                if (edge->dest()->label() == node->label()) {
                    edges.emplace_back(edge);
                }
            }
            return edges;
        }

        vector<Edge*> out_edges(const Node* node) const {
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

        bool isTemp(DataNode* node) {
            return (!isReturn(node) && !isSource(node) && !isSink(node) && output(node->label()) < 0);
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

        void fuse() {
            vector<CompNode*> nodes = this->comp_nodes();
            CompNode* first = nodes[0];
            for (unsigned i = 1; i < nodes.size(); i++) {
                this->fuse(*first->comp(), *nodes[i]->comp());
            }
        }

        void fuse(initializer_list<string> names) {
            vector<string> vec(names.begin(), names.end());
            fuse(vec);
        }

        void fuse(vector<vector<string> > names) {
            for (vector<string>& subset : names) {
                fuse(subset);
            }
        }

        void fuse(vector<string> names) {
            CompNode* first = (CompNode*) this->get(*names.begin());
            vector<CompNode*> others;
            for (auto itr = names.begin() + 1; itr != names.end(); ++itr) {
                CompNode* next = (CompNode*) this->get(*itr);
                if (next) {
                    fuse(*first->comp(), *next->comp());
                }
            }
        }

        void fuse(Comp& lhs, Comp& rhs) {
            CompNode* first = this->get(lhs);
            CompNode* next = this->get(rhs);

            // Update iteration graph...
            updateIterGraph(first, next);

            // Move incoming edges...
            vector<Edge*> ins = in_edges(next);
            for (Edge* in : ins) {
                cerr << "Removing edge '" << in->source()->label() << "' -> '" << in->dest()->label() << "'\n";
                remove(in);
                cerr << "Adding edge '" << in->source()->label() << "' -> '" << first->label() << "'\n";
                add(in->source(), first, in->label());
            }

            // Move outgoing edges...
            vector<Edge*> outs = out_edges(next);
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
            vector<string> tile_iters(iters.begin(), iters.end());
            vector<unsigned> tile_sizes(sizes.begin(), sizes.end());
            tile(tile_iters, tile_sizes);
        }

        void tile(vector<string> iters, vector<unsigned> sizes) {
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
            vector<Tuple> prevScheds = prev->schedules();
            vector<Tuple> currScheds = curr->schedules();

            // 1) Align iterators if necessary, in the case of Proto, this interchanges the component loops.
            if (_alignIters) {
                unsigned nPrevScheds = prevScheds[0].size();
                unsigned nCurrScheds = currScheds[0].size();
                if (nPrevScheds != nCurrScheds && !Lists::match<Iter>(prevScheds[0],currScheds[0], nPrevScheds - 1)) {
                    if (nPrevScheds < nCurrScheds) {
                        curr->comp()->interchanged(alignIterators(currScheds, prevScheds[0][0]));
                        curr->schedules(currScheds);
                    } else {
                        prev->comp()->interchanged(alignIterators(prevScheds, currScheds[0][0]));
                        prev->schedules(prevScheds);
                    }
                }
            }

            // 2) Get current position in iteration graph.
            string inode, inext, iprev, iter;
            Digraph* ig = prev->iter_graph();
            inode = ig->last_node();
            inext = ig->last_leaf();

            //if (curr->label() == "consToPrim1" || curr->label() == "laplacian" || curr->label() == "interpL_d1" || curr->label() == "interpH_d1") {
//            if (curr->label() == "Assq") {
//                cerr << ig->to_dot() << endl;
//                int stop = 1;
//            }

            // 3) Compute initial shift from loop bounds
            IntTuple prevLowers = to_int(prev->comp()->lowers());
            IntTuple currLowers = to_int(curr->comp()->lowers());
            IntTuple shifts = prevLowers - currLowers;

            // TODO: Assuming the node to be fused has only one schedule for now...
            Tuple sched = currScheds[0];
            Tuple prev_sched = prevScheds[0];

            // 4) Adjust the shift based on the span of node (i.e, read/write distances).
            IntTuple span = nodeSpan(curr, sched);
            //if (!producers.empty() && sum(span)) {
            if (sum(span)) {
                shifts += span;
            }

            // 5) Construct set of producer-consumer dependences...
            unordered_map<string, CompNode*> producers = getProducers(prev, curr);
            unordered_map<string, IntTuple> prod_paths;

            IntTuple max_shifts = shifts;
            for (const auto& itr : producers) {
                CompNode* prod = itr.second;
                IntTuple prod_path;
                ig->find(prod->label(), prod_path);
                prod_paths[prod->label()] = prod_path;
                if (prod->shifts()) {
                    max_shifts = max(max_shifts, *prod->shifts());
                }
            }

            // 6) Finally, adjust shifts for read/write max values, ensuring that necessary data points are computed.
            if (shifts != max_shifts) {
                //shifts = max(max_shifts, max_shifts + shifts);
                shifts = max(shifts, max_shifts);
                IntTuple maxes = nodeMaxes(curr, sched);
                if (sum(maxes)) { // && maxes != shifts) {
                    shifts += maxes;
                }
            }
            //cerr << curr->label() << " -> (" << Strings::join<int>(shifts, ",") << ")\n";

            // 7) Populate iter graph skip list (internal nodes that do not appear in schedule)...
            vector<string> skips;
            for (const string& node : ig->int_labels()) {
                if (std::find(sched.begin(), sched.end(), node) == sched.end()) {
                    skips.push_back(node);
                }
            }

            // 8) Traverse the iteration graph and find a place to insert this node.
            inode = ig->root();
            IntTuple path;
            for (unsigned j = 0; j < sched.size() - 1; j++) {
                iter = sched[j].text();
                inext = ig->find(inode, iter, path, skips);

                // Check producer paths for dependence violations...
                for (const auto &itr : producers) {
                    CompNode *producer = itr.second;
                    IntTuple prod_path = prod_paths[itr.first];
                    iprev = inext;

                    if (j < prod_path.size()) {
                        while (!inext.empty() && path[j] < prod_path[j]) {
                            // Find next valid iterator node, skipping those already visited...
                            path.resize(j);
                            skips.push_back(inext);
                            inext = ig->find(inode, iter, path, skips);
                        }
                    }

                    if (!inext.empty() && !shifts.empty() && j > 1 && shifts[j - 1] != 0) {
                        // TODO: This horrible code is intended to solve the problem of leaf nodes being placed
                        // with next to siblings whose parents have different shifts, which results in incorrect
                        // (and ugly) code generation by Omega. Refactor!!!
                        int pos = ig->size(inext) - 1;
                        if (pos >= 0) {
                            string isib = ig->child(inext, pos);
                            CompNode *sib_comp = prev->find_child(isib);
                            if (sib_comp && sib_comp->shifts()) {
                                int sib_shift = sib_comp->shifts()->at(j - 2);
                                if (sib_shift != 0 && sib_shift != shifts[j - 2]) {
                                    inext = "";
                                }
                            }
                        }
                    }
                }

                if (inext.empty()) {    // Iterator not found on current path, create a new one...
                    inext = ig->node(iter, iter);
                    ig->edge(inode, inext, ig->size(inode));
                }

                iprev = inode;
                inode = inext;
            }

            // 9) Insert this leaf node (statement) into the iteration graph.
            inode = ig->insertLeaf(inode, curr->label(), path);

            // 10) Apply shifts if necessary.
            if (sum(shifts) != 0) {
                curr->shifts(shifts);
                string shift = Strings::str<int>(shifts).substr(1);
                shift = shift.substr(0, shift.size() - 1);
                this->get(inode)->attr("shift", shift);
                ig->attr(inode, "shift", shift);
            }

//            if (curr->label() == "Adiv") {
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

        unordered_map<string, CompNode*> getProducers(CompNode* prev, CompNode* curr) {
            unordered_map<string, CompNode*> producers;
            for (Edge* edge : this->in_edges(curr)) {
                DataNode* dnode = (DataNode*) edge->source();
                vector<Edge*> ins = this->in_edges(dnode);
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
                            producers[child->label()] = child;
                        } else {
                            producers[prev->label()] = prev;
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

        IntTuple nodeSpan(CompNode* node, const Tuple& schedule) {
            unsigned niters = schedule.size() - 1;
            IntTuple max_offsets(niters, INT_MIN);
            IntTuple min_offsets(niters, INT_MAX);
            nodeBounds(node, schedule, min_offsets, max_offsets);

            IntTuple span(niters, 0);
            for (unsigned i = 0; i < niters; i++) {
                if (max_offsets[i] == INT_MIN) {
                    max_offsets[i] = 0;
                }
                if (min_offsets[i] == INT_MAX) {
                    min_offsets[i] = 0;
                }
                span[i] = max_offsets[i] - min_offsets[i];
            }
            return span;
        }

        IntTuple nodeMins(CompNode* node, const Tuple& schedule) {
            unsigned niters = schedule.size() - 1;
            IntTuple span(niters, 0);
            IntTuple max_offsets(niters, INT_MIN);
            IntTuple min_offsets(niters, INT_MAX);
            nodeBounds(node, schedule, min_offsets, max_offsets);

            for (unsigned i = 0; i < niters; i++) {
                if (min_offsets[i] == INT_MAX) {
                    min_offsets[i] = 0;
                }
            }
            return min_offsets;
        }

        IntTuple nodeMaxes(CompNode* node, const Tuple& schedule) {
            unsigned niters = schedule.size() - 1;
            IntTuple span(niters, 0);
            IntTuple max_offsets(niters, INT_MIN);
            IntTuple min_offsets(niters, INT_MAX);
            nodeBounds(node, schedule, min_offsets, max_offsets);

            for (unsigned i = 0; i < niters; i++) {
                if (max_offsets[i] == INT_MIN) {
                    max_offsets[i] = 0;
                }
            }
            return max_offsets;
        }

        void nodeBounds(CompNode* node, const Tuple& schedule, IntTuple& min_offsets, IntTuple& max_offsets) {
            unsigned niters = schedule.size() - 1;
            vector<Access*> accesses = node->accesses();
            for (Access* access : accesses) {
                ExprTuple tuple = access->tuple();
                for (unsigned i = 0; i < niters; i++) {
                    //if (!schedule[i].is_int()) {
                    string iter = schedule[i].text();
                    bool match = false;
                    for (unsigned j = 0; j < tuple.size() && !match; j++) {
                        string expr = tuple[j].text();
                        size_t pos = expr.find(iter);
                        match = (pos != string::npos);
                        if (match) { // && expr != iter) {
                            int offset = 0;
                            if (expr != iter) {
                                expr.erase(pos, iter.size());
                                offset = unstring<int>(expr);
                            }

                            if (offset < min_offsets[i]) {
                                min_offsets[i] = offset;
                            }
                            if (offset > max_offsets[i]) {
                                max_offsets[i] = offset;
                            }
                        }
                    }
                    //}
                }
            }
        }

        void getOffsets(const Tuple& schedule, const map<string, Access*> accmap, IntTuple& offsets, int comp = 1) {
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

        void copy(const FlowGraph& other) {
            _alignIters = other._alignIters;
            _ignoreCycles = other._ignoreCycles;
            _tileSize = other._tileSize;

            _label = other._label;
            _indexType = other._indexType;
            _returnName = other._returnName;
            _returnType = other._returnType;
            _defaultVal = other._defaultVal;
            _outputs = other._outputs;
            _attrs = other._attrs;

            for (Node* node : other._nodes) {
                if (node->is_comp()) {
                    this->add(new CompNode(*((CompNode*) node)));
                } else if (node->is_data()) {
                    this->add(new DataNode(*((DataNode*) node)));
                } else {
                    this->add(new Node(*node));
                }
            }

            for (Edge* edge : other._edges) {
                Node* source = this->get(edge->source()->label());
                Node* dest = this->get(edge->dest()->label());
                add(source, dest);
            }
        }

        bool _alignIters;
        bool _ignoreCycles;
        unsigned _tileSize;

        string _indexType;
        string _returnName;
        string _returnType;
        string _defaultVal;

        map<string, Node*> _symtable;
        map<pair<Node*, Node*>, Edge*> _edgemap;
        map<string, unsigned> _outputs;
        map<string, Node*> _child_prods;

        vector<Node*> _nodes;
        vector<Edge*> _edges;
    };
}

#endif  // POLYEXT_FLOWGRAPH_H
