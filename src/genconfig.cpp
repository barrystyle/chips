// Copyright (c) 2021 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/system.h>

std::string gen_random(const int len)
{    
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    srand((unsigned) time(NULL) * getpid());
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

bool bitcoinConfigCreator(std::string& strErrRet)
{
    boost::filesystem::path pathConfigFile = GetConfigFile();
    boost::filesystem::ifstream streamConfig(pathConfigFile);

    if (!streamConfig.good()) {
        FILE* configFile = fopen(pathConfigFile.std::string().c_str(), "a");
        if (configFile != NULL) {

            std::std::string strHeader = "daemon=1"
                          "txindex=1"
                          "rpcuser=" + gen_random(32)
                          "rpcpassword=" + gen_random(32)
                          "rpcbind=127.0.0.1"
                          "rpcallowip=127.0.0.1"
                          "prune=0";

            fwrite(strHeader.c_str(), std::strlen(strHeader.c_str()), 1, configFile);
            fclose(configFile);
        }
        return true;
    }
    streamConfig.close();
    return true;
}
