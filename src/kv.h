// Copyright (c) 2020 The Vectorium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KV_H
#define KV_H

#include "base58.h"
#include "key.h"
#include "main.h"
#include "timedata.h"
#include "util.h"

using namespace std;

class CKV;

//
// The KV Class. For key/value records management.
//
class CKV
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:
    uint256 kv_txid;
    CKeyID kv_source;
    CKeyID kv_destination;
    std::string key;
    std::string value;
    uint256 hash_of_key;
    uint256 hash_of_key_dest;
    uint256 hash_of_key_dest_src;
    uint256 hash_unique;

    CKV();
    CKV(const CKV& ckv);
    CKV(const CTransaction& tx);

    uint256 get_hash_of_key()
    {
        CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
        ss << this->key;
        return ss.GetHash();
    }

    uint256 get_hash_of_key_dest(const CKV &kv)
    {
        CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
        ss << kv.key << kv.kv_destination;
        return ss.GetHash();
    }

    uint256 get_hash_of_key_dest_src(const CKV &kv)
    {
        CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
        ss << kv.key << kv.kv_destination << kv.kv_source;
        return ss.GetHash();
    }

    uint256 get_hash_unique(const CKV &kv)
    {
        CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
        ss << kv.key  << kv.kv_txid;
        return ss.GetHash();
    }

    CKV& operator=(const CKV &from)
    {
        this->kv_txid = from.kv_txid;
        this->kv_source = from.kv_source;
        this->kv_destination = from.kv_destination;
        this->key = from.key;
        this->value = from.value;
        this->hash_of_key = from.hash_of_key;
        this->hash_of_key_dest = from.hash_of_key_dest;
        this->hash_of_key_dest_src = from.hash_of_key_dest_src;
        this->hash_unique = from.hash_unique;
        return *this;
    }

    friend bool operator == (const CKV& a, const CKV& b)
    {
        return (a.kv_txid == b.kv_txid && a.key == b.key && a.value == b.value);
    }

    friend bool operator != (const CKV& a, const CKV& b)
    {
        return (a.kv_txid  != b.kv_txid || a.key != b.key || a.value != b.value);
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        LOCK(cs);

        READWRITE(kv_txid);
        READWRITE(kv_source);
        READWRITE(kv_destination);
        READWRITE(key);
        READWRITE(value);
        READWRITE(hash_of_key);
        READWRITE(hash_of_key_dest);
        READWRITE(hash_of_key_dest_src);
    }


};

#endif
