// ---- <PersistenceInterface.h> -----------------
class PersistenceInterface {
public:
    virtual ~PersistenceInterface() = default;

    bool read(/*...*/) const {
        return do_read(/*...*/);
    }

    bool write(/*...*/) {
        return do_write(/*...*/);
    }

    // ...
private:
    virtual bool do_read(/*...*/) const = 0;
    virtual bool do_write(/*...*/) = 0;
};

// Strategy: allows you to inject dependencies and switch implementations
PersistenceInterface* get_persistence_interface();
void set_persistence_interface(PersistenceInterface* persistence);

extern PersistenceInterface* instance;

// ---- <PersistenceInterface.cpp> -----------------

#include <Database.h>

PersistenceInterface* instance = nullptr;

PersistenceInterface* get_persistence_interface() {
    // 'Immediately Invoked Lambda Expression (IILE)
    static bool init = []() {
        if (!instance) {
            static Database db;
            instance = &db;
        }
        return true;    // or false, as the actual value does not matter
    }();

    return instance;
}

void set_persistence_interface(PersistenceInterface* persistence) {
    instance = persistence;
}

/*
Note that the above get and set are not thread safe, but this is not something
that we need to address.

If we assume that set_persistence_interface() can be called from anywhere
in the code at any time, in general we can’t expect that after calling
set_persistence_interface(), a call to get_persistence_interface() would
return the set value.

From this perspective, the set_persistence_interface() function should be
used at the very beginning of the program or at the beginning of a single test,
not arbitrarily.
*/