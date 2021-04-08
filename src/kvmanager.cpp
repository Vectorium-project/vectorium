// Copyright (c) 2020 The Vectorium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "kvmanager.h"
#include "kv.h"
#include "util.h"
#include "key_io.h"
#include "consensus/validation.h"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>


/** KV manager */
CKVManager kvdata;

//
// CKVDB
//

CKVDB::CKVDB()
{
    pathKV = GetDataDir() / "kvdb.dat";
    strMagicMessage = "KeyValueDB";
}

bool CKVDB::Write(const CKVManager& kvdataToSave)
{
    int64_t nStart = GetTimeMillis();

    // serialize, checksum data up to that point, then append checksum
    CDataStream ssKVs(SER_DISK, CLIENT_VERSION);
    ssKVs << strMagicMessage;                   // masternode cache file specific magic message
    ssKVs << FLATDATA(Params().MessageStart()); // network specific magic number
    ssKVs << kvdataToSave;
    uint256 hash = Hash(ssKVs.begin(), ssKVs.end());
    ssKVs << hash;

    // open output file, and associate with CAutoFile
    FILE* file = fopen(pathKV.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, pathKV.string());

    // Write and commit header, data
    try {
        fileout << ssKVs;
    } catch (std::exception& e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    //    FileCommit(fileout);
    fileout.fclose();

    LogPrint("kv","Written info to %s  %dms\n", pathKV.string(), GetTimeMillis() - nStart);
    LogPrint("kv","  %s\n", kvdataToSave.ToString());

    return true;
}

CKVDB::ReadResult CKVDB::Read(CKVManager& kvdataToLoad, bool fDryRun)
{
    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE* file = fopen(pathKV.string().c_str(), "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull()) {
        error("%s : Failed to open file %s", __func__, pathKV.string());
        return FileError;
    }

    // use file size to size memory buffer
    int fileSize = boost::filesystem::file_size(pathKV);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if (dataSize < 0)
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char*)&vchData[0], dataSize);
        filein >> hashIn;
    } catch (std::exception& e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    CDataStream ssKVs(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssKVs.begin(), ssKVs.end());
    if (hashIn != hashTmp) {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }

    unsigned char pchMsgTmp[4];
    std::string strMagicMessageTmp;
    try {
        // de-serialize file header (masternode cache file specific magic message) and ..

        ssKVs >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp) {
            error("%s : Invalid kvdb magic message", __func__);
            return IncorrectMagicMessage;
        }

        // de-serialize file header (network specific magic number) and ..
        ssKVs >> FLATDATA(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp))) {
            error("%s : Invalid network magic number", __func__);
            return IncorrectMagicNumber;
        }
        // de-serialize data into CMasternodeMan object
        ssKVs >> kvdataToLoad;
    } catch (std::exception& e) {
        kvdataToLoad.Clear();
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }

    LogPrint("kv","Loaded info from %s  %dms\n", pathKV.string(), GetTimeMillis() - nStart);
    LogPrint("kv","  %s\n", kvdataToLoad.ToString());
    if (!fDryRun) {
        LogPrint("kv","KV manager - cleaning....\n");
        //kvdataToLoad.CheckAndRemove(true);
        LogPrint("kv","KV manager - result:\n");
        LogPrint("kv","  %s\n", kvdataToLoad.ToString());
    }

    return Ok;
}

void DumpKVManager()
{
    int64_t nStart = GetTimeMillis();

    CKVDB kvdb;
    CKVManager tempKVman;

    LogPrint("kv","Verifying kvdb.dat format...\n");
    CKVDB::ReadResult readResult = kvdb.Read(tempKVman, true);
    // there was an error and it was not an error on file opening => do not proceed
    if (readResult == CKVDB::FileError) {
        LogPrint("kv","Missing kv data file - kvdb.dat, will try to recreate\n");
    } else if (readResult != CKVDB::Ok) {
        LogPrint("kv","Error reading kvdb.dat: ");
        if (readResult == CKVDB::IncorrectFormat) {
            LogPrint("kv","magic is ok but data has invalid format, will try to recreate\n");
        } else {
            LogPrint("kv","file format is unknown or invalid, please fix it manually\n");
            return;
        }
    }
    LogPrint("kv","Writting data to kvdb.dat...\n");
    kvdb.Write(kvman);

    LogPrint("kv","KV data dump finished  %dms\n", GetTimeMillis() - nStart);
}

CKVManager::CKVManager()
{
    // init
    std::vector<std::string> v_scan = mapMultiArgs["-scankvdest"];
    if (v_scan.size())
    {
        KeyIO keyIO(Params());
        for (std::string destaddr: v_scan)
        {
            CTxDestination dest = keyIO.DecodeDestination(destaddr);
            if (IsValidDestination(dest))
            {
                CKeyID keyid = boost::get<CKeyID>(dest);
                if (std::find(this->v_kv_scan_targets.begin(), this->v_kv_scan_targets.end(), keyid) != this->v_kv_scan_targets.end())
                {
                    this->v_kv_scan_targets.push_back(keyid); 
                    LogPrintf("KV: Address %s added to kv scan targets\n", destaddr);
                }
                else
                {
                    LogPrintf("KV: Warning: Duplicate kv scan target address %s\n", destaddr);
                }
            }
            else
            {
                LogPrintf("KV: Error: Invalid kv scan target address %s\n", destaddr);
            }     
        }

        if (this->v_kv_scan_targets.size() > 0)
        {
            LogPrintf("KV: One or more kv scan targets set, kv features would be ENABLED\n");

            // now to load kvdb.dat

        }
        else
        {
            LogPrintf("KV: No scan targets defined, kv features would be DISABLED\n");
        }

    }
    else
    {
        LogPrintf("KV: No scan targets defined, kv features would be DISABLED\n");
    }
    


}

bool CKVManager::kv_hash_exists(const uint256 unique_hash)
{
    return map_kvs.find(unique_hash) != map_kvs.end();
}

bool CKVManager::Add(const CKV& kv)
{
    LOCK(cs);

    if (!kv_hash_exists(kv.hash_unique))
    {
        LogPrint("kv", "CKVManager: Adding new kv entry %s - %i now\n", kv.hash_unique.ToString(), size() + 1);
        map_kvs.emplace(kv.hash_unique, kv);
        map_key_hashes[kv.hash_of_key].push_back(kv.hash_unique);
        map_key_dest_hashes[kv.hash_of_key_dest].push_back(kv.hash_unique);
        map_key_dest_src_hashes[kv.hash_of_key_dest_src].push_back(kv.hash_unique);
        return true;
    }

    return false;
}

void CKVManager::Clear()
{
    LOCK(cs);
    map_kvs.clear();
    map_key_hashes.clear();
    map_key_dest_hashes.clear();
    map_key_dest_src_hashes.clear();
}



