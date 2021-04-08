// Copyright (c) 2020 The Vectorium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "kv.h"
#include "kvmanager.h"
#include "util.h"
#include "consensus/validation.h"
#include <boost/lexical_cast.hpp>

#include "key_io.h"

// keep track of the scanning errors I've seen
map<uint256, int> mapSeenMasternodeScanningErrors;
// cache block hashes as we calculate them
std::map<int64_t, uint256> mapCacheBlockHashes;



CKV::CKV()
{
    LOCK(cs);
    kv_txid.SetNull();
    kv_source = CKeyID();
    kv_destination = CKeyID();
    key = "";
    value = "";
    hash_of_key.SetNull();
    hash_of_key_dest.SetNull();
    hash_of_key_dest_src.SetNull();
}

CKV::CKV(const CKV& other)
{
    LOCK(cs);
    kv_txid = other.kv_txid;
    kv_source = other.kv_source;
    kv_destination = other.kv_destination;
    key = other.key;
    value = other.value;
    hash_of_key = other.hash_of_key;
    hash_of_key_dest = other.hash_of_key_dest;
    hash_of_key_dest_src = other.hash_of_key_dest_src;
}


