//
//  Q.cpp
//
//  Created by Jami Couch on 8/15/15.
//  Copyright (c) 2015 Jami Couch. All rights reserved.
//

#include "Q.h"


PromiseVal *PromiseVal::withPromise(void *promise) {
    this->m_promise = true;
    this->m_data = promise;
    return this;
}

PromiseVal *PromiseVal::withValue(void *value) {
    this->m_promise = false;
    this->m_data = value;
    return this;
}

bool PromiseVal::isPromise() {
    return m_promise;
}

bool PromiseVal::isNull() {
    return !m_data;
}

PromiseVal *PromiseVal::value(void *value) {
    return (new PromiseVal())->withValue(value);
}

PromiseVal *PromiseVal::promise(void *promise) {
    return (new PromiseVal())->withPromise(promise);
}

PromiseVal *PromiseVal::null() {
    return new PromiseVal();
}



void Promise::_resolve(PromiseVal *value) {
    if (value->isPromise()) {
        _resolve_promise(value);
    } else {
        _resolve_value(value);
    }
    
    if (m_finally) m_finally();
    resolved_value = value;
    resolved = true;
}

void Promise::_resolve_promise(PromiseVal *value) {
    auto promise = value->getData<Promise*>();
    auto _m_successes = &m_successes;
    auto _m_promises = &m_promises;

    promise->then([_m_successes, _m_promises](PromiseVal *data) -> PromiseVal*{
        PromiseVal *propagate_value;
        for (int i = 0; i < _m_successes->size(); i++) {
            propagate_value = _m_successes->at(i)(data);
            _m_promises->at(i)->resolve(propagate_value);
        }
        return PromiseVal::null();
    });
}

void Promise::_resolve_value(PromiseVal *value) {
    PromiseVal *propagate_value;
    for (int i = 0; i < m_successes.size(); i++) {
        propagate_value = m_successes[i](value);
        m_promises[i]->resolve(propagate_value);
    }
}

void Promise::_reject(void *value) {
    if (m_failure)
        m_failure(value);
    else
        for (auto promise : m_promises)
            promise->reject(value);
    
    if (m_finally) m_finally();
    rejected_value = value;
    rejected = true;
}

Promise *Promise::then(Success &&success, Failure &&failure, Finally &&finally) {
    m_successes.push_back(std::forward<Success>(success));
    m_failure = std::forward<Failure>(failure);
    m_finally = std::forward<Finally>(finally);
    
    auto new_promise = new Promise();
    m_promises.push_back(new_promise);
    
    if (resolved) _resolve(resolved_value);
    if (rejected) _reject(rejected_value);
    
    return new_promise;
}

void Promise::resolve(PromiseVal *value) {
    if (resolved || rejected) return;
    _resolve(value);
}

void Promise::reject(void *value) {
    if (resolved || rejected) return;
    _reject(value);
}

Deferred::Deferred() {
    m_promise = new Promise();
}

Promise *Deferred::promise() {
    return m_promise;
}

void Deferred::resolve(PromiseVal *value) {
    m_promise->resolve(value);
}

void Deferred::reject(void *value) {
    m_promise->reject(value);
}

Deferred *Q::defer() {
    return new Deferred();
}

Promise *Q::when(PromiseVal *value) {
    auto deferred = Q::defer();
    deferred->resolve(value);
    return deferred->promise();
}

Promise *Q::resolve(PromiseVal *value) {
    return when(value);
}

Promise *Q::reject(void *value) {
    auto deferred = Q::defer();
    deferred->reject(value);
    return deferred->promise();
}
