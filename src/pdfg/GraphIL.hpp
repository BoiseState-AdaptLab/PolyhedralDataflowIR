//
// Created by edavis on 4/8/19.
//
#pragma once
#ifndef POLYEXT_GRAPHIL_H
#define POLYEXT_GRAPHIL_H

#include <algorithm>
using std::find;
#include <array>
using std::array;
#include <deque>
using std::deque;
#include <initializer_list>
using std::initializer_list;
#include <iostream>
using std::istream;
using std::ostream;
using std::cerr;
using std::endl;
#include <map>
using std::map;
#include <sstream>
using std::istringstream;
using std::ostringstream;
#include <string>
using std::string;
using std::to_string;
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;
#include <vector>
using std::vector;
#include <poly/PolyLib.hpp>
using poly::PolyLib;

namespace pdfg {
    template<class T>
    string stringify(const T& obj) {
        ostringstream os;
        os << obj;
        return os.str();
    }

    template<class T>
    T unstring(const string& str) {
        T out;
        istringstream is(str);
        is >> out;
        return out;
    }

    template<class T>
    vector<string> stringify(const vector<T>& vec) {
        vector<string> out;
        for (const T& obj : vec) {
            out.push_back(stringify<T>(obj));
        }
        return out;
    }

    void init(const string &name);

    struct Expr {
    public:
        explicit Expr(const string &text = "", char type = 'E'): _text(text), _type(type) {
        }

        Expr(const Expr &other) {
            copy(other);
        }

        virtual Expr &operator=(const Expr &other) {
            copy(other);
            return *this;
        }

        virtual bool is_iter() const {
            return _type == 'I';
        }

        virtual bool is_func() const {
            return _type == 'F' || _type == 'S';    // UFs & SCs
        }

        virtual bool is_scalar() const {
            return _type == 'N' || _type == 'R';    // UFs & SCs
        }

        virtual bool is_space() const {
            return _type == 'P';
        }

        virtual string text() const {
            return _text;
        }

        bool empty() const {
            return _text.empty();
        }

        char type() const {
            return _type;
        }

        virtual bool operator<(const Expr &other) const {
            return _text < other._text;     // Lexicographic ordering...
        }

        virtual bool equals(const Expr &other) const {
            return _text == other._text;
        }

        friend istream &operator>>(istream &is, Expr &expr) {
            is >> expr._text;
            return is;
        }

    protected:
        void copy(const Expr &other) {
            _text = other.text();
            _type = other.type();
        }

        string _text;
        char _type;
    };

    ostream &operator<<(ostream &os, const Expr &expr) {
        os << expr.text();
        return os;
    }

    static Expr NullExpr;

    struct Int : public Expr {
    public:
        explicit Int(int val = 0) {
            _val = val;
            _text = to_string(val);
            _type = 'N';
        }

        explicit Int(const Expr &expr) : Int(unstring<int>(expr.text())) {
        }

        Int(const Int &other) {
            copy(other);
        }

        Int &operator=(const Expr &expr) override {
            auto intexp = dynamic_cast<const Int *>(&expr);
            copy(*intexp); // calling Derived::copy()
            return *this;
        }

    protected:
        void copy(const Int &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _val = other._val;
        }

        int _val;
    };

    struct Real : public Expr {
    public:
        explicit Real(double val = 0.) {
            _val = val;
            _text = to_string(val);
            _type = 'R';
        }

        Real(const Real &other) {
            copy(other);
        }

        explicit Real(const Expr &expr) : Real(unstring<double>(expr.text())) {
        }

        Real &operator=(const Expr &expr) override {
            auto real = dynamic_cast<const Real *>(&expr);
            copy(*real); // calling Derived::copy()
            return *this;
        }

    protected:
        void copy(const Real &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _val = other._val;
        }

        double _val;
    };

    struct Pointer : public Expr {
    public:
        explicit Pointer(void* ptr = nullptr) {
            _ptr = ptr;
            _text = to_string(ptr);
            _type = 'O';
        }

        Pointer(const Pointer &other) {
            copy(other);
        }

        explicit Pointer(const Expr &expr) : Pointer(unstring<void*>(expr.text())) {
        }

        Pointer &operator=(const Expr &expr) override {
            auto ptr = dynamic_cast<const Pointer*>(&expr);
            copy(*ptr); // calling Derived::copy()
            return *this;
        }

    protected:
        void copy(const Pointer &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _ptr = other._ptr;
        }

        void* _ptr;
    };

    void addSpace(const Expr& expr);

    struct Math : public Expr {
    public:
        explicit Math() {
            _oper = "";
        }

        explicit Math(const Expr &lhs, const Expr &rhs, const string &oper) {
            _lhs = lhs;
            _rhs = rhs;
            _oper = oper;
            _text = stringify<Math>(*this);
            _type = 'M';
        }

        Math(const Math &other) {
            copy(other);
        }

        explicit Math(const Expr &expr) : Math(unstring<Math>(expr.text())) {
        }

        Math &operator=(const Expr &expr) override {
            auto math = dynamic_cast<const Math *>(&expr);
            copy(*math); // calling Derived::copy()
            return *this;
        }

        Math operator()(const Expr &expr) {
            return Math(*this, expr, "(");
        }

        Expr lhs() const {
            return _lhs;
        }

        Expr rhs() const {
            return _rhs;
        }

        string oper() const {
            return _oper;
        }

        friend istream &operator>>(istream &is, Math& math) {
            char chr;
            string lhs, rhs, oper;
            while (is.get(chr)) {
                if (Strings::isOperator(chr)) {
                    oper += chr;
                } else if (!oper.empty()) {
                    rhs += chr;
                } else {
                    lhs += chr;
                }
            }
            math._lhs = Expr(lhs);
            math._oper = oper;
            if (Strings::isDigit(rhs)) {
                math._rhs = Int(unstring<int>(rhs));
            } else if (Strings::isNumber(rhs)) {
                math._rhs = Real(unstring<double>(rhs));
            } else {
                math._rhs = Expr(rhs);
            }
            return is;
        }

    protected:
        void copy(const Math &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _lhs = other._lhs;
            _rhs = other._rhs;
            _oper = other._oper;
        }

        Expr _lhs;
        Expr _rhs;
        string _oper;
    };

    Math abs(const Expr& expr) {
        return Math(expr, Int(0), "abs(");
    }

    Math min(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "min(");
    }

    Math max(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "max(");
    }

    Math absmin(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "absmin(");
    }

    Math absmax(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "absmax(");
    }

    Math pow(const Expr &lhs, int exp) {
        return Math(lhs, Int(exp), "pow(");
    }

    Math pow(const Expr &lhs, double exp) {
        return Math(lhs, Real(exp), "pow(");
    }

    Math pow(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "pow(");
    }

    Math pinv(const Expr& expr) {
        return Math(expr, Int(0), "pinv(");
    }

    Math urand(const Expr& expr) {
        return Math(Int(0), expr, "urand(");
    }

    Math urand() {
        return urand(Int(1));
    }

    Math sqrt(const Expr& expr) {
        return Math(NullExpr, expr, "sqrt(");
    }

    Math paren(const Expr& expr) {
        return Math(NullExpr, expr, "(");
    }

    vector<Math> operator^(const Math& lhs, const Math& rhs) {
        return vector<Math>({lhs, rhs});
    }

    Math operator+(const Expr &expr, const unsigned val) {
        return Math(expr, Int(val), "+");
    }

//    Math operator+(const Expr& expr, const double val) {
//        return Math(expr, Real(val), "+");
//    }

    Math operator+(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "+");
    }

    Math operator+=(const Expr &expr, const unsigned val) {
        addSpace(expr);
        return Math(expr, Int(val), "+=");
    }

//    Math operator+=(const Expr& expr, const double val) {
//        return Math(expr, Real(val), "+=");
//    }

    Math operator+=(const Expr &lhs, const Expr &rhs) {
//        if (lhs.empty()) {
//            return Math(rhs, Int(0), "+");
//        } else {
//            return Math(lhs, rhs, "+");
//        }
        addSpace(lhs);
        return Math(lhs, rhs, "+=");
    }

    Math operator-(const Expr &expr, const unsigned val) {
        return Math(expr, Int(val), "-");
    }

    Math operator-(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "-");
    }

    Math operator-=(const Expr &expr, const unsigned val) {
        addSpace(expr);
        return Math(expr, Int(val), "-=");
    }

    Math operator-=(const Expr &lhs, const Expr &rhs) {
        addSpace(lhs);
        return Math(lhs, rhs, "-=");
    }

    Math operator*(const Expr &expr, const unsigned val) {
        return Math(expr, Int(val), "*");
    }

    Math operator*(const Expr &expr, const double val) {
        return Math(expr, Real(val), "*");
    }

    Math operator*(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "*");
    }

    Math operator*=(const Expr &expr, const unsigned val) {
        addSpace(expr);
        return Math(expr, Int(val), "*=");
    }

    Math operator*=(const Expr &lhs, const Expr &rhs) {
        addSpace(lhs);
        return Math(lhs, rhs, "*=");
    }

    Math operator/(const Expr &expr, const unsigned val) {
        Math math(expr, Int(val), "/");
        return math;
    }

    Math operator/(const Expr &lhs, const Expr &rhs) {
        return Math(lhs, rhs, "/");
    }

    Math operator/=(const Expr &expr, const unsigned val) {
        addSpace(expr);
        return Math(expr, Int(val), "/=");
    }

    Math operator/=(const Expr &lhs, const Expr &rhs) {
        addSpace(lhs);
        return Math(lhs, rhs, "/=");
    }

    Math operator%(const Expr &expr, const unsigned val) {
        Math math(expr, Int(val), "%");
        return math;
    }

    ostream &operator<<(ostream &os, const Math &math) {
        string oper = math.oper();
        if (oper.find('(') == string::npos) {
            string lhs = stringify<Expr>(math.lhs());
            // Wrap parens around sums to ensure proper precedence
            if ((oper == "*" || oper == "/" || oper == "%") &&
                (lhs.find('+') != string::npos || lhs.find('-') != string::npos)) {
                lhs = "(" + lhs + ")";
            }
            os << lhs;
            if (!math.rhs().empty()) {
                string rhs = stringify<Expr>(math.rhs());
                // Remove superfluous identity ops, e.g., *1 or +0.
                if (!(oper == "*" && rhs == "1") && !(oper == "/" && rhs == "1") && !(oper == "%" && rhs == "1") &&
                    !(oper == "+" && rhs == "0") && !(oper == "-" && rhs == "0")) {
                    if (oper == "-" && rhs[0] == '-') {
                        oper = "+";
                        rhs = rhs.substr(1);
                    }
                    os << oper << rhs;
                }
            }
        } else {
            os << oper;
            if (!math.lhs().empty()) {
                os << math.lhs() << ',';
            }
            os << math.rhs() << ')';
        }
        return os;
    }

    struct Iter;
    void addIterator(const Iter &iter);

    struct Iter : public Expr {
    public:
        explicit Iter(const string &name = "") {
            _name = _text = name;
            _type = 'I';
            addIterator(*this);
        }

        explicit Iter(const Expr &expr) : Iter(expr.text()) {
        }

        explicit Iter(char chr) : Iter(string(1, chr)) {
        }

        Iter(const Iter &other) {
            copy(other);
        }

        Iter &operator=(const Expr &expr) override {
            auto iter = dynamic_cast<const Iter *>(&expr);
            copy(*iter); // calling Derived::copy()
            return *this;
        }

        bool equals(const Iter &other) const {
            return _name == other._name;
        }

        bool operator<(const Iter &other) const {
            return _name < other._name;     // Lexicographic ordering...
        }

        bool operator==(const Iter &other) {
            return _text == other._text;
        }

        explicit operator int() const {
            return (int) _text.size();
        }

        bool is_iter() const override {
            return true;
        }

        string name() const {
            return _name;
        }

    protected:
        void copy(const Iter &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _name = other._name;
        }

        string _name;
    };

    struct Func;
    void addFunction(const Func &func);

    struct Func : public Expr {
    public:
        explicit Func(const string &name = "", const unsigned arity = 1, initializer_list<Expr> args = {}) {
            _name = name;
            if (args.size() > 0) {
                for (const auto &arg : args) {
                    _args.push_back(arg);
                }
                _arity = (unsigned) _args.size();
            } else {
                _arity = arity;
            }
            _type = 'F';
            _text = stringify<Func>(*this);
            addFunction(*this);
        }

        Func(const Func &other) : _arity(other._arity) {
            copy(other);
            _text = stringify<Func>(*this);
        }

        Func(const Expr &expr) {
            _arity = 0;
            _type = expr.type();
            string text = expr.text();
            if (_type == 'A') {
                size_t pos = text.find('[');
                if (pos != string::npos) {
                    text = text.substr(0, pos);
                }
            }
            istringstream is(text);
            is >> *this;
        }

        Func &operator=(const Expr &expr) override {
            auto func = dynamic_cast<const Func *>(&expr);
            copy(*func); // calling Derived::copy()
            return *this;
        }

        Math operator=(const Math &math) {
            return Math(*this, math, "=");
        }

        Func operator()(const Expr &arg) {
            return Func(_name, _arity, {arg});
        }

        Func operator()(const Expr &arg1, const Expr &arg2) {
            return Func(_name, _arity, {arg1, arg2});
        }

        Func operator()(const int arg1, const Expr &arg2) {
            return Func(_name, _arity, {Int(arg1), arg2});
        }

        Func operator()(const Expr &arg1, const int arg2) {
            return Func(_name, _arity, {arg1, Int(arg2)});
        }

        Func operator()(initializer_list<Expr> args) {
            return Func(_name, _arity, args);
        }

        string name() const {
            return _name;
        }

        void name(const string& name) {
            _name = name;
        }

        virtual void eval() {
            _text = stringify<Func>(*this);
        }

        unsigned arity() const {
            return _arity;
        }

        Expr arg(unsigned pos) const {
            return _args[pos];
        }

        vector<Expr> args() const {
            return _args;
        }

        void add(const vector<Expr>& args) {
            for (const auto& arg : args) {
                add(arg);
            }
        }

        void add(const Expr& arg) {
            _args.push_back(arg);
            _arity = _args.size();
        }

        friend istream &operator>>(istream &is, Func &func) {
            func._arity = 0;
            is >> func._text;
            bool inArgs = false;
            string arg;
            for (char chr : func._text) {
                if (chr == '(' || chr == '[') {
                    inArgs = true;
                } else if (chr == ')' || chr == ']') {
                    inArgs = false;
                } else if (inArgs) {
                    if (chr == ',') {
                        func._arity += 1;
                        func._args.emplace_back(Expr(arg));
                        arg = "";
                    } else {
                        arg += chr;
                    }
                } else {
                    func._name += chr;
                }
            }
            if (!arg.empty()) {
                func._arity += 1;
                func._args.emplace_back(Expr(arg));
            }
            return is;
        }

    protected:
        void copy(const Func &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _name = other._name;
            _arity = other._arity;
            _args = other._args;
        }

        string _name;
        unsigned _arity;
        vector<Expr> _args;
    };

    ostream &operator<<(ostream &os, const Func &func) {
        os << func.name() << '(';
        unsigned i = 0;
        vector<Expr> args = func.args();
        for (const auto &arg : args) {
            os << arg;
            if (i < args.size() - 1) {
                os << ',';
            }
            i += 1;
        }
        os << ')';
        return os;
    }

    struct Const;
    void addConstant(const Const &con);

    struct Const : public Func {
    public:
        explicit Const(const string &name = "", const int val = 0) : Func(name, 0) {
            _val = val;
            _type = 'S';
            if (val == 0) {
                _text = name;
            } else {
                _text = to_string(val);
            }
            addConstant(*this);
        }

        explicit Const(const char chr, const int val = 0) : Const(string(1, chr), val) {
        }

        Const(const Const &other) {
            copy(other);
        }

        virtual Const &operator=(const Expr& expr) {
            auto con = dynamic_cast<const Const *>(&expr);
            copy(*con); // calling Derived::copy()
            return *this;
        }

        Math operator=(const Math& math) {
            return Math(*this, math, "=");
        }

        Math operator=(const Func& func) {
            return Math(*this, func, "=");
        }

        int val() const {
            return _val;
        }

    protected:
        void copy(const Const &other) {
            Func::copy(other); // Let Base::copy() handle copying Base things
            _val = other._val;
        }

        int _val;
    };

    ostream &operator<<(ostream &os, const Const &constant) {
        int val = constant.val();
        if (val != 0) {
            os << val;
        } else {
            os << constant.name();
        }
        return os;
    }

    struct Macro;
    void addMacro(Macro& macro);

    struct Macro : public Func {
    public:
        explicit Macro(const string &name = "", const vector<Expr>& args = {}, const vector<Expr>& exprs = {}) :
                 Func(name, args.size()) {
            _args = args;
            _exprs = exprs;
            _type = '#';
            _text = stringify<Macro>(*this);
        }

        explicit Macro(const string &name, const vector<Iter>& iters, const vector<Expr>& exprs = {}) : Macro(name) {
            _args = vector<Expr>(iters.begin(), iters.end());
            _exprs = exprs;
        }

        Macro(const Macro& other) {
            copy(other);
        }

        virtual Macro &operator=(const Func& func) {
            auto macro = dynamic_cast<const Macro*>(&func);
            copy(*macro); // calling Derived::copy()
            return *this;
        }

        vector<Expr> expressions() const {
            return _exprs;
        }

        virtual void eval() {
            _text = stringify<Macro>(*this);
        }

    protected:
        void copy(const Macro& other) {
            Func::copy(other); // Let Base::copy() handle copying Base things
            _exprs = other._exprs;
        }

        vector<Expr> _exprs;
    };

    ostream &operator<<(ostream &os, const Macro& macro) {
        os << "{\\\n";
        vector<Expr> exprs = macro.expressions();
        for (unsigned i = 0; i < exprs.size(); i++) {
            string expr = exprs[i].text();
            os << expr << ";\\\n";
        }
        os << '}';
        return os;
    }

    Math call(const Func& func) {
        return Math(NullExpr, func, "");
    }

    Math call(Macro& macro) {
        addMacro(macro);
        macro.eval();
        return Math(NullExpr, macro, "");
    }

    Math call(const Expr& retval, const Func& func) {
        return Math(retval, func, "=");
    }

    struct Constr : public Expr {
    public:
        explicit Constr() {
            _text = "";
            _type = 'C';
        }

        explicit Constr(const Expr &lhs, const Expr &rhs, const string &relop = "", bool inexists = false) {
            _lhs = lhs;
            _rhs = rhs;
            _relop = relop;
            _type = 'C';
            _inexists = inexists;
            _text = stringify<Constr>(*this);
        }

        Constr(const Constr &other) {
            copy(other);
        }

        Constr &operator=(const Expr &expr) override {
            auto constr = dynamic_cast<const Constr *>(&expr);
            copy(*constr); // calling Derived::copy()
            return *this;
        }

        bool operator==(const Constr &other) {
            return _text == other._text;
        }

        Expr lhs() const {
            return _lhs;
        }

        void lhs(const Expr& expr) {
            _lhs = expr;
        }

        Expr rhs() const {
            return _rhs;
        }

        void rhs(const Expr& expr) {
            _rhs = expr;
        }

        string relop() const {
            return _relop;
        }

        void relop(const string& relop) {
            _relop = relop;
        }

        bool contains(const Iter& iter) const {
            return (_lhs.text().find(iter.text()) != string::npos || _rhs.text().find(iter.text()) != string::npos);
        }

        bool inexists() const {
            return _inexists;
        }

        void inexists(bool exists) {
            _inexists = exists;
        }

    protected:
        void copy(const Constr &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _lhs = other._lhs;
            _rhs = other._rhs;
            _relop = other._relop;
            _inexists = other._inexists;
        }

        Expr _lhs;
        Expr _rhs;
        string _relop;
        bool _inexists;
    };

    Constr operator<=(int val, const Iter &iter) {
        Constr constr(Int(val), iter, "<=");
        return constr;
    }

    Constr operator<(int val, const Iter &iter) {
        Constr constr(Int(val), iter, "<");
        return constr;
    }

    Constr operator>=(int val, const Iter &iter) {
        Constr constr(Int(val), iter, ">=");
        return constr;
    }

    Constr operator>(int val, const Iter &iter) {
        Constr constr(Int(val), iter, ">");
        return constr;
    }

    Constr operator<=(const Expr &lhs, const Expr &rhs) {
        Constr constr(lhs, rhs, "<=");
        return constr;
    }

    Constr operator<(const Expr &lhs, const Expr &rhs) {
        Constr constr(lhs, rhs, "<");
        return constr;
    }

    Constr operator>=(const Expr &lhs, const Expr &rhs) {
        Constr constr(lhs, rhs, ">=");
        return constr;
    }

    Constr operator>(const Expr &lhs, const Expr &rhs) {
        Constr constr(lhs, rhs, ">");
        return constr;
    }

    Constr operator==(const Iter &iter, const int val) {
        Constr constr(iter, Int(val), "=");
        return constr;
    }

    Constr operator==(const Expr &lhs, const Expr &rhs) {
        Constr constr(lhs, rhs, "=");
        return constr;
    }

    ostream &operator<<(ostream &os, const Constr &constr) {
        if (!constr.lhs().text().empty() && !constr.rhs().text().empty()) {
            os << constr.lhs() << ' ' << constr.relop() << ' ' << constr.rhs();
        }
        return os;
    }

    struct Range : public Constr {
    public:
        explicit Range(const Constr &lower, const Constr &upper) {
            _lower = lower;
            _upper = upper;
            _type = 'G';
            transClosure();
        }

        Range(const Range &other) {
            copy(other);
        }

        virtual Range &operator=(const Constr &constr) {
            auto range = dynamic_cast<const Range *>(&constr);
            copy(*range); // calling Derived::copy()
            return *this;
        }

        Constr lower() const {
            return _lower;
        }

        Constr upper() const {
            return _upper;
        }

        Constr trans() const {
            return _trans;
        }

    protected:
        void copy(const Range &other) {
            Constr::copy(other); // Let Base::copy() handle copying Base things
            _lower = other._lower;
            _upper = other._upper;
            _trans = other._trans;
        }

        void transClosure() {
            // Transitive Property Rules, forall a, b, c in |R:
            // a >= b ^ b >= c -> a >= c (1)
            // a <= b ^ b <= c -> a <= c (2)
            // If either is a strict inequality, then result is a strict inequality:
            // a >= b ^ b > c -> a > c   (3)
            // a > b ^ b >= c -> a > c   (4)
            // a <= b ^ b < c -> a < c   (5)
            // a < b ^ b <= c -> a < c   (6)
            // a = b -> a >= b:
            // a = b ^ b > c -> a > c    (7)
            // a > b ^ b = c -> a > c    (8)
            // a = b -> a <= b:
            // a = b ^ b < c -> a < c    (9)
            // a < b ^ b = c -> a < c   (10)
            //if (!Strings::isDigit(_upper.rhs().text())) {
            _trans = Constr(_lower.lhs(), _upper.rhs(), _upper.relop());
            //}
        }

        Constr _lower;
        Constr _upper;
        Constr _trans;       // Transitive property...
    };

    vector<Constr> operator^(const Range &lhs, const Range &rhs) {
        return vector<Constr>({lhs.lower(), lhs.upper(), lhs.trans(),
                               rhs.lower(), rhs.upper(), rhs.trans()});
    }

    vector<Constr> operator^(const Range &lhs, const Constr &rhs) {
        return vector<Constr>({lhs.lower(), lhs.upper(), lhs.trans(), rhs});
    }

    vector<Constr> operator^(const Constr &lhs, const Range &rhs) {
        return vector<Constr>({lhs, rhs.lower(), rhs.upper(), rhs.trans()});
    }

    vector<Constr> operator^(const vector<Constr> &lhs, const Range &rhs) {
        vector<Constr> cpy = lhs;
        cpy.push_back(rhs.lower());
        cpy.push_back(rhs.upper());
        cpy.push_back(rhs.trans());
        return cpy;
    }

    vector<Constr> operator^(const vector<Constr> &lhs, const Constr &rhs) {
        vector<Constr> cpy = lhs;
        cpy.push_back(rhs);
        return cpy;
    }

    Range operator<=(const Constr &constr, const Expr &expr) {
        Range range(constr, Constr(constr.rhs(), expr, "<="));
        return range;
    }

    Range operator<(const Constr &constr, const Expr &expr) {
        Range range(constr, Constr(constr.rhs(), expr, "<"));
        return range;
    }

    Range operator>=(const Constr &constr, const Expr &expr) {
        Range range(constr, Constr(constr.rhs(), expr, ">="));
        return range;
    }

    Range operator>(const Constr &constr, const Expr &expr) {
        Range range(constr, Constr(constr.rhs(), expr, ">"));
        return range;
    }

    ostream &operator<<(ostream &os, const Range &range) {
        os << range.lower() << " && " << range.upper();
        return os;
    }

    struct Space;
    void addSpace(const Space &space);

    struct Access;
    void addAccess(const Access& access);

    struct Access : public Expr {
    public:
        Access(const Space &space, initializer_list<Expr> tuple, char refchar = '(', const vector<int>& offsets = {}) :
            _space(space) {
            init(tuple, refchar, offsets);
            _text = stringify<Access>(*this);
        }

        Access(const Space &space, const vector<Expr>& tuple, char refchar = '(', const vector<int>& offsets = {}) :
            _space(space) {
            init(tuple, refchar, offsets);
            _text = stringify<Access>(*this);
        }

        Access(const Access &other) : _space(other._space) {
            copy(other);
        }

        Access &operator=(const Expr &expr) override {
            auto access = dynamic_cast<const Access*>(&expr);
            copy(*access); // calling Derived::copy()
            return *this;
        }

        Math operator=(const Math& math) {
            return Math(*this, math, "=");
        }

        const Space &space() const {
            return _space;
        }

        char refchar() const {
            return _refchar;
        }

        vector<Expr> tuple() const {
            return _tuple;
        }

        vector<int> offsets() const {
            return _offsets;
        }

    protected:
        void init(initializer_list<Expr> tuple, char refchar = '(', const vector<int>& offsets = {}) {
            init(Lists::initListToVec<Expr>(tuple), refchar, offsets);
        }

        void init(const vector<Expr>& tuple, char refchar = '(', const vector<int>& offsets = {}) {
            _refchar = refchar;
            _offsets = offsets;
            _type = 'A';
            for (const auto &expr : tuple) {
                _tuple.push_back(expr);
            }
            addSpace(_space);
            addAccess(*this);
        }

        void copy(const Access& other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _refchar = other._refchar;
            _tuple = other._tuple;
            _offsets = other._offsets;
        }

        const Space &_space;
        char _refchar;
        vector<Expr> _tuple;
        vector<int> _offsets;
    };

    unsigned _iter_counter = 0;
    unsigned _space_counter = 0;

    map<string, Space*> _spaces;

    struct Space : public Expr {
    public:
        explicit Space(const string &name = "", initializer_list<Iter> iterators = {}) {
            init(name, iterators, {});
        }

        Space(const string &name, const int upper) {
            init(name, Int(0), Int(upper));
        }

        Space(const string &name, const Expr &upper) {
            init(name, Int(0), upper);
        }

        Space(const string &name, const Range &range) {
            init(name, {range.lower(), range.upper(), range.trans()});
        }

        Space(const string &name, const int lower, const Expr &upper) {
            init(name, Int(lower), upper);
        }

        Space(const string &name, const Expr &upper1, const Expr &upper2) {
            init(name, Int(0), upper1, Int(0), upper2);
        }

        Space(const string &name, const Expr &upper1, const Expr &upper2, const Expr &upper3) {
            init(name, Int(0), upper1, Int(0), upper2, Int(0), upper3);
        }

        Space(const string &name, int lower1, int upper1, int lower2, int upper2) {
            init(name, Int(lower1), Int(upper1), Int(lower2), Int(upper2));
        }

        Space(const string &name, const Expr &lower1, const Expr &upper1, const Expr &lower2, const Expr &upper2) {
            init(name, lower1, upper1, lower2, upper2);
        }

//        Space(const string& name, const int lower1, const Expr& upper1, const int lower2, const Expr& upper2) {
//            init(name, Int(lower1), upper1, Int(lower2), upper2);
//        }
//
//        Space(const string& name, const Expr& lower1, const Expr& upper1, const Expr& lower2, const Expr& upper2) {
//            init(name, lower1, upper1, lower2, upper2);
//        }

        Space(const string &name, const vector<Iter> &iterators) {
            init(name, iterators, {});
        }

//        Space(const string& name, initializer_list<Iter> iterators) {
//            init(name, iterators, {});
//        }
//
//        Space(const string& name, initializer_list<Iter> iterators, initializer_list<Constr> constraints) {
//            init(name, iterators, constraints);
//        }
//
        Space(const string &name, const vector<Iter> &iterators, const vector<Constr> &constraints) {
            init(name, iterators, constraints);
        }
//
//        Space(const string& name, initializer_list<Constr> constraints) {
//            init(name, constraints);
//        }


        Space(const string &name, const vector<Constr> &constraints) {
            init(name, constraints);
        }

        Space(const string &name, const vector<Range> &ranges) {
            init(name, ranges);
        }

        Space(const Space &other) {
            copy(other);
        }

        Space &operator=(const Expr &expr) override {
            auto space = dynamic_cast<const Space *>(&expr);
            copy(*space); // calling Derived::copy()
            return *this;
        }

        Math operator=(const Math& math) {
            addSpace(*this);
            return Math(*this, math, "=");
        }

        string text() const override {
            if (_text.empty()) {
                return _name;
            }
            return _text;
        }

        // overload function calls
        Access operator()(int index) {
            Iter iter("i" + to_string(_iter_counter++));
            return Access(*this, {Int(index)}, '(');
        }

        Access operator()(const Expr &one) {
            return Access(*this, {one}, '(');
        }

        Access operator()(const Expr &one, const Expr &two) {
            return Access(*this, {one, two}, '(');
        }

        Access operator()(const Expr &one, const Expr &two, const Expr &three) {
            return Access(*this, {one, two, three}, '(');
        }

        Access operator()(initializer_list<Expr> tuple) {
            return Access(*this, tuple, '(');
        }

        // overload array indexing
        Access operator[](int index) {
            return Access(*this, {Int(index)}, '[');
        }

        Access operator[](const Expr &one) {
            return Access(*this, {one}, '[');
        }

        Access operator[](initializer_list<Expr> tuple) {
            return Access(*this, tuple, '[');
        }

        void operator^=(const Range &rng) {
            add(rng);
        }

        void operator^=(const Constr &constr) {
            add(constr);
        }

        string name() const {
            return _name;
        }

        void name(const string &name) {
            _name = name;
        }

        vector<Iter> iterators() const {
            return _iterators;
        }

        void iterators(const vector<Iter>& iters) {
            _iterators = iters;
        }

        vector<Constr> constraints(const string &itername = "") const {
            vector<Constr> constraints;
            if (!itername.empty()) {
                auto itr = _itermap.find(itername);
                if (itr != _itermap.end()) {
                    for (const auto &ndx : itr->second) {
                        constraints.push_back(_constraints[ndx]);
                    }
                }
            } else {
                constraints = _constraints;
            }
            return constraints;
        }

        void add(const Iter &iter) {
            if (find(_iterators.begin(), _iterators.end(), iter) == _iterators.end()) {
                _iterators.push_back(iter);
            }
        }

        void add(const Range &range) {
            add(range.lower());
            add(range.upper());
            add(range.trans());
        }

        void add(const vector<Constr>& constraints) {
            for (const auto& constr : constraints) {
                add(constr);
            }
        }

        void add(const Constr &constr) {
            if (!has(constr)) {
//            if (constr.lhs().is_iter()) {
//                add((Iter(constr.lhs().text())));
//            }
                if (constr.rhs().is_iter()) {
                    add((Iter(constr.rhs().text())));
                }
                _constraints.push_back(constr);
            }
        }

        bool has(const Iter& other) {
            bool exists = false;
            for (const auto& iter : _iterators) {
                exists = (iter.name() == other.name());
                if (exists) {
                    break;
                }
            }
            return exists;
        }

        bool has(const Constr& other) {
            bool exists = false;
            for (const auto& constr : _constraints) {
                exists = (constr.equals(other));
                if (exists) {
                    break;
                }
            }
            return exists;
        }

        void set(unsigned pos, const Iter &iter) {
            if (pos < _iterators.size()) {
                _iterators[pos] = iter;
            } else {
                add(iter);
            }
        }

        void set(unsigned pos, const Constr &constr) {
            if (pos < _constraints.size()) {
                _constraints[pos] = constr;
            } else {
                add(constr);
            }
        }

        string to_iegen() const {
            ostringstream os;
            os << _name << " := {[";
            unsigned n = 0, niters = _iterators.size();
            for (const auto &iter : _iterators) {
                os << iter;
                if (n < niters - 1) {
                    os << ',';
                }
                n += 1;
            }
            os << "]: ";

            n = 0;
            unsigned nconstraints = _constraints.size();
            for (const auto &constr : _constraints) {
                os << constr;
                if (n < nconstraints - 1) {
                    os << " && ";
                }
                n += 1;
            }

            os << "}";
            return os.str();
        }

//        Space to_omega() const {
//            Space space(_name, _iterators);
//            map<string, int> counts;
////            symbolic N,col(2),rp(1),rp1(1);
////            Icsr := {[i,n,j]:j-col(i,n)=0&&i>=0&&n-rp(i)>=0&&N-1>=0&&-i+N-1>=0&&-n+rp1(i)-1>=0&&-rp(i)+rp1(i)-1>=0};
////            codegen(Icsr) given {[i,n,j]: N-1>=0&&-rp(i)+rp1(i)-1>=0};
//            //os << this->name << "(" << this->arity << ") -> (";
//
//            for (const auto &constr : _constraints) {
//                array<Expr, 2> exprs = {constr.lhs(), constr.rhs()};
//                for (unsigned i = 0; i < exprs.size(); i++) { //const auto& expr : exprs) {
//                    if (exprs[i].is_func()) {
//                        string fxn = exprs[i].text();
//                        auto pos = fxn.find('(');
//                        string fname = fxn.substr(0, pos);
//                        string fargs = fxn.substr(pos + 1);
//                        fargs = fargs.substr(0, fargs.size() - 1);
//
//                        // TODO: Cases wherein we need a new fxn...
//                        // 1) Argument is a math operator, e.g., rp(i+1)
//                        // 2) Or in general, whenever arg is not an iterator!
//                        //for (const auto& arg : )
//                        if (counts.find(fname) != counts.end()) {
//                            fname += to_string(counts[fname]);
//                            counts[fname] += 1;
//                        } else {
//                            counts[fname] = 1;
//                        }
//                    }
//                }
//                Constr newcon(exprs[0], exprs[1], constr.relop());
//                space.add(newcon);
//            }
//
//            return space;
//        }

        Math size() const {
            Math expr;
            for (unsigned n = 0; n < _constraints.size(); n += 2) {
                Constr low = _constraints[n];
                Constr high = _constraints[n+1];
                Expr lower = low.lhs();
                Expr upper = high.rhs();

                Math diff = (upper - lower);
                if (high.relop().find('=') != string::npos) {
                    diff = diff + Int(1);
                }

                Math rhs = paren(diff); //Math(NullExpr, diff, "(");
                if (expr.empty()) {
                    expr = rhs;
                } else {
                    expr = expr * rhs;
                }
            }
            return expr;
        }

        friend istream &operator>>(istream &is, Space &space) {
            string token;
            is >> token;
            space.name(token);
            bool inIter = false;
            bool inConstr = false;
            string constr;
            while (is >> token) {
                if (!inIter && token[0] == '[') {
                    inIter = true;
                    token = token.substr(1);
                } else if (!inConstr && token == ":") {
                    inConstr = true;
                }
                if (inIter) {
                    if (token[token.size() - 1] == ']') {
                        inIter = false;
                        token = token.substr(0, token.size() - 1);
                    } else {
                        token = Strings::rtrim(token, ',');
                    }
                    vector<string> iters = Strings::split(token, ',');
                    for (const string &iter : iters) {
                        space.add(Iter(iter));
                    }
                } else if (inConstr) {
                    if (token == "&&" || token == "and") {
                        // TODO: Come back to this...
                        //space.add(Constr(constr));
                    }
                    constr += token + ' ';
                }
                cerr << token << " ";
                int stop = 1;
            }
            return is;
        }

    protected:
        void copy(const Space &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _name = other._name;
            _iterators = other._iterators;
            _constraints = other._constraints;
            _itermap = other._itermap;
        }

        string _name;
        vector<Iter> _iterators;
        vector<Constr> _constraints;
        map<string, vector<size_t>> _itermap;

    private:
        void init(const string &name, const Expr &lower, const Expr &upper) {
            Iter iter("i" + to_string(_iter_counter));
            _iter_counter += 1;
            Constr lhs(lower, iter, "<=");
            Constr rhs(iter, upper, "<");
            init(name, {iter}, {lhs, rhs});
        }

        void init(const string &name, const Expr &lower1, const Expr &upper1, const Expr &lower2, const Expr &upper2) {
            Iter iter1("i" + to_string(_iter_counter));
            Iter iter2("j" + to_string(_iter_counter));
            _iter_counter += 1;
            Constr lhs1(lower1, iter1, "<=");
            Constr rhs1(iter1, upper1, "<");
            Constr lhs2(lower2, iter2, "<=");
            Constr rhs2(iter2, upper2, "<");
            init(name, {iter1, iter2}, {lhs1, rhs1, lhs2, rhs2});
        }

        void init(const string &name, const Expr &lower1, const Expr &upper1, const Expr &lower2, const Expr &upper2,
                  const Expr &lower3, const Expr &upper3) {
            Iter iter1("i" + to_string(_iter_counter));
            Iter iter2("j" + to_string(_iter_counter));
            Iter iter3("k" + to_string(_iter_counter));
            _iter_counter += 1;
            Constr lhs1(lower1, iter1, "<=");
            Constr rhs1(iter1, upper1, "<");
            Constr lhs2(lower2, iter2, "<=");
            Constr rhs2(iter2, upper2, "<");
            Constr lhs3(lower3, iter3, "<=");
            Constr rhs3(iter3, upper3, "<");
            init(name, {iter1, iter2, iter3}, {lhs1, rhs1, lhs2, rhs2, lhs3, rhs3});
        }

        void init(const string &name, initializer_list<Iter> iterators, initializer_list<Constr> constraints) {
            vector<Iter> ivec;
            for (const auto &iter : iterators) {
                ivec.push_back(iter);
            }
            vector<Constr> cvec;
            for (const auto &constr : constraints) {
                cvec.push_back(constr);
            }
            init(name, ivec, cvec);
        }

        void init(const string &name, initializer_list<Constr> constraints) {
            vector<Constr> cvec;
            for (const auto &constr : constraints) {
                cvec.push_back(constr);
            }
            init(name, cvec);
        }

        void init(const string &name, const vector<Range> &ranges) {
            vector<Constr> constrs;
            for (const auto& range : ranges) {
                constrs.emplace_back(range.lower());
                constrs.emplace_back(range.upper());
                constrs.emplace_back(range.trans());
            }
            init(name, constrs);
        }

        void init(const string &name, const vector<Constr> &constraints) {
            vector<Iter> ivec;
            vector<Constr> cvec;
            unordered_map<string, bool> vmap;
            for (const auto &constr : constraints) {
                Expr lhs = constr.lhs();
                Expr rhs = constr.rhs();
                if (lhs.is_iter() && (vmap.find(lhs.text()) == vmap.end())) {
                    ivec.push_back((Iter) lhs);
                    vmap[lhs.text()] = true;
                }
                if (rhs.is_iter() && (vmap.find(rhs.text()) == vmap.end())) {
                    ivec.push_back((Iter) rhs);
                    vmap[rhs.text()] = true;
                }
                cvec.push_back(constr);
            }
            init(name, ivec, cvec);
        }

        void init(const string &name, const vector<Iter> &iterators, const vector<Constr> &constraints) {
            _name = name;
            if (_name.empty()) {
                _name = "c" + to_string(_space_counter);
                _space_counter += 1;
            }
            _text = _name;
            _constraints = constraints;
            _type = 'P';
            for (const auto &iter : iterators) {
                string itername = iter.name();
                unsigned ncons = 0;
                for (const auto &constr : constraints) {
                    // TODO: Need to do more than check for membership to determine whether a constraint applies to an iterator.
                    if (constr.text().find(itername) != string::npos) {
                        //cerr << name << ": Adding constr(" << ncons << ") '" << constr << "' to iter '" << itername << "\n";
                        _itermap[itername].push_back(ncons);
                    }
                    ncons += 1;
                }
                _iterators.push_back(iter);
            }
            if (!_name.empty()) { // && _iterators.size() > 0) {
                _spaces[_name] = new Space(*this);
                //addSpace(*this);
            }
        }
    };

    Space operator^(const Space& lhs, const Range &rhs) {
        Space spc = lhs;
        spc.add(rhs);
        return spc;
    }

    ostream &operator<<(ostream &os, const Space &space) {
        os << space.name() << '(';
        unsigned i = 0, niters = space.iterators().size();
        for (const auto &iter : space.iterators()) {
            os << iter;
            if (i < niters - 1) {
                os << ',';
            }
            i += 1;
        }

        os << ") = { ";

        i = 0;
        unsigned nconstraints = space.constraints().size();
        if (nconstraints > 0) {
            vector<Constr> existConstrs;
            for (const auto &constr : space.constraints()) {
                if (constr.inexists()) {
                    existConstrs.push_back(constr);
                }
            }

            for (const auto &constr : space.constraints()) {
                os << constr;
                if (i < nconstraints - 1) {
                    os << " ^ ";
                }
                i += 1;
            }
        } else {
            os << "[0]";    // Potential abuse of notation...
        }
        os << " }";
        return os;
    }

    Expr findUpperBound(const Iter &iter) {
        for (const auto& keyval : _spaces) {
            Space* space = keyval.second;
            for (const auto& constr : space->constraints(iter.name())) {
                if (constr.lhs().equals(iter)) {
                    return constr.rhs();
                }
            }
        }
        return Expr();
    }

    ostream &operator<<(ostream &os, const Access &access) {
        unsigned size = access.tuple().size();
        Space space = access.space();
        os << space.name();
        if (size > 0) { //} && access.tuple().at(0).type() != 'N') {
            char refchar = access.refchar();
            os << refchar;
            if (size < 2) {
                os << access.tuple().at(0);
            } else {
                for (unsigned i = 0; i < size; i++) {
                    os << access.tuple().at(i);
                    if (i < size - 1) {
                        os << ',';
                    }
                }
            }
            os << (refchar == '[' ? ']' : ')');
        }
        return os;
    }

    Math mathSpace(const Space& space, const Access& acc, const string& oper) {
        addSpace(space);
        return Math(space, acc, oper);
    }

    Math operator+(const Space& space, const Access& acc) {
        return mathSpace(space, acc, "+");
    }

    Math operator-(const Space& space, const Access& acc) {
        return mathSpace(space, acc, "-");
    }

    Math operator*(const Space& space, const Access& acc) {
        return mathSpace(space, acc, "*");
    }

    Math operator/(const Space& space, const Access& acc) {
        return mathSpace(space, acc, "/");
    }

    Math operator%(const Space& space, const Access& acc) {
        return mathSpace(space, acc, "%");
    }

    struct Rel;
    void addRelation(const Rel &rel);

    struct Rel : public Expr {
    public:
        explicit Rel(const string &name = "", const vector<Iter> &srcIters = {}, const vector<Iter> &destIters = {}) {
            init(name, srcIters, destIters);
        }

        Rel(const string &name, initializer_list<Iter> srcIters, initializer_list<Iter> destIters) {
            init(name, srcIters, destIters);
        }

        Rel(const string &name, const Space &src, const Space &dest) {
            init(name, src, dest);
        }

        Rel(const Rel &other) {
            copy(other);
        }

        Rel &operator=(const Expr &expr) override {
            auto rel = dynamic_cast<const Rel *>(&expr);
            copy(*rel); // calling Derived::copy()
            return *this;
        }

        string name() const {
            return _name;
        }

        void name(const string name) {
            _name = name;
        }

        Space source() const {
            return _src;
        }

        void source(const Space& src) {
            _src = src;
        }

        Space dest() const {
            return _dest;
        }

        void dest(const Space& dst) {
            _dest = dst;
        }

        string to_iegen() const {
            ostringstream os;
            if (!_name.empty()) {
                os << _name << " := ";
            }

            //os << '{' << _src.name() << '[';
            os << "{[";
            vector<Iter> srcIters = _src.iterators();
            unsigned n = 0;
            unsigned niters = srcIters.size();

            for (const auto &iter : srcIters) {
                os << iter;
                if (n < niters - 1) {
                    os << ',';
                }
                n += 1;
            }

            //os << "] -> " << _dest.name() << '[';
            os << "] -> [";
            n = 0;
            vector<Iter> destIters = _dest.iterators();
            niters = destIters.size();

            for (const auto &iter : destIters) {
                os << iter;
                if (n < niters - 1) {
                    os << ',';
                }
                n += 1;
            }
            os << ']';

            vector<Constr> relCons;
            vector<Constr> existCons;
            set<Iter> existIters;
            vector<Constr> srcCons = _src.constraints();
            vector<Constr> destCons = _dest.constraints();

            for (const auto &constr : destCons) {
                if (constr.inexists()) {
                    existCons.push_back(constr);
                    array<Expr, 2> exprs = {constr.lhs(), constr.rhs()};
                    for (const auto &expr : exprs) {
                        if (expr.is_iter()) {
                            Iter iter = Iter(expr);
                            if (find(destIters.begin(), destIters.end(), iter) == destIters.end()) {
                                existIters.insert(iter);
                            }
                        }
                    }
                } else if (find(srcCons.begin(), srcCons.end(), constr) == srcCons.end()) {
                    relCons.push_back(constr);
                }
            }

            if (!existCons.empty() || !relCons.empty()) {
                os << " : ";
            }

            unsigned nconstrs = existCons.size();
            if (nconstrs > 0) {
                n = 0;
                niters = existIters.size();
                os << "exists(";

                for (const auto &iter : existIters) {
                    os << iter;
                    if (n < niters - 1) {
                        os << ',';
                    }
                    n += 1;
                }
                os << " : ";

                n = 0;
                for (const auto &constr : existCons) {
                    os << constr;
                    if (n < nconstrs - 1) {
                        os << " && ";
                    }
                    n += 1;
                }
                os << ')';
            }

            nconstrs = relCons.size();
            if (nconstrs > 0) {
                if (!existCons.empty()) {
                    os << " && ";
                }
                n = 0;
                for (const auto &constr : relCons) {
                    os << constr;
                    if (n < nconstrs - 1) {
                        os << " && ";
                    }
                    n += 1;
                }
            }

            os << '}';
            //cerr << os.str() << endl;
            return os.str();
        }

    protected:
        void copy(const Rel &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _name = other._name;
            _src = other._src;
            _dest = other._dest;
            _itermap = other._itermap;
        }

        void init(const string &name, initializer_list<Iter> srcIters, initializer_list<Iter> destIters) {
            Space src("", srcIters);
            Space dest("", destIters);
            init(name, src, dest);
        }

        void init(const string &name, const vector<Iter> &srcIters, const vector<Iter> &destIters) {
            Space src("", srcIters);
            Space dest("", destIters);
            init(name, src, dest);
        }

        void init(const string &name, const Space &src, const Space &dest) {
            _name = name;
            _src = src;
            _dest = dest;
            //build();
            addRelation(*this);
        }

        string _name;
        Space _src;
        Space _dest;
        map<string, string> _itermap;

    private:
        void build() {
            for (const auto &iter : _src.iterators()) {
                string itername = iter.name();
                vector<Constr> srcCons = _src.constraints(itername);
                vector<Constr> dstCons = _dest.constraints(itername);

                cerr << "iter: " << itername << ", src: ";
                for (const auto &constr : srcCons) {
                    cerr << constr << " && ";
                }
                cerr << ", dst: ";
                for (const auto &constr : dstCons) {
                    cerr << constr << " && ";
                }
                cerr << endl;

                // TODO: Merge constraints...
                int stop = 1;
            }
        }
    };

    ostream& operator<<(ostream& os, const Rel& rel) {
        string name = rel.name();
        Space src = rel.source();
        Space dest = rel.dest();

        if (!name.empty()) {
            os << name << " = ";
        }

        os << '{' << src.name() << '(';
        unsigned n = 0, niters = src.iterators().size();
        for (const auto &iter : src.iterators()) {
            os << iter;
            if (n < niters - 1) {
                os << ',';
            }
            n += 1;
        }

        os << ") -> " << dest.name() << '(';
        n = 0;
        niters = dest.iterators().size();
        for (const auto &iter : dest.iterators()) {
            os << iter;
            if (n < niters - 1) {
                os << ',';
            }
            n += 1;
        }
        os << ')';

        vector<Constr> relCons;
        vector<Constr> srcCons = src.constraints();
        vector<Constr> destCons = dest.constraints();
        for (const auto &constr : destCons) {
            if (find(srcCons.begin(), srcCons.end(), constr) == srcCons.end()) {
                relCons.push_back(constr);
            }
        }

        if (!relCons.empty()) {
            os << " : ";
            n = 0;
            unsigned nconstrs = relCons.size();
            for (const auto &constr : relCons) {
                os << constr;
                if (n < nconstrs - 1) {
                    os << " ^ ";
                }
                n += 1;
            }
        }
        os << '}';

        return os;
    }

    vector<Constr> exists(const vector<Constr> &incons) {
        vector<Constr> outcons;
        for (auto &incon : incons) {
            Constr outcon = incon;
            outcon.inexists(true);
            outcons.push_back(outcon);
        }
        return outcons;
    }

    struct Comp;

    void addComputation(Comp &comp);
    void fuseComps(Comp& comp1, const Comp& comp2);

    struct Comp : public Expr {
    public:
        Comp(const string& name, const vector<Constr>& constrs, const Math &statement) {
            Space space(name, constrs);
            init(name, space, {statement});
        }

        Comp(const vector<Constr>& constrs, const Math &statement) {
            Space space("", constrs);
            init(space, {statement});
        }

        Comp(const string& name, Space &space, const Math &statement) { //: _space(space) {
            init(name, space, {statement});
        }

        Comp(const Space &space, const Math &statement) : _space(space) {
            init(space, {statement});
        }

        Comp(const Space &space, const Math &statement, const Constr &guard) : _space(space) {
            init(space, {statement}, {guard});
        }

        Comp(const Space &space, initializer_list<Math> statements) : _space(space) {
            init(space, statements);
        }

        Comp(const Space &space, initializer_list<Math> statements, initializer_list<Constr> guards) : _space(space) {
            init(space, statements, guards);
        }

        Comp(const Space &space, const Math &expr, const vector<Constr> &guards) : _space(space) {
            init(space, {expr}, guards);
        }

        Comp(const Space &space, const vector<Math> &statements, const Constr &guard) : _space(space) {
            init(space, statements, {guard});
        }

        Comp(const string& name, Space &space, const vector<Math> &statements) { //: _space(space) {
            init(name, space, statements, {});
        }

        Comp(const Space &space, const vector<Math> &statements) : _space(space) {
            init(space, statements, {});
        }

        Comp(const Space &space, const vector<Math> &statements, const vector<Constr> &guards) : _space(space) {
            init(space, statements, guards);
        }

        Comp(const Comp &other) : _space(other._space) {
            copy(other);
        }

        Comp& operator=(const Comp &other) {
            copy(other);
            return *this;
        }

        Comp& operator=(const Expr &expr) override {
            auto comp = dynamic_cast<const Comp *>(&expr);
            copy(*comp); // calling Derived::copy()
            return *this;
        }

        const string name() const {
            return _space.name();
        }

        void name(const string& name) {
            _space.name(name);
        }

        const Space &space() const {
            return _space;
        }

        void space(const Space &space) {
            _space = space;
        }

        vector<Math> statements() const {
            return _statements;
        }

        vector<Constr> guards() const {
            return _guards;
        }

        vector<Rel> schedules() const {
            return _schedules;
        }

        deque<Rel> transforms() const {
            return _transforms;
        }

        void add(const Constr& constr) {
            _guards.push_back(constr);
        }

        void add(const Math& statement) {
            _statements.push_back(statement);
        }

        Comp fuse(Comp &other) {
            vector<vector<Iter> > schedfxns;
            for (const Rel& rel : _schedules) {
                schedfxns.push_back(rel.dest().iterators());
            }
            for (const Rel& rel : other._schedules) {
                schedfxns.push_back(rel.dest().iterators());
            }

            unsigned maxiter = 0;
            for (unsigned i = 0; i < schedfxns.size(); i++) {
                unsigned niter = schedfxns[i].size();
                maxiter = (niter > maxiter) ? niter : maxiter;
            }

            // Find the deepest common level
            unsigned level = 0;
            unsigned n = 0;
            for (; n < maxiter; n++) {
                if (n < schedfxns[0].size()) {
                    unsigned ndx = 0;
                    Iter iter = schedfxns[0][n];
                    for (unsigned i = 1; i < schedfxns.size() && ndx < 1; i++) {
                        if (!(n < schedfxns[i].size() && schedfxns[i][n] == iter)) {
                            ndx = i;
                        }
                    }
                    if (ndx < 1) {
                        level = n + 1;
                    } else {
                        // 1) Insert a tuple for each statement in each schedule
                        for (unsigned i = 0; i < schedfxns.size(); i++) {
                            schedfxns[i].insert(schedfxns[i].begin() + level, Iter(i + '0'));
                        }
                        // 2) Bust outta this loop...
                        n = maxiter;
                    }
                }
            }

            // Replace schedules with updated tuples...
            n = 0;
            for (unsigned i = 0; i < _schedules.size(); i++) {
                Space dst = _schedules[i].dest();
                dst.iterators(schedfxns[n]);
                _schedules[i].dest(dst);
                n += 1;
            }

            for (unsigned i = 0; i < other._schedules.size(); i++) {
                Space dst = other._schedules[i].dest();
                dst.iterators(schedfxns[n]);
                other._schedules[i].name("r" + to_string(n));
                other._schedules[i].dest(dst);
                n += 1;
            }

            fuseComps(*this, other);
            return *this;
        }

        string make_dense(const Range &dnsConstr) {
            Space sdense = _space;
            sdense.name("Idense");
            sdense ^= dnsConstr;
            Rel rdense("Tdense", _space, sdense);
//            cerr << sdense.to_iegen() << endl;
//            cerr << rdense.to_iegen() << endl;
//            Comp comp = apply(rdense, sdense.name());
//            return comp;
            return apply(rdense, sdense.name());
        }

        string tile(const Iter &i0, const Expr &s0, Iter &t0) {
            vector<Iter> titers;
            for (const Iter &iter : _space.iterators()) {
                if (iter.equals(i0)) {
                    titers.push_back(t0);
                    titers.push_back(i0);
                } else {
                    titers.push_back(iter);
                }
            }

            Iter r0("r0");
            vector<Constr> tcons = exists(0 <= r0 < s0 ^ i0 == t0 * s0 + r0);
            Space stile("Itile", titers, tcons);
            Rel rtile("Ttile", _space, stile);

            return apply(rtile, stile.name());
        }

        //Comp tile(const Iter& i0, const Iter& i1, const Expr& s0, const Expr& s1, Iter& t0, Iter& t1) {
        string tile(const Iter &i0, const Iter &i1, const Expr &s0, const Expr &s1, Iter &t0, Iter &t1) {
            vector<Iter> titers;
            for (const Iter &iter : _space.iterators()) {
                if (iter.equals(i0)) {
                    titers.push_back(t0);
                    titers.push_back(i0);
                } else if (iter.equals(i1)) {
                    titers.push_back(t1);
                    titers.push_back(i1);
                } else {
                    titers.push_back(iter);
                }
            }

            Iter r0("r0"), r1("r1");
            // Add tiling constraints...
            vector<Constr> tcons = exists(0 <= r0 < s0 ^ i0 == t0 * s0 + r0 ^ 0 <= r1 < s1 ^ i1 == t1 * s1 + r1);
            Space stile("Itile", titers, tcons);
            Rel rtile("Ttile", _space, stile);

//            Comp comp = apply(rtile, stile.name());
//            return comp;
            return apply(rtile, stile.name());
        }

        //Comp apply(const Rel& rel, const string& resName = "Ires") const {
        string apply(const Rel &rel, const string &resName = "Ires") {
            PolyLib poly;
            string setStr = poly.add(_space.to_iegen());
            cerr << rel.to_iegen() << endl;
            string relStr = poly.add(rel.to_iegen());
            string newStr = poly.apply(rel.name(), _space.name(), resName);
            _transforms.push_back(rel);
//            Space newSpace = unstring<Space>(newStr);
//            Comp newComp(newSpace, _statements, _guards);
//            return newComp;
            return newStr;
        }

        //Comp operator*(const Rel& rel) {
        string operator*(const Rel &rel) {
            return apply(rel);
        }

        Comp operator+=(const Math& statement) {
            this->add(statement);
            return *this;
        }

        Comp operator+=(const vector<Math>& statements) {
            for (const auto& statement: statements) {
                this->add(statement);
            }
            return *this;
        }

    protected:
        void init(const string& name, Space &space, initializer_list<Math> statements) {
            space.name(name);
            init(space, statements, {});
        }

        void init(const Space &space, initializer_list<Math> statements) {
            init(space, statements, {});
        }

        void init(const string& name, Space &space, initializer_list<Math> statements, initializer_list<Constr> guards) {
            space.name(name);
            init(space, statements, guards);
        }

        void init(const Space &space, initializer_list<Math> statements, initializer_list<Constr> guards) {
            vector<Math> stmts;
            vector<Constr> grds;
            for (const auto &expr : statements) {
                stmts.push_back(expr);
            }
            for (const auto &constr : guards) {
                grds.push_back(constr);
            }
            init(space, stmts, grds);
        }

        void init(const string& name, Space &space, const vector<Math> &statements, const vector<Constr> &guards) {
            space.name(name);
            init(space, statements, guards);
        }

        void init(const Space &space, const vector<Math> &statements, const vector<Constr> &guards) {
            _space = space;
            _type = '0';
            _statements = statements;
            _guards = guards;
            while (_guards.size() < _statements.size()) {
                _guards.emplace_back(Constr());
            }
            schedule();
            _text = stringify<Comp>(*this);
            addComputation(*this);
        }

        void copy(const Comp &other) {
            Expr::copy(other); // Let Base::copy() handle copying Base things
            _statements = other._statements;
            _guards = other._guards;
            _schedules = other._schedules;
        }

        void schedule() {
            _schedules.clear();
            vector<Iter> srcIters = _space.iterators();
            for (unsigned n = 0; n < _statements.size(); n++) {
                vector<Iter> destIters = srcIters;
                destIters.emplace_back(Iter(to_string(n)));
                _schedules.emplace_back(Rel("r" + to_string(n), srcIters, destIters));
            }
        }

        Space _space;
        vector<Math> _statements;
        vector<Constr> _guards;
        vector<Rel> _schedules;
        deque<Rel> _transforms;
    };

    ostream &operator<<(ostream &os, const Comp &comp) {
        os << comp.space();

        unsigned size = comp.guards().size();
        if (size > 0 && !comp.guards().at(0).text().empty()) {
            os << " : { ";
            for (unsigned i = 0; i < size; i++) {
                os << comp.guards().at(i);
                if (i < size - 1) {
                    os << ", ";
                }
            }
            os << " }";
        }

        size = comp.statements().size();
        if (size > 0) {
            os << " : { ";
            for (unsigned i = 0; i < size; i++) {
                os << comp.statements().at(i);
                if (i < size - 1) {
                    os << "; ";
                }
            }
            os << " }";
        }
        return os;
    }

    Comp operator+(const Space &space, const Math &expr) {
        return Comp(space, expr);
    }

    Comp operator+(const Space &space, const vector<Math>& exprs) {
        return Comp(space, exprs);
    }

    Comp operator+(const Space &space, const Constr &constr) {
        return Comp(space, {}, {constr});
    }

    Comp operator+(const Constr &constr, const Comp &comp) {
        return Comp(comp.space(), comp.statements(), constr);
    }

    Comp operator+(const Comp &comp, const Constr &constr) {
        return Comp(comp.space(), comp.statements(), constr);
    }

    Comp operator+(const Comp &comp, const Math &expr) {
        return Comp(comp.space(), expr, comp.guards());
    }
}

#include <pdfg/FlowGraph.hpp>
#include <pdfg/Visitor.hpp>

namespace pdfg {
    // GraphMaker Class
    class GraphMaker {
    public:
        static GraphMaker& get() {
            static GraphMaker instance; // Guaranteed to be destroyed.
            return instance;            // Instantiated on first use.
        }

        GraphMaker(GraphMaker const&) = delete;         // Don't Implement

        void operator=(GraphMaker const&) = delete;     // Don't implement

        void newGraph(const string& name) {
            _flowGraph = FlowGraph(name);
            _graphs[name] = _flowGraph;
        }

        void fuse(Comp& comp1, const Comp& comp2) {
            _flowGraph.fuse({comp1, comp2});
        }

        void addComp(Comp& comp) {
            Node *compNode, *dataNode;

            // 1) Make statement node for comp.
            compNode = new CompNode(&comp, comp.space().name());

            // Collect domain expressions
            vector<Expr> domExprs;
            for (Constr& constr : comp.space().constraints()) {
                domExprs.push_back(constr.lhs());
                domExprs.push_back(constr.rhs());
            }

            // 2) Add integer data nodes for UFs + SCs
            for (Expr& expr : domExprs) {
                if (expr.is_func()) {
                    Expr* size = getSize(comp, Func(expr));
                    if (_flowGraph.contains(expr.text())) {
                        dataNode = _flowGraph.get(expr.text());
                        ((DataNode*) dataNode)->size(size);
                    } else {
                        // Make integer typed data node and add incoming edge to stmt node.
                        dataNode = _flowGraph.add(new DataNode(&expr, expr.text(), size, _indexType));
                        cerr << "Added read node '" << dataNode->label() << "'" << endl;
                    }
                    _flowGraph.add(dataNode, compNode);
                }
            }

            // 3) Create data nodes from statements.
            vector<Expr> readExprs;
            vector<Expr> writeExprs;
            for (Math& stmt : comp.statements()) {
                if (stmt.oper().find('=') != string::npos) {
                    // If operator has an equal sign, it is an assignment, so the LHS is an output node.
                    writeExprs.push_back(stmt.lhs());
                } else if (!stmt.lhs().text().empty()) {
                    readExprs.push_back(stmt.lhs());
                }
                // Assume write hand side is a read...
                char rtype = stmt.rhs().type();
                if (rtype == 'M' || rtype == 'F') {
                    // Math expressions may need to be broken into multiple accesses...
                    string expr = stmt.rhs().text();
                    for (const auto& sit : _spaces) {
                        if (Strings::in(expr, sit.first, true)) {
                            readExprs.push_back(sit.second);
                        }
                    }
                    if (rtype == 'M') {
                        for (const auto &sit : _funcs) {
                            if (Strings::in(expr, sit.first, true)) {
                                readExprs.push_back(sit.second);
                            }
                        }
                    }
                } else if (!stmt.rhs().is_scalar()) {
                    readExprs.push_back(stmt.rhs());
                }
            }

            for (Expr& expr : readExprs) {
                Func func = Func(expr);
                Expr* size = getSize(comp, func);
                string type = getType(comp, func);
                if (_flowGraph.contains(expr.text())) {
                    dataNode = _flowGraph.get(expr.text());
                    ((DataNode*) dataNode)->size(size);
                } else {
                    // Create data node from Access object, and add incoming edge edge to statement node.
                    dataNode = _flowGraph.add(new DataNode(&expr, expr.text(), size, type));
                    cerr << "Added read node '" << dataNode->label() << "'" << endl;
                }
                _flowGraph.add(dataNode, compNode);
            }

            _flowGraph.add(compNode);
            cerr << "Adding comp node '" << compNode->label() << "'" << endl;

            for (Expr& expr : writeExprs) {
                Func func = Func(expr);
                Expr* size = getSize(comp, func);
                string type = getType(comp, func);
                if (_flowGraph.contains(expr.text())) {
                    dataNode = _flowGraph.get(expr.text());
                    ((DataNode*) dataNode)->size(size);
                } else {
                    // Create data node from Access object, and add outgoing edge edge to statement node.
                    // Mark output node as persistent (immutable) or temporary (optimizable) -- how to tell?
                    dataNode = _flowGraph.add(new DataNode(&expr, expr.text(), size, type)); //, getMapping(expr.text())));
                    cerr << "Added write node '" << dataNode->label() << "'" << endl;
                }
                if (_flowGraph.contains(dataNode, compNode)) {
                    // Cycle! => create hierarchical node
                    _flowGraph.remove(compNode);
                    cerr << "Removed comp node '" << compNode->label() << "' to prevent cycle" << endl;
                    makeHierarchical((CompNode*) compNode, (DataNode*) dataNode);
                } else {
                    _flowGraph.add(compNode, dataNode);
                }
            }
        }

        string indexType() const {
            return _indexType;
        }

        void indexType(const string& type) {
            if (type == "i") {
                _indexType = "int";
            } else if (type == "u") {
                _indexType = "unsigned";
            } else {
                _indexType = type;
            }
        }

        string dataType() const {
            return _dataType;
        }

        void dataType(const string& type) {
            if (type == "d") {
                _dataType = "double";
            } else if (type == "f") {
                _dataType = "float";
            } else {
                _dataType = type;
            }
        }

        string returnName() const {
            return _flowGraph.returnName();
        }

        void returnName(const string& returnName) {
            _flowGraph.returnName(returnName);
            _flowGraph.returnType(_dataType);
        }

        void addSpace(const Space& space) {
            string sname = space.name();
            if (!sname.empty()) {
                _spaces[sname] = space;
            }
        }

        void addRel(const Rel& rel) {
            _relations[rel.name()] = rel;
        }

        void addIter(const Iter& iter) {
            _iters[iter.name()] = iter;
        }

        void addFunc(const Func& func) {
            _funcs[func.name()] = func;
        }

        void addConst(const Const& con) {
            _consts[con.name()] = con;
        }

        void addAccess(const Access& access) {
            unsigned size = access.tuple().size();
            vector<int> offsets = access.offsets();
            Space space = access.space();

            if (size > 0 && access.tuple().at(0).type() != 'N') {
                string sname = space.name();
                if (access.refchar() != '[') {
                    ostringstream os;
                    os << sname << '[';
                    if (size < 2) {
                        Expr expr = access.tuple().at(0);
                        os << expr;
                        if (offsets.size() > 0 && offsets[0] != 0) {
                            if (offsets[0] > 0) {
                                os << '+';
                            }
                            os << offsets[0];
                        }
                    } else {
                        os << "offset" << size << '(';
                        for (unsigned i = 0; i < size; i++) {
                            os << '(' << access.tuple().at(i) << ')';
                            if (offsets.size() > i && offsets[i] != 0) {
                                if (offsets[i] > 0) {
                                    os << '+';
                                }
                                os << offsets[i];
                            }
                            os << ',';
                        }
                        for (unsigned i = 1; i < size; i++) {
                            unsigned k = i * 2 + 1;
                            Expr upper;
                            if (space.constraints().size() < k) {
                                upper = findUpperBound(Iter(access.tuple().at(i)));
                            } else {
                                upper = space.constraints().at(k).rhs();
                            }
                            os << '(' << upper << ')';
                            if (i < size - 1) {
                                os << ',';
                            }
                        }
                        os << ')';
                    }
                    os << ']';

                    string mapping = os.str();
                    addMapping(access, mapping);
                }
                _accesses[sname].push_back(access);
            }
        }

        void addMacro(Macro& macro) {
            string macname = macro.name();
            if (_macros.find(macname) == _macros.end()) {
                _macros[macname] = vector<Macro>();
            }
            macro.name(macname + to_string(_macros[macname].size() + 1));
            _macros[macname].push_back(macro);
        }

        void addMapping(const Access& access, const string& mapping) {
            string accname = access.space().name();
            auto itr = _mappings.find(accname);
            if (itr == _mappings.end()) {
                string accstr = stringify<Access>(access);
                _mappings[accname] = make_pair<string, string>(string(accstr), string(mapping));
            }
        }

        pair<string, string> getMapping(const string& name) {
            auto itr = _mappings.find(name);
            if (itr != _mappings.end()) {
                return itr->second;
            }
            return make_pair<string, string>("", "");
        }

        Space getSpace(const string& name) {
            if (name.empty()) {
                int stop = 1;
            }
            return _spaces[name];
        }

        map<string, Space>& spaces() {
            return _spaces;
        }

        string codegen(const string& path = "", const string& name = "") {
            string cpath = path;
            bool isobj = (cpath.find(".o") != string::npos);
            if (!cpath.empty() && cpath.find('.') == string::npos) {
                cpath += ".c";
            } else if (isobj) {
                cpath = Strings::replace(cpath, ".o", ".c");
            }

            CodeGenVisitor cgen(cpath); //, _iters.size());
            for (const auto& mapping : _mappings) {
                cgen.define(mapping.second);
            }

//            for (const auto& iter : _macros) {
//                for (const auto& macro : iter.second) {
//                    string code = stringify<Macro>(macro);
//                    size_t pos = code.find(' ');
//                    string lhs = code.substr(0, pos);
//                    string rhs = code.substr(pos + 1);
//                    cgen.define(lhs, rhs);
//                }
//            }

            if (name.empty()) {
                cgen.walk(&_flowGraph);
            } else {
                cgen.walk(&_graphs[name]);
            }

            if (isobj) {
                compile(cpath, path);
            }

            return cgen.str();
        }

        string compile(const string& src, const string& obj) {
            // gcc -g -O3 -c dsr_spmv.c -o dsr_spmv.o
            string compCmd = "/usr/bin/gcc -g -O3 -c " + src + " -o " + obj;
            cerr << "compile: '" << compCmd << "'\n";
            int stat = system(compCmd.c_str());
            return "";
        }

        void print(const string& file = "") {
            if (!file.empty()) {
                ofstream ofs(file.c_str(), ofstream::out);
                ofs << _flowGraph << endl;     // Emit the graph!
                ofs.close();
            } else {
                cerr << _flowGraph << endl;     // Emit the graph!
            }
        }

    protected:
        GraphMaker() {
            _indexType = "unsigned";
            _dataType = "float";
        }

        bool hasIter(const string& expr) {
            for (const auto& keyval : _iters) {
                if (expr.rfind(keyval.first, 0) == 0) {
                    return true;
                }
            }
            return false;
        }

        Expr* getSize(const Comp& comp, const Func& func) {
            Expr* expr = nullptr;
            Math math;
            if (func.arity() > 0) {
                expr = new Math(getFuncSize(comp, func));
            } else if (_spaces.find(func.name()) != _spaces.end()) {
                Space space = _spaces[func.name()];
                if (!space.constraints().empty()) {
                    expr = new Math(space.size());
                }
            }
            if (expr == nullptr) {
                expr = new Int(1);      // Assume scalar
            }
            return expr;
        }

        Math getFuncSize(const Comp& comp, const Func& func) {
            Math math;
            for (Expr& arg : func.args()) {
                string argtxt = arg.text();
                auto mapkey = _iters.find(argtxt);
                bool isIter = (mapkey != _iters.end());
                if (!isIter && hasIter(argtxt)) {
                    istringstream is(argtxt);
                    is >> math;
                    argtxt = math.lhs().text();
                    mapkey = _iters.find(argtxt);
                    isIter = (mapkey != _iters.end());
                }
                if (isIter) {
                    Iter iter = mapkey->second;
                    vector<Constr> constraints = comp.space().constraints(argtxt);
                    Expr lower, upper;
                    string oplow, opup;
                    for (const Constr& constr : constraints) {
                        if (constr.lhs().text() == iter.name()) {
                            upper = constr.rhs();
                            opup = constr.relop();
                        } else if (constr.rhs().text() == iter.name()) {
                            lower = constr.lhs();
                            oplow = constr.relop();
                        }
                    }

                    Math diff = (upper - lower);
                    if (opup == "<=") {
                        diff = Math(diff, Int(1), "+");
                    }
                    if (oplow == "<") {
                        diff = Math(diff, Int(1), "-");
                    }
                    diff = paren(diff); //Math(NullExpr, diff, "(");

                    if (argtxt == math.lhs().text()) {
                        math = Math(diff, math.rhs(), math.oper());
                    } else if (!math.oper().empty()) {
                        math = math * diff;
                    } else {
                        math = diff;
                    }
                } else if (_consts.find(argtxt) != _consts.end()) {
                    math = Math(_consts[argtxt], Int(0), "+");
                }
            }
            return math;
        }

        string getType(const Comp& comp, const Func& func) const {
            if (_consts.find(func.name()) != _consts.end()) {
                // Assume symbolic constants are ints...
                return _indexType;
            } else {
                for (const Math& stmt : comp.statements()) {
                    // Assignment to a symbolic constant or iter, or vice versa...
                    if ((stmt.rhs().text().rfind(func.name(), 0) == 0 &&
                        (_consts.find(stmt.lhs().text()) != _consts.end() ||
                         _iters.find(stmt.lhs().text()) != _iters.end())) ||
                        (stmt.lhs().text().rfind(func.name(), 0) == 0 &&
                        (_consts.find(stmt.rhs().text()) != _consts.end() ||
                         _iters.find(stmt.rhs().text()) != _iters.end()))) {
                        return _indexType;
                    }
                }
            }
            return _dataType;
        }

        void makeHierarchical(CompNode* compNode, DataNode* dataNode) {
            vector<Access> accs = _accesses[dataNode->label()];
            vector<int> rdist = calcReuseDist(accs);
            // Find first nonzero reuse distance
            unsigned ipos = rdist.size();
            for (unsigned i = 0; ipos == rdist.size() && i < ipos; i++) {
                if (rdist[i] > 0) {
                    ipos = i;
                }
            }

            if (ipos < rdist.size()) {
                unsigned unrollfac = rdist[ipos];
                unsigned unrollcnt = 0;
                //if (_flowGraph.isSource(dataNode)) {
                    // Peel!
                    Comp* comp = (Comp*) compNode->expr();
                    // Copy the computation space
                    Space cspace = comp->space();

                    string dname = dataNode->expr()->text();
                    Space dspace;  // Temporary space
                    dspace.name(dname + to_string(unrollcnt));

                    string cname = cspace.name();
                    Space pspace(cname + to_string(unrollcnt));
                    unrollcnt += 1;

                    Iter iter = cspace.iterators()[ipos];
                    vector<Constr> constraints = cspace.constraints();
                    vector<Constr> others;
                    unsigned cpos = constraints.size();

                    for (unsigned i = 0; i < constraints.size(); i++) {
                        if (!constraints[i].contains(iter)) {
                            others.push_back(constraints[i]);
                        } else if (cpos == constraints.size()) {
                            cpos = i;
                        }
                    }

                    Constr lower = constraints[cpos];
                    Constr upper = constraints[cpos + 1];

                    Constr lpeel = (lower.lhs() == lower.rhs());
                    Constr upeel = (upper.lhs() == upper.rhs());

                    vector<Constr> newcons;
                    newcons.push_back(lpeel);
                    for (const auto& constr : others) {
                        newcons.push_back(constr);
                    }
                    pspace.add(newcons);

                    Expr oldlb = lower.lhs();
                    Expr newlb;
                    if (oldlb.type() == 'N') {
                        newlb = Int(unstring<unsigned>(oldlb.text()) + 1);
                    } else {
                        newlb = oldlb + 1;
                    }
                    lower.lhs(newlb);
                    cspace.set(cpos, lower);

                    Expr oldub = upper.rhs();
                    Expr newub;
                    if (oldub.type() == 'N') {
                        newub = Int(unstring<unsigned>(oldlb.text()) - 1);
                    } else {
                        newub = oldub - 1;
                    }
                    upper.rhs(newub);
                    cspace.set(cpos+1, upper);

                    vector<Math> statements;
                    for (const auto& stmt : comp->statements()) {
                        if (stmt.oper().find('=') != string::npos) {
                            string expr = stmt.lhs().text();
                            size_t pos = expr.find('(');
                            if (pos == string::npos) {
                                pos = expr.find('[');
                            }
                            if (pos != string::npos && expr.substr(0, pos) == dname) {
                                for (const auto& acc : accs) {
                                    if (expr == stringify<Access>(acc)) {
                                        Access newacc(dspace, acc.tuple(), acc.refchar());
                                        Math newstmt(newacc, stmt.rhs(), stmt.oper());
                                        statements.push_back(newstmt);
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    // TODO: Do NOT instantiate new computations until new data spaces are allocated to avoid infinite recursion!
                    Comp pcomp = pspace + statements;
                    cerr << pcomp << endl;

                    // Make the next dspace (e.g., A1).
                    string rname = dspace.name();
                    dspace.name(dname + to_string(unrollcnt));
                    string wname = dspace.name();

                    cspace.name(cname + to_string(unrollcnt));
                    unrollcnt += 1;

                    updateStatements(accs, dname, rname, wname, comp->statements(), statements);

                    Comp ccomp = cspace + statements;
                    cerr << ccomp << endl;

                    // Last unroll!
                    Space espace(cname + to_string(unrollcnt));
                    newcons.clear();
                    newcons.push_back(upeel);
                    for (const auto& constr : others) {
                        newcons.push_back(constr);
                    }
                    espace.add(newcons);

                    // Make the last dspace (e.g., A2).
                    rname = wname;
                    dspace.name(dname + to_string(unrollcnt));
                    wname = dspace.name();

                    updateStatements(accs, dname, rname, wname, comp->statements(), statements);

                    Comp ecomp = espace + statements;
                    cerr << ecomp << endl;
                //}
            } else {
                cerr << "ERROR: Circular data reference in computation!\n";
            }
        }

        void updateStatements(const vector<Access>& accs, const string& dname, const string& rname, const string& wname,
                              const vector<Math>& inStmts, vector<Math>& outStmts) {
            outStmts.clear();
            for (const auto& stmt : inStmts) {
                // Update statements...
                string lhs = stmt.lhs().text();
                string rhs = stmt.rhs().text();
                for (const auto& acc : accs) {
                    string accstr = stringify<Access>(acc);
                    string read = Strings::replace(accstr, dname, rname, true);
                    string write = Strings::replace(accstr, dname, wname, true);
                    if (lhs == accstr) {
                        lhs = write;
                    }
                    if (rhs.find(accstr) != string::npos) {
                        rhs = Strings::replace(rhs, accstr, read);
                    }
                }
                Math newstmt(Expr(lhs, stmt.lhs().type()), Expr(rhs, stmt.rhs().type()), stmt.oper());
                outStmts.push_back(newstmt);
            }
        }

        vector<int> calcReuseDist(const vector<Access>& accs) {
            vector<int> rdist(accs[0].tuple().size(), 0);
            for (const auto& acc : accs) {
                vector<Expr> tuple = acc.tuple();
                for (unsigned i = 0; i < tuple.size(); i++) {
                    string num = Strings::number(tuple[i].text());
                    int dist = num.empty() ? 0 : unstring<int>(num);
                    dist -= rdist[i];
                    rdist[i] = dist;
                }
            }
            for (unsigned i = 0; i < rdist.size(); i++) {
                if (rdist[i] < 0) {
                    rdist[i] = -rdist[i];
                }
                rdist[i] += 1;
            }
            return rdist;
        }

        string _indexType;
        string _dataType;

        map<string, Iter> _iters;
        map<string, Func> _funcs;
        map<string, Const> _consts;
        map<string, FlowGraph> _graphs;
        map<string, Space> _spaces;
        map<string, Rel> _relations;
        map<string, vector<Access> > _accesses;
        map<string, pair<string, string> > _mappings;
        map<string, vector<Macro> > _macros;

        FlowGraph _flowGraph;
    };

    void init(const string& name) {
        pdfg::_spaces.clear();
        GraphMaker::get().newGraph(name);
    }

    void print(const string& file = "") {
        GraphMaker::get().print(file);
    }

    void clearSpaces() {
        for (const auto& itr : _spaces) {
            delete itr.second;
        }
        _spaces.clear();
    }

    string codegen(const string& name = "") {
        string code = GraphMaker::get().codegen(name);
        clearSpaces();
        return code;
    }

    void addIterator(const Iter& iter) {
        if (!iter.name().empty()) {
            GraphMaker::get().addIter(iter);
        }
    }

    void addConstant(const Const& con) {
        if (!con.name().empty()) {
            GraphMaker::get().addConst(con);
        }
    }

    void addFunction(const Func& func) {
        if (!func.name().empty()) {
            GraphMaker::get().addFunc(func);
        }
    }

    void addMacro(Macro& macro) {
        if (!macro.name().empty()) {
            GraphMaker::get().addMacro(macro);
        }
    }

    void addComputation(Comp& comp) {
        if (!comp.text().empty()) {
            GraphMaker::get().addComp(comp);
        }
    }

    void addSpace(const Expr& expr) {
        if (expr.is_space()) {
            addSpace(Space(expr.text()));
        }
    }

    void addSpace(const Space& space) {
        if (!space.name().empty()) {
            GraphMaker::get().addSpace(space);
        }
    }

    Space getSpace(const string& name) {
        //return *_spaces[name];
        return GraphMaker::get().getSpace(name);
    }

    void addRelation(const Rel& rel) {
        if (!rel.name().empty()) {
            GraphMaker::get().addRel(rel);
        }
    }

    void addAccess(const Access& access) {
        if (!access.space().name().empty()) {
            GraphMaker::get().addAccess(access);
        }
    }

    void fuseComps(Comp& comp1, const Comp& comp2) {
        if (!comp1.text().empty()) {
            GraphMaker::get().fuse(comp1, comp2);
        }
    }
}

#endif //POLYEXT_GRAPHIL_H
