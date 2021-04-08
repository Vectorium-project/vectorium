// Copyright (c) 2020 The Vectorium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KVMANAGER_H
#define KVMANAGER_H

#include "base58.h"
#include "key.h"
#include "main.h"
#include "kv.h"
#include "net.h"
#include "sync.h"
#include "util.h"

using namespace std;

class CKVManager;

extern CKVManager kvman;
void DumpKVs();

/** Access to the KV database (kvcache.dat)
 */
class CKVDB
{
private:
    boost::filesystem::path pathKV;
    std::string strMagicMessage;

public:
    enum ReadResult {
        Ok,
        FileError,
        HashReadError,
        IncorrectHash,
        IncorrectMagicMessage,
        IncorrectMagicNumber,
        IncorrectFormat
    };

    CKVDB();
    bool Write(const CKVManager& kvToSave);
    ReadResult Read(CKVManager& kvToLoad, bool fDryRun = false);
};

class CKVManager
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

    std::vector<CKeyID> v_kv_scan_targets;

    // map to hold all KVs
    std::map<uint256, CKV> map_kvs;
    // map to hold key relation to KV
    std::map<uint256, std::vector<uint256>> map_key_hashes;
    // map to hold key and destination relation to KV
    std::map<uint256, std::vector<uint256>> map_key_dest_hashes;
    // map to hold key, destination and source relation to KV
    std::map<uint256, std::vector<uint256>> map_key_dest_src_hashes;


public:

    // do I need to serialize KVManager class ??
    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        LOCK(cs);
        READWRITE(map_kvs);
        READWRITE(map_key_hashes);
        READWRITE(map_key_dest_hashes);
        READWRITE(map_key_dest_src_hashes);
    }

    CKVManager();

    // Do I need to copy an instance of KVManager, ever ?
    CKVManager(const CKVManager& other);

    /// Add an entry
    bool Add(const CKV& kv);

    /// Check all KV entries
    void Check();

    /// Clear all KV maps
    void Clear();


    /// Find all entries hashes that match
    std::vector<uint256> FindAllByKeyHash(const uint256 key_hash);
    std::vector<uint256> FindAllByKeyDestHash(const uint256 key_dest_hash);
    std::vector<uint256> FindAllByKeyDestSrcHash(const uint256 key_dest_src_hash);

    bool kv_hash_exists(const uint256 unique_hash);

    std::map<uint256, CKV> GetFullKVMap()
    {
        Check();
        return map_kvs;
    }

    /// Return the number of KVs
    int size() { return map_kvs.size(); }

    std::string ToString() const;


};

#endif
