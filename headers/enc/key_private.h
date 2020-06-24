#ifndef KEY_PRIVATE_H
#define KEY_PRIVATE_H

//#include <string>
//#include <sstream>
//#include <iostream>
//#include <cstring>
//#include <QDir>

#include <QDebug>
#include "utils/bignumber.h"
#include "enc/algorithms/blowfish_crypt.h"
#include "enc/algorithms/ecc/ellipticpoint.h"
#include "enc/algorithms/ecc/curves.h"
#include "enc/algorithms/ecc/eccmath.h"

class KeyPrivate
{
private:
    ECC::curve curve;
    BigNumber prkey;
    EllipticPoint pbkey;

public:
    /**
     * @brief New keys
     */
    KeyPrivate();
    /**
     * @brief Existing keys
     * @param keyPair - [prKey:pubKey]
     */
    KeyPrivate(const QJsonObject &json);
    KeyPrivate(const KeyPrivate &keyPrivate);
    ~KeyPrivate();

public:
    EllipticPoint generate();

public: // Cryptor interface
    QByteArray encrypt(const QByteArray &data);
    QByteArray decrypt(const QByteArray &data);
    QByteArray encryptSymmetric(const QByteArray &data);
    QByteArray decryptSymmetric(const QByteArray &data);

public: // Signer interface
    QByteArray sign(const QByteArray &data);
    bool verify(const QByteArray &data, const QByteArray &dsignBase64);

public:
    BigNumber getPrivateKey() const;
    EllipticPoint getPublicKey() const;
};

#endif // KEY_PRIVATE_H
