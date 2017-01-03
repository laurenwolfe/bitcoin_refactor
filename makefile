SRC=BTCMiner.cpp Connect.cpp BlockChain.cpp Block.cpp Transaction.cpp Encryption.cpp InputSpecifier.cpp OutputSpecifier.cpp Shared.cpp Botan/botan_all.cpp
HEADERS=BTCMiner.h Connect.h BlockChain.h Block.h Transaction.h Encryption.h InputSpecifier.h OutputSpecifier.h Shared.h Botan/botan_all.h
CFLAGS= -Wall -Wpedantic -Wno-unused-variable -Wno-unused-private-field -std=c++11

all: btc-miner

btc-miner: $(SRC) $(HEADERS)
	g++ $(CFLAGS) -g -O0 -o btc-miner $(SRC)

clean:
	rm -f *.o *~ btc-miner