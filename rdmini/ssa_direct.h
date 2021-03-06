#ifndef SSA_DIRECT_H_
#define SSA_DIRECT_H_

#include <random>

#include "rdmini/exceptions.h"

/** Implementation of 'direct' SSA method. */

// KeyType must be unsigned integral
// ValueType must be floating point

template <typename KeyType,typename ValueType>
struct ssa_direct {
    typedef KeyType key_type;
    typedef ValueType value_type;

    class event_type: std::pair<key_type,double> {
        typedef std::pair<key_type,double> pair_type;
    public:
        using typename pair_type::first_type;
        using typename pair_type::second_type;
        
        event_type() =default;
        event_type(key_type k_,double dt_): pair_type(k_,dt_) {}

        key_type key() const { return this->first; }
        double dt() const { return this->second; }
    };


    // simplest ladder implementation
    explicit ssa_direct(size_t n_key_=0) { reset(n_key_); }

    size_t size() const { return n_key; }

    template <typename R>
    event_type next(R &g) {
        std::uniform_real_distribution<value_type> U(0,total);
        value_type x=U(g);

        key_type i=0;
        for (i=0; i<n_key; ++i) {
            x-=propensities[i];
            if (x<0) break;
        }
        if (i>=n_key) throw rdmini::ssa_error("fell off propensity ladder (rounding?)");

        double dt=E(g)/total;
        return event_type{i,dt};
    }

    void reset(size_t n_key_) {
        n_key=n_key_;
        propensities.assign(n_key,0.0);
        total=0.0;
    }

    void update(key_type k,value_type r) {
        value_type &p=propensities[k];
        total+=r-p;
        p=r;
    }

    value_type propensity(key_type k) const { return propensities[k]; }

    value_type total_propensity() const { return total; };

private:
    size_t n_key;
    std::exponential_distribution<value_type> E;
    std::vector<value_type> propensities;
    value_type total;
};

#endif // ndef SSA_DIRECT_H_
