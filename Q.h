//
//  Q.h
//
//  Created by Jami Couch on 8/15/15.
//  Copyright (c) 2015 Jami Couch. All rights reserved.
//

#ifndef Q_h
#define Q_h

#include <functional>
#include <iostream>
#include <vector>

class PromiseVal {
private:
    void *m_data;
    bool m_promise;
public:
    PromiseVal *withPromise(void *promise);
    PromiseVal *withValue(void *value);
    bool isPromise();
    bool isNull();
    template<typename T> T getData() {
        return (T)m_data;
    };
    
    static PromiseVal *value(void *value);
    static PromiseVal *promise(void *promise);
    static PromiseVal *null();
};

typedef std::function<PromiseVal*(PromiseVal*)> Success;
typedef std::function<void(void*)> Failure;
typedef std::function<void()> Finally;

class Promise{
private:
    std::vector<Success> m_successes;
    Failure m_failure;
    Finally m_finally;
    
    std::vector<Promise*> m_promises;
    
    bool resolved = false;
    bool rejected = false;
    PromiseVal *resolved_value;
    void *rejected_value;
    
    void _resolve(PromiseVal *value);
    void _resolve_promise(PromiseVal *value);
    void _resolve_value(PromiseVal *value);
    void _reject(void *value);
    
public:
    Promise *then(Success &&success, Failure &&failure = NULL, Finally &&finally = NULL);
    void resolve(PromiseVal *value);
    void resolve(Promise *value);
    void reject(void *value);
};

class Deferred {
private:
    Promise *m_promise;
public:
    Deferred();
    Promise *promise();
    void resolve(PromiseVal *value);
    void reject(void *value);
};

class Q {
public:
    static Deferred *defer();
    static Promise *when(PromiseVal *value);
    static Promise *resolve(PromiseVal *value);
    static Promise *reject(void *value);
};

#endif