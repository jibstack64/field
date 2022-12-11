// A subcommand-based argument parsing library for C++.
// https://github.com/jibstack64/field

#ifndef FIELD_HPP
#define FIELD_HPP

#include <functional>
#include <string.h>
#include <vector>

namespace field {

    // holds an argument's data
    struct arg;

    // holds a flag's data
    struct flag;

    // parses arguments
    class parser;

    // holds context data
    struct context;

    // templates for default runnables
    namespace tlt {
        void arg(field::context& ctx, std::vector<const char *> values) {}
        void flag(field::context& ctx) {}
    }

    struct arg {       
        private:
        std::vector<const char *> finalValues_{};

        public:
        const std::function<void(context&, std::vector<const char *>)> runnable;
        const int takes;
        const char * name;
        bool passed = false;

        std::vector<const char *> values();

        void run(context& ctx, std::vector<const char *> values);
        void run(context& ctx, const char * value);

        arg(const char * name, std::function<void(context&, std::vector<const char *>)> run, int takes);
        arg(const char * name, std::function<void(context&, std::vector<const char *>)> run);
        arg(const char * name, int takes);
        arg(const char * name);
    };
    //
    arg::arg(const char * name, std::function<void(context&, std::vector<const char *>)> run, int takes) : name(name),
            runnable(run), takes(takes) {}
    //
    arg::arg(const char * name, std::function<void(context&, std::vector<const char *>)> run) : name(name),
            runnable(run), takes(-1) {}
    //
    arg::arg(const char * name, int takes) : name(name), runnable(tlt::arg), takes(takes) {}
    //
    arg::arg(const char * name) : name(name), runnable(tlt::arg), takes(-1) {}
    //
    std::vector<const char *> arg::values() {
        return finalValues_;
    }
    //
    void arg::run(context& ctx, std::vector<const char *> values) {
        passed = true;
        finalValues_ = values;
        runnable(ctx, values);
    }
    //
    void arg::run(context& ctx, const char * value) {
        run(ctx, std::vector<const char *>{value});
    }

    struct flag {
        public:
        const std::function<void(context&)> runnable;
        const char * name;
        bool passed = false;

        void run(context& ctx);

        flag(const char * name, std::function<void(context&)> run);
        flag(const char * name);
    };
    //
    flag::flag(const char * name, std::function<void(context&)> run) : name(name), runnable(run) {}
    //
    flag::flag(const char * name) : name(name), runnable(tlt::flag) {}
    //
    void flag::run(context& ctx) {
        passed = true;
        runnable(ctx);
    }

    struct context {
        public:
        std::vector<flag*> passedFlags;
        std::vector<const char *> overflowValues;

        context(std::vector<flag*> passedFlags, std::vector<const char *> overflowValues);
        context(std::initializer_list<flag*> passedFlags, std::initializer_list<const char *> overflowValues);
        context();
    };
    //
    context::context(std::vector<flag*> passedFlags, std::vector<const char *> overflowValues) : passedFlags(passedFlags),
            overflowValues(overflowValues) {}
    context::context(std::initializer_list<flag*> passedFlags, std::initializer_list<const char *> overflowValues) :
            passedFlags(passedFlags), overflowValues(overflowValues) {}
    context::context() : passedFlags({}), overflowValues({}) {}

    class parser {
        private:
        std::vector<arg> args_;
        std::vector<flag> flags_;

        public:
        bool disableLock;
        context ctx;

        arg* parse(int argc, char ** argv);
        arg* parse(char ** argv);

        arg* getArg(const char * name);
        
        flag* getFlag(const char * name);

        arg* add(const char * name, std::function<void(context&, std::vector<const char *>)> run, int takes);
        arg* add(const char * name, std::function<void(context&, std::vector<const char *>)> run);
        arg* add(const char * name, int takes);
        arg* add(const char * name);
        void add(arg& ar);

        flag* addFlag(const char * name, std::function<void(context&)> run);
        flag* addFlag(const char * name);
        void addFlag(flag& fl);

        context formContext(std::vector<const char *> overflowValues);

        parser();
        parser(std::initializer_list<arg> args);
        parser(std::initializer_list<arg> args, std::initializer_list<const char *> flags);
    };
    //
    parser::parser() {}
    //
    parser::parser(std::initializer_list<arg> args) : args_(args) {}
    //
    arg* parser::parse(int argc, char ** argv) {
        arg* main = nullptr; // the sub-command found
        std::vector<flag*> flagsToRun;
        std::vector<const char *> overflowValues;
        std::vector<const char *> values; // final values to be passed to main->run(...)
        for (int i = 1; i < argc; i++) {
            flag* fl = getFlag(argv[i]);
            if (fl != nullptr) {
                if (!fl->passed) {
                    flagsToRun.push_back(fl);
                    fl->passed = true;
                }
                continue;
            }
            arg* ar = getArg(argv[i]);
            if (ar != nullptr && (main == nullptr || disableLock)) {
                main = ar;
                int depth = ar->takes;
                if (depth < 1 || i + depth + 1 >= argc) { 
                    int x;
                    for (x = i + 1; x < argc; x++) {
                        values.push_back(argv[x]);
                    }
                    i = x - 1;
                    //break; no!! stops flags
                } else {
                    int x;
                    for (x = i + 1; x < i + depth + 1; x++) {
                        values.push_back(argv[x]);
                    }
                    i = x - 1;
                }
                continue;
            }
            // if not a flag, arg or value
            overflowValues.push_back(argv[i]);
        }
        ctx = formContext(overflowValues);
        // then run
        for (auto& fl : flagsToRun) {
            fl->run(ctx);
        }
        if (main != nullptr) {
            main->run(ctx, values);
        }
        return main;
    }
    //
    arg* parser::parse(char ** argv) {
        int i;
        while (argv[i] != nullptr) {
            i++;
        }
        return parse(i, argv);
    }
    //
    arg* parser::getArg(const char * name) {
        for (auto& ar : args_) {
            if (strcmp(ar.name, name) == 0) {
                return &ar;
            }
        }
        return nullptr;
    }
    //
    flag* parser::getFlag(const char * name) {
        for (auto& fl : flags_) {
            if (strcmp(fl.name, name) == 0) {
                return &fl;
            }
        }
        return nullptr;
    }
    //
    context parser::formContext(std::vector<const char *> overflowValues) {
        std::vector<flag*> passedFlags;
        for (auto& fl : flags_) {
            if (fl.passed) {
                passedFlags.push_back(&fl);
            }
        }
        context ctx(passedFlags, overflowValues);
        return ctx;
    }
    //
    arg* parser::add(const char * name, std::function<void(context&, std::vector<const char *>)> run, int takes) {
        arg newArg(name, run, takes);
        args_.push_back(newArg);
        return &args_.back();
    }
    //
    arg* parser::add(const char * name, std::function<void(context&, std::vector<const char *>)> run) {
        arg newArg(name, run);
        args_.push_back(newArg);
        return &args_.back();
    }
    //
    arg* parser::add(const char * name, int takes) {
        arg newArg(name, takes);
        args_.push_back(newArg);
        return &args_.back();
    }
    //
    arg* parser::add(const char * name) {
        arg newArg(name);
        args_.push_back(newArg);
        return &args_.back();
    }
    //
    void parser::add(arg& ar) {
        args_.push_back(ar);
    }
    //
    flag* parser::addFlag(const char * name, std::function<void(context&)> run) {
        flag newFlag(name, run);
        flags_.push_back(newFlag);
        return &flags_.back();
    }
    //
    flag* parser::addFlag(const char * name) {
        flag newFlag(name);
        flags_.push_back(newFlag);
        return &flags_.back();
    }
    //
    void parser::addFlag(flag& fl) {
        flags_.push_back(fl);
    }
}


#endif
