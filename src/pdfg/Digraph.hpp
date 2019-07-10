//
// Created by edavis on 3/21/19.
//

#ifndef _DIGRAPH_HPP_
#define _DIGRAPH_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

using std::ostream;
using std::map;
using std::string;
using std::vector;
using std::pair;
using std::make_pair;

namespace pdfg {
    typedef pair<string, string> Pair;

    class Digraph {
        friend ostream& operator<<(ostream& out, Digraph& g) {
            out << "{ \"name\": \"" << g._name << "\", \"nodes\": { ";
            unsigned m = 0;
            for (const string& src : g._nodes) {
                unsigned ndx = g._indices[src];
                vector<Pair> edges = g._edges[ndx];
                if (edges.size() > 0) {
                    out << "\"" << src << "\": { ";
                    unsigned n = 0;
                    for (Pair e : edges) {
                        string dest = e.first;
                        string label = e.second;
                        out << "(\"" << dest << "\", \"" << label << "\")";
                        if (n < edges.size() - 1) {
                            out << ", ";
                        }
                        n += 1;
                    }
                    out << " }";
                    if (m < g._nodes.size() - 1) {
                        out << ", ";
                    }
                    m += 1;
                }
            }
            out << "} }";
            return out;
        }

    public:
        explicit Digraph(const string& name = "", const unsigned size = 0) {
            _name = name;
            _edges.resize(size);
        }

        string name() const {
            return _name;
        }

        string root() const {
            if (!_nodes.empty()) {
                return _nodes[0];
            }
            return "";
        }

        bool is_root(const string& node) const {
            return node == root();
        }

        bool is_leaf(const string& node) const {
            return this->edges(node).size() < 1;
        }

        vector<string>& nodes() {
            return _nodes;
        }

        vector<Pair> edges(const string& name) const {
            auto itr = _indices.find(name);
            if (itr != _indices.end() && itr->second < _edges.size()) {
                return _edges[itr->second];
            }
            return vector<Pair>();
        }

        unsigned size(const string& name) const {
            return edges(name).size();
        }

        string label(const string& name) const {
            auto itr = _labels.find(name);
            if (itr != _labels.end()) {
                return itr->second;
            }
            return name;
        }

        map<string, string> attrs(const string& name) {
            auto itr = _attrs.find(name);
            if (itr != _attrs.end()) {
                return itr->second;
            }
            return map<string,string>();
        }

        string find(const string& node, const string& key) {
            int depth = 0;
            return find(node, key, &depth);
        }

        string find(const string& node, const string& key, int* depth) {
            string label = this->label(node);
            if (label.find(key) != string::npos) {
                return node;
            } else {    // Visit children
                vector<Pair> edges = this->edges(node);
                int n = 0;
                for (Pair& edge : edges) {
                    *depth += 1;
                    string sub = find(edge.first, key, depth);
                    if (!sub.empty()) {
                        return sub;
                    }
                    n += 1;
                }
            }
            return "";
        }

        string node(string name) {
            return node(name, name);
        }

        string node(string name, int label, initializer_list<string> attrs = {}) {
            return node(name, to_string(label), attrs);
        }

        string node(string name, const string& label, initializer_list<string> attrs = {}) {
            // Find a unique name, labels can be repeated.
            string uname = name;
            auto itr = _indices.find(uname);
            unsigned count = 0;
            while (itr != _indices.end()) {
                uname = name + to_string(count);
                itr = _indices.find(uname);
                count += 1;
            }

            name = uname;
            //if (itr == _indices.end()) {
                _indices[name] = _nodes.size();
                _nodes.push_back(name);
            //}

            _labels[name] = label;

            unsigned nattrs = attrs.size();
            if (nattrs > 0) {
                vector<string> vec(attrs.begin(), attrs.end());
                for (unsigned i = 0; i < nattrs; i += 2) {
                    string key = vec[i];
                    string val;
                    if (i + 1 < nattrs) {
                        val = vec[i + 1];
                    }
                    attr(name, key, val);
                }
            }

            return name;
        }

        int index(const string& name) {
            auto itr = _indices.find(name);
            if (itr != _indices.end()) {
                return itr->second;
            }
            return -1;
        }

        void edge(const string& src, const string& dest, const int label) {
            edge(src, dest, to_string(label));
        }

        void edge(const string& src, const string& dest, const string& label) {
            auto itr = _indices.find(src);
            unsigned ndx;
            if (itr == _indices.end()) {
                ndx = _indices.size();
                _indices[src] = ndx;
                _nodes.push_back(src);
            } else {
                ndx = itr->second;
            }
            if (ndx >= _edges.size()) {
                _edges.resize(_indices.size());
            }
            _edges[ndx].push_back(make_pair(dest, label));
        }

        void attr(const string& name, const string& val) {
            _attrs["Digraph"][name] = val;
        }

        void attr(const string& node, const string& name, const string& val) {
            _attrs[node][name] = val;
        }

        string to_dot() {
            ostringstream os;
            os << "// " << _name << "\n";
            os << "digraph {\n";

            for (const string& name : _nodes) {
                string label = this->label(name);
                map<string, string> attrs = _attrs[name];
                os << "  " << name << " [label=\"" << label << "\"";
                for (auto itr : attrs) {
                    os << " " << itr.first << "=\"" << itr.second << "\"";
                }
                os << "]\n";

            }

            for (const string& name : _nodes) {
                unsigned ndx = _indices[name];
                vector<Pair> edges = _edges[ndx];
                for (Pair e : edges) {
                    string dest = e.first;
                    string label = e.second;
                    os << "  " << name << " -> " << dest << " [label=\"" << label << "\"]\n";
                }
            }

            os << "}";
            return os.str();
        }

    private:
        string _name;
        vector<vector<Pair>> _edges;
        map<string, unsigned> _indices;
        vector<string> _nodes;
        map<string, string> _labels;
        map<string, map<string, string> > _attrs;
    };
}

#endif // _DIGRAPH_HPP_
