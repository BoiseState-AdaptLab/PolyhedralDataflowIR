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
    class Digraph {
        typedef pair<string, string> Pair;

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

        void node(const string& name, int label = 0, initializer_list<string> attrs = {}) {
            node(name, to_string(label), attrs);
        }

        void node(const string& name, const string& label = "", initializer_list<string> attrs = {}) {
            auto itr = _indices.find(name);
            if (itr == _indices.end()) {
                _indices[name] = _nodes.size();
                _nodes.push_back(name);
            }

            if (!label.empty()) {
                _labels[name] = label;
            }

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
