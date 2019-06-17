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

//        const string& space() const {
//            return _space;
//        }

        Expr* expr() const {
            return _expr;
        }

//        void name(const string& name) {
//            _space.name(name);
//        }

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

//        void space(const string& space) {
//            _space = space;
//        }

        void expr(Expr* expr) {
            _expr = expr;
        }

        const char type() const {
            return _type;
        }

        friend ostream& operator<<(ostream& out, Node& node) {
            out << "{ \"label\": \"" << node._label << "\", \"space\": \"" << *(node.expr()) << "\" }";
            return out;
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

        //string _space;
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
                          const string& defval = "0", const string& mapping = "", const MemAlloc& alloc = DYNAMIC) :
            Node(new Expr(*access), label), _size(size), _datatype(type), _defval(defval), _alloc(alloc) {
            attr("shape", "box");
            attr("color", "gray");
            _mapping = mapping;
            _type = 'D';
        }

        ~DataNode() {
            delete _size;
            _size = 0;
            delete _expr;
            _expr = 0;
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

        string defval() const {
            return _defval;
        }

        void defval(const string& val) {
            _defval = val;
        }

        string mapping() const {
            return _mapping;
        }

        void mapping(const string& mapping) {
            _mapping = mapping;
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
            out << "\"type\": \"" << node._datatype << "\" }";
            return out;
        }

    protected:
        Expr* _size;
        string _datatype;
        string _defval;
        string _mapping;
        MemAlloc _alloc;
    };

    struct CompNode : public Node {
    public:
        //explicit CompNode(const string& space, const string& label = "") : Node(space, label), _comp(comp) {
        explicit CompNode(Comp* comp, const string& label = "") : Node(new Comp(*comp), label) {
            attr("shape", "invtriangle");
            _type = 'C';
        }

        Comp* comp() const {
            return (Comp*) _expr;
        }

        void comp(Comp* comp) {
            delete _expr;
            _expr = comp;
        }


        vector<CompNode*> children() const {
            return _children;
        }

        void fuse(CompNode* other) {
            this->label(this->label() + "*" + other->label());
            _children.push_back(other);
        }

    protected:
        vector<CompNode*> _children;
    };

    struct RelNode : public Node {
    public:
        //explicit RelNode(const Rel& rel, const string& label = "") : Node(rel.source(), label), _rel(rel) {
        explicit RelNode(Expr* rel, const string& label = "") : Node(rel, label) { //}, _rel(rel) {
            _type = 'R';
        }

//        Rel relation() const {
//            return _rel;
//        }
//
//        void relation(const Rel& rel) {
//            _rel = rel;
//        }
//
//    protected:
//        Rel _rel;
    };

    struct FlowGraph {
    public:
        explicit FlowGraph(const string& name = "", const string& returnType = "void", const string& returnName = "") :
            _name(name), _returnType(returnType), _returnName(returnName), _indexType("unsigned") {
            _nodes.reserve(100);
            _edges.reserve(100);
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
            os << "{ \"name\": \"" << graph._name << "\", \"nodes\": [\n";
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
            string key = source->label() + "-" + label + ">" + dest->label();
            return _edgemap.find(key) != _edgemap.end();
        }

        Node* get(const string& name) {
            string fname = formatName(name);
            auto itr = _symtable.find(fname);
            if (itr != _symtable.end()) {
                return itr->second;
            } else {
                return nullptr;
            }
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
            return node;
        }

        Edge* add(Node* source, Node* dest, const string& label = "") {
            Edge* edge;
            string key = source->label() + "-" + label + ">" + dest->label();
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

//        Edge* add(Edge* edge) {
//            _edges.push_back(edge);
//            return edge;
//        }

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
            string key = source->label() + "-" + label + ">" + dest->label();
            auto iter = _edgemap.find(key);
            if (iter != _edgemap.end()) {
                _edges.erase(find(_edges.begin(), _edges.end(), iter->second));
                _edgemap.erase(iter);
            }
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

        const vector<Node*>& nodes() const {
            return _nodes;
        }

        const vector<Edge*>& edges() const {
            return _edges;
        }

        const vector<Edge*> inedges(const Node* node) const {
            vector<Edge*> edges;
            for (Edge* edge : _edges) {
                if (edge->dest()->label() == node->label()) {
                    edges.emplace_back(edge);
                }
            }
            return edges;
        }

        const vector<Edge*> outedges(const Node* node) const {
            vector<Edge*> edges;
            for (Edge* edge : _edges) {
                if (edge->source()->label() == node->label()) {
                    edges.emplace_back(edge);
                }
            }
            return edges;
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

        void fuse(initializer_list<Comp> comps) {
            vector<CompNode*> compNodes;
            for (const Comp& comp : comps) {
                CompNode* node = (CompNode*) this->get(comp.name());
                node->comp(new Comp(comp));
                compNodes.push_back(node);
            }
            if (!compNodes.empty()) {
                CompNode* first = compNodes[0];
                for (unsigned i = 1; i < compNodes.size(); i++) {
                    CompNode* next = compNodes[i];
                    vector<Edge*> ins = inedges(next);
                    for (Edge* in : ins) {
                        cerr << "Removing edge '" << in->source()->label() << "' -> '" << in->dest()->label() << "'\n";
                        remove(in);
                        cerr << "Adding edge '" << in->source()->label() << "' -> '" << first->label() << "'\n";
                        add(in->source(), first, in->label());
                    }
                    vector<Edge*> outs = outedges(next);
                    for (Edge* out : outs) {
                        cerr << "Removing edge '" << out->source()->label() << "' -> '" << out->dest()->label() << "'\n";
                        remove(out);
                        cerr << "Adding edge '" << first->label() << "' -> '" << out->dest()->label() << "'\n";
                        add(first, out->dest(), out->label());
                    }
                    cerr << "Removing node '" << next->label() << "'\n";
                    first->fuse(next);
                    remove(next);
                }
            }
        }

    protected:
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

        string _name;
        string _indexType;
        string _returnName;
        string _returnType;

        map<string, Node*> _symtable;
        map<string, Edge*> _edgemap;

        vector<Node*> _nodes;
        vector<Edge*> _edges;
    };
}

#endif  // POLYEXT_FLOWGRAPH_H
